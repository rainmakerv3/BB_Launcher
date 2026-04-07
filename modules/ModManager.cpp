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
    connect(ui->ActivateButton, &QPushButton::pressed, this, &ModManager::ActivateMod);
    connect(ui->DeactivateButton, &QPushButton::pressed, this, &ModManager::DeactivateMod);
    connect(this, &ModManager::progressChanged, ui->progressBar, &QProgressBar::setValue);

    ModInstallPath = Common::installPath;
    ModInstallPath += "-mods";
    ModBackupPath = ModInstallPath.parent_path() / (Common::game_serial + "-modsBACKUP");

    if (!std::filesystem::exists(ModInstallPath / "dvdroot_ps4"))
        std::filesystem::create_directories(ModInstallPath / "dvdroot_ps4");

    if (!std::filesystem::exists(Common::ModPath)) {
        std::filesystem::create_directories(Common::ModPath);
    }

    if (!std::filesystem::exists(ModBackupPath)) {
        std::filesystem::create_directories(ModBackupPath);
    }
}

void ModManager::ActivateMod() {
    if (ui->InactiveModList->selectedItems().size() == 0) {
        QMessageBox::warning(
            this, "No mod selected",
            "Click on a mod in the Inactive Mods List before pressing Activate Mod");
        return;
    }

    const std::string ModName = ui->InactiveModList->currentItem()->text().toStdString();

#ifdef _WIN32
    const std::wstring ModString = ui->InactiveModList->currentItem()->text().toStdWString();
#else
    const std::string ModString = ModName;
#endif

    const std::filesystem::path ModFolderPath = Common::ModPath / ModString;
    const std::filesystem::path ModBackupFolderPath = ModBackupPath / ModString;
    const std::filesystem::path ModActiveFolderPath = ModActivePath / ModString;

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

    bool hasconflict = false;
    std::vector<std::string> FileList = GetModifiedFileList(ModName);
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

#if defined FORCE_UAC or !defined _WIN32
    ui->FileTransferLabel->setText("Backing up original files, symlinking to shadPS4 mods folder");
#else
    ui->FileTransferLabel->setText("Backing up original files, copying to shadPS4 mods folder");
#endif

    ui->progressBar->setMaximum(getFileCount(ModActiveFolderPath));

    bool haserror = false;
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

void ModManager::DeactivateMod() {
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
    const std::filesystem::path ModActiveFolderPath = ModActivePath / ModString;

    if (!std::filesystem::exists(ModBackupFolderPath)) {
        QMessageBox::warning(this,
                             "Unable to find Mod Backup " +
                                 QString::fromStdString(ModBackupFolderPath.string()),
                             "Unable to find Backup Folder, it may have been renamed or deleted.");

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

    std::vector<std::string> FileList = GetModifiedFileList(ModName);
    bool hasconflict = false;
    int conflictfilecount;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModActiveFolderPath)) {
        if (!entry.is_directory()) {
            auto relative_path = std::filesystem::relative(entry, ModActiveFolderPath);
            std::string relative_path_string = Common::PathToU8(relative_path);
            for (int i = 0; i < FileList.size(); i++) {
                std::size_t filelist_comma_pos = FileList[i].find(',');
                std::string filelist_relative_string = FileList[i].substr(0, filelist_comma_pos);
                if (filelist_relative_string == relative_path_string) {
                    hasconflict = true;
                    break;
                }
            }
            if (hasconflict)
                break;
        }
    }

    std::vector<std::string> ConflictMods;
    std::string line;
    if (hasconflict) {
        std::ifstream ConflictFile(Common::ModPath / "ConflictMods.txt", std::ios::binary);
        while (std::getline(ConflictFile, line)) {
            ConflictMods.push_back(line);
        }
        ConflictFile.close();

        if (ConflictMods.size() != 0 && ConflictMods.back() != ModName) {
            QMessageBox::warning(this, "Most recent conflicting mod must be uninstalled first",
                                 "The last installed conflicting mod must be uninstalled before "
                                 "any others.\n\nLast conflicting mod is " +
                                     QString::fromStdString(ConflictMods.back()));
            ui->progressBar->setValue(0);
            ui->FileTransferLabel->setText("No Current File Transfers");
            return;
        } else if (ConflictMods.size() != 0 && ConflictMods.back() == ModName) {
            ConflictRemove(ModName);
        }
    }

    bool haserror = false;
    ui->progressBar->setValue(0);
    ui->FileTransferLabel->setText("Removing from shadPS4 mods Folder");
    ui->progressBar->setMaximum(getFileCount(ModActiveFolderPath));

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModActiveFolderPath)) {
        auto relative_path = std::filesystem::relative(entry, ModActiveFolderPath);
        if (!std::filesystem::is_directory(ModBackupFolderPath / relative_path)) {
            try {
                if (!std::filesystem::exists(
                        (ModInstallPath / "dvdroot_ps4" / relative_path).parent_path())) {
                    std::filesystem::create_directories(
                        (ModInstallPath / "dvdroot_ps4" / relative_path).parent_path());
                }

                if (std::filesystem::exists(ModInstallPath / "dvdroot_ps4" / relative_path))
                    std::filesystem::remove(ModInstallPath / "dvdroot_ps4" / relative_path);

            } catch (std::exception& ex) {
                QMessageBox::critical(this, "Filesystem error removing mod files", ex.what());
                haserror = true;
                break;
            }
            emit progressChanged(ui->progressBar->value() + 1);
        }
    }

    ui->FileTransferLabel->setText("Reverting backup");
    ui->progressBar->setValue(0);
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

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);
    ui->FileTransferLabel->setText("No Current File Transfers");

    // Remove Empty Folders
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
                             "to the BBLauncher mods folder\n\nError message: " +
                                 QString::fromStdString(ex.what()));
    }

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
    QStringList ActiveModStringList;
    QStringList InactiveModStringList;

    ui->ActiveModList->clear();
    ui->InactiveModList->clear();

    for (const auto& FolderEntry : std::filesystem::directory_iterator(ModActivePath)) {
        if (FolderEntry.is_directory()) {
            std::string FolderName = Common::PathToU8(FolderEntry.path().filename());
            ActiveModStringList.append(QString::fromStdString(FolderName));
        }
    }
    ui->ActiveModList->addItems(ActiveModStringList);

    for (auto& FolderEntry : std::filesystem::directory_iterator(Common::ModPath)) {
        if (FolderEntry.is_directory()) {
            std::string FolderName = Common::PathToU8(FolderEntry.path().filename());
            InactiveModStringList.append(QString::fromStdString(FolderName));
        }
    }
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
    std::vector<std::string> ConflictMods;

    std::ifstream ConflictFile(Common::ModPath / "ConflictMods.txt", std::ios::binary);
    while (std::getline(ConflictFile, line)) {
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
    std::vector<std::string> ConflictMods;

    std::ifstream ConflictFile(Common::ModPath / "ConflictMods.txt", std::ios::binary);
    while (std::getline(ConflictFile, line)) {
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
        std::filesystem::create_directories(ModBackupPath);

        std::filesystem::remove(Common::ModPath / "ConflictMods.txt");

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

std::vector<std::string> ModManager::GetModifiedFileList(std::string ExcludeMod) {
    std::vector<std::string> vec;
    for (const auto& FolderEntry : std::filesystem::directory_iterator(ModActivePath)) {
        if (FolderEntry.is_directory()) {
            std::string FolderName = Common::PathToU8(FolderEntry.path().filename());

            if (FolderName != ExcludeMod) {
                for (const auto& FileEntry :
                     std::filesystem::recursive_directory_iterator(FolderEntry)) {
                    if (!FileEntry.is_directory()) {
                        auto relative_path = std::filesystem::relative(FileEntry, FolderEntry);
                        const auto u8_string = Common::PathToU8(relative_path) + ", " + FolderName;
                        std::string relative_path_string{u8_string.begin(), u8_string.end()};
                        vec.push_back(relative_path_string);
                    }
                }
            }
        }
    }
    return vec;
}

ModManager::~ModManager() {
    delete ui;
}
