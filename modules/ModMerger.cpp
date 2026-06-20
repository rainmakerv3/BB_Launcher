// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>
#include <fstream>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

#include "ModMerger.h"
#include "settings/config.h"
#include "ui_ModMerger.h"

namespace fs = std::filesystem;

ModMerger::ModMerger(QWidget* parent) : QDialog(parent), ui(new Ui::ModMerger) {
    ui->setupUi(this);
    ui->waitLabel->setVisible(false);

    this->setFixedHeight(this->height());
    this->setFixedWidth(this->width());

    RefreshModList();

    ui->modList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(ui->modList, &QListWidget::itemSelectionChanged, this, &ModMerger::EnforceTwoItemLimit);

    connect(ui->mergeButton, &QPushButton::pressed, this, &ModMerger::AttemptMerge);
    connect(ui->witchyBrowseButton, &QPushButton::pressed, this, &ModMerger::SetWitchyPath);

    Common::PathToQString(witchyPath, Config::WitchyPath);

    if (!fs::exists(Common::PathFromQString(witchyPath))) {
        ui->witchyLabel->setStyleSheet("color: red;");
        ui->witchyLabel->setText("WitchyBND Path not found - merge tool cannot be used");
        ui->witchyLabel->setText("");
    } else {
        ui->witchyLabel->setText("Valid witchyBND binary detected - merge tool can be used");
        ui->witchyLabel->setStyleSheet("color: green;");
        ui->witchyPathLineEdit->setText(witchyPath);
    }
}

void ModMerger::AttemptMerge() {
    if (!fs::exists(Common::PathFromQString(witchyPath))) {
        QMessageBox::warning(this, "Cannot merge",
                             "Valid witchyBND required, please set it with the browse button");
        return;
    }

    if (fs::exists(Common::GetBBLFilesPath() / "Temp" / "ModMerge"))
        fs::remove_all(Common::GetBBLFilesPath() / "Temp" / "ModMerge");

    currentPriority = ModPriority::NotSet;
    abortAttempt = false;

    GetConflictedFiles();
    if (conflictedFiles.empty()) {
        ui->mergeStatusText->append("No conflicted files, no merge required");
        return;
    }

    fs::path mod1BasePath = StandardizeBasePath(Common::ModPath / mod1Name);
    for (const auto& entry : fs::directory_iterator(mod1BasePath)) {
        if (entry.is_directory()) {
            auto relative_path = fs::relative(entry, mod1BasePath);
            std::string relative_path_string = Common::PathToU8(relative_path);
            if (std::find(BBFolders.begin(), BBFolders.end(), relative_path_string) ==
                BBFolders.end()) {
                QMessageBox::warning(this, "Invalid Mod",
                                     "Folders inside mod folder must include either dvdroot_ps4"
                                     " or Bloodborne dvdroot_ps4 subfolders (ex. sfx, parts, map)");
                return;
            }
        }
    }

    fs::path mod2BasePath = StandardizeBasePath(Common::ModPath / mod2Name);
    for (const auto& entry : fs::directory_iterator(mod2BasePath)) {
        if (entry.is_directory()) {
            auto relative_path = fs::relative(entry, mod2BasePath);
            std::string relative_path_string = Common::PathToU8(relative_path);
            if (std::find(BBFolders.begin(), BBFolders.end(), relative_path_string) ==
                BBFolders.end()) {
                QMessageBox::warning(this, "Invalid Mod",
                                     "Folders inside mod folder must include either dvdroot_ps4"
                                     " or Bloodborne dvdroot_ps4 subfolders (ex. sfx, parts, map)");
                return;
            }
        }
    }

    ui->waitLabel->setVisible(true);

    for (const auto& file : conflictedFiles) {
        fs::path origFilePathOld = GetUpdatedFile(file);
        fs::path origFilePath = baseTempPath / file;

        if (!fs::exists(origFilePath.parent_path())) {
            fs::create_directories(origFilePath.parent_path());
        }
        fs::copy_file(origFilePathOld, origFilePath);

        fs::path mod1filePathOld = StandardizeBasePath(Common::ModPath / mod1Name) / file;
        fs::path mod1filePath = mod1TempPath / file;
        if (!fs::exists(mod1filePath.parent_path())) {
            fs::create_directories(mod1filePath.parent_path());
        }
        fs::copy_file(mod1filePathOld, mod1filePath);

        fs::path mod2filePathOld = StandardizeBasePath(Common::ModPath / mod2Name) / file;
        fs::path mod2filePath = mod2TempPath / file;
        if (!fs::exists(mod2filePath.parent_path())) {
            fs::create_directories(mod2filePath.parent_path());
        }
        fs::copy_file(mod2filePathOld, mod2filePath);

        ui->mergeStatusText->append(QString("Copied %1 to temporary folder").arg(file));
    }

    ExamineConflictedFiles();
}

