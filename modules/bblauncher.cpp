// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <thread>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

#include "bblauncher.h"
#include "modules/ModManager.h"
#include "modules/SaveManager.h"
#include "modules/ui_bblauncher.h"
#include "settings/LauncherSettings.h"
#include "settings/ShadCheatsPatches.h"
#include "settings/ShadSettings.h"
#include "settings/toml.hpp"
#include "settings/updater/CheckUpdate.h"

std::string installPathString = "";
std::filesystem::path installPath = "";
std::filesystem::path EbootPath = "";
std::string game_serial = "";
std::filesystem::path SaveDir = "";
char VERSION[] = "Release4.4";
std::filesystem::path shadPs4Executable;

BBLauncher::BBLauncher(bool noGUI, bool noInstanceRunning, QWidget* parent)
    : QMainWindow(parent), noGUIset(noGUI), noinstancerunning(noInstanceRunning),
      ui(new Ui::BBLauncher) {

    ui->setupUi(this);

    LoadLauncherSettings();
    if (shadPs4Executable == "" || !std::filesystem::exists(shadPs4Executable)) {
        GetShadExecutable();
    }

    if (installPathString == "") {
        const QString NoPathText = "no Bloodborne Folder selected (CUSA****)";
        ui->ExeLabel->setText(NoPathText);
    } else {
        ui->ExeLabel->setText(QString::fromStdString(installPathString));
    }

    QString shadLabelString;
    PathToQString(shadLabelString, shadPs4Executable);
    ui->ShadLabel->setText(shadLabelString);

#ifdef _WIN32
    ui->ShadInstallGroupBox->hide();
#endif

    this->setFixedSize(this->width(), this->height());
    this->statusBar()->setSizeGripEnabled(false);
    QApplication::setStyle("Fusion");

    std::string versionstring(VERSION);
    setWindowTitle(QString::fromStdString("BBLauncher " + versionstring));

    // this->installEventFilter(this); if needed

    UpdateSettingsList();
    UpdateModList();

    connect(ui->ExeSelectButton, &QPushButton::pressed, this,
            &BBLauncher::ExeSelectButton_isPressed);
    connect(ui->ShadSelectButton, &QPushButton::pressed, this,
            &BBLauncher::ShadSelectButton_isPressed);
    connect(ui->LaunchButton, &QPushButton::pressed, this,
            [this]() { BBLauncher::LaunchButton_isPressed(noGUIset); });

    connect(ui->TrophyButton, &QPushButton::pressed, this, &BBLauncher::WIPButton_isPressed);

    connect(ui->SaveManagerButton, &QPushButton::pressed, this, [this]() {
        if (!CheckBBInstall())
            return;

        if (!std::filesystem::exists(SaveDir / "1" / game_serial /
                                     "SPRJ0005" / "userdata0010")) {
            QMessageBox::warning(this, "No saves detected",
                                 "Launch Bloodborne to generate saves before using Save Manager");
            return;
        }

        SaveManager* SaveManagerWindow = new SaveManager(this);
        SaveManagerWindow->exec();
    });

    connect(ui->PatchesButton, &QPushButton::pressed, this, [this]() {
        if (!CheckBBInstall())
            return;
        CheatsPatches* cheatsPatches = new CheatsPatches(this);
        cheatsPatches->show();
        connect(this, &QWidget::destroyed, cheatsPatches,
                [cheatsPatches]() { cheatsPatches->deleteLater(); });
    });

    connect(ui->ModManagerButton, &QPushButton::pressed, this, [this]() {
        if (!CheckBBInstall())
            return;
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

    if (AutoUpdateEnabled) {
        auto checkUpdate = new CheckUpdate(false);
        checkUpdate->exec();
    }

    if (noGUI && noInstanceRunning)
        LaunchButton_isPressed(noGUI);
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
            installPath = QBBInstallLoc.toStdString();
            EbootPath = installPath / "eboot.bin";
            SaveConfigOption("installPath", PathToU8(installPath));
        } else {
            QMessageBox::warning(
                this, "Install Location not valid",
                "Select valid BB Install folder starting with CUSA (ex: CUSA03173, CUSA00900)");
        }
    }
}

