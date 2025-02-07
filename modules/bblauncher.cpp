// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <iostream>
#include <thread>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

#include "bblauncher.h"
#include "modules/Common.h"
#include "modules/ModManager.h"
#include "modules/SaveManager.h"
#include "modules/ui_bblauncher.h"
#include "settings/LauncherSettings.h"
#include "settings/ShadCheatsPatches.h"
#include "settings/ShadSettings.h"
#include "settings/control_settings.h"
#include "settings/kbm_config_dialog.h"
#include "settings/updater/CheckUpdate.h"

BBLauncher::BBLauncher(bool noGUI, bool noInstanceRunning, QWidget* parent)
    : QMainWindow(parent), noGUIset(noGUI), noinstancerunning(noInstanceRunning),
      ui(new Ui::BBLauncher) {

    ui->setupUi(this);
    Config::LoadLauncherSettings();

    if (Common::shadPs4Executable == "" || !std::filesystem::exists(Common::shadPs4Executable)) {
        GetShadExecutable();
    }

    if (Common::installPath == "") {
        ui->ExeLabel->setText("no Bloodborne Folder selected (CUSA****)");
    } else {
        QString installQString;
        Common::PathToQString(installQString, Common::installPath);
        ui->ExeLabel->setText(installQString);
    }

    QString shadLabelString;
    Common::PathToQString(shadLabelString, Common::shadPs4Executable);
    ui->ShadLabel->setText(shadLabelString);

#ifdef _WIN32
    ui->ShadInstallGroupBox->hide();
#else
    ui->verticalSpacer->changeSize(0,0);
    ui->verticalSpacer_2->changeSize(0,0);
#endif

    this->setFixedSize(this->width(), this->height());
    this->statusBar()->setSizeGripEnabled(false);
    QApplication::setStyle("Fusion");

    std::string versionstring(Common::VERSION);
    setWindowTitle(QString::fromStdString("BBLauncher " + versionstring));

    // this->installEventFilter(this); if needed

    UpdateSettingsList();
    UpdateModList();

    ui->ControllerButton->setIcon(QIcon(":controller_icon.png"));
    ui->ControllerButton->setIconSize(QSize(48, 48));
    ui->KBMButton->setIcon(QIcon(":keyboard.png"));
    ui->KBMButton->setIconSize(QSize(48, 48));
    UpdateIcons();

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

        if (!std::filesystem::exists(Common::SaveDir / "1" / Common::game_serial / "SPRJ0005" /
                                     "userdata0010")) {
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
        if (!std::filesystem::exists(Common::GetShadUserDir() / "config.toml")) {
            QMessageBox::warning(
                this, "No shadPS4 config file found. Run shadPS4 once to generate it.",
                QString::fromStdString((Common::GetShadUserDir() / "config.toml").string() +
                                       " not found"));
            return;
        }
        ShadSettings* ShadSettingsWindow = new ShadSettings(this);
        ShadSettingsWindow->exec();
    });

    connect(ui->LauncherSettingsButton, &QPushButton::pressed, this, [this]() {
        LauncherSettings* LauncherSettingsWindow = new LauncherSettings(this);
        LauncherSettingsWindow->exec();
        UpdateSettingsList();
        UpdateIcons();
    });

    connect(ui->KBMButton, &QPushButton::clicked, this, [this]() {
        EditorDialog* editorWindow = new EditorDialog(this);
        editorWindow->exec();
    });

    connect(ui->ControllerButton, &QPushButton::clicked, this, [this]() {
        ControlSettings* RemapWindow = new ControlSettings(this);
        RemapWindow->exec();
    });

    if (Config::AutoUpdateEnabled) {
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
        Common::game_serial = QBBInstallLoc.last(9).toStdString();
        if (std::find(BBSerialList.begin(), BBSerialList.end(), Common::game_serial) !=
            BBSerialList.end()) {
            ui->ExeLabel->setText(QBBInstallLoc);
            Common::installPath = Common::PathFromQString(QBBInstallLoc);
            Config::SaveConfigPath("installPath", Common::installPath);
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
}

void BBLauncher::ShadSelectButton_isPressed() {

    QString ShadLoc;
#if defined(__linux__)
    ShadLoc = QFileDialog::getOpenFileName(
        this,
        "Select ShadPS4 executable (ex. /usr/bin/shadps4, "
        "Shadps4-qt.AppImage, etc.)",
        QDir::homePath(),
        "QT AppImage (Shadps4-qt.AppImage);;SDL AppImage (Shadps4-sdl.AppImage);;"
        "non-AppImage (shadps4)",
        0, QFileDialog::DontUseNativeDialog);
#elif defined(__APPLE__)
    ShadLoc = QFileDialog::getOpenFileName(this, "Select ShadPS4 executable (ex. shadps4.app)",
                                           QDir::homePath(), "App Bundle (shadps4.app)");
#endif

    if (ShadLoc != "") {
        Common::shadPs4Executable = Common::PathFromQString(ShadLoc);
        Config::SaveConfigPath("shadPath", Common::shadPs4Executable);
        ui->ShadLabel->setText(ShadLoc);
    }
}

void BBLauncher::LaunchButton_isPressed(bool noGUIset) {
    const std::filesystem::path EbootPath = Common::installPath / "eboot.bin";
    if (Common::installPath == "") {
        QMessageBox::warning(this, "No Bloodborne Install folder",
                             "Set-up Bloodborne Install folder before launching");
        if (noGUIset) {
            QApplication::quit();
        } else {
            return;
        }
    } else if (!std::filesystem::exists(EbootPath)) {
        QMessageBox::warning(this, "Bloodborne eboot.bin not found",
                             QString::fromStdString(EbootPath.string()) + " not found");
        if (noGUIset) {
            QApplication::quit();
        } else {
            return;
        }
    }

    if (Config::SoundFixEnabled) {
        std::filesystem::path savePath = Common::SaveDir / "1" / Common::game_serial / "SPRJ0005";
        if (std::filesystem::exists(savePath / "userdata0010")) {
            std::ofstream savefile1;
            savefile1.open(savePath / "userdata0010",
                           std::ios::in | std::ios::out | std::ios::binary);
            savefile1.seekp(0x204E);
            savefile1.put(0x1);
            savefile1.close();
        }
    }

    if (Config::BackupSaveEnabled) {
        std::thread saveThread(StartBackupSave);
        saveThread.detach();
    }

    QMainWindow::hide();

    QString PKGarg;
    Common::PathToQString(PKGarg, EbootPath);

    QProcess* process = new QProcess;
    QStringList processArg;
    processArg << "-g" << PKGarg;

    QString shadBinary;
    Common::PathToQString(shadBinary, Common::shadPs4Executable);

    process->start(shadBinary, processArg);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus) { QApplication::quit(); });

    connect(process, &QProcess::readyReadStandardOutput, [process, this]() {
        QString output = process->readAllStandardOutput();
        std::cout << output.toStdString();
    });

    connect(process, &QProcess::readyReadStandardError, [process]() {
        QString err = process->readAllStandardError();
        std::cout << err.toStdString();
    });
}