void ModMerger::ExamineConflictedFiles() {
    auto CleanUp = [this](bool aborted = true) {
        fs::remove_all(Common::GetBBLFilesPath() / "Temp" / "ModMerge");
        ui->waitLabel->setVisible(false);
        if (aborted)
            ui->mergeStatusText->append(
                FormatTextForBrowser("Mod merge attempt aborted by user", Format::BoldRed));
    };

    for (const auto& file : conflictedFiles) {
        fs::path basefile = baseTempPath / file;
        fs::path mod1file = mod1TempPath / file;
        fs::path mod2file = mod2TempPath / file;

        if (UnextractrableFileProcessed(basefile, mod1file, mod2file)) {
            continue;
        }

        ui->mergeStatusText->append("Processing file: " + QString::fromStdString(file));

        std::string folderName = basefile.filename().string();
        std::replace(folderName.begin(), folderName.end(), '.', '-');

        ExtractFile(basefile);
        ExtractFile(mod1file);
        ExtractFile(mod2file);

        fs::path baseExtractedFolder = basefile.parent_path() / folderName;
        fs::path mod1ExtractedFolder = mod1file.parent_path() / folderName;
        fs::path mod2ExtractedFolder = mod2file.parent_path() / folderName;

        if (!fs::exists(baseExtractedFolder) || !fs::exists(mod1ExtractedFolder) ||
            !fs::exists(mod2ExtractedFolder)) {
            QString msg = "Extracted folder not found, most likely due "
                          "to a WitchyBND error. Aborting...";
            ui->mergeStatusText->append(FormatTextForBrowser(msg, Format::BoldRed));
            CleanUp();
            return;
        }

        for (const auto& baseFileEntry : fs::recursive_directory_iterator(baseExtractedFolder)) {
            if (!baseFileEntry.is_directory()) {
                fs::path relative_path = fs::relative(baseFileEntry, baseTempPath);
                fs::path mod1ExtractedFile = mod1TempPath / relative_path;
                fs::path mod2ExtractedFile = mod2TempPath / relative_path;

                bool file1modified = !AreFilesIdentical(baseFileEntry.path(), mod1ExtractedFile);
                bool file2modified = !AreFilesIdentical(baseFileEntry.path(), mod2ExtractedFile);

                auto MergeModified = [this, baseFileEntry](fs::path modFilePath,
                                                           std::string modName) {
                    // TODO FIXME
                    // workaround for too large texture repacking issues
                    if (fs::file_size(modFilePath) > 4 * fs::file_size(baseFileEntry.path())) {
                        QString msg = QString("Modified file: %1 from mod: %2 too large to be "
                                              "merged. Skipping...")
                                          .arg(Common::PathToU8(modFilePath.filename()), modName);
                        ui->mergeStatusText->append(FormatTextForBrowser(msg, Format::Yellow));
                    } else {
                        fs::copy_file(modFilePath, baseFileEntry.path(),
                                      fs::copy_options::overwrite_existing);
                        ui->mergeStatusText->append(
                            QString("Modified file: %1 from mod: %2 merged")
                                .arg(Common::PathToU8(modFilePath.filename()), modName));
                    }
                };

                if (file1modified && file2modified) {
                    QString path;
                    Common::PathToQString(path, relative_path);

                    ui->mergeStatusText->append(
                        QString("Check item level-conflicts in file %1...").arg(path));
                    HandleConflict(baseFileEntry.path(), mod1ExtractedFile, mod2ExtractedFile);
                    if (abortAttempt) {
                        CleanUp();
                        return;
                    }
                } else if (file1modified && !file2modified) {
                    MergeModified(mod1ExtractedFile, mod1Name);
                } else if (file2modified && !file1modified) {
                    MergeModified(mod2ExtractedFile, mod2Name);
                }
            }
        }

        for (const auto& mod1FileEntry : fs::recursive_directory_iterator(mod1ExtractedFolder)) {
            if (!mod1FileEntry.is_directory()) {
                fs::path relative_path = fs::relative(mod1FileEntry, mod1TempPath);
                fs::path mod2ExtractedFile = mod2TempPath / relative_path;
                fs::path baseExtractedFile = baseTempPath / relative_path;

                if (fs::exists(baseExtractedFile)) {
                    continue;
                }

                if (fs::exists(mod2ExtractedFile) &&
                    !AreFilesIdentical(mod1FileEntry.path(), mod2ExtractedFile)) {
                    HandleConflict(baseExtractedFile, mod1FileEntry.path(), mod2ExtractedFile);
                    if (abortAttempt) {
                        CleanUp();
                        return;
                    }
                }

                fs::copy_file(mod1FileEntry.path(), baseExtractedFile,
                              fs::copy_options::overwrite_existing);
                ui->mergeStatusText->append(
                    QString("New file: %1 merged from mod: %2")
                        .arg(Common::PathToU8(mod1FileEntry.path()), mod1Name));
            }
        }

        for (const auto& mod2FileEntry : fs::recursive_directory_iterator(mod2ExtractedFolder)) {
            if (!mod2FileEntry.is_directory()) {
                fs::path relative_path = fs::relative(mod2FileEntry, mod2TempPath);
                fs::path baseExtractedFile = baseTempPath / relative_path;

                if (fs::exists(baseExtractedFile)) {
                    continue;
                }

                fs::copy_file(mod2FileEntry.path(), baseExtractedFile,
                              fs::copy_options::overwrite_existing);
                ui->mergeStatusText->append(
                    QString("New file: %1 merged from mod: %2")
                        .arg(Common::PathToU8(mod2FileEntry.path()), mod2Name));
            }
        }

        fs::remove(basefile);
        RepackItem(baseExtractedFolder);

        if (!fs::exists(basefile)) {
            QString msg = QString::fromStdString(Common::PathToU8(baseExtractedFolder)) +
                          " did not produce any output after repacking, most likely due "
                          "to silent repacking error. Aborting...";
            ui->mergeStatusText->append(FormatTextForBrowser(msg, Format::BoldRed));
            CleanUp();
            return;
        } else {
            ui->mergeStatusText->append("File processing complete: " +
                                        QString::fromStdString(file));
        }

        fs::remove_all(baseExtractedFolder);
    }

    CombineModFiles();
    RefreshModList();
    CleanUp(false);

    ui->mergeStatusText->append(
        FormatTextForBrowser("Mod merge attempt complete. New mod is now available to activate "
                             "using the Mod Manager",
                             Format::BoldGreen));
}

