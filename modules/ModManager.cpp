#include <fstream>
#include <QMessageBox>
#include <QProgressBar>
#include "../bblauncher.h"
#include "ModManager.h"
#include "modules/ui_ModManager.h"

ModManager::ModManager(QWidget* parent) : QDialog(parent), ui(new Ui::ModManager) {
    ui->setupUi(this);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);

    RefreshLists();

    connect(ui->ActivateButton, &QPushButton::pressed, this, &ModManager::ActivateButton_isPressed);
    connect(ui->DeactivateButton, &QPushButton::pressed, this,
            &ModManager::DeactivateButton_isPressed);
    connect(this, &ModManager::progressChanged, ui->progressBar, &QProgressBar::setValue);
}

void ModManager::ActivateButton_isPressed() {

    if (ui->InactiveModList->selectedItems().size() == 0) {
        QMessageBox::warning(
            this, "No mod selected",
            "Click on a mod in the Inactive Mods List before pressing Activate Mod");
        return;
    }

    const std::string ModName = ui->InactiveModList->currentItem()->text().toStdString();
    const std::filesystem::path ModFolderPath = ModPath / ModName;
    const std::filesystem::path ModBackupPath =
        std::filesystem::current_path() / "BBLauncher" / "Mods-BACKUP" / ModName;
    const bool has_dvdroot = std::filesystem::exists(ModFolderPath / "dvdroot_ps4");
    std::vector<std::string> FileList;
    std::vector<std::string> ActiveModList;
    std::string line;
    int lineCount = 0;

    std::filesystem::path ModSourcePath;
    if (!has_dvdroot) {
        ModSourcePath = ModFolderPath;
    } else {
        ModSourcePath = ModFolderPath / "dvdroot_ps4";
    }

    for (const auto& entry : std::filesystem::directory_iterator(ModSourcePath)) {
        if (entry.is_directory()) {
            auto relative_path = std::filesystem::relative(entry, ModSourcePath);
            if (std::find(BBFolders.begin(), BBFolders.end(), relative_path.u8string()) ==
                BBFolders.end()) {
                QMessageBox::warning(this, "Invalid Mod",
                                     "Folders inside mod folder must include either dvdroot_ps4 or "
                                     "Blooborne dvdroot_ps4 subfolders (ex. sfx, parts, map)");
                return;
            }
        }
    }

    std::ifstream ModInfoFile(ModPath / "ModifiedFiles.txt", std::ios::binary);
    while (std::getline(ModInfoFile, line)) {
        lineCount++;
        FileList.push_back(line);
    }
    ModInfoFile.close();

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModSourcePath)) {
        if (!entry.is_directory()) {
            auto relative_path = std::filesystem::relative(entry, ModSourcePath);
            if (std::find(FileList.begin(), FileList.end(), relative_path.u8string()) !=
                FileList.end()) {
                if (QMessageBox::Yes ==
                    QMessageBox::question(
                        this, "Mod conflict found",
                        "This mod conflicts with an active mod. Some conflicting mods cannot "
                        "function properly together.\n\nProceed with activation?",
                        QMessageBox::Yes | QMessageBox::No)) {
                    ConflictAdd(ModName);
                    break;
                } else {
                    return;
                }
            }
        }
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModSourcePath)) {
        if (!entry.is_directory()) {
            auto relative_path = std::filesystem::relative(entry, ModSourcePath);
            FileList.push_back(relative_path.u8string());
        }
    }

    std::ofstream ModInfoFileSave(ModPath / "ModifiedFiles.txt", std::ios::binary);
    for (const auto& i : FileList)
        ModInfoFileSave << i << "\n";
    ModInfoFileSave.close();

    ui->FileTransferLabel->setText("Backing up original files");
    ui->progressBar->setMaximum(getFileCount(ModSourcePath));

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModSourcePath)) {

        auto relative_path = std::filesystem::relative(entry, ModSourcePath);
        std::string relative_path_string = relative_path.u8string();
        if (!entry.is_directory()) {
            try {
                if (!std::filesystem::exists(
                        (ModBackupPath / relative_path_string).parent_path())) {
                    std::filesystem::create_directories(
                        (ModBackupPath / relative_path_string).parent_path());
                }
                if (std::filesystem::exists(installPath / "dvdroot_ps4" / relative_path_string)) {
                    std::filesystem::copy_file(installPath / "dvdroot_ps4" / relative_path_string,
                                               (ModBackupPath / relative_path_string),
                                               std::filesystem::copy_options::overwrite_existing);
                }
            } catch (std::exception& ex) {
                QMessageBox::warning(this, "Filesystem error backing up files", ex.what());
                break;
            }
        }
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        emit progressChanged(ui->progressBar->value() + 1);
    }

    ui->progressBar->setValue(0);
    ui->FileTransferLabel->setText("Copying mod files to Bloodborne Folder");

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModSourcePath)) {
        auto relative_path = std::filesystem::relative(entry, ModSourcePath);
        std::string relative_path_string = relative_path.u8string();
        const std::filesystem::path destDir = installPath / "dvdroot_ps4";

        if (!entry.is_directory()) {
            try {
                if (!std::filesystem::exists((destDir / relative_path_string).parent_path())) {
                    std::filesystem::create_directories(
                        (destDir / relative_path_string).parent_path());
                }
                std::filesystem::copy_file(ModSourcePath / relative_path_string,
                                           destDir / relative_path_string,
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
    std::ifstream ActiveFile(ModPath / "ActiveMods.txt", std::ios::binary);
    lineCount = 0;
    while (std::getline(ActiveFile, line)) {
        lineCount++;
        ActiveModList.push_back(line);
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        emit progressChanged(ui->progressBar->value() + 1);
    }
    ActiveFile.close();
    ActiveModList.push_back(ModName);

    std::ofstream ActiveFileSave(ModPath / "ActiveMods.txt", std::ios::binary);
    for (const auto& l : ActiveModList)
        ActiveFileSave << l << "\n";
    ActiveFileSave.close();

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
    const std::filesystem::path ModFolderPath = ModPath / ModName;
    const std::filesystem::path ModBackupPath =
        std::filesystem::current_path() / "BBLauncher" / "Mods-BACKUP" / ModName;
    std::vector<std::string> FileList;
    std::vector<std::string> ActiveModList;
    std::vector<std::string> ConflictMods;
    std::string line;
    int lineCount = 0;

    std::ifstream ConflictFile(ModPath / "ConflictMods.txt", std::ios::binary);
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

    const bool has_dvdroot = std::filesystem::exists(ModFolderPath / "dvdroot_ps4");
    std::filesystem::path ModSourcePath;
    if (!has_dvdroot) {
        ModSourcePath = ModFolderPath;
    } else {
        ModSourcePath = ModFolderPath / "dvdroot_ps4";
    }

    ui->FileTransferLabel->setText("Deleting Mod Files");
    ui->progressBar->setMaximum(getFileCount(ModSourcePath));

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModSourcePath)) {
        auto relative_path = std::filesystem::relative(entry, ModSourcePath);
        std::string relative_path_string = relative_path.u8string();
        if (!entry.is_directory()) {
            try {
                if (std::filesystem::exists(installPath / "dvdroot_ps4" / relative_path_string)) {
                    std::filesystem::remove(installPath / "dvdroot_ps4" / relative_path_string);
                }
            } catch (std::exception& ex) {
                QMessageBox::warning(this, "Filesystem error deleting mod files", ex.what());
                break;
            }
        }
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        emit progressChanged(ui->progressBar->value() + 1);
    }

    ui->progressBar->setValue(0);
    ui->FileTransferLabel->setText("Copying Backup Folder to Install Folder");
    ui->progressBar->setMaximum(getFileCount(ModBackupPath));

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModBackupPath)) {
        auto relative_path = std::filesystem::relative(entry, ModBackupPath);
        std::string relative_path_string = relative_path.u8string();
        if (!entry.is_directory()) {
            try {
                std::filesystem::copy(ModBackupPath / relative_path_string,
                                      installPath / "dvdroot_ps4" / relative_path_string);
            } catch (std::exception& ex) {
                QMessageBox::critical(nullptr, "Filesystem error reverting backup", ex.what());
                break;
            }
        }
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        emit progressChanged(ui->progressBar->value() + 1);
    }

    std::filesystem::remove_all(ModBackupPath);
    ui->progressBar->setValue(0);

    lineCount = 0;
    std::ifstream ActiveFile(ModPath / "ActiveMods.txt", std::ios::binary);
    while (std::getline(ActiveFile, line)) {
        lineCount++;
        ActiveModList.push_back(line);
    }
    ActiveFile.close();

    auto itr = std::find(ActiveModList.begin(), ActiveModList.end(), ModName);
    if (itr != ActiveModList.end())
        ActiveModList.erase(itr);

    std::ofstream ActiveFileSave(ModPath / "ActiveMods.txt", std::ios::binary);
    for (const auto& l : ActiveModList)
        ActiveFileSave << l << "\n";
    ActiveFileSave.close();

    std::filesystem::remove(ModPath / "ModifiedFiles.txt");
    for_each(ActiveModList.begin(), ActiveModList.end(), [&](std::string& e) {
        const std::filesystem::path ActiveModFolderPath = ModPath / e;
        ui->FileTransferLabel->setText("Generating Modified File list for " +
                                       QString::fromStdString(e));
        ui->progressBar->setMaximum(getFileCount(ActiveModFolderPath));
        for (const auto& entry :
             std::filesystem::recursive_directory_iterator(ActiveModFolderPath)) {
            if (!entry.is_directory()) {
                auto relative_path = std::filesystem::relative(entry, ActiveModFolderPath);
                FileList.push_back(relative_path.u8string());
            }
            ui->progressBar->setValue(ui->progressBar->value() + 1);
            emit progressChanged(ui->progressBar->value() + 1);
        }
        ui->progressBar->setValue(0);
    });

    std::ofstream ModInfoFileSave(ModPath / "ModifiedFiles.txt", std::ios::binary);
    for (const auto& i : FileList)
        ModInfoFileSave << i << "\n";
    ModInfoFileSave.close();

    RefreshLists();
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
    std::ifstream ActiveFile(ModPath / "ActiveMods.txt", std::ios::binary);

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
    for (auto& FolderEntry : std::filesystem::directory_iterator(ModPath)) {
        if (FolderEntry.is_directory()) {
            std::string Foldername = FolderEntry.path().filename().string();
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

int ModManager::getFileCount(std::filesystem::path ModFolderPath) {
    using std::filesystem::recursive_directory_iterator;
    return std::distance(std::filesystem::recursive_directory_iterator(ModFolderPath),
                         std::filesystem::recursive_directory_iterator{});
}

void ModManager::ConflictAdd(std::string ModName) {
    std::string line;
    int lineCount = 0;
    std::vector<std::string> ConflictMods;

    std::ifstream ConflictFile(ModPath / "ConflictMods.txt", std::ios::binary);
    while (std::getline(ConflictFile, line)) {
        lineCount++;
        ConflictMods.push_back(line);
    }
    ConflictFile.close();

    ConflictMods.push_back(ModName);

    std::ofstream ConflictFileSave(ModPath / "ConflictMods.txt", std::ios::binary);
    for (const auto& i : ConflictMods)
        ConflictFileSave << i << "\n";
    ConflictFileSave.close();
}

void ModManager::ConflictRemove(std::string ModName) {
    std::string line;
    int lineCount = 0;
    std::vector<std::string> ConflictMods;

    std::ifstream ConflictFile(ModPath / "ConflictMods.txt", std::ios::binary);
    while (std::getline(ConflictFile, line)) {
        lineCount++;
        ConflictMods.push_back(line);
    }
    ConflictFile.close();

    auto itr = std::find(ConflictMods.begin(), ConflictMods.end(), ModName);
    ConflictMods.erase(itr);

    std::ofstream ConflictFileSave(ModPath / "ConflictMods.txt", std::ios::binary);
    for (const auto& i : ConflictMods)
        ConflictFileSave << i << "\n";
    ConflictFileSave.close();
}

ModManager::~ModManager() {
    delete ui;
}
