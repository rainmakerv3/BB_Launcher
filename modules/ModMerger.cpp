// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QMessageBox>
#include <QtConcurrent/QtConcurrentRun>

#include "ModMerger.h"
#include "modules/BBFormats/BBFormats.h"
#include "modules/BBFormats/ConflictHandler.h"
#include "modules/BBFormats/Dcx.h"
#include "ui_ModMerger.h"

using namespace FileHelper;
namespace fs = std::filesystem;

ModMerger::ModMerger(QWidget* parent) : QDialog(parent), ui(new Ui::ModMerger) {
    ui->setupUi(this);
    ui->waitLabel->setVisible(false);
    this->setFixedHeight(this->height());
    this->setFixedWidth(this->width());

    RefreshModList();

    ui->modList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(ui->modList, &QListWidget::itemSelectionChanged, this, &ModMerger::EnforceTwoItemLimit);

    connect(ui->mergeButton, &QPushButton::pressed, this, [this]() {
        QList<QListWidgetItem*> selectedList = ui->modList->selectedItems();
        if (selectedList.size() != 2) {
            QMessageBox::warning(this, "Error", "Two Mods Need to Selected");
            return;
        }

        mod1Name = selectedList.at(0)->text().toStdString();
        mod2Name = selectedList.at(1)->text().toStdString();
        currentPriority = ModPriority::NotSet;
        conflictedFiles.clear();

        ui->waitLabel->setVisible(true);
        ui->mergeStatusText->clear();
        ui->mergeButton->setEnabled(false);
        ui->buttonBox->setEnabled(false);
        activeMerge = QtConcurrent::run(&ModMerger::AttemptMerge, this);
    });

    connect(this, &ModMerger::CleanUpRequested, this, [this](bool aborted) {
        std::error_code ec;
        fs::remove_all(Common::GetBBLFilesPath() / "Temp" / "ModMerge", ec);

        if (ec) {
            Log(QString("Cleanup delayed: %1").arg(QString::fromStdString(ec.message())));
        }

        ui->waitLabel->setVisible(false);
        ui->mergeButton->setEnabled(true);
        ui->buttonBox->setEnabled(true);

        if (aborted) {
            Log("Mod merge attempt aborted", Format::BoldRed);
        } else {
            Log("Mod merge complete", Format::BoldGreen);
        }
    });

    connect(this, &ModMerger::LogRequested, this, [this](QString msg) {
        ui->mergeStatusText->append(msg);
        ui->mergeStatusText->moveCursor(QTextCursor::End);
    });
}