void ModMerger::CombineModFiles() {
    fs::path mod1Folder = StandardizeBasePath(Common::ModPath / mod1Name);
    for (const auto& FileEntry : fs::recursive_directory_iterator(mod1Folder)) {
        if (!FileEntry.is_directory()) {
            const auto relativePath = fs::relative(FileEntry, mod1Folder);
            if (!fs::exists(baseTempPath / relativePath)) {
                if (!fs::exists(baseTempPath / relativePath.parent_path())) {
                    fs::create_directories(baseTempPath / relativePath.parent_path());
                }
                fs::copy_file(FileEntry, baseTempPath / relativePath);
            }
        }
    }

    fs::path mod2Folder = StandardizeBasePath(Common::ModPath / mod2Name);
    for (const auto& FileEntry : fs::recursive_directory_iterator(mod2Folder)) {
        if (!FileEntry.is_directory()) {
            const auto relativePath = fs::relative(FileEntry, mod2Folder);
            if (!fs::exists(baseTempPath / relativePath)) {

                if (!fs::exists(baseTempPath / relativePath.parent_path())) {
                    fs::create_directories(baseTempPath / relativePath.parent_path());
                }
                fs::copy_file(FileEntry, baseTempPath / relativePath);
            }
        }
    }

    std::string newName = mod1Name + " + " + mod2Name;

    try {
        if (fs::exists(Common::ModPath / newName))
            fs::remove_all(Common::ModPath / newName);

        fs::rename(baseTempPath, Common::ModPath / newName);
    } catch (std::exception& e) {
        ui->mergeStatusText->append(FormatTextForBrowser(
            "Moving mod files to mod folder failed: " + QString(e.what()), Format::BoldRed));
    }
}

void ModMerger::GetConflictedFiles() {
    QList<QListWidgetItem*> selectedList = ui->modList->selectedItems();
    if (selectedList.size() != 2) {
        QMessageBox::warning(this, "Error", "Two Mods Need to Selected");
        return;
    }

    conflictedFiles.clear();
    ui->mergeStatusText->clear();

    mod1Name = selectedList.at(0)->text().toStdString();
    fs::path mod1Path = StandardizeBasePath(modPath / mod1Name);
    mod2Name = selectedList.at(1)->text().toStdString();
    fs::path mod2Path = StandardizeBasePath(modPath / mod2Name);

    std::vector<std::string> fileListMod1;
    for (const auto& FileEntry : fs::recursive_directory_iterator(mod1Path)) {
        if (!FileEntry.is_directory()) {
            auto relative_path = fs::relative(FileEntry, mod1Path);
            const auto u8_string = Common::PathToU8(relative_path);
            std::string relative_path_string{u8_string.begin(), u8_string.end()};
            fileListMod1.push_back(relative_path_string);
        }
    }

    for (const auto& entry : fs::recursive_directory_iterator(mod2Path)) {
        if (!entry.is_directory()) {
            auto relative_path = fs::relative(entry, mod2Path);
            std::string relative_path_string = Common::PathToU8(relative_path);
            for (int i = 0; i < fileListMod1.size(); i++) {
                if (fileListMod1[i] == relative_path_string) {
                    conflictedFiles.push_back(relative_path_string);
                    ui->mergeStatusText->append("Conflicted file found: " +
                                                QString::fromStdString(relative_path_string));
                }
            }
        }
    }
}

bool ModMerger::AreFilesIdentical(const fs::path& path1, const fs::path& path2) {
    if (!fs::exists(path1) || !fs::exists(path2)) {
        return false;
    }

    if (fs::file_size(path1) != fs::file_size(path2)) {
        return false;
    }

    std::ifstream file1(path1, std::ios::binary);
    std::ifstream file2(path2, std::ios::binary);

    if (!file1.is_open() || !file2.is_open()) {
        return false;
    }

    const std::size_t bufferSize = 4096;
    std::vector<char> buffer1(bufferSize);
    std::vector<char> buffer2(bufferSize);

    while (file1 && file2) {
        file1.read(buffer1.data(), bufferSize);
        file2.read(buffer2.data(), bufferSize);

        std::streamsize bytesRead1 = file1.gcount();
        std::streamsize bytesRead2 = file2.gcount();

        if (bytesRead1 != bytesRead2) {
            return false;
        }

        if (bytesRead1 == 0) {
            break;
        }

        if (std::memcmp(buffer1.data(), buffer2.data(), static_cast<std::size_t>(bytesRead1)) !=
            0) {
            return false;
        }
    }
    return true;
}

