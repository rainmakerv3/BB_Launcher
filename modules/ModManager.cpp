// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <QMessageBox>
#include <QProgressBar>

#include "ModDownloader.h"
#include "ModManager.h"
#include "modules/ui_ModManager.h"
#include "settings/config.h"

ModManager::ModManager(QWidget* parent) : QDialog(parent), ui(new Ui::ModManager) {
    ui->setupUi(this);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    this->setFixedSize(this->width(), this->height());

    if (Config::theme == "Dark") {
        ui->ActiveModList->setStyleSheet(
            "QListView::item { background-color: #000000; }"              // Color for odd rows
            "QListView::item:alternate { background-color: #242424; }"    // Color for even rows
            "QListWidget::item:selected { background-color: #AAB7DF; }"); // Color for selected row

        ui->InactiveModList->setStyleSheet(
            "QListView::item { background-color: #000000; }"
            "QListView::item:alternate { background-color: #242424; }"
            "QListWidget::item:selected { background-color: #AAB7DF; }");
    } else {
        ui->ActiveModList->setStyleSheet(
            "QListView::item { background-color: #ECECEC; }"
            "QListView::item:alternate { background-color: #D3D3D3; }"
            "QListWidget::item:selected { background-color: #AAB7DF; }");

        ui->InactiveModList->setStyleSheet(
            "QListView::item { background-color: #ECECEC; }"
            "QListView::item:alternate { background-color: #D3D3D3; }"
            "QListWidget::item:selected { background-color: #AAB7DF; }");
    }

    ui->ModHelpLabel->setText("<a "
                              "href=\"https://docs.google.com/document/d/"
                              "19ofjr6k4qqm9l9MJFrbDHEoNVyGXlK_8o95rI3xEh2k\">Click here for help "
                              "installing mods (especially BB Enhanced, Remaster, etc)</a>");
    ui->ModHelpLabel->setTextFormat(Qt::RichText);
    ui->ModHelpLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->ModHelpLabel->setOpenExternalLinks(true);

    RefreshLists();

    connect(ui->ResetButton, &QPushButton::pressed, this, &ModManager::ResetInstallation);
    connect(ui->ActivateButton, &QPushButton::pressed, this, &ModManager::ActivateButton_isPressed);
    connect(ui->DeactivateButton, &QPushButton::pressed, this,
            &ModManager::DeactivateButton_isPressed);
    connect(this, &ModManager::progressChanged, ui->progressBar, &QProgressBar::setValue);

    ModInstallPath = Common::installPath;
    ModInstallPath += "-mods";

    ModBackupPath = ModInstallPath.parent_path() / (Common::game_serial + "-modsBACKUP");
    ModUniquePath = ModBackupPath / "Mods-UNIQUEFILES";

    if (!std::filesystem::exists(ModInstallPath / "dvdroot_ps4"))
        std::filesystem::create_directories(ModInstallPath / "dvdroot_ps4");

    if (!std::filesystem::exists(Common::ModPath)) {
        std::filesystem::create_directories(Common::ModPath);
    }

    if (!std::filesystem::exists(ModBackupPath)) {
        std::filesystem::create_directories(ModBackupPath);
    }

    if (!std::filesystem::exists(ModUniquePath)) {
        std::filesystem::create_directories(ModUniquePath);
    }
}