void ModMerger::AttemptMerge() {
    if (fs::exists(Common::GetBBLFilesPath() / "Temp" / "ModMerge"))
        fs::remove_all(Common::GetBBLFilesPath() / "Temp" / "ModMerge");

    GetConflictedFiles();
    if (conflictedFiles.empty()) {
        Log("No conflicted files found for these two mods, merge is not required", Format::Yellow);
        return;
    }

    fs::path mod1BasePath = StandardizeBasePath(Common::ModPath / mod1Name);
    fs::path mod2BasePath = StandardizeBasePath(Common::ModPath / mod2Name);
    if (!GetMergeFiles(mod1BasePath, mod2BasePath)) {
        emit CleanUpRequested(true);
        return;
    }

    for (const auto& file : conflictedFiles) {
        bool canExtract = true;
        if (!IsFolderSupported(file)) {
            canExtract = false;
        }

        fs::path basefile = baseTempPath / file;
        fs::path mod1file = mod1TempPath / file;
        fs::path mod2file = mod2TempPath / file;

        if (!fs::exists(basefile)) {
            Log("Skipping file not present in original game " + QString::fromStdString(file),
                Format::Yellow);
            continue;
        }

        std::string fileExt = file.substr(file.length() - 3);
        std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        std::vector<char> baseData;

        Dcx origDcx(this);
        // first level extration, only dcx (maybe hks later on)
        if (fileExt == "dcx") {
            if (!origDcx.UnpackDcx(basefile, baseData)) {
                emit CleanUpRequested(true);
                return;
            }
        } else {
            canExtract = false;
        }

        std::string filetype;
        // what can be extracted after DCX, rn only these, maybe esd/emevd after
        if (canExtract) {
            filetype = GetFileType(baseData);
            canExtract = filetype.contains("TPF") || filetype.contains("BND4");
        }

        if (!canExtract) {
            if (!ChooseBaseFile(basefile, mod1file, mod2file)) {
                emit CleanUpRequested(true);
                return;
            }
            continue;
        }

        std::vector<char> mod1Data;
        std::vector<char> mod2Data;

        Dcx mod1Dcx(this);
        if (!mod1Dcx.UnpackDcx(mod1file, mod1Data)) {
            emit CleanUpRequested(true);
            return;
        }

        Dcx mod2Dcx(this);
        if (!mod2Dcx.UnpackDcx(mod2file, mod2Data)) {
            emit CleanUpRequested(true);
            return;
        }

        ConflictHandler handler = ConflictHandler(filetype, this);
        if (!filetype.contains("BND4")) {
            if (!handler.HandleItemConflict(baseData, mod1Data, mod2Data)) {
                emit CleanUpRequested(true);
                return;
            } else {
                origDcx.RepackDcx(baseData);
            }
        } else {
            if (!handler.HandleBinderConflict(baseData, mod1Data, mod2Data)) {
                emit CleanUpRequested(true);
            } else {
                origDcx.RepackDcx(baseData);
            }
        }
    }

    CombineModFiles();
    emit CleanUpRequested(false);
}

bool ModMerger::GetMergeFiles(std::filesystem::path mod1Base, std::filesystem::path mod2Base) {
    try {
        for (const auto& file : conflictedFiles) {
            fs::path origFilePathOld = GetUpdatedFile(file);
            fs::path origFilePath = baseTempPath / file;

            if (!fs::exists(origFilePath.parent_path())) {
                fs::create_directories(origFilePath.parent_path());
            }

            if (fs::exists(origFilePathOld)) {
                fs::copy_file(origFilePathOld, origFilePath);
            } else {
                Log("Skipping file not present in original game " + QString::fromStdString(file),
                    Format::Yellow);
                continue;
            }

            fs::path mod1BasePath = StandardizeBasePath(Common::ModPath / mod1Name);
            fs::path mod1filePathOld = StandardizeBasePath(Common::ModPath / mod1Name) / file;
            fs::path mod1filePath = mod1TempPath / file;

            if (!fs::exists(mod1filePath.parent_path())) {
                fs::create_directories(mod1filePath.parent_path());
            }
            fs::copy_file(mod1filePathOld, mod1filePath);

            fs::path mod2BasePath = StandardizeBasePath(Common::ModPath / mod2Name);
            fs::path mod2filePathOld = StandardizeBasePath(Common::ModPath / mod2Name) / file;
            fs::path mod2filePath = mod2TempPath / file;

            if (!fs::exists(mod2filePath.parent_path())) {
                fs::create_directories(mod2filePath.parent_path());
            }
            fs::copy_file(mod2filePathOld, mod2filePath);

            Log(QString("Copied %1 to temporary folder").arg(file));
        }
    } catch (const std::exception& e) {
        Log("ERROR: File operations failed: " + QString(e.what()), Format::BoldRed);
        return false;
    }

    return true;
}

bool ModMerger::IsFolderSupported(std::filesystem::path filePath) {
    std::string firstFolder;
    auto it = filePath.begin();
    if (it != filePath.end()) {
        if (it->empty() || *it == "/") {
            ++it;
        }

        if (it != filePath.end()) {
            firstFolder = it->string();
        }
    }

    const std::vector<std::string> unextractableFolders = {"event", "facegen", "shader",
                                                           "paramdef"};

    if (std::find(unextractableFolders.begin(), unextractableFolders.end(), firstFolder) !=
        unextractableFolders.end()) {
        Log("Unable to process hard conflicts in the folder: " +
            QString::fromStdString(firstFolder));
        return false;
    }

    return true;
}