fs::path ModMerger::GetUpdatedFile(fs::path relativePath) {
    std::string patchString = Common::game_serial + "-patch";
    std::string updateString = Common::game_serial + "-UPDATE";

    if (fs::exists(Common::installPath.parent_path() / patchString / "dvdroot_ps4" /
                   relativePath)) {
        return Common::installPath.parent_path() / patchString / "dvdroot_ps4" / relativePath;
    } else if (fs::exists(Common::installPath.parent_path() / updateString / "dvdroot_ps4" /
                          relativePath)) {
        return Common::installPath.parent_path() / updateString / "dvdroot_ps4" / relativePath;
    }

    return (Common::installPath.parent_path() / Common::game_serial / "dvdroot_ps4" / relativePath);
}

void ModMerger::ExtractFile(fs::path file) {
    QString Qfile;
    Common::PathToQString(Qfile, file);
    ui->mergeStatusText->append("Extracting with WitchyBND:\n" + Qfile);

    QProcess* process = new QProcess(this);
    QStringList arguments;
    arguments << QDir::toNativeSeparators(Qfile);

    if (file.extension().string() == ".param") {
        arguments << "--silent";

        // workaround, manually add the BB game line to silently extract
        fs::path xmlFile = file.parent_path() / "_witchy-bnd4.xml";
        pugi::xml_document doc;
        if (!doc.load_file(xmlFile.c_str())) {
            ui->mergeStatusText->append("Could not xml load file: " +
                                        QString::fromStdString(Common::PathToU8(xmlFile)));
            process->deleteLater();
            return;
        }

        pugi::xml_node root = doc.child("bnd4");
        if (!root) {
            ui->mergeStatusText->append("Root node <bnd4> not found");
            process->deleteLater();
            return;
        }

        if (!root.child("game")) {
            pugi::xml_node game_node = root.append_child("game");
            game_node.text().set("BB");
        }

        doc.save_file(xmlFile.c_str(), "  ");
    }

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
                process->deleteLater();
                QString stdOut = process->readAllStandardOutput();
                if (!stdOut.isEmpty()) {
                    ui->mergeStatusText->append("WitchyBND Output:\n" + stdOut);
                }

                QString stdErr = process->readAllStandardError();
                if (!stdErr.isEmpty()) {
                    ui->mergeStatusText->append("WitchyBND Error Log:\n" + stdErr);
                }

                emit WitchyBndFinished();
            });

    connect(process, &QProcess::errorOccurred,
            [this, process, Qfile](QProcess::ProcessError error) {
                ui->mergeStatusText->append("Error: Failed to Extract File: " + Qfile);

                QString stdErr = process->readAllStandardError();
                if (!stdErr.isEmpty()) {
                    ui->mergeStatusText->append("WitchyBND Error Log:\n" + stdErr);
                }
            });

    process->start(witchyPath, arguments);

    QEventLoop loop;
    QObject::connect(this, &ModMerger::WitchyBndFinished, &loop, &QEventLoop::quit);
    loop.exec();

    process->deleteLater();
    ui->mergeStatusText->append("File Extracted: " + Qfile);
}

void ModMerger::RepackItem(fs::path itemPath) {
    // workaround, manually remove the added BB game line to avoid parsing errors
    if (fs::is_directory(itemPath) && itemPath.filename().string() == "gameparam-parambnd-dcx") {
        fs::path xmlFile = itemPath / "_witchy-bnd4.xml";
        pugi::xml_document doc;
        if (!doc.load_file(xmlFile.c_str())) {
            ui->mergeStatusText->append(FormatTextForBrowser(
                "Could not xml load file: " + QString::fromStdString(Common::PathToU8(xmlFile)),
                Format::BoldRed));
            abortAttempt = true;
            return;
        }

        pugi::xml_node root = doc.child("bnd4");
        if (!root) {
            ui->mergeStatusText->append(FormatTextForBrowser(
                "Root node <bnd4> not found, xml file invalid", Format::BoldRed));
            abortAttempt = true;
            return;
        }

        if (root.child("game")) {
            root.remove_child("game");
            doc.save_file(xmlFile.c_str(), "  ");
        }
    }

    QString targetPath;
    Common::PathToQString(targetPath, itemPath);
    ui->mergeStatusText->append("Repacking with WitchyBND:\n" + targetPath);

    QProcess* process = new QProcess(this);
    QStringList arguments;
    arguments << QDir::toNativeSeparators(targetPath);
    arguments << "-p";

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
                process->deleteLater();
                QString stdOut = process->readAllStandardOutput();
                if (!stdOut.isEmpty()) {
                    ui->mergeStatusText->append("WitchyBND Output:\n" + stdOut);
                }

                QString stdErr = process->readAllStandardError();
                if (!stdErr.isEmpty()) {
                    ui->mergeStatusText->append("WitchyBND Error Log:\n" + stdErr);
                }

                emit WitchyBndFinished();
            });

    connect(process, &QProcess::errorOccurred,
            [this, process, targetPath](QProcess::ProcessError error) {
                ui->mergeStatusText->append("Error: Failed to Repack File: " + targetPath);

                QString stdErr = process->readAllStandardError();
                if (!stdErr.isEmpty()) {
                    ui->mergeStatusText->append("WitchyBND Error Log:\n" + stdErr);
                }
            });

    process->start(witchyPath, arguments);

    QEventLoop loop;
    QObject::connect(this, &ModMerger::WitchyBndFinished, &loop, &QEventLoop::quit);
    loop.exec();
}

