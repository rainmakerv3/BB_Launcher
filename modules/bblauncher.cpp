// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <thread>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

#include "bblauncher.h"
#include "modules/ModManager.h"
#include "modules/ui_bblauncher.h"
#include "settings/LauncherSettings.h"
#include "settings/ShadCheatsPatches.h"
#include "settings/ShadSettings.h"
#include "settings/toml.hpp"

std::string installPathString = "";
std::filesystem::path installPath = "";
std::filesystem::path EbootPath = "";
std::string game_serial = "";

BBLauncher::BBLauncher(QWidget* parent) : QMainWindow(parent), ui(new Ui::BBLauncher) {
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->statusBar()->setSizeGripEnabled(false);
    QApplication::setStyle("Fusion");
    setWindowTitle("BB Launcher Release 3.5");

    // this->installEventFilter(this); if needed

    LoadLauncherSettings();
    UpdateSettingsList();
    UpdateModList();

    connect(ui->ExeSelectButton, &QPushButton::pressed, this,
            &BBLauncher::ExeSelectButton_isPressed);

    connect(ui->LaunchButton, &QPushButton::pressed, this, &BBLauncher::LaunchButton_isPressed);
    connect(ui->TrophyButton, &QPushButton::pressed, this, &BBLauncher::WIPButton_isPressed);
    connect(ui->SaveManagerButton, &QPushButton::pressed, this, &BBLauncher::WIPButton_isPressed);
    connect(ui->PatchesButton, &QPushButton::pressed, this, &BBLauncher::WIPButton_isPressed);

    /*
    connect(ui->PatchesButton, &QPushButton::pressed, this, [this]() {
        CheckBBInstall();
        QMessageBox::warning(
            this, "NOT FUNCTIONAL YET",
            "Patches module is currently WIP, and should not be used. GUI is enabled "
            "only for testing purposes.");
        CheatsPatches* cheatsPatches = new CheatsPatches(this);
        cheatsPatches->show();
        connect(this, &QWidget::destroyed, cheatsPatches,
                [cheatsPatches]() { cheatsPatches->deleteLater(); });
    });
    */

    connect(ui->ModManagerButton, &QPushButton::pressed, this, [this]() {
        CheckBBInstall();
        ModManager* ModWindow = new ModManager(this);
        ModWindow->exec();
        UpdateModList();
    });

    connect(ui->shadSettingsButton, &QPushButton::pressed, this, [this]() {
        if (!std::filesystem::exists(GetShadUserDir() / "config.toml")) {
            QMessageBox::warning(
                this, "No shadPS4 config file found. Run shadPS4 once to generate it.",
                QString::fromStdString((GetShadUserDir() / "config.toml").string() + " not found"));
            return;
        }
        ShadSettings* ShadSettingsWindow = new ShadSettings(this);
        ShadSettingsWindow->exec();
    });

    connect(ui->LauncherSettingsButton, &QPushButton::pressed, this, [this]() {
        LauncherSettings* LauncherSettingsWindow = new LauncherSettings(this);
        LauncherSettingsWindow->exec();
        UpdateSettingsList();
    });

    if (installPathString == "") {
        const QString NoPathText = "no Bloodborne Folder selected (CUSA****)";
        ui->ExeLabel->setText(NoPathText);
    } else {
        ui->ExeLabel->setText(QString::fromStdString(installPathString));
    }
}

void BBLauncher::ExeSelectButton_isPressed() {
    QString QBBInstallLoc = "";
    QBBInstallLoc = QFileDialog::getExistingDirectory(
        this, "Select Bloodborne install location (ex. CUSA03173, CUSA00900", QDir::currentPath());
    if (QBBInstallLoc != "") {
        game_serial = QBBInstallLoc.last(9).toStdString();
        if (std::find(BBSerialList.begin(), BBSerialList.end(), game_serial) !=
            BBSerialList.end()) {
            ui->ExeLabel->setText(QBBInstallLoc);
            installPathString = QBBInstallLoc.toStdString();
            installPath = installPathString;
            EbootPath = installPath / "eboot.bin";
            SaveInstallLoc();
        } else {
            QMessageBox::warning(
                this, "Install Location not valid",
                "Select valid BB Install folder starting with CUSA (ex: CUSA03173, CUSA00900)");
        }
    }
}