void BBLauncher::StartBackupSave() {
    const std::filesystem::path BackupPath = Common::BBLFilesPath / "SaveBackups";

    if (!std ::filesystem::exists(BackupPath)) {
        std::filesystem::create_directory(BackupPath);
    }

    for (int i = 1; i < Config::BackupNumber + 1; i++) {
        std::string backupstring = "BACKUP" + std::to_string(i);
        auto backup_dircreate = BackupPath / backupstring;
        if (!std ::filesystem::exists(backup_dircreate)) {
            std::filesystem::create_directory(backup_dircreate);
        }
    }

    const auto save_dir = Common::SaveDir / "1" / Common::game_serial;
    const auto backup_dir = BackupPath / "BACKUP1";

    while (true) {
        std::this_thread::sleep_for(std::chrono::minutes(Config::BackupInterval));

        if (Config::BackupNumber > 1) {
            const std::string lastDirstring = "BACKUP" + std::to_string(Config::BackupNumber);
            const auto lastdir = BackupPath / lastDirstring;
            std::filesystem::remove_all(lastdir);
            for (int i = Config::BackupNumber - 1; i > 0; i--) {
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

void BBLauncher::UpdateSettingsList() {
    using namespace Config;
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
    std::ifstream ActiveFile(Common::ModPath / "ActiveMods.txt", std::ios::binary);

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

    Common::shadPs4Executable = "";

#ifdef _WIN32
    if (std::filesystem::exists(std::filesystem::current_path() / "shadPS4.exe")) {
        Common::shadPs4Executable = std::filesystem::current_path() / "shadPS4.exe";
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
    Common::shadPs4Executable = std::filesystem::u8path(ShadLocString);
#elif defined(__APPLE__)
    QString ShadLoc =
        QFileDialog::getOpenFileName(this, "Select ShadPS4 executable (ex. shadps4.app)",
                                     QDir::homePath(), "App Bundle (shadps4.app)");
    const std::string ShadLocString = ShadLoc.toStdString();
    Common::shadPs4Executable = std::filesystem::u8path(ShadLocString);
#endif

    Config::SaveConfigPath("shadPath", Common::shadPs4Executable);

    if (Common::shadPs4Executable == "")
        canLaunch = false;
}

// bool BBLauncher::eventFilter(QObject* obj, QEvent* event) {}

bool BBLauncher::CheckBBInstall() {
    if (Common::installPath == "") {
        QMessageBox::warning(this, "No Bloodborne install path selected",
                             "Select Bloodborne install folder before using this feature");
        return false;
    } else if (!std::filesystem::exists(Common::installPath) || Common::installPath.empty()) {
        QMessageBox::warning(this, "Bloodborne install folder empty or does not exist",
                             QString::fromStdString(Common::installPath.string()) +
                                 " is empty or does not exist");
        return false;
    } else {
        return true;
    }
}

void BBLauncher::UpdateIcons() {
    if (Config::theme == "Dark") {
        ui->ControllerButton->setIcon(RecolorIcon(ui->ControllerButton->icon(), false));
    } else {
        ui->ControllerButton->setIcon(RecolorIcon(ui->ControllerButton->icon(), true));
    }

    if (Config::theme == "Light") {
        ui->KBMButton->setIcon(RecolorIcon(ui->KBMButton->icon(), true));
    } else {
        ui->KBMButton->setIcon(RecolorIcon(ui->KBMButton->icon(), false));
    }
}

QIcon BBLauncher::RecolorIcon(const QIcon& icon, bool isWhite) {
    QPixmap pixmap(icon.pixmap(icon.actualSize(QSize(128, 128))));
    QColor clr(isWhite ? Qt::white : Qt::black);
    QBitmap mask = pixmap.createMaskFromColor(clr, Qt::MaskOutColor);
    pixmap.fill(QColor(isWhite ? Qt::black : Qt::white));
    pixmap.setMask(mask);
    return QIcon(pixmap);
}

BBLauncher::~BBLauncher() {
    delete ui;
}