void ModManager::ActivateButton_isPressed() {
    if (ui->InactiveModList->selectedItems().size() == 0) {
        QMessageBox::warning(
            this, "No mod selected",
            "Click on a mod in the Inactive Mods List before pressing Activate Mod");
        return;
    }

    bool haserror = false;
    bool hasconflict = false;
    const std::string ModName = ui->InactiveModList->currentItem()->text().toStdString();

#ifdef _WIN32
    const std::wstring ModString = ui->InactiveModList->currentItem()->text().toStdWString();
#else
    const std::string ModString = ModName;
#endif

    const std::filesystem::path ModFolderPath = Common::ModPath / ModString;
    const std::filesystem::path ModBackupFolderPath = ModBackupPath / ModString;
    const std::filesystem::path ModUniqueFolderPath = ModUniquePath / ModString;
    const std::filesystem::path ModActiveFolderPath = ModActivePath / ModString;

    std::vector<std::string> FileList;
    std::string line;
    int lineCount = 0;

    const bool has_dvdroot = std::filesystem::exists(ModFolderPath / "dvdroot_ps4");
    std::filesystem::path ModSourcePath;
    if (!has_dvdroot) {
        ModSourcePath = ModFolderPath;
    } else {
        ModSourcePath = ModFolderPath / "dvdroot_ps4";
    }

    bool HasBBFolders = false;
    for (const auto& entry : std::filesystem::directory_iterator(ModSourcePath)) {
        if (entry.is_directory()) {
            auto relative_path = std::filesystem::relative(entry, ModSourcePath);
            std::string relative_path_string = Common::PathToU8(relative_path);
            if (std::find(BBFolders.begin(), BBFolders.end(), relative_path_string) !=
                BBFolders.end()) {
                HasBBFolders = true;
                break;
            }
        }
    }

    if (!HasBBFolders) {
        QMessageBox::warning(this, "Invalid Mod",
                             "Folders inside mod folder must include either dvdroot_ps4"
                             " or Bloodborne dvdroot_ps4 subfolders (ex. sfx, parts, map)");
        return;
    }

    std::ifstream ModInfoFile(Common::ModPath / "ModifiedFiles.txt", std::ios::binary);
    while (std::getline(ModInfoFile, line)) {
        lineCount++;
        FileList.push_back(line);
    }
    ModInfoFile.close();

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModSourcePath)) {
        if (!entry.is_directory()) {
            auto relative_path = std::filesystem::relative(entry, ModSourcePath);
            std::string relative_path_string = Common::PathToU8(relative_path);
            for (int i = 0; i < FileList.size(); i++) {
                if (hasconflict)
                    break;

                std::size_t filelist_comma_pos = FileList[i].find(',');
                std::string filelist_relative_string = FileList[i].substr(0, filelist_comma_pos);
                if (filelist_relative_string == relative_path_string) {
                    hasconflict = true;
                    if (QMessageBox::Yes ==
                        QMessageBox::question(
                            this, "Mod conflict found",
                            QString::fromStdString(FileList[i]) +
                                "\n\nThis file conflicts with the same file in this mod."
                                " Some conflicting mods cannot function properly "
                                "together.\n\nProceed with activation?",
                            QMessageBox::Yes | QMessageBox::No)) {
                        break;
                    } else {
                        return;
                    }
                }
            }
        }
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModSourcePath)) {
        if (!entry.is_directory()) {
            auto relative_path = std::filesystem::relative(entry, ModSourcePath);
            const auto u8_string = Common::PathToU8(relative_path) + ", " + ModName;
            std::string relative_path_string{u8_string.begin(), u8_string.end()};
            FileList.push_back(relative_path_string);
        }
    }

    try {
        if (std::filesystem::exists(ModActiveFolderPath))
            std::filesystem::remove_all(ModActiveFolderPath);
        if (!std::filesystem::exists(ModActivePath))
            std::filesystem::create_directories(ModActivePath);

        std::filesystem::rename(ModSourcePath, ModActiveFolderPath);
        std::filesystem::remove_all(ModFolderPath);
    } catch (std::exception& ex) {
        QMessageBox::warning(
            this, "Filesystem error",
            "Error accessing folders, make sure they are not open or in use.\n\nError message: " +
                QString::fromStdString(ex.what()));
        return;
    }

    std::ofstream ModInfoFileSave(Common::ModPath / "ModifiedFiles.txt", std::ios::binary);
    for (const auto& i : FileList)
        ModInfoFileSave << i << "\n";
    ModInfoFileSave.close();

#if defined FORCE_UAC or !defined _WIN32
    ui->FileTransferLabel->setText("Backing up original files, symlinking to BB folder");
#else
    ui->FileTransferLabel->setText("Backing up original files, copying to BB folder");
