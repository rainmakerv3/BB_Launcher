// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <iostream>
#include <thread>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

#include "bblauncher.h"
#include "modules/Common.h"
#include "modules/ModManager.h"
#include "modules/SaveManager.h"
#include "modules/TrophyManager.h"
#include "modules/ui_bblauncher.h"
#include "settings/LauncherSettings.h"
#include "settings/ShadCheatsPatches.h"
#include "settings/ShadSettings.h"
#include "settings/config.h"
#include "settings/control_settings.h"
#include "settings/hotkeys.h"
#include "settings/kbm_gui.h"
#include "settings/updater/CheckUpdate.h"

static bool save_backups = false;

BBLauncher::BBLauncher(bool noGUI, bool noInstanceRunning, QWidget* parent)
    : QMainWindow(parent), noGUIset(noGUI), noinstancerunning(noInstanceRunning),
      ui(new Ui::BBLauncher) {

    m_ipc_client->gameClosedFunc = [this]() { onGameClosed(); };
    m_ipc_client->restartEmulatorFunc = [this]() { RestartEmulator(); };
    m_ipc_client->startGameFunc = [this]() { RunGame(); };

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

    this->setFixedSize(this->width(), this->height());
    this->statusBar()->setSizeGripEnabled(false);
    QApplication::setStyle("Fusion");

    std::string versionstring(Common::VERSION);
    setWindowTitle(("BBLauncher " + QString::fromStdString(versionstring).right(5)));

    UpdateSettingsList();
    UpdateModList();
    UpdateIcons();

    connect(ui->ExeSelectButton, &QPushButton::pressed, this,
            &BBLauncher::ExeSelectButton_isPressed);
    connect(ui->ShadSelectButton, &QPushButton::pressed, this,
            &BBLauncher::ShadSelectButton_isPressed);
    connect(ui->LaunchButton, &QPushButton::pressed, this,
            [this]() { BBLauncher::StartGameWithArgs({}); });

    connect(ui->TrophyButton, &QPushButton::pressed, this, [this]() {
        QString trophyPath, gameTrpPath;
        Common::PathToQString(trophyPath, Common::game_serial);
        Common::PathToQString(gameTrpPath, Common::installPath);

        if (std::filesystem::exists(Common::installUpdatePath)) {
            Common::PathToQString(gameTrpPath, Common::installUpdatePath);
        }

        TrophyViewer* TrophyWindow = new TrophyViewer(trophyPath, gameTrpPath);
        TrophyWindow->show();
        connect(this->parent(), &QWidget::destroyed, TrophyWindow,
                [TrophyWindow]() { TrophyWindow->deleteLater(); });
    });

    connect(ui->SaveManagerButton, &QPushButton::pressed, this, [this]() {
        if (!CheckBBInstall())
            return;

        if (Common::game_serial == "CUSA03173") {
            if (!std::filesystem::exists(Common::SaveDir / "1" / "CUSA00207" / "SPRJ0005" /
                                         "userdata0010")) {
                QMessageBox::warning(
                    this, "No saves detected",
                    "Launch Bloodborne to generate saves before using Save Manager");
                return;
            }
        } else if (Common::game_serial == "CUSA03023") {
            if (!std::filesystem::exists(Common::SaveDir / "1" / "CUSA01363" / "SPRJ0005" /
                                         "userdata0010")) {
                QMessageBox::warning(
                    this, "No saves detected",
                    "Launch Bloodborne to generate saves before using Save Manager");
                return;
            }
        } else if (!std::filesystem::exists(Common::SaveDir / "1" / Common::game_serial /
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
        CheatsPatches* cheatsPatches = new CheatsPatches(m_ipc_client, Config::GameRunning, this);
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
        if (!CheckBBInstall())
            return;

        if (!std::filesystem::exists(Common::GetShadUserDir() / "config.toml")) {
            QMessageBox::warning(
                this, "No global config file found",
                QString::fromStdString((Common::GetShadUserDir() / "config.toml").string() +
                                       " not found. Run shadPS4 once to generate it."));
            return;
        }
        ShadSettings* ShadSettingsWindow = new ShadSettings(m_ipc_client, false, this);
        ShadSettingsWindow->exec();
    });

    connect(ui->shadSettingsGSButton, &QPushButton::pressed, this, [this]() {
        if (!CheckBBInstall())
            return;

        std::string filename = Common::game_serial + ".toml";
        std::filesystem::path gsConfig = Common::GetShadUserDir() / "custom_configs" / filename;

        if (!std::filesystem::exists(gsConfig)) {
            if (QMessageBox::Yes ==
                QMessageBox::question(this, "No game-specific config file found",
                                      QString::fromStdString(gsConfig.string()) +
                                          " not found. Do you want to create it?",
                                      QMessageBox::Yes | QMessageBox::No)) {
                Config::CreateGSFile();
            } else {
                return;
            }
        }

        ShadSettings* ShadSettingsWindow = new ShadSettings(m_ipc_client, true, this);
        ShadSettingsWindow->exec();
    });

    connect(ui->LauncherSettingsButton, &QPushButton::pressed, this, [this]() {
        LauncherSettings* LauncherSettingsWindow = new LauncherSettings(this);
        LauncherSettingsWindow->exec();
        UpdateSettingsList();
        UpdateIcons();
    });

    connect(ui->KBMButton, &QPushButton::clicked, this, [this]() {
        KBMSettings* KBMWindow = new KBMSettings(m_ipc_client, this);
        KBMWindow->exec();
    });

    connect(ui->ControllerButton, &QPushButton::clicked, this, [this]() {
        ControlSettings* RemapWindow = new ControlSettings(m_ipc_client, this);
        RemapWindow->exec();
    });

    connect(ui->ModFolderButton, &QPushButton::clicked, this, [this]() {
        if (!std::filesystem::exists(Common::ModPath)) {
            std::filesystem::create_directories(Common::ModPath);
        }

        QString ModFolder;
        Common::PathToQString(ModFolder, Common::ModPath);
        QDir(ModFolder).mkpath(ModFolder);
        QDesktopServices::openUrl(QUrl::fromLocalFile(ModFolder));
    });

    connect(ui->HotkeyButton, &QPushButton::clicked, this, [this]() {
        Hotkeys* HKWindow = new Hotkeys(m_ipc_client, this);
        HKWindow->exec();
    });

    if (Config::AutoUpdateEnabled) {
        auto checkUpdate = new CheckUpdate(false);
        checkUpdate->exec();
    }

    if (!noGUI && Config::AutoUpdateShadEnabled) {
        auto checkShadUpdate = new CheckShadUpdate(true);
        checkShadUpdate->exec();
    }

#if defined(FORCE_UAC) || defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    if (Config::CheckPortableSettings) {

        if (!std::filesystem::exists(Common::GetCurrentPath() / "user") &&
            std::filesystem::exists(Common::shadPs4Executable.parent_path() / "user")) {
            if (QMessageBox::Yes ==
                QMessageBox::question(
                    this, "Settings not linked",
                    "Portable settings folder (user folder) found in shadPS4 folder but not in "
                    "BBLauncher folder. Create symlink to shadPS4 portable settings "
                    "folder (Automatically syncs shadPS4 and BBLauncher settings)?",
                    QMessageBox::Yes | QMessageBox::No)) {

                try {
                    std::filesystem::create_directory_symlink(
                        Common::shadPs4Executable.parent_path() / "user",
                        Common::GetCurrentPath() / "user");
                } catch (const std::filesystem::filesystem_error& e) {
                    std::cerr << "Error creating directory symlink: " << e.what() << std::endl;
                }
            }
        }

        if (std::filesystem::exists(Common::GetCurrentPath() / "user") &&
            !std::filesystem::exists(Common::shadPs4Executable.parent_path() / "user")) {
            if (QMessageBox::Yes ==
                QMessageBox::question(
                    this, "Settings not linked",
                    "Portable settings folder found in BBLauncher folder but not in "
                    "shadPS4 folder. Create symlink to BBLauncher portable settings "
                    "folder (Automatically syncs shadPS4 and BBLauncher settings)?",
                    QMessageBox::Yes | QMessageBox::No)) {

                try {
                    std::filesystem::create_directory_symlink(
                        Common::GetCurrentPath() / "user",
                        Common::shadPs4Executable.parent_path() / "user");
                } catch (const std::filesystem::filesystem_error& e) {
                    std::cerr << "Error creating directory symlink: " << e.what() << std::endl;
                }
            }
        }
    }
#endif

    if (noGUI && noInstanceRunning)
        StartGameWithArgs({});
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
#elif _WIN32
    ShadLoc = QFileDialog::getOpenFileName(this, "Select ShadPS4 executable (ex. shadPS4.exe)",
                                           QDir::homePath(), "shadPS4 exe (shadPS4.exe)");
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

void BBLauncher::StartBackupSave() {
    save_backups = true;
    const std::filesystem::path BackupPath = Common::GetBBLFilesPath() / "SaveBackups";

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

    auto save_dir = Common::SaveDir / "1" / Common::game_serial;
    if (Common::game_serial == "CUSA03173")
        save_dir = Common::SaveDir / "1" / "CUSA00207";
    if (Common::game_serial == "CUSA03023")
        save_dir = Common::SaveDir / "1" / "CUSA01363";

    auto backup_dir = BackupPath / "BACKUP1";

    while (true) {
        std::this_thread::sleep_for(std::chrono::minutes(Config::BackupInterval));

        if (!save_backups)
            return;

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
    QString CheckPortableSetting =
        "Check portable settings folder = " + QVariant(CheckPortableSettings).toString();

    QString BackupIntSetting;
    QString BackupNumSetting;

    if (BackupSaveEnabled) {
        BackupIntSetting = "Backup Interval = " + QString::number(BackupInterval);
        BackupNumSetting = "Backup Copies = " + QString::number(BackupNumber);
    } else {
        BackupIntSetting = "Backup Interval = disabled";
        BackupNumSetting = "Backup Copies = disabled";
    }

    QStringList SettingStrings = {SoundhackSetting,    ThemeSetting,     BackupEnableSetting,
                                  BackupIntSetting,    BackupNumSetting, AutoUpdateSetting,
                                  CheckPortableSetting};

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
            "No mods active.\n\nPlace mods in the Mods Folder (click Mods Button) and "
            "activate them using the Mod Manager";
        ActiveModStringList.append(NoModMsg);
    }

    ui->ModList->addItems(ActiveModStringList);
}

void BBLauncher::GetShadExecutable() {

    Common::shadPs4Executable = "";

#ifdef _WIN32
    QMessageBox::warning(this, "No shadPS4.exe found",
                         "Select ShadPS4 executable before using BB Launcher.");
    QString ShadLoc =
        QFileDialog::getOpenFileName(this, "Select ShadPS4 executable (ex. shadPS4.exe)",
                                     QDir::homePath(), "shadPS4 exe (shadPS4.exe)");
    Common::shadPs4Executable = std::filesystem::path(ShadLoc.toStdWString());
#elif defined(__linux__)
    QMessageBox::warning(this, "No ShadPS4 path selected",
                         "Select ShadPS4 path before using BB Launcher.");

    QString ShadLoc = QFileDialog::getOpenFileName(
        this,
        "Select ShadPS4 executable (ex. /usr/bin/shadps4, "
        "Shadps4-qt.AppImage, etc.)",
        QDir::homePath(),
        "QT AppImage (Shadps4-qt.AppImage);;SDL AppImage (Shadps4-sdl.AppImage);;"
        "non-AppImage (shadps4)",
        0, QFileDialog::DontUseNativeDialog);
    const std::string ShadLocString = ShadLoc.toStdString();
    Common::shadPs4Executable = std::filesystem::path(ShadLocString);
#elif defined(__APPLE__)
    QMessageBox::warning(this, "No ShadPS4 path selected",
                         "Select ShadPS4 path before using BB Launcher.");
    QString ShadLoc =
        QFileDialog::getOpenFileName(this, "Select ShadPS4 executable (ex. shadps4.app)",
                                     QDir::homePath(), "App Bundle (shadps4.app)");
    const std::string ShadLocString = ShadLoc.toStdString();
    Common::shadPs4Executable = std::filesystem::path(ShadLocString);
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
    ui->ControllerButton->setIcon(QIcon(":controller_icon.png"));
    ui->ControllerButton->setIconSize(QSize(48, 48));
    ui->KBMButton->setIcon(QIcon(":keyboard.png"));
    ui->KBMButton->setIconSize(QSize(48, 48));
    ui->ModFolderButton->setIcon(QIcon(":mod_folder.png"));
    ui->ModFolderButton->setIconSize(QSize(48, 48));
    ui->HotkeyButton->setIcon(QIcon(":hotkey.png"));
    ui->HotkeyButton->setIconSize(QSize(48, 48));

    if (Config::theme == "Dark") {
        ui->ControllerButton->setIcon(RecolorIcon(ui->ControllerButton->icon(), false));
        ui->KBMButton->setIcon(RecolorIcon(ui->KBMButton->icon(), false));
        ui->ModFolderButton->setIcon(RecolorIcon(ui->ModFolderButton->icon(), false));
        ui->HotkeyButton->setIcon(RecolorIcon(ui->HotkeyButton->icon(), false));
    }
}

QIcon BBLauncher::RecolorIcon(const QIcon& icon, bool isWhite) {
    QPixmap pixmap(icon.pixmap(icon.actualSize(QSize(120, 120))));
    QColor clr(isWhite ? Qt::white : Qt::black);
    QBitmap mask = pixmap.createMaskFromColor(clr, Qt::MaskOutColor);
    pixmap.fill(QColor(isWhite ? Qt::black : Qt::white));
    pixmap.setMask(mask);
    return QIcon(pixmap);
}

void BBLauncher::onGameClosed() {
    Config::GameRunning = false;
    is_paused = false;
    save_backups = false;

    if (noGUIset)
        QApplication::quit();
}

void BBLauncher::RunGame() {
    auto patches = readPatches(Common::game_serial, "01.09");
    for (auto patch : patches) {
        m_ipc_client->sendMemoryPatches(patch.modName, patch.address, patch.value, patch.target,
                                        patch.size, patch.maskOffset, patch.littleEndian,
                                        patch.mask, patch.maskOffset);
    }

    m_ipc_client->startGame();
}

void BBLauncher::RestartEmulator() {
    QString exe;
    Common::PathToQString(exe, Common::shadPs4Executable);
    QStringList args{"--game",
                     QString::fromStdWString((Common::installPath / "eboot.bin").wstring())};

    if (m_ipc_client->parsedArgs.size() > 0) {
        args.clear();
        for (auto arg : m_ipc_client->parsedArgs) {
            args.append(QString::fromStdString(arg));
        }
        m_ipc_client->parsedArgs.clear();
    }

    QFileInfo fileInfo(exe);
    QString workDir = fileInfo.absolutePath();

    m_ipc_client->startEmulator(fileInfo, args, workDir);
}

std::vector<MemoryPatcher::PendingPatch> BBLauncher::readPatches(std::string gameSerial,
                                                                 std::string appVersion) {
    std::vector<MemoryPatcher::PendingPatch> pending;
    QString patchDir;
    Common::PathToQString(patchDir, (Common::GetShadUserDir() / "patches"));
    QDir dir(patchDir);
    QStringList folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& folder : folders) {
        QString filesJsonPath = patchDir + "/" + folder + "/files.json";

        QFile jsonFile(filesJsonPath);
        if (!jsonFile.open(QIODevice::ReadOnly)) {
            // LOG_ERROR(Loader, "Unable to open files.json for reading in repository {}",
            //         folder.toStdString());
            continue;
        }
        const QByteArray jsonData = jsonFile.readAll();
        jsonFile.close();

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
        const QJsonObject jsonObject = jsonDoc.object();

        QString selectedFileName;
        const QString serial = QString::fromStdString(gameSerial);

        for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it) {
            const QString filePath = it.key();
            const QJsonArray idsArray = it.value().toArray();
            if (idsArray.contains(QJsonValue(serial))) {
                selectedFileName = filePath;
                break;
            }
        }

        if (selectedFileName.isEmpty()) {
            // LOG_ERROR(Loader, "No patch file found for the current serial in repository {}",
            //       folder.toStdString());
            continue;
        }

        const QString filePath = patchDir + "/" + folder + "/" + selectedFileName;
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // LOG_ERROR(Loader, "Unable to open the file for reading.");
            continue;
        }
        const QByteArray xmlData = file.readAll();
        file.close();

        QXmlStreamReader xmlReader(xmlData);

        bool isEnabled = false;
        std::string currentPatchName;

        bool versionMatches = true;

        while (!xmlReader.atEnd()) {
            xmlReader.readNext();

            if (!xmlReader.isStartElement()) {
                continue;
            }

            if (xmlReader.name() == QStringLiteral("Metadata")) {
                QString name = xmlReader.attributes().value("Name").toString();
                currentPatchName = name.toStdString();

                const QString appVer = xmlReader.attributes().value("AppVer").toString();

                isEnabled = false;
                for (const QXmlStreamAttribute& attr : xmlReader.attributes()) {
                    if (attr.name() == QStringLiteral("isEnabled")) {
                        isEnabled = (attr.value().toString() == "true");
                    }
                }
                versionMatches = (appVer.toStdString() == appVersion);
            } else if (xmlReader.name() == QStringLiteral("PatchList")) {
                while (!xmlReader.atEnd() &&
                       !(xmlReader.tokenType() == QXmlStreamReader::EndElement &&
                         xmlReader.name() == QStringLiteral("PatchList"))) {

                    xmlReader.readNext();

                    if (xmlReader.tokenType() != QXmlStreamReader::StartElement ||
                        xmlReader.name() != QStringLiteral("Line")) {
                        continue;
                    }

                    const QXmlStreamAttributes a = xmlReader.attributes();
                    const QString type = a.value("Type").toString();
                    const QString addr = a.value("Address").toString();
                    QString val = a.value("Value").toString();
                    const QString offStr = a.value("Offset").toString();
                    const QString tgt =
                        (type == "mask_jump32") ? a.value("Target").toString() : QString{};
                    const QString sz =
                        (type == "mask_jump32") ? a.value("Size").toString() : QString{};

                    if (!isEnabled) {
                        continue;
                    }
                    if ((type != "mask" && type != "mask_jump32") && !versionMatches) {
                        continue;
                    }

                    MemoryPatcher::PendingPatch pp;
                    pp.modName = currentPatchName;
                    pp.address = addr.toStdString();

                    if (type == "mask" || type == "mask_jump32") {
                        if (!offStr.toStdString().empty()) {
                            pp.maskOffset = std::stoi(offStr.toStdString(), nullptr, 10);
                        }
                        pp.mask = (type == "mask") ? MemoryPatcher::PatchMask::Mask
                                                   : MemoryPatcher::PatchMask::Mask_Jump32;
                        pp.value = val.toStdString();
                        pp.target = tgt.toStdString();
                        pp.size = sz.toStdString();
                    } else {
                        pp.value =
                            MemoryPatcher::convertValueToHex(type.toStdString(), val.toStdString());
                        pp.target.clear();
                        pp.size.clear();
                        pp.mask = MemoryPatcher::PatchMask::None;
                        pp.maskOffset = 0;
                    }

                    pp.littleEndian = (type == "bytes16" || type == "bytes32" || type == "bytes64");
                    pending.emplace_back(std::move(pp));
                }
            }
        }

        if (xmlReader.hasError()) {
            // LOG_ERROR(Loader, "Failed to parse XML for {}", gameSerial);
        } else {
            // LOG_INFO(Loader, "Patches parsed successfully, repository: {}",
            // folder.toStdString());
        }
    }
    return pending;
}