void ModMerger::HandleConflict(fs::path targetFile, fs::path mod1File, fs::path mod2File) {
    if (UnextractrableFileProcessed(targetFile, mod1File, mod2File)) {
        return;
    }

    if (abortAttempt)
        return;

    ExtractFile(targetFile);
    ExtractFile(mod1File);
    ExtractFile(mod2File);

#ifdef Q_OS_WINDOWS
    std::wstring xmlFileName = targetFile.filename().wstring() + L".xml";
#else
    std::string xmlFileName = targetFile.filename().string() + ".xml";
#endif

    fs::path targetXml = targetFile.parent_path() / xmlFileName;
    fs::path mod1Xml = mod1File.parent_path() / xmlFileName;
    fs::path mod2Xml = mod2File.parent_path() / xmlFileName;

    pugi::xml_document targetDoc;
    targetDoc.load_file(targetXml.c_str());
    pugi::xml_document mod1Doc;
    mod1Doc.load_file(mod1Xml.c_str());
    pugi::xml_document mod2Doc;
    mod2Doc.load_file(mod2Xml.c_str());

    if (targetFile.extension().string() == ".param") {
        ProcessParamConflicts(targetDoc, mod1Doc, mod2Doc);
    } else if (targetFile.extension().string() == ".fmg") {
        ProcessFmgConflicts(targetDoc, mod1Doc, mod2Doc);
    } else if (targetFile.extension().string() == ".mtd") {
        // to do
    }

    if (abortAttempt)
        return;

    if (!targetDoc.save_file(targetXml.c_str(), "  ")) {
        QString msg = "Unable to save xml " + QString(targetXml.c_str());
        ui->mergeStatusText->append(FormatTextForBrowser(msg, Format::BoldRed));
        abortAttempt = true;
        return;
    }

    fs::remove(targetFile);
    RepackItem(targetXml);

    if (abortAttempt)
        return;

    if (!fs::exists(targetFile)) {
        QMessageBox::warning(
            this, "Error",
            QString::fromStdString(Common::PathToU8(targetFile) +
                                   " did not produce any output after repacking, most likely due "
                                   "to silent repacking error. Aborting..."));
        abortAttempt = true;
    }
}

bool ModMerger::UnextractrableFileProcessed(fs::path targetFile, fs::path mod1File,
                                            fs::path mod2File) {
    auto relative_path = fs::relative(targetFile, baseTempPath);
    std::string firstFolder;
    auto it = relative_path.begin();
    if (it != relative_path.end()) {
        if (it->empty() || *it == "/") {
            ++it;
        }

        if (it != relative_path.end()) {
            firstFolder = it->string();
        }
    }

    const std::vector<std::string> unextractableFolders = {"event", "movie",    "chr",   "facegen",
                                                           "map",   "paramdef", "parts", "remo",
                                                           "mtd",   "script",   "sound"};
    const bool hasExtractableFolder =
        std::find(unextractableFolders.begin(), unextractableFolders.end(), firstFolder) ==
        unextractableFolders.end();
    if (!hasExtractableFolder) {
        ui->mergeStatusText->append("Unable to process hard conflicts in the folder: " +
                                    QString::fromStdString(firstFolder));
    }

    const std::vector<std::string> extractableExtensions = {".dcx", ".fmg", ".mtd", ".param"};
    bool hasExtractableExtension =
        std::find(extractableExtensions.begin(), extractableExtensions.end(),
                  targetFile.extension().string()) != extractableExtensions.end();

    if (!hasExtractableExtension) {
        ui->mergeStatusText->append("Unable to process hard conflicts thie file extension : " +
                                    QString::fromStdString(targetFile.extension().string()));
    } else if (Common::PathToU8(targetFile).ends_with(".gparam.dcx")) {
        ui->mergeStatusText->append("Unable to process hard conflicts with extension: gparam.dcx");
        hasExtractableExtension = false;
    }

    if (hasExtractableExtension && hasExtractableFolder) {
            return false;
    }

    if (currentPriority == ModPriority::NotSet) {
        ModMerger::SetModPriority();
        if (abortAttempt) {
            return true;
        }
    }

    if (currentPriority == ModPriority::Mod1) {
        fs::copy_file(mod1File, targetFile, fs::copy_options::overwrite_existing);
        QString msg =
            QString("Unresolvable conflict, instead copying whole file: %1 prioritized mod: %2")
                .arg(Common::PathToU8(mod1File.filename()), mod1Name);
        ui->mergeStatusText->append(FormatTextForBrowser(msg, Format::Yellow));
    } else if (currentPriority == ModPriority::Mod2) {
        fs::copy_file(mod2File, targetFile, fs::copy_options::overwrite_existing);
        QString msg =
            QString("Unresolvable conflict, instead copying whole file: %1 prioritized mod: %2")
                .arg(Common::PathToU8(mod2File.filename()), mod2Name);
        ui->mergeStatusText->append(FormatTextForBrowser(msg, Format::Yellow));
    }

    return true;
}