#endif

    ui->progressBar->setMaximum(getFileCount(ModActiveFolderPath));

    std::vector<std::string> UniqueList;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModActiveFolderPath)) {
        auto relative_path = std::filesystem::relative(entry, ModActiveFolderPath);
        try {
            if (!entry.is_directory()) {
                if (!std::filesystem::exists(ModBackupFolderPath / relative_path.parent_path())) {
                    std::filesystem::create_directories(ModBackupFolderPath /
                                                        relative_path.parent_path());
                }

                if (std::filesystem::exists(ModInstallPath / "dvdroot_ps4" / relative_path) ||
                    std::filesystem::is_symlink(ModInstallPath / "dvdroot_ps4" / relative_path)) {
                    std::filesystem::rename(ModInstallPath / "dvdroot_ps4" / relative_path,
                                            ModBackupFolderPath / relative_path);
                } else {
                    UniqueList.push_back(Common::PathToU8(relative_path) + ", " + ModName);
                }

                if (!std::filesystem::exists(ModInstallPath / "dvdroot_ps4" /
                                             relative_path.parent_path())) {
                    std::filesystem::create_directories(ModInstallPath / "dvdroot_ps4" /
                                                        relative_path.parent_path());
                }
#if defined FORCE_UAC or !defined _WIN32
                std::filesystem::create_symlink(ModActiveFolderPath / relative_path,
                                                ModInstallPath / "dvdroot_ps4" / relative_path);
#else
                std::filesystem::copy_file(ModActiveFolderPath / relative_path,
                                           ModInstallPath / "dvdroot_ps4" / relative_path,
                                           std::filesystem::copy_options::overwrite_existing);
#endif
                emit progressChanged(ui->progressBar->value() + 1);
            }
        } catch (std::exception& ex) {
            QMessageBox::warning(this, "Filesystem error backing up files", ex.what());
            haserror = true;
            break;
        }
    }

    if (!std::filesystem::exists(ModUniqueFolderPath))
        std::filesystem::create_directories(ModUniqueFolderPath);

#ifdef _WIN32
    std::wstring FileString = ModString + L".txt";
#else
    std::string FileString = ModString + ".txt";
#endif
    const std::filesystem::path IndexPath = ModUniquePath / ModString / FileString;
    std::ofstream UniqueIndexFile(IndexPath, std::ios::binary);

    for (const std::string& f : UniqueList)
        UniqueIndexFile << f << "\n";
    UniqueIndexFile.close();

    ui->FileTransferLabel->setText("Modified File List is being written");
    ui->progressBar->setValue(0);

    std::vector<std::string> ActiveModList;
    std::ifstream ActiveFile(Common::ModPath / "ActiveMods.txt", std::ios::binary);
    lineCount = 0;
    while (std::getline(ActiveFile, line)) {
        lineCount++;
        ActiveModList.push_back(line);
        emit progressChanged(ui->progressBar->value() + 1);
    }
    ActiveFile.close();
    ActiveModList.push_back(ModName);

    std::ofstream ActiveFileSave(Common::ModPath / "ActiveMods.txt", std::ios::binary);
    for (const auto& l : ActiveModList)
        ActiveFileSave << l << "\n";
    ActiveFileSave.close();

    if (hasconflict)
        ConflictAdd(ModName);

    RefreshLists();
    ui->progressBar->setValue(0);
    ui->FileTransferLabel->setText("No Current File Transfers");

    if (haserror) {
        QMessageBox::information(this, "Error Activating Mod",
                                 "An error occurred activating mod " +
                                     QString::fromStdString(ModName) +
                                     ". Bloodbone may no longer function correctly. Resetting "
                                     "installation is recommendeded",
                                 QMessageBox::Ok);
    } else {
        QMessageBox::information(this, "Mod Activated",
                                 "Successfully activated mod " + QString::fromStdString(ModName),
                                 QMessageBox::Ok);
    }
}