void BBLauncher::WIPButton_isPressed() {
    QMessageBox::warning(
        this, "On hold",
        "Work on trophy manager will be put on-hold until it's easier to dump trophy keys");
    UpdateSettingsList();
}

void BBLauncher::ShadSelectButton_isPressed() {

    std::string ShadLocString;
#if defined(__linux__)
    QString ShadLoc = QFileDialog::getOpenFileName(
        this,
        "Select ShadPS4 executable (ex. /usr/bin/shadps4, "
        "Shadps4-qt.AppImage, etc.)",
        QDir::homePath(),
        "QT AppImage (Shadps4-qt.AppImage);;SDL AppImage (Shadps4-sdl.AppImage);;"
        "non-AppImage (shadps4)",
        0, QFileDialog::DontUseNativeDialog);
    ShadLocString = ShadLoc.toStdString();
    if (ShadLocString != "")
        shadPs4Executable = std::filesystem::u8path(ShadLocString);
#elif defined(__APPLE__)
    QString ShadLoc =
        QFileDialog::getOpenFileName(this, "Select ShadPS4 executable (ex. shadps4.app)",
                                     QDir::homePath(), "App Bundle (shadps4.app)");
    ShadLocString = ShadLoc.toStdString();
    if (ShadLocString != "")
        shadPs4Executable = std::filesystem::u8path(ShadLocString);
#endif

    if (ShadLocString != "") {
        SaveConfigOption("shadPath", PathToU8(shadPs4Executable));
        QString shadLabelString;
        PathToQString(shadLabelString, shadPs4Executable);
        ui->ShadLabel->setText(shadLabelString);
    }
}