void BBLauncher::StartGameWithArgs(QStringList args) {
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
        if (Common::game_serial == "CUSA03173")
            savePath = Common::SaveDir / "1" / "CUSA00207" / "SPRJ0005";

        if (Common::game_serial == "CUSA03023")
            savePath = Common::SaveDir / "1" / "CUSA01363" / "SPRJ0005";

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

    if (EbootPath != "") {
        StartEmulator(EbootPath, args);
    }
}

void BBLauncher::StartEmulator(std::filesystem::path path, QStringList args) {
    if (Config::GameRunning) {
        QMessageBox::critical(nullptr, tr("Run Game"), QString(tr("Game is already running!")));
        return;
    }

    QString exe;
    Common::PathToQString(exe, Common::shadPs4Executable);
    QFileInfo fileInfo(exe);
    if (!fileInfo.exists()) {
        QMessageBox::critical(
            nullptr, tr("shadPS4"),
            QString(tr("shadPS4 is not found!\nPlease change shadPS4 path in settings.")));
        return;
    }

    QStringList final_args{"--game", QString::fromStdWString(path.wstring())};
    final_args.append(args);
    QString workDir = fileInfo.absolutePath();
    m_ipc_client->startEmulator(fileInfo, final_args, workDir);

    Config::GameRunning = true;
}

BBLauncher::~BBLauncher() {
    delete ui;
}