void ModManager::DeactivateButton_isPressed() {
    if (ui->ActiveModList->selectedItems().size() == 0) {
        QMessageBox::warning(
            this, "No mod selected",
            "Click on a mod in the Active Mods List before pressing Deactivate Mod");
        return;
    }

    const std::string ModName = ui->ActiveModList->currentItem()->text().toStdString();
#ifdef _WIN32
    const std::wstring ModString = ui->ActiveModList->currentItem()->text().toStdWString();
#else
    const std::string ModString = ModName;
#endif

    const std::filesystem::path ModFolderPath = Common::ModPath / ModString;
    const std::filesystem::path ModBackupFolderPath = ModBackupPath / ModString;
    const std::filesystem::path ModUniqueFolderPath = ModUniquePath / ModString;
    const std::filesystem::path ModActiveFolderPath = ModActivePath / ModString;

    std::vector<std::string> ConflictMods;
    std::string line;
    int lineCount = 0;

    if (!std::filesystem::exists(ModBackupFolderPath)) {
        QMessageBox::warning(this,
                             "Unable to find Mod Backup " +
                                 QString::fromStdString(ModBackupFolderPath.string()),
                             "Unable to find Backup Folder, it may have been renamed or deleted.");
        ActiveModRemove(ModName);
        if (std::filesystem::exists(ModActiveFolderPath)) {
            if (std::filesystem::exists(ModFolderPath))
                std::filesystem::remove_all(ModFolderPath);
            std::filesystem::rename(ModActiveFolderPath, ModFolderPath);
        }
        RefreshLists();
        QMessageBox::information(this, "Error Deactivating Mod",
                                 "An error occurred deactivating mod " +
                                     QString::fromStdString(ModName) +
                                     ". Bloodborne might not function correctly due to the error, "
                                     "resetting installation is recommended.",
                                 QMessageBox::Ok);

        return;
    }

    std::vector<std::string> FileList;
    std::ifstream ModInfoFile(Common::ModPath / "ModifiedFiles.txt", std::ios::binary);
    while (std::getline(ModInfoFile, line)) {
        lineCount++;
        FileList.push_back(line);
    }
    ModInfoFile.close();

    bool hasconflict = false;
    int conflictfilecount;

    // First check if any files in the backup folder have conflicts
    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModBackupFolderPath)) {
        if (!entry.is_directory()) {
            auto relative_path = std::filesystem::relative(entry, ModBackupFolderPath);

            // Relative path seemingly not working on symlinks
            if (std::filesystem::is_symlink(entry.symlink_status())) {
#ifdef _WIN32
                std::wstring pathString = entry.path().wstring();
                std::wstring baseString = ModBackupFolderPath.wstring();
#else
                std::string pathString = entry.path().string();
                std::string baseString = ModBackupFolderPath.string();
#endif
                size_t pos = std::string::npos;
                while ((pos = pathString.find(baseString)) != std::string::npos) {
                    pathString.erase(pos, baseString.length());
                }
                pathString.erase(0, 1);
                relative_path = pathString;
            }

            std::string relative_path_string = Common::PathToU8(relative_path);
            conflictfilecount = 0;
            for (int i = 0; i < FileList.size(); i++) {
                std::size_t filelist_comma_pos = FileList[i].find(',');
                std::string filelist_relative_string = FileList[i].substr(0, filelist_comma_pos);
                if (filelist_relative_string == relative_path_string)
                    conflictfilecount = conflictfilecount + 1;
                if (conflictfilecount > 1) {
                    hasconflict = true;
                    break;
                }
            }
            if (hasconflict)
                break;
        }
    }

#ifdef _WIN32
    std::wstring FileString = ModString + L".txt";
#else
    std::string FileString = ModString + ".txt";
