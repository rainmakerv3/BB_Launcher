// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <QMessageBox>
#include <QProgressBar>
#include "ModManager.h"
#include "modules/ui_ModManager.h"

std::filesystem::path ModInstallPath;

ModManager::ModManager(QWidget* parent) : QDialog(parent), ui(new Ui::ModManager) {
    ui->setupUi(this);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);

    ui->RecModsLabel->setText("<a "
                              "href=\"https://docs.google.com/document/d/"
                              "1H5d-RrOE6q3lKTupkZxZRFKP2Q7zoWMTCL0hsPmYuAo\">Click here for a "
                              "list of cool mods to try out!</a>");
    ui->RecModsLabel->setTextFormat(Qt::RichText);
    ui->RecModsLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->RecModsLabel->setOpenExternalLinks(true);

    if (!std::filesystem::exists(Common::ModPath)) {
        std::filesystem::create_directories(Common::ModPath);
    }

    if (!std::filesystem::exists(ModBackupPath)) {
        std::filesystem::create_directories(ModBackupPath);
    }

    if (!std::filesystem::exists(ModUniquePath)) {
        std::filesystem::create_directories(ModUniquePath);
    }

    RefreshLists();

    connect(ui->ActivateButton, &QPushButton::pressed, this, &ModManager::ActivateButton_isPressed);
    connect(ui->DeactivateButton, &QPushButton::pressed, this,
            &ModManager::DeactivateButton_isPressed);
    connect(this, &ModManager::progressChanged, ui->progressBar, &QProgressBar::setValue);

    if (std::filesystem::exists(Common::installPath.parent_path() /
                                (Common::game_serial + "-UPDATE"))) {
        ModInstallPath = Common::installPath.parent_path() / (Common::game_serial + "-UPDATE");
    } else {
        ModInstallPath = Common::installPath;
    }
}

void ModManager::ActivateButton_isPressed() {
    if (ui->InactiveModList->selectedItems().size() == 0) {
        QMessageBox::warning(
            this, "No mod selected",
            "Click on a mod in the Inactive Mods List before pressing Activate Mod");
        return;
    }

    bool hasconflict = false;
    const std::string ModName = ui->InactiveModList->currentItem()->text().toStdString();
    const std::string ModFolderString = (Common::ModPath / ModName).string();
    const std::filesystem::path ModFolderPath = std::filesystem::u8path(ModFolderString);
    const std::string ModBackupString = (ModBackupPath / ModName).string();
    const std::filesystem::path ModBackupFolderPath = std::filesystem::u8path(ModBackupString);
    const std::string ModUniqueString = (ModUniquePath / ModName).string();
    const std::filesystem::path ModUniqueFolderPath = std::filesystem::u8path(ModUniqueString);

    std::vector<std::string> FileList;
    std::vector<std::string> ActiveModList;
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
                             " or Blooborne dvdroot_ps4 subfolders (ex. sfx, parts, map)");
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
                if (FileList[i].contains(relative_path_string)) {
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
                if (hasconflict)
                    break;
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

    std::ofstream ModInfoFileSave(Common::ModPath / "ModifiedFiles.txt", std::ios::binary);
    for (const auto& i : FileList)
        ModInfoFileSave << i << "\n";
    ModInfoFileSave.close();

    ui->FileTransferLabel->setText("Backing up original files");
    ui->progressBar->setMaximum(getFileCount(ModSourcePath));

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModSourcePath)) {
        auto relative_path = std::filesystem::relative(entry, ModSourcePath);
        try {
            if (!entry.is_directory()) {
                if (!std::filesystem::exists((ModBackupFolderPath / relative_path.parent_path()))) {
                    std::filesystem::create_directories(
                        (ModBackupFolderPath / relative_path.parent_path()));
                }
                if (!std::filesystem::exists((ModUniqueFolderPath / relative_path.parent_path()))) {
                    std::filesystem::create_directories(
                        (ModUniqueFolderPath / relative_path.parent_path()));
                }
                if (std::filesystem::exists(ModInstallPath / "dvdroot_ps4" / relative_path)) {

                    std::filesystem::copy_file(ModInstallPath / "dvdroot_ps4" / relative_path,
                                               (ModBackupFolderPath / relative_path),
                                               std::filesystem::copy_options::overwrite_existing);
                } else {

                    std::filesystem::copy_file(ModSourcePath / relative_path,
                                               (ModUniqueFolderPath / relative_path),
                                               std::filesystem::copy_options::overwrite_existing);
                }
            }
        } catch (std::exception& ex) {
            QMessageBox::warning(this, "Filesystem error backing up files", ex.what());
            break;
        }
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        emit progressChanged(ui->progressBar->value() + 1);
    }

    ui->progressBar->setValue(0);
    ui->FileTransferLabel->setText("Copying mod files to Bloodborne Folder");

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModSourcePath)) {
        const std::filesystem::path destDir = ModInstallPath / "dvdroot_ps4";
        auto relative_path = std::filesystem::relative(entry, ModSourcePath);
        if (!entry.is_directory()) {
            try {
                if (!std::filesystem::exists((destDir / relative_path).parent_path())) {
                    std::filesystem::create_directories((destDir / relative_path).parent_path());
                }
                std::filesystem::copy_file(ModSourcePath / relative_path, destDir / relative_path,
                                           std::filesystem::copy_options::overwrite_existing);
            } catch (std::exception& ex) {
                QMessageBox::warning(this, "Filesystem error copying mod files", ex.what());
                break;
            }
        }
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        emit progressChanged(ui->progressBar->value() + 1);
    }

    ui->FileTransferLabel->setText("Modified File List is being written");
    ui->progressBar->setValue(0);
    std::ifstream ActiveFile(Common::ModPath / "ActiveMods.txt", std::ios::binary);
    lineCount = 0;
    while (std::getline(ActiveFile, line)) {
        lineCount++;
        ActiveModList.push_back(line);
        ui->progressBar->setValue(ui->progressBar->value() + 1);
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

    QMessageBox::information(this, "Mod Activated",
                             "Successfully activated mod " + QString::fromStdString(ModName),
                             QMessageBox::Ok);
}