void BBLauncher::WIPButton_isPressed() {
    QMessageBox::warning(this, "WIP", "Still working on this :)");
    UpdateSettingsList();
}

void BBLauncher::LaunchButton_isPressed() {
    if (installPath == "") {
        QMessageBox::warning(this, "No Bloodborne Install folder selected",
                             "Select valid Bloodborne Install folder first");
        return;
    } else if (!std::filesystem::exists(EbootPath)) {
        QMessageBox::warning(this, "Bloodborne eboot.PKG not found",
                             QString::fromStdString(EbootPath.string()) + " not found");
        return;
    }

    if (SoundFixEnabled) {
        std::filesystem::path savePath =
            GetShadUserDir() / "savedata" / "1" / game_serial / "SPRJ0005";
        if (std::filesystem::exists(savePath / "userdata0010")) {
            std::ofstream savefile1;
            savefile1.open(savePath / "userdata0010",
                           std::ios::in | std::ios::out | std::ios::binary);
            savefile1.seekp(0x204E);
            savefile1.put(0x1);
            savefile1.close();
        }
    }

    QMainWindow::hide();
    std::thread shadThread(startShad);
    shadThread.detach();

    if (BackupSaveEnabled) {
        std::thread saveThread(StartBackupSave);
        saveThread.detach();
    }
}

void BBLauncher::startShad() {
    QString PKGarg;
#ifdef _WIN32
    PKGarg = QString::fromStdWString(EbootPath.wstring());
#else
    PKGarg = QString::fromStdString(EbootPath.string());
#endif

    QProcess* process = new QProcess;
    QString processCommand;
    QStringList processArg;
    processArg << "-g" << PKGarg;

#ifdef _WIN32
    processCommand = "shadps4.exe";
#elif defined(__linux__)
    QProcess* chmod = new QProcess;
    QString chmodCommand = "chmod";
    QStringList chmodArg;

    if (std::filesystem::exists(std::filesystem::current_path() / "Shadps4-qt.AppImage")) {
        chmodArg << "+x"
                 << "Shadps4-qt.AppImage";
        processCommand = "./Shadps4-qt.AppImage";
    } else if (std::filesystem::exists(std::filesystem::current_path() / "Shadps4-sdl.AppImage")) {
        chmodArg << "+x"
                 << "Shadps4-sdl.AppImage";
        processCommand = "./Shadps4-sdl.AppImage";
    } else {
        chmodArg << "+x" << "shadps4";
        processCommand = "./shadps4";
    }

    chmod->start("chmod", chmodArg);
#elif defined(__APPLE__)
    processCommand = "open shadPS4";
#endif

    process->startDetached(processCommand, processArg);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus) { QApplication::quit(); });
}

void BBLauncher::StartBackupSave() {
    const std::filesystem::path BackupPath =
        std::filesystem::current_path() / "BBLauncher" / "SaveBackups";

    if (!std ::filesystem::exists(BackupPath)) {
        std::filesystem::create_directory(BackupPath);
    }

    for (int i = 1; i < BackupNumber + 1; i++) {
        std::string backupstring = "BACKUP" + std::to_string(i);
        auto backup_dircreate = BackupPath / backupstring;
        if (!std ::filesystem::exists(backup_dircreate)) {
            std::filesystem::create_directory(backup_dircreate);
        }
    }

    const auto save_dir = GetShadUserDir() / "savedata" / "1" / game_serial;
    const auto backup_dir = BackupPath / "BACKUP1";

    while (true) {
        std::this_thread::sleep_for(std::chrono::minutes(BackupInterval));

        if (BackupNumber > 1) {
            const std::string lastDirstring = "BACKUP" + std::to_string(BackupNumber);
            const auto lastdir = BackupPath / lastDirstring;
            std::filesystem::remove_all(lastdir);
            for (int i = BackupNumber - 1; i > 0; i--) {
                std::string sourceString = "BACKUP" + std::to_string(i);
                std::string destString = "BACKUP" + std::to_string(i + 1);
                const auto sourceDir = BackupPath / sourceString;
                const auto destDir = BackupPath / destString;
                try {
                    std::filesystem::rename(sourceDir, destDir);
                } catch (std::exception& ex) {
                    // handle?;
                }
            }
        }

        try {
            std::filesystem::copy(save_dir, backup_dir,
                                  std::filesystem::copy_options::overwrite_existing |
                                      std::filesystem::copy_options::recursive);
        } catch (std::exception& ex) {
            // handle?;
        }
    }
}