#endif

    const std::filesystem::path IndexPath = ModUniquePath / ModString / FileString;
    std::ifstream UniqueIndexFile(IndexPath, std::ios::binary);
    std::vector<std::string> UniqueList;
    int UniqueLineCount = 0;

    // Then check if any file in the unique index has conflicts
    if (std::filesystem::exists(IndexPath)) {

        while (std::getline(UniqueIndexFile, line)) {
            UniqueLineCount++;
            UniqueList.push_back(line);
        }
        UniqueIndexFile.close();

        for (const std::string& fileline : UniqueList) {
            conflictfilecount = 0;
            std::size_t comma_pos = fileline.find(',');
            std::string relative_string = fileline.substr(0, comma_pos);
            for (int i = 0; i < FileList.size(); i++) {
                std::size_t filelist_comma_pos = FileList[i].find(',');
                std::string filelist_relative_string = FileList[i].substr(0, filelist_comma_pos);

                if (filelist_relative_string == relative_string) {
                    conflictfilecount = conflictfilecount + 1;
                }

                if (conflictfilecount > 1) {
                    hasconflict = true;
                    break;
                }
            }
            if (hasconflict)
                break;
        }
    }

    std::ifstream ConflictFile(Common::ModPath / "ConflictMods.txt", std::ios::binary);
    while (std::getline(ConflictFile, line)) {
        lineCount++;
        ConflictMods.push_back(line);
    }
    ConflictFile.close();

    if (ConflictMods.size() != 0 && ConflictMods.back() != ModName) {
        if (hasconflict) {
            QMessageBox::warning(this, "Most recent conflicting mod must be uninstalled first",
                                 "The last installed conflicting mod must be uninstalled before "
                                 "any others.\n\nLast conflicting mod is " +
                                     QString::fromStdString(ConflictMods.back()));
            ui->progressBar->setValue(0);
            ui->FileTransferLabel->setText("No Current File Transfers");
            return;
        }
    } else if (ConflictMods.size() != 0 && ConflictMods.back() == ModName) {
        ConflictRemove(ModName);
    }

    ui->FileTransferLabel->setText("Deleting Mod Files");

    bool haserror = false;
    if (std::filesystem::exists(ModUniqueFolderPath)) {
        if (std::filesystem::exists(IndexPath)) {
            ui->progressBar->setMaximum(UniqueLineCount);
            for (std::string fileline : UniqueList) {

                std::size_t comma_pos = fileline.find(',');
                fileline = fileline.substr(0, comma_pos);

#ifdef _WIN32
                auto basePath = ModInstallPath / "dvdroot_ps4";
                std::string npath = Common::PathToU8(basePath) + "/" + fileline;
                std::filesystem::path path = Common::Utf8ToUtf16(npath);
#else
                std::filesystem::path path = ModInstallPath / "dvdroot_ps4" / fileline;
#endif

                try {
                    if (std::filesystem::exists(path))
                        std::filesystem::remove(path);
                } catch (std::exception& ex) {
                    QMessageBox::warning(this, "Filesystem error deleting mod files", ex.what());
                    haserror = true;
                    break;
                }
            }
        }
    }

    ui->progressBar->setValue(0);
    ui->FileTransferLabel->setText("Moving backup to Install Folder");
    ui->progressBar->setMaximum(getFileCount(ModBackupFolderPath));

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModBackupFolderPath)) {
        auto relative_path = std::filesystem::relative(entry, ModBackupFolderPath);

        // Relative path seemingly not working on symlinks
        if (std::filesystem::is_symlink(entry.symlink_status())) {
#ifdef _WIN32
            std::wstring pathString = entry.path().wstring();
            std::wstring baseString = ModBackupFolderPath.wstring();
#else
            std::string pathString = entry.path().string();
            std::string baseString = ModBackupFolderPath.string();
#endif
            size_t pos = std::string::npos;
            while ((pos = pathString.find(baseString)) != std::string::npos) {
                pathString.erase(pos, baseString.length());
            }
            pathString.erase(0, 1);
            relative_path = pathString;
        }

        if (!std::filesystem::is_directory(ModBackupFolderPath / relative_path)) {
            try {
                if (std::filesystem::exists(ModInstallPath / "dvdroot_ps4" / relative_path))
                    std::filesystem::remove(ModInstallPath / "dvdroot_ps4" / relative_path);

                if (!std::filesystem::exists(
                        (ModInstallPath / "dvdroot_ps4" / relative_path).parent_path())) {
                    std::filesystem::create_directories(
                        (ModInstallPath / "dvdroot_ps4" / relative_path).parent_path());
                }

                std::filesystem::rename(ModBackupFolderPath / relative_path,
                                        ModInstallPath / "dvdroot_ps4" / relative_path);

            } catch (std::exception& ex) {
                QMessageBox::critical(this, "Filesystem error reverting backup", ex.what());
                haserror = true;
                break;
            }
            emit progressChanged(ui->progressBar->value() + 1);
        }
    }

    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
    ui->FileTransferLabel->setText("No Current File Transfers");

    std::filesystem::path dir_path;
    std::vector<std::filesystem::path> directories;
    for (auto& p : std::filesystem::recursive_directory_iterator(ModInstallPath / "dvdroot_ps4")) {
        dir_path = p.path();
        if (std::filesystem::is_directory(p)) {
            directories.push_back(std::filesystem::canonical(dir_path));
        }
    }

    try {
        for (std::vector<std::filesystem::path>::reverse_iterator rit = directories.rbegin();
             rit != directories.rend(); ++rit) {
            if (std::filesystem::is_empty(*rit)) {
                std::filesystem::remove(*rit);
            }
        }

        if (std::filesystem::exists(ModBackupFolderPath))
            std::filesystem::remove_all(ModBackupFolderPath);
        if (std::filesystem::exists(ModUniqueFolderPath))
            std::filesystem::remove_all(ModUniqueFolderPath);
    } catch (std::exception& ex) {
        QMessageBox::warning(this, "Filesystem error",
                             "Error message: " + QString::fromStdString(ex.what()));
        haserror = true;
        return;
    }

    try {
        if (std::filesystem::exists(ModActiveFolderPath)) {
            if (std::filesystem::exists(ModFolderPath))
                std::filesystem::remove_all(ModFolderPath);
            std::filesystem::rename(ModActiveFolderPath, ModFolderPath);
        }
    } catch (std::exception& ex) {
        QMessageBox::warning(this, "Filesystem error",
                             "Mod deactivated successfully but mod folder could not be moved back "
                             "to the mods folder\n\nError message: " +
                                 QString::fromStdString(ex.what()));
    }

    ActiveModRemove(ModName);
    RefreshLists();

    if (haserror) {
        QMessageBox::information(this, "Error Deactivating Mod",
                                 "An error occurred deactivating mod " +
                                     QString::fromStdString(ModName) +
                                     ". Bloodborne might not function correctly due to the error, "
                                     "resetting installation is recommended.",
                                 QMessageBox::Ok);
    } else {
        QMessageBox::information(this, "Mod Deactivated",
                                 "Successfully deactivated mod " + QString::fromStdString(ModName),
                                 QMessageBox::Ok);
    }
}