void ModMerger::ProcessParamConflicts(pugi::xml_document& baseDoc, pugi::xml_document& mod1Doc,
                                      pugi::xml_document& mod2Doc) {
    auto IndexNodeMap = [this](auto&& self, pugi::xml_node node,
                               std::map<std::string, pugi::xml_node>& nodeMap) -> void {
        if (node.children().begin() == node.children().end()) {
            for (pugi::xml_attribute attr : node.attributes()) {
                if (std::string(attr.name()) == "id" || std::string(attr.name()) == "name") {
                    std::string id = attr.value();
                    nodeMap[id] = node;
                    break;
                }
            }
            return;
        }

        for (pugi::xml_node child : node.children()) {
            self(self, child, nodeMap);
        }
    };

    std::map<std::string, pugi::xml_node> baseDocNodeMap;
    IndexNodeMap(IndexNodeMap, baseDoc.root(), baseDocNodeMap);

    std::map<std::string, pugi::xml_node> mod1DocNodeMap;
    IndexNodeMap(IndexNodeMap, mod1Doc.root(), mod1DocNodeMap);

    std::map<std::string, pugi::xml_node> mod2DocNodeMap;
    IndexNodeMap(IndexNodeMap, mod2Doc.root(), mod2DocNodeMap);

    for (auto& entry : baseDocNodeMap) {
        std::string currentId = entry.first;
        auto doc1Node = mod1DocNodeMap.find(currentId);
        auto doc2Node = mod2DocNodeMap.find(currentId);

        auto firstAttr = entry.second.first_attribute();
        for (pugi::xml_attribute attr = firstAttr.next_attribute(); attr;
             attr = attr.next_attribute()) {

            pugi::xml_attribute doc1Attr = doc1Node->second.attribute(attr.name());
            pugi::xml_attribute doc2Attr = doc2Node->second.attribute(attr.name());

            if (std::string(attr.name()) == "name") {
                continue;
            }

            bool doc1Modified =
                std::string(attr.value()) != std::string(doc1Attr.value()) && doc1Attr;
            bool doc2Modified =
                std::string(attr.value()) != std::string(doc2Attr.value()) && doc2Attr;

            if (doc1Modified && doc2Modified) {
                if (currentPriority == ModPriority::NotSet) {
                    ModMerger::SetModPriority();
                    if (abortAttempt) {
                        return;
                    }
                }

                if (currentPriority == ModPriority::Mod1) {
                    QString msg = QString("Unresolvable conflict, using data "
                                          "from user-prioritized mod: %1 identifier: %4 attribute: "
                                          "%2 value: %3")
                                      .arg(mod1Name, attr.name(), doc1Attr.value(), entry.first);
                    ui->mergeStatusText->append(FormatTextForBrowser(msg, Format::Yellow));
                    attr.set_value(doc1Attr.value());
                } else if (currentPriority == ModPriority::Mod2) {
                    QString msg = QString("Unresolvable conflict, using data "
                                          "from user-prioritized mod: %1 identifier: %4 attribute: "
                                          "%2 value: %3")
                                      .arg(mod2Name, attr.name(), doc2Attr.value(), entry.first);
                    ui->mergeStatusText->append(FormatTextForBrowser(msg, Format::Yellow));
                    attr.set_value(doc2Attr.value());
                }
                continue;
            }

            if (doc1Modified && !doc2Modified) {
                ui->mergeStatusText->append(
                    QString("Merged attribute: %2 from mod: %1 with value: %3")
                        .arg(mod1Name, attr.name(), doc1Attr.value()));
                attr.set_value(doc1Attr.value());
            } else if (!doc1Modified && doc2Modified) {
                ui->mergeStatusText->append(
                    QString("Merged attribute: %2 from mod: %1 with value: %3")
                        .arg(mod2Name, attr.name(), doc2Attr.value()));
                attr.set_value(doc2Attr.value());
            }
        }

        if (doc1Node != mod1DocNodeMap.end()) {
            for (pugi::xml_attribute attr = doc1Node->second.first_attribute().next_attribute();
                 attr; attr = attr.next_attribute()) {
                pugi::xml_attribute entryAttr = entry.second.attribute(attr.name());
                if (!entryAttr) {
                    entry.second.append_attribute(attr.name()).set_value(attr.value());
                    ui->mergeStatusText->append(
                        QString("Merged new attribute in identifier: %4 : %2 from mod: %1 with "
                                "value %3")
                            .arg(mod1Name, attr.name(), attr.value(), entry.first));
                }
            }
        }

        if (doc2Node != mod2DocNodeMap.end()) {
            for (pugi::xml_attribute attr = doc2Node->second.first_attribute().next_attribute();
                 attr; attr = attr.next_attribute()) {
                pugi::xml_attribute entryAttr = entry.second.attribute(attr.name());
                if (!entryAttr) {
                    entry.second.append_attribute(attr.name()).set_value(attr.value());
                    ui->mergeStatusText->append(
                        QString("Merged new attribute in identifier: %4 : %2 from mod: %1 with "
                                "value %3")
                            .arg(mod2Name, attr.name(), attr.value(), entry.first));
                }
            }
        }
    }

    for (const auto& mod1Entry : mod1DocNodeMap) {
        std::string currentId = mod1Entry.first;
        auto firstAttr = mod1Entry.second.first_attribute();
        auto baseDocNode = baseDocNodeMap.find(currentId);

        if (baseDocNode == baseDocNodeMap.end()) {
            pugi::xml_node parent;
            if (std::string(firstAttr.name()) == "name") {
                parent = baseDoc.child("param").child("fields");
            } else {
                parent = baseDoc.child("param").child("rows");
            }

            parent.append_copy(mod1Entry.second);
            ui->mergeStatusText->append(
                QString("Merged new %1 param entry from mod: %3: identifier: %2")
                    .arg(mod1Entry.second.name(), mod1Entry.first, mod1Name));
        }
    }

    for (const auto& mod2Entry : mod2DocNodeMap) {
        std::string currentId = mod2Entry.first;
        auto firstAttr = mod2Entry.second.first_attribute();
        auto baseDocNode = baseDocNodeMap.find(currentId);

        if (baseDocNode == baseDocNodeMap.end()) {
            pugi::xml_node parent;
            if (std::string(firstAttr.name()) == "name") {
                parent = baseDoc.child("param").child("fields");
            } else {
                parent = baseDoc.child("param").child("rows");
            }

            parent.append_copy(mod2Entry.second);
            ui->mergeStatusText->append(
                QString("Merged new %1 param entry from mod: %3: identifier: %2")
                    .arg(mod2Entry.second.name(), mod2Entry.first, mod2Name));
        }
    }
}