void BBLauncher::LaunchButton_isPressed(bool noGUIset) {
    if (installPath == "") {
        QMessageBox::warning(this, "No Bloodborne Install folder",
                             "Set-up Bloodborne Install folder before launching");
        if (noGUIset) {
            QApplication::quit();
        } else {
            return;
        }
    } else if (!std::filesystem::exists(EbootPath)) {
        QMessageBox::warning(this, "Bloodborne eboot.PKG not found",
                             QString::fromStdString(EbootPath.string()) + " not found");
        if (noGUIset) {
            QApplication::quit();
        } else {
            return;
        }
    }

    if (SoundFixEnabled) {
        std::filesystem::path savePath = SaveDir / "1" / game_serial / "SPRJ0005";
        if (std::filesystem::exists(savePath / "userdata0010")) {
            std::ofstream savefile1;
            savefile1.open(savePath / "userdata0010",
                           std::ios::in | std::ios::out | std::ios::binary);
            savefile1.seekp(0x204E);
            savefile1.put(0x1);
            savefile1.close();
        }
    }

    if (BackupSaveEnabled) {
        std::thread saveThread(StartBackupSave);
        saveThread.detach();
    }

    QMainWindow::hide();
    QString PKGarg;
#ifdef _WIN32
    PKGarg = QString::fromStdWString(EbootPath.wstring());
#else
    PKGarg = QString::fromStdString(EbootPath.string());
#endif

    QProcess* process = new QProcess;
    QStringList processArg;
    processArg << "-g" << PKGarg;

    QString shadBinary;
    PathToQString(shadBinary, shadPs4Executable);

    process->start(shadBinary, processArg);
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

    const auto save_dir = SaveDir / "1" / game_serial;
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

void BBLauncher::SaveConfigOption(std::string configKey, std::string configValue) {
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

    data["Launcher"][configKey] = configValue;
    std::ofstream file(SettingsFile, std::ios::binary);
    file << data;
    file.close();
}

void BBLauncher::UpdateSettingsList() {
    QString SoundhackSetting = "60 FPS sound fix = " + QVariant(SoundFixEnabled).toString();
    QString ThemeSetting = "Selected Theme = " + QString::fromStdString(theme);
    QString BackupEnableSetting =
        "Back up saves enabled = " + QVariant(BackupSaveEnabled).toString();
    QString AutoUpdateSetting =
        "Check updates on startup enabled = " + QVariant(AutoUpdateEnabled).toString();

    QString BackupIntSetting;
    QString BackupNumSetting;

    if (BackupSaveEnabled) {
        BackupIntSetting = "Backup Interval = " + QString::number(BackupInterval);
        BackupNumSetting = "Backup Copies = " + QString::number(BackupNumber);
    } else {
        BackupIntSetting = "Backup Interval = disabled";
        BackupNumSetting = "Backup Copies = disabled";
    }

    QStringList SettingStrings = {SoundhackSetting, ThemeSetting,     BackupEnableSetting,
                                  BackupIntSetting, BackupNumSetting, AutoUpdateSetting};

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

void BBLauncher::GetShadExecutable() {

    shadPs4Executable = "";

#ifdef _WIN32
    if (std::filesystem::exists(std::filesystem::current_path() / "shadPS4.exe")) {
        shadPs4Executable = std::filesystem::current_path() / "shadPS4.exe";
    } else {
        QMessageBox::warning(
            this, "No shadPS4.exe found",
            "No shadPS4.exe found. Move BB_Launcher.exe next to shadPS4.exe.\n\nMove all other "
            "files/folders in BB_Launcher folder to shadPS4 folder only if you are using a non-QT "
            "(no GUI) version of shadPS4.");
    }
#elif defined(__linux__)
    QMessageBox::warning(this, "No ShadPS4 executable path selected",
                         "Select ShadPS4 executable before using BB Launcher.");

    QString ShadLoc = QFileDialog::getOpenFileName(
        this,
        "Select ShadPS4 executable (ex. /usr/bin/shadps4, "
        "Shadps4-qt.AppImage, etc.)",
        QDir::homePath(),
        "QT AppImage (Shadps4-qt.AppImage);;SDL AppImage (Shadps4-sdl.AppImage);;"
        "non-AppImage (shadps4)",
        0, QFileDialog::DontUseNativeDialog);
    const std::string ShadLocString = ShadLoc.toStdString();
    shadPs4Executable = std::filesystem::u8path(ShadLocString);
#elif defined(__APPLE__)
    QString ShadLoc =
        QFileDialog::getOpenFileName(this, "Select ShadPS4 executable (ex. shadps4.app)",
                                     QDir::homePath(), "App Bundle (shadps4.app)");
    const std::string ShadLocString = ShadLoc.toStdString();
    shadPs4Executable = std::filesystem::u8path(ShadLocString);
#endif

    SaveConfigOption("shadPath", PathToU8(shadPs4Executable));

    if (shadPs4Executable == "")
        canLaunch = false;
}

// bool BBLauncher::eventFilter(QObject* obj, QEvent* event) {}

bool BBLauncher::CheckBBInstall() {
    if (installPath == "") {
        QMessageBox::warning(this, "No Bloodborne install path selected",
                             "Select Bloodborne install folder before using this feature");
        return false;
    } else if (!std::filesystem::exists(installPath) || installPath.empty()) {
        QMessageBox::warning(this, "Bloodborne install folder empty or does not exist",
                             QString::fromStdString(installPath.string()) +
                                 " is empty or does not exist");
        return false;
    } else {
        return true;
    }
}

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

std::filesystem::path PathFromQString(const QString& path) {
#ifdef _WIN32
    return std::filesystem::path(path.toStdWString());
#else
    return std::filesystem::path(path.toStdString());
#endif
}

std::string PathToU8(const std::filesystem::path& path) {
    const auto u8_string = path.u8string();
    return std::string{u8_string.begin(), u8_string.end()};
}

BBLauncher::~BBLauncher() {
    delete ui;
}