std::string ModMerger::GetFileType(const std::vector<char>& filedata) {
    if (filedata.size() < 8)
        return "";

    std::stringstream s(std::string(filedata.begin(), filedata.end()),
                        std::ios_base::in | std::ios_base::binary);

    std::string strBuffer(8, '\0');
    s.read(&strBuffer[0], 8);
    strBuffer.erase(std::remove(strBuffer.begin(), strBuffer.end(), '\0'), strBuffer.end());

    return strBuffer;
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
        Log("Moving mod files to mod folder failed: " + QString(e.what()), Format::BoldRed);
    }
}

void ModMerger::GetConflictedFiles() {
    fs::path mod1Path = StandardizeBasePath(modPath / mod1Name);
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
                    Log("Conflicted file found: " + QString::fromStdString(relative_path_string));
                }
            }
        }
    }
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

bool ModMerger::ChooseBaseFile(fs::path targetFile, fs::path mod1File, fs::path mod2File) {
    if (currentPriority == ModPriority::NotSet) {
        QMetaObject::invokeMethod(this, &ModMerger::OpenPriorityDialog,
                                  Qt::BlockingQueuedConnection);
        if (currentPriority == ModPriority::NotSet) {
            return false;
        }
    }

    try {
        if (currentPriority == ModPriority::Mod1) {
            if (fs::exists(mod1File)) {
                fs::copy_file(mod1File, targetFile, fs::copy_options::overwrite_existing);
                QString msg = QString("Unresolvable conflict, using file: %1 prioritized mod: %2")
                                  .arg(Common::PathToU8(mod1File.filename()), mod1Name);
                Log(msg, Format::Yellow);
            }
        } else if (currentPriority == ModPriority::Mod2) {
            if (fs::exists(mod2File)) {
                fs::copy_file(mod2File, targetFile, fs::copy_options::overwrite_existing);
                QString msg = QString("Unresolvable conflict, using file: %1 prioritized mod: %2")
                                  .arg(Common::PathToU8(mod2File.filename()), mod2Name);
                Log(msg, Format::Yellow);
            }
        }
    } catch (const std::exception& e) {
        Log("Filesystem error copying files: " + QString(e.what()), Format::BoldRed);
        return false;
    }

    return true;
}

void ModMerger::OpenPriorityDialog() {
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Handle data conflict");
    dialog->setMinimumWidth(500);

    QLabel* label = new QLabel(
        "The same information is modified for both mods. You can force the merge attempt to "
        "continue by choosing which mod files to prioritize, but the final merged "
        "mod will likely not work correctly when there are unresolvable conflicts",
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
    QString style;
    if (format == Format::BoldRed) {
        style = "color: red; font-weight: bold;";
    } else if (format == Format::BoldGreen) {
        style = "color: green; font-weight: bold;";
    } else if (format == Format::Yellow) {
        style = "color: yellow;";
    } else {
        style = "color: white;";
    }

    return QString("<span style=\"%1\">%2</span>").arg(style, input);
}

void ModMerger::Log(QString msg, Format format) {
    msg = FormatTextForBrowser(msg, format);

    // ensures thread-safe logging from merge thread
    emit LogRequested(msg);
}

void ModMerger::Log(QString msg, int logFormat) {
    Format format = static_cast<Format>(logFormat);

    Log(msg, format);
}

ModMerger::ModPriority ModMerger::GetModPriority() {
    return currentPriority;
}

std::string ModMerger::Mod1Name() {
    return mod1Name;
}

std::string ModMerger::Mod2Name() {
    return mod2Name;
}

ModMerger::~ModMerger() {
    if (activeMerge.isRunning()) {
        activeMerge.cancel();
        activeMerge.waitForFinished();
    }

    delete ui;
}