void ModMerger::ProcessFmgConflicts(pugi::xml_document& baseDoc, pugi::xml_document& mod1Doc,
                                    pugi::xml_document& mod2Doc) {
    auto MapLeafNodes = [this](auto&& self, pugi::xml_node node,
                               std::map<std::string, pugi::xml_node>& sourceMap) -> void {
        bool isLeaf = true;
        for (pugi::xml_node child : node.children()) {
            if (child.type() == pugi::node_element) {
                isLeaf = false;
                self(self, child, sourceMap);
            }
        }

        if (isLeaf && node.type() == pugi::node_element) {
            pugi::xml_attribute idAttr = node.attribute("id");
            if (idAttr) {
                sourceMap[idAttr.value()] = node;
            }
        }
    };

    std::map<std::string, pugi::xml_node> baseDocNodeMap;
    MapLeafNodes(MapLeafNodes, baseDoc.root(), baseDocNodeMap);

    std::map<std::string, pugi::xml_node> mod1DocNodeMap;
    MapLeafNodes(MapLeafNodes, mod1Doc.root(), mod1DocNodeMap);

    std::map<std::string, pugi::xml_node> mod2DocNodeMap;
    MapLeafNodes(MapLeafNodes, mod2Doc.root(), mod2DocNodeMap);

    for (auto& baseEntry : baseDocNodeMap) {
        std::string id = baseEntry.first;
        std::string baseValue = baseEntry.second.text().get();

        auto doc1Node = mod1DocNodeMap.find(id);
        auto doc2Node = mod2DocNodeMap.find(id);

        if (doc1Node == mod1DocNodeMap.end() || doc2Node == mod2DocNodeMap.end()) {
            QString msg = QString(">Xml document mismatch for fmg text id %1, "
                                  "unable to handle. Skipping...")
                              .arg(id);
            ui->mergeStatusText->append(FormatTextForBrowser(msg, Format::Yellow));
            continue;
        }

        std::string doc1Value = doc1Node->second.text().get();
        std::string doc2Value = doc2Node->second.text().get();

        bool doc1Modified = baseValue != doc1Value;
        bool doc2Modified = baseValue != doc2Value;

        if (doc1Modified && doc2Modified) {
            if (currentPriority == ModPriority::NotSet) {
                ModMerger::SetModPriority();
                if (abortAttempt) {
                    return;
                }
            }

            if (currentPriority == ModPriority::Mod1) {
                QString msg = QString("Unresolvable conflict, "
                                      "using data from user-prioritized mod: %1 id: "
                                      "%2 with value: %3")
                                  .arg(mod1Name, id, doc1Value);
                ui->mergeStatusText->append(FormatTextForBrowser(msg, Format::Yellow));
                baseEntry.second.text().set(doc1Value);
            } else if (currentPriority == ModPriority::Mod2) {
                QString msg = QString("Unresolvable conflict, "
                                      "using data from user-prioritized mod: %1 id: "
                                      "%2 with value: %3")
                                  .arg(mod2Name, id, doc2Value);
                ui->mergeStatusText->append(FormatTextForBrowser(msg, Format::Yellow));
                baseEntry.second.text().set(doc2Value);
            }
            continue;
        }

        if (doc1Modified && !doc2Modified) {
            ui->mergeStatusText->append(QString("Merged text id: %2 from mod: %1 with value: %3")
                                            .arg(mod1Name, id, doc1Value));
            baseEntry.second.text().set(doc1Value);
        } else if (!doc1Modified && doc2Modified) {
            ui->mergeStatusText->append(QString("Merged text id: %2 from mod: %1 with value: %3")
                                            .arg(mod2Name, id, doc2Value));
            baseEntry.second.text().set(doc2Value);
        }
    }

    for (const auto& mod1Entry : mod1DocNodeMap) {
        std::string currentId = mod1Entry.first;
        auto baseDocNode = baseDocNodeMap.find(currentId);

        if (baseDocNode == baseDocNodeMap.end()) {
            pugi::xml_node parent = baseDoc.child("fmg").child("entries");
            parent.append_copy(mod1Entry.second);
            ui->mergeStatusText->append(QString("Merged new text entry from mod: %2 id: %1")
                                            .arg(mod1Entry.first, mod1Name));
        }
    }

    for (const auto& mod2Entry : mod2DocNodeMap) {
        std::string currentId = mod2Entry.first;
        auto baseDocNode = baseDocNodeMap.find(currentId);

        if (baseDocNode == baseDocNodeMap.end()) {
            pugi::xml_node parent = baseDoc.child("fmg").child("entries");
            parent.append_copy(mod2Entry.second);
            ui->mergeStatusText->append(QString("Merged new text entry from mod: %2 id: %1")
                                            .arg(mod2Entry.first, mod2Name));
        }
    }
}