void ModManager::RefreshLists() {
    std::vector<std::string> ActiveModList;
    std::string line;
    int lineCount = 0;
    std::ifstream ActiveFile(Common::ModPath / "ActiveMods.txt", std::ios::binary);

    ui->ActiveModList->clear();
    ui->InactiveModList->clear();

    while (std::getline(ActiveFile, line)) {
        lineCount++;
        ActiveModList.push_back(line);
    }
    ActiveFile.close();

    QStringList ActiveModStringList;
    for_each(ActiveModList.begin(), ActiveModList.end(),
             [&](std::string s) { ActiveModStringList.append(QString::fromStdString(s)); });
    ui->ActiveModList->addItems(ActiveModStringList);

    std::vector<std::string> InactiveModFolders;
    for (auto& FolderEntry : std::filesystem::directory_iterator(Common::ModPath)) {
        if (FolderEntry.is_directory()) {
            std::string Foldername = Common::PathToU8(FolderEntry.path().filename());
            if (std::find(ActiveModList.begin(), ActiveModList.end(), Foldername) ==
                ActiveModList.end()) {
                InactiveModFolders.push_back(Foldername);
            }
        }
    }

    QStringList InactiveModStringList;
    for_each(InactiveModFolders.begin(), InactiveModFolders.end(),
             [&](std::string s) { InactiveModStringList.append(QString::fromStdString(s)); });
    ui->InactiveModList->addItems(InactiveModStringList);
}

int ModManager::getFileCount(std::filesystem::path Path) {
    int fileCount = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(Path)) {
        if (!entry.is_directory()) {
            fileCount = fileCount + 1;
        }
    }
    return fileCount;
}

void ModManager::ConflictAdd(std::string ModName) {
    std::string line;
    int lineCount = 0;
    std::vector<std::string> ConflictMods;

    std::ifstream ConflictFile(Common::ModPath / "ConflictMods.txt", std::ios::binary);
    while (std::getline(ConflictFile, line)) {
        lineCount++;
        ConflictMods.push_back(line);
    }
    ConflictFile.close();

    ConflictMods.push_back(ModName);

    std::ofstream ConflictFileSave(Common::ModPath / "ConflictMods.txt", std::ios::binary);
    for (const auto& i : ConflictMods)
        ConflictFileSave << i << "\n";
    ConflictFileSave.close();
}