void BBLauncher::SaveInstallLoc() {
    toml::value data;
    std::error_code error;

    if (std::filesystem::exists(SettingsFile, error)) {
        try {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(SettingsFile, std::ios_base::binary);
            data = toml::parse(SettingsFile);
        } catch (const std::exception& ex) {
            QMessageBox::critical(this, "Filesystem error", ex.what());
            return;
        }
    } else {
        if (error) {
            QMessageBox::critical(this, "Filesystem error",
                                  QString::fromStdString(error.message()));
        }
    }

    data["Launcher"]["installPath"] = installPathString;

    std::ofstream file(SettingsFile, std::ios::binary);
    file << data;
    file.close();
}

void BBLauncher::UpdateSettingsList() {
    QString SoundhackSetting = "60 FPS sound fix = " + QVariant(SoundFixEnabled).toString();
    QString ThemeSetting = "Selected Theme = " + QString::fromStdString(theme);
    QString BackupEnableSetting =
        "Back up saves enabled = " + QVariant(BackupSaveEnabled).toString();

    QString BackupIntSetting;
    QString BackupNumSetting;

    if (BackupSaveEnabled) {
        BackupIntSetting = "Backup Interval = " + QString::number(BackupInterval);
        BackupNumSetting = "Backup Copies = " + QString::number(BackupNumber);
    } else {
        BackupIntSetting = "Backup Interval = disabled";
        BackupNumSetting = "Backup Copies = disabled";
    }

    QStringList SettingStrings = {SoundhackSetting, ThemeSetting, BackupEnableSetting,
                                  BackupIntSetting, BackupNumSetting};

    ui->SettingList->clear();
    ui->SettingList->addItems(SettingStrings);
}

void BBLauncher::UpdateModList() {
    std::vector<std::string> ActiveModList;
    std::string line;
    int lineCount = 0;
    std::ifstream ActiveFile(ModPath / "ActiveMods.txt", std::ios::binary);

    ui->ModList->clear();

    while (std::getline(ActiveFile, line)) {
        lineCount++;
        ActiveModList.push_back(line);
    }
    ActiveFile.close();

    QStringList ActiveModStringList;

    if (ActiveModList.size() != 0) {
        for_each(ActiveModList.begin(), ActiveModList.end(),
                 [&](std::string s) { ActiveModStringList.append(QString::fromStdString(s)); });
    } else {
        const QString NoModMsg =
            "No mods active.\n\nPlace mods in the (BB Launcher exe folder)/BBLauncher/Mods and "
            "activate them using the Mod Manager button";
        ActiveModStringList.append(NoModMsg);
    }

    ui->ModList->addItems(ActiveModStringList);
}

// bool BBLauncher::eventFilter(QObject* obj, QEvent* event) {}

std::filesystem::path GetShadUserDir() {

    auto user_dir = std::filesystem::current_path() / "user";
    if (!std::filesystem::exists(user_dir)) {
#ifdef __APPLE__
        user_dir =
            std::filesystem::path(getenv("HOME")) / "Library" / "Application Support" / "shadPS4";
#elif defined(__linux__)
        const char* xdg_data_home = getenv("XDG_DATA_HOME");
        if (xdg_data_home != nullptr && strlen(xdg_data_home) > 0) {
            user_dir = std::filesystem::path(xdg_data_home) / "shadPS4";
        } else {
            user_dir = std::filesystem::path(getenv("HOME")) / ".local" / "share" / "shadPS4";
        }
#endif
    }
    return user_dir;
}

void PathToQString(QString& result, const std::filesystem::path& path) {
#ifdef _WIN32
    result = QString::fromStdWString(path.wstring());
#else
    result = QString::fromStdString(path.string());
#endif
}

void BBLauncher::CheckBBInstall() {
    if (installPath == "") {
        QMessageBox::warning(this, "No Bloodborne install path selected",
                             "Select Bloodborne install folder before using Mod Manager");
        return;
    } else if (!std::filesystem::exists(installPath) || installPath.empty()) {
        QMessageBox::warning(this, "Bloodborne install folder empty or does not exist",
                             QString::fromStdString(installPath.string()) +
                                 " is empty or does not exist");
        return;
    }
}

BBLauncher::~BBLauncher() {
    delete ui;
}
