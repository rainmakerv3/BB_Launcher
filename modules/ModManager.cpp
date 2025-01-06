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

    if (!std::filesystem::exists(ModPath)) {
        std::filesystem::create_directories(ModPath);
    }

    if (!std::filesystem::exists(std::filesystem::current_path() / "BBLauncher" / "Mods-BACKUP")) {
        std::filesystem::create_directories(std::filesystem::current_path() / "BBLauncher" /
                                            "Mods-BACKUP");
    }

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
    const std::string ModFolderPathString = (ModPath / ModName).string();
    const std::filesystem::path ModFolderPath = std::filesystem::u8path(ModFolderPathString);
    const std::string ModBackupPathString =
        (std::filesystem::current_path() / "BBLauncher" / "Mods-BACKUP" / ModName).string();
    const std::filesystem::path ModBackupPath = std::filesystem::u8path(ModBackupPathString);

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

    for (const auto& entry : std::filesystem::directory_iterator(ModSourcePath)) {
        if (entry.is_directory()) {
            auto relative_path = std::filesystem::relative(entry, ModSourcePath);
            std::string relative_path_string = PathToU8(relative_path);
            if (std::find(BBFolders.begin(), BBFolders.end(), relative_path_string) ==
                BBFolders.end()) {
                QMessageBox::warning(this, "Invalid Mod",
                                     "Folders inside mod folder must include either dvdroot_ps4"
                                     " or Blooborne dvdroot_ps4 subfolders (ex. sfx, parts, "
                                     " map ");
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
            std::string relative_path_string = PathToU8(relative_path);
            if (std::find(FileList.begin(), FileList.end(), relative_path_string) !=
                FileList.end()) {
                if (QMessageBox::Yes ==
                    QMessageBox::question(
                        this, "Mod conflict found",
                        "This mod conflicts with an active mod. Some conflicting mods cannot"
                        " function properly together.\n\nProceed with activation?",
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
            std::string relative_path_string = PathToU8(relative_path);
            FileList.push_back(relative_path_string);
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
        try {
            if (!entry.is_directory()) {
                if (!std::filesystem::exists((ModBackupPath / relative_path.parent_path()))) {
                    std::filesystem::create_directories(
                        (ModBackupPath / relative_path.parent_path()));
                }
                if (std::filesystem::exists(installPath / "dvdroot_ps4" / relative_path)) {
                    std::filesystem::copy_file(installPath / "dvdroot_ps4" / relative_path,
                                               (ModBackupPath / relative_path),
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
        const std::filesystem::path destDir = installPath / "dvdroot_ps4";
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
    const std::string ModFolderPathString = (ModPath / ModName).string();
    const std::filesystem::path ModFolderPath = std::filesystem::u8path(ModFolderPathString);
    const std::string ModBackupPathString =
        (std::filesystem::current_path() / "BBLauncher" / "Mods-BACKUP" / ModName).string();
    const std::filesystem::path ModBackupPath = std::filesystem::u8path(ModBackupPathString);

    std::vector<std::string> FileList;
    std::vector<std::string> ActiveModList;
    std::vector<std::string> ConflictMods;
    std::string line;
    int lineCount = 0;

    const bool has_dvdroot = std::filesystem::exists(ModFolderPath / "dvdroot_ps4");
    std::filesystem::path ModSourcePath;
    if (!has_dvdroot) {
        ModSourcePath = ModFolderPath;
    } else {
        ModSourcePath = ModFolderPath / "dvdroot_ps4";
    }

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

    ui->FileTransferLabel->setText("Deleting Mod Files");
    ui->progressBar->setMaximum(getFileCount(ModSourcePath));

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModSourcePath)) {
        auto relative_path = std::filesystem::relative(entry, ModSourcePath);
        if (!entry.is_directory()) {
            try {
                if (std::filesystem::exists(installPath / "dvdroot_ps4" / relative_path)) {
                    std::filesystem::remove(installPath / "dvdroot_ps4" / relative_path);
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
    ui->FileTransferLabel->setText("Moving backup to Install Folder");
    ui->progressBar->setMaximum(getFileCount(ModBackupPath));

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ModBackupPath)) {
        auto relative_path = std::filesystem::relative(entry, ModBackupPath);
        if (!entry.is_directory()) {
            try {
                std::filesystem::rename(ModBackupPath / relative_path,
                                        installPath / "dvdroot_ps4" / relative_path);
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
        const std::string ActiveModSourcePathString = (ModPath / e).string();
        const std::string ActiveModSourcePathStringDVD = (ModPath / e / "dvdroot_ps4").string();

        std::filesystem::path ActiveModSourcePath;
        if (!has_dvdroot) {
            ActiveModSourcePath = std::filesystem::u8path(ActiveModSourcePathString);
        } else {
            ActiveModSourcePath = std::filesystem::u8path(ActiveModSourcePathStringDVD);
        }

        ui->FileTransferLabel->setText("Generating Modified File list for " +
                                       QString::fromStdString(e));
        ui->progressBar->setMaximum(getFileCount(ActiveModSourcePath));

        for (const auto& entry :
             std::filesystem::recursive_directory_iterator(ActiveModSourcePath)) {
            if (!entry.is_directory()) {
                auto relative_path = std::filesystem::relative(entry, ActiveModSourcePath);
                std::string relative_path_string = PathToU8(relative_path);
                FileList.push_back(relative_path_string);
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
            std::string Foldername = PathToU8(FolderEntry.path().filename());
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

std::string ModManager::PathToU8(const std::filesystem::path& path) {
    const auto u8_string = path.u8string();
    return std::string{u8_string.begin(), u8_string.end()};
}

ModManager::~ModManager() {
    delete ui;
}