void ModManager::ConflictRemove(std::string ModName) {
    std::string line;
    int lineCount = 0;
    std::vector<std::string> ConflictMods;

    std::ifstream ConflictFile(Common::ModPath / "ConflictMods.txt", std::ios::binary);
    while (std::getline(ConflictFile, line)) {
        lineCount++;
        ConflictMods.push_back(line);
    }
    ConflictFile.close();

    auto itr = std::find(ConflictMods.begin(), ConflictMods.end(), ModName);
    ConflictMods.erase(itr);

    std::ofstream ConflictFileSave(Common::ModPath / "ConflictMods.txt", std::ios::binary);
    for (const auto& i : ConflictMods)
        ConflictFileSave << i << "\n";
    ConflictFileSave.close();
}

void ModManager::ActiveModRemove(std::string ModName) {
    std::vector<std::string> ActiveModList;
    std::string line;
    int lineCount = 0;

    std::ifstream ActiveFile(Common::ModPath / "ActiveMods.txt", std::ios::binary);
    while (std::getline(ActiveFile, line)) {
        ActiveModList.push_back(line);
    }
    ActiveFile.close();

    auto itr = std::find(ActiveModList.begin(), ActiveModList.end(), ModName);
    if (itr != ActiveModList.end())
        ActiveModList.erase(itr);

    std::ofstream ActiveFileSave(Common::ModPath / "ActiveMods.txt", std::ios::binary);
    for (const auto& l : ActiveModList)
        ActiveFileSave << l << "\n";
    ActiveFileSave.close();

    std::vector<std::string> FileList;
    std::ifstream ModInfoFile(Common::ModPath / "ModifiedFiles.txt", std::ios::binary);
    while (std::getline(ModInfoFile, line)) {
        size_t commaPos = line.find(',');
        std::string lineModName = line.substr(commaPos + 2);
        if (lineModName == ModName) {
            continue;
        }

        FileList.push_back(line);
    }

    std::ofstream ModInfoFileSave(Common::ModPath / "ModifiedFiles.txt", std::ios::binary);
    for (const auto& i : FileList)
        ModInfoFileSave << i << "\n";
    ModInfoFileSave.close();
}

void ModManager::ResetInstallation() {
    if (QMessageBox::No == QMessageBox::question(this, "Reset Installation",
                                                 "This will deactivate all mods (original files "
                                                 "remain untouched).\n\nProceed with reset?",
                                                 QMessageBox::Yes | QMessageBox::No)) {
        return;
    }

    try {
        std::filesystem::remove_all(ModInstallPath);
        std::filesystem::create_directories(ModInstallPath / "dvdroot_ps4");

        std::filesystem::remove_all(ModBackupPath);
        std::filesystem::create_directories(ModUniquePath);

        std::filesystem::remove(Common::ModPath / "ModifiedFiles.txt");
        std::filesystem::remove(Common::ModPath / "ConflictMods.txt");
        std::filesystem::remove(Common::ModPath / "ActiveMods.txt");

        if (std::filesystem::exists(ModActivePath)) {
            for (const auto& entry : std::filesystem::directory_iterator(ModActivePath)) {
                if (entry.is_directory()) {
#ifdef _WIN32
                    const std::wstring ModString = entry.path().filename().wstring();
#else
                    const std::string ModString = entry.path().filename().string();
#endif
                    std::filesystem::rename(entry.path(), Common::ModPath / ModString);
                }
            }
        }
    } catch (std::exception& ex) {
        RefreshLists();
        QMessageBox::warning(this, "Filesystem error",
                             "Error resetting installation. Make sure all game and mod "
                             "folders/files are not open or in use\n\nError message: " +
                                 QString::fromStdString(ex.what()));
        return;
    }

    RefreshLists();
    QMessageBox::information(this, "Reset Complete", "Reset Successfully completed",
                             QMessageBox::Ok);
}

ModManager::~ModManager() {
    delete ui;
}