void ModManager::DeactivateButton_isPressed() {
    if (ui->ActiveModList->selectedItems().size() == 0) {
        QMessageBox::warning(
            this, "No mod selected",
            "Click on a mod in the Active Mods List before pressing Deactivate Mod");
        return;
    }

    const std::string ModName = ui->ActiveModList->currentItem()->text().toStdString();
    const std::string ModBackupString = (ModBackupPath / ModName).string();
    const std::filesystem::path ModBackupFolderPath = std::filesystem::u8path(ModBackupString);
    const std::string ModUniqueString = (ModUniquePath / ModName).string();
    const std::filesystem::path ModUniqueFolderPath = std::filesystem::u8path(ModUniqueString);

    std::vector<std::string> ConflictMods;
    std::string line;
    int lineCount = 0;

    if (!std::filesystem::exists(ModBackupFolderPath)) {
        QMessageBox::warning(this,
                             "Unable to find Mod Backup " + QString::fromStdString(ModBackupString),
                             "Unable to find Backup Folder, it may have been renamed or deleted.");
        ActiveModRemove(ModName);
        RefreshLists();
        return;
    }

    std::ifstream ConflictFile(Common::ModPath / "ConflictMods.txt", std::ios::binary);
    while (std::getline(ConflictFile, line)) {
        lineCount++;
        ConflictMods.push_back(line);
    }
    ConflictFile.close();

    if (ConflictMods.size() != 0 && ConflictMods.back() != ModName) {
        QMessageBox::warning(this, "Most recent conflicting mod must be uninstalled first",
                             "The last installed conflicting mod must be uninstalled before "
                             "any others.\n\nLast conflicting mod is " +
                                 QString::fromStdString(ConflictMods.back()));
        return;
    } else if (ConflictMods.size() != 0 && ConflictMods.back() == ModName) {
        ConflictRemove(ModName);
    }

    ui->FileTransferLabel->setText("Deleting Mod Files");

    if (std::filesystem::exists(ModUniqueFolderPath)) {
        ui->progressBar->setMaximum(getFileCount(ModBackupFolderPath) +
                                    getFileCount(ModUniqueFolderPath));
    } else {
        ui->progressBar->setMaximum(getFileCount(ModBackupFolderPath));
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModBackupFolderPath)) {
        auto relative_path = std::filesystem::relative(entry, ModBackupFolderPath);
        if (!entry.is_directory()) {
            try {
                if (std::filesystem::exists(ModInstallPath / "dvdroot_ps4" / relative_path)) {
                    std::filesystem::remove(ModInstallPath / "dvdroot_ps4" / relative_path);
                }
            } catch (std::exception& ex) {
                QMessageBox::warning(this, "Filesystem error deleting mod files", ex.what());
                break;
            }
        }
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        emit progressChanged(ui->progressBar->value() + 1);
    }

    if (std::filesystem::exists(ModUniqueFolderPath)) {
        for (const auto& entry :
             std::filesystem::recursive_directory_iterator(ModUniqueFolderPath)) {
            auto relative_path = std::filesystem::relative(entry, ModUniqueFolderPath);
            if (!entry.is_directory()) {
                try {
                    if (std::filesystem::exists(ModInstallPath / "dvdroot_ps4" / relative_path)) {
                        std::filesystem::remove(ModInstallPath / "dvdroot_ps4" / relative_path);
                    }
                } catch (std::exception& ex) {
                    QMessageBox::warning(this, "Filesystem error deleting mod files", ex.what());
                    break;
                }
            }
            ui->progressBar->setValue(ui->progressBar->value() + 1);
            emit progressChanged(ui->progressBar->value() + 1);
        }
    }

    std::filesystem::path dir_path;
    std::vector<std::filesystem::path> directories;
    for (auto& p : std::filesystem::recursive_directory_iterator(ModInstallPath / "dvdroot_ps4")) {
        dir_path = p.path();
        if (std::filesystem::is_directory(p)) {
            directories.push_back(std::filesystem::canonical(dir_path));
        }
    }

    for (std::vector<std::filesystem::path>::reverse_iterator rit = directories.rbegin();
         rit != directories.rend(); ++rit) {
        if (std::filesystem::is_empty(*rit)) {
            std::filesystem::remove(*rit);
        }
    }

    ui->progressBar->setValue(0);
    ui->FileTransferLabel->setText("Moving backup to Install Folder");
    ui->progressBar->setMaximum(getFileCount(ModBackupFolderPath));

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModBackupFolderPath)) {
        auto relative_path = std::filesystem::relative(entry, ModBackupFolderPath);
        if (!entry.is_directory()) {
            try {
                if (!std::filesystem::exists(ModInstallPath / "dvdroot_ps4" /
                                             relative_path.parent_path())) {
                    std::filesystem::create_directories(ModInstallPath / "dvdroot_ps4" /
                                                        relative_path.parent_path());
                }
                std::filesystem::rename(ModBackupFolderPath / relative_path,
                                        ModInstallPath / "dvdroot_ps4" / relative_path);
            } catch (std::exception& ex) {
                QMessageBox::critical(this, "Filesystem error reverting backup", ex.what());
                break;
            }
        }
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        emit progressChanged(ui->progressBar->value() + 1);
    }

    std::filesystem::remove_all(ModBackupFolderPath);
    std::filesystem::remove_all(ModUniqueFolderPath);

    ActiveModRemove(ModName);
    RefreshLists();
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
    ui->FileTransferLabel->setText("No Current File Transfers");

    QMessageBox::information(this, "Mod Deactivated",
                             "Successfully deactivated mod " + QString::fromStdString(ModName),
                             QMessageBox::Ok);
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
    auto dirIter = std::filesystem::recursive_directory_iterator(Path);
    int fileCount = 0;

    for (auto& entry : dirIter) {
        if (!entry.is_directory()) {
            ++fileCount;
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
    std::vector<std::string> FileList;
    std::vector<std::string> ActiveModList;
    std::string line;
    int lineCount = 0;

    std::ifstream ActiveFile(Common::ModPath / "ActiveMods.txt", std::ios::binary);
    while (std::getline(ActiveFile, line)) {
        lineCount++;
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

    std::filesystem::remove(Common::ModPath / "ModifiedFiles.txt");

    for (auto& FolderEntry : std::filesystem::directory_iterator(ModBackupPath)) {
        if (FolderEntry.is_directory()) {
            std::string Foldername = Common::PathToU8(FolderEntry.path().filename());
            const std::string BackupModPathString = (ModBackupPath / Foldername).string();
            const std::filesystem::path BackupModPath =
                std::filesystem::u8path(BackupModPathString);

            if (Foldername != "Mods-UNIQUEFILES") {
                ui->FileTransferLabel->setText("Generating Modified File list for " +
                                               QString::fromStdString(Foldername));
                ui->progressBar->setMaximum(getFileCount(BackupModPath));
                for (const auto& entry :
                     std::filesystem::recursive_directory_iterator(BackupModPath)) {
                    if (!entry.is_directory()) {
                        auto relative_path = std::filesystem::relative(entry, BackupModPath);
                        const auto u8_string = Common::PathToU8(relative_path) + ", " + Foldername;
                        std::string relative_path_string{u8_string.begin(), u8_string.end()};
                        FileList.push_back(relative_path_string);
                    }
                    ui->progressBar->setValue(ui->progressBar->value() + 1);
                    emit progressChanged(ui->progressBar->value() + 1);
                }
            }
        }
    }

    ui->progressBar->setValue(0);
    for (auto& FolderEntry : std::filesystem::directory_iterator(ModUniquePath)) {
        if (FolderEntry.is_directory()) {
            std::string Foldername = Common::PathToU8(FolderEntry.path().filename());
            const std::string UniqueModPathString = (ModUniquePath / Foldername).string();
            const std::filesystem::path UniqueModPath =
                std::filesystem::u8path(UniqueModPathString);

            ui->FileTransferLabel->setText("Generating Unique File list for " +
                                           QString::fromStdString(Foldername));
            ui->progressBar->setMaximum(getFileCount(UniqueModPath));
            for (const auto& entry : std::filesystem::recursive_directory_iterator(UniqueModPath)) {
                if (!entry.is_directory()) {
                    auto relative_path = std::filesystem::relative(entry, UniqueModPath);
                    const auto u8_string = Common::PathToU8(relative_path) + ", " + Foldername;
                    std::string relative_path_string{u8_string.begin(), u8_string.end()};
                    FileList.push_back(relative_path_string);
                }
                ui->progressBar->setValue(ui->progressBar->value() + 1);
                emit progressChanged(ui->progressBar->value() + 1);
            }
        }
    }

    ui->progressBar->setValue(0);
    std::ofstream ModInfoFileSave(Common::ModPath / "ModifiedFiles.txt", std::ios::binary);
    for (const auto& i : FileList)
        ModInfoFileSave << i << "\n";
    ModInfoFileSave.close();
}

ModManager::~ModManager() {
    delete ui;
}