void ModMerger::SetModPriority() {
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Handle data conflict");
    dialog->setMinimumWidth(500);

    QLabel* label = new QLabel(
        "The same information is modified for both mods. You can force the merge attempt to "
        "continue by choosing which mod files to prioritize, but the final merged "
        "mod will likely work correctly when there are multiple unresolvable conflicts",
        dialog);
    label->setWordWrap(true);

    QPushButton* btn1 =
        new QPushButton("Use data from " + QString::fromStdString(mod1Name), dialog);
    QPushButton* btn2 =
        new QPushButton("Use data from " + QString::fromStdString(mod2Name), dialog);
    QPushButton* btn3 = new QPushButton("Abort merge attempt", dialog);

    QObject::connect(btn1, &QPushButton::clicked, [this, dialog]() {
        currentPriority = ModPriority::Mod1;
        dialog->accept();
        dialog->deleteLater();
    });

    QObject::connect(btn2, &QPushButton::clicked, [this, dialog]() {
        currentPriority = ModPriority::Mod2;
        dialog->accept();
        dialog->deleteLater();
    });

    QObject::connect(btn3, &QPushButton::clicked, [this, dialog]() {
        abortAttempt = true;
        dialog->accept();
        dialog->deleteLater();
    });

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->addWidget(label);
    layout->addWidget(btn1);
    layout->addWidget(btn2);
    layout->addWidget(btn3);

    dialog->setLayout(layout);
    dialog->exec();
}

fs::path ModMerger::StandardizeBasePath(fs::path basePath) {
    if (fs::exists(basePath / "dvdroot_ps4")) {
        basePath = basePath / "dvdroot_ps4";
    }

    return basePath;
}

void ModMerger::SetWitchyPath() {
    QString exePath;
#ifdef _WIN32
    exePath = QFileDialog::getOpenFileName(this, "Select WitchyBND executable", QDir::currentPath(),
                                           "Executables (WitchyBND.exe)");
#else
    exePath = QFileDialog::getOpenFileName(this, "Select WitchyBND binary", QDir::homePath(),
                                           "WitchyBND Binary (WitchyBND*)");
#endif

    if (exePath.isEmpty()) {
        return;
    } else {
        witchyPath = exePath;
        ui->witchyLabel->setText("Valid witchyBND binary detected - merge tool can be used");
        ui->witchyLabel->setStyleSheet("color: green;");
        ui->witchyPathLineEdit->setText(witchyPath);
        Config::WitchyPath = Common::PathFromQString(witchyPath);
        Config::SaveLauncherSettings();
    }
}

void ModMerger::RefreshModList() {
    ui->modList->clear();

    QStringList ModStringList;
    for (const auto& FolderEntry : fs::directory_iterator(modPath)) {
        if (FolderEntry.is_directory()) {
            std::string FolderName = Common::PathToU8(FolderEntry.path().filename());
            ModStringList.append(QString::fromStdString(FolderName));
        }
    }

    ModStringList.sort(Qt::CaseInsensitive);
    ui->modList->addItems(ModStringList);
}

void ModMerger::EnforceTwoItemLimit() {
    QList<QListWidgetItem*> currentSelected = ui->modList->selectedItems();

    for (QListWidgetItem* item : currentSelected) {
        if (!selectedHistory.contains(item)) {
            selectedHistory.append(item);
        }
    }

    for (int i = 0; i < selectedHistory.size(); ++i) {
        if (!currentSelected.contains(selectedHistory.at(i))) {
            selectedHistory.removeAt(i);
            --i;
        }
    }

    if (selectedHistory.size() > 2) {
        QListWidgetItem* itemToDeselect = selectedHistory.takeFirst();
        itemToDeselect->setSelected(false);
    }
}

QString ModMerger::FormatTextForBrowser(QString input, Format format) {
    if (format == Format::BoldRed) {
        input = "<span style=\"color: red; font-weight: bold;\">" + input + "</span>";
    } else if (format == Format::BoldGreen) {
        input = "<span style=\"color: green; font-weight: bold;\">" + input + "</span>";
    } else if (format == Format::Yellow) {
        input = "<span style=\"color: yellow;\">" + input + "</span>";
    }

    return input;
}

ModMerger::~ModMerger() {
    delete ui;
}
