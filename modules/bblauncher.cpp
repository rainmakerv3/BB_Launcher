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
#include "settings/PSF/psf.h"
#include "settings/ShadCheatsPatches.h"
#include "settings/ShadSettings.h"
#include "settings/config.h"
#include "settings/control_settings.h"
#include "settings/hotkeys.h"
#include "settings/kbm_gui.h"
#include "settings/updater/CheckUpdate.h"
#include "version_dialog.h"

static bool save_backups = false;

BBLauncher::BBLauncher(bool noGUI, bool noInstanceRunning, QWidget* parent)
    : QMainWindow(parent), noGUIset(noGUI), noinstancerunning(noInstanceRunning),
      ui(new Ui::BBLauncher) {

    m_ipc_client->gameClosedFunc = [this]() { onGameClosed(); };
    m_ipc_client->restartEmulatorFunc = [this]() { RestartEmulator(); };
    m_ipc_client->startGameFunc = [this]() { RunGame(); };

    ui->setupUi(this);
    Config::LoadSettings();

    if (Common::shadPs4Executable == "" || !std::filesystem::exists(Common::shadPs4Executable)) {
        QMessageBox::warning(this, "No shadPS4 build",
                             "ShadPS4 build is either not selected or cannot be found. BBLauncher "
                             "requires a valid shadPS4 build, you can download builds "
                             "with the 'Manage Builds' button");
        if (!std::filesystem::exists(Common::shadPs4Executable)) {
            Common::shadPs4Executable = "";
            Config::SaveLauncherSettings();
        }
    }

    if (Common::installPath == "") {
        ui->ExeLabel->setText("no Bloodborne Folder selected (CUSA****)");
    } else if (!std::filesystem::exists(Common::installPath)) {
        QMessageBox::warning(this, "Selected Bloodborne folder not found",
                             "Previously set Bloodborne folder cannot be found. It may have been "
                             "moved or deleted. Bloodborne folder path will be reset.");
        Common::installPath = "";
        Config::SaveLauncherSettings();
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

    connect(ui->BBSelectButton, &QPushButton::pressed, this, &BBLauncher::BBSelectButton_isPressed);
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

        // Releases older than 0.9.0 will need to use the game serial as save folder
        std::string savePath = Config::isReleaseOlder(9)
                                   ? Common::game_serial
                                   : PSFdata::getSavePath(Common::installPath);
        std::filesystem::path saveFile =
            Common::GetSaveDir() / "1" / savePath / "SPRJ0005" / "userdata0010";

        if (!std::filesystem::exists(saveFile)) {
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

        if (Config::isReleaseOlder(11)) {
            QMessageBox::warning(
                this, "Old release not supported",
                "Releases older than v0.11.0 do not support game-specific configs");
            return;
        }

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
        if (Config::isReleaseOlder(7)) {
            QMessageBox::warning(this, "Old release not supported",
                                 "Releases older than v0.7.0 do not support input remapping");
            return;
        }

        KBMSettings* KBMWindow = new KBMSettings(m_ipc_client, this);
        KBMWindow->exec();
    });

    connect(ui->ControllerButton, &QPushButton::clicked, this, [this]() {
        if (Config::isReleaseOlder(7)) {
            QMessageBox::warning(this, "Old release not supported",
                                 "Releases older than v0.7.0 do not support input remapping");
            return;
        }

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
        if (Config::isReleaseOlder(11)) {
            QMessageBox::warning(this, "Old release not supported",
                                 "Releases older than v0.11.0 do not support custom hotkeys");
            return;
        }

        Hotkeys* HKWindow = new Hotkeys(m_ipc_client, this);
        HKWindow->exec();
    });

    if (Config::AutoUpdateEnabled) {
        auto checkUpdate = new CheckUpdate(false, this);
        checkUpdate->exec();
    }

    if (Config::AutoUpdateShadEnabled) {
        auto versionDialog = new VersionDialog(this);
        versionDialog->checkUpdatePre(false);
    }

    if (Config::AutoUpdateVersionsEnabled) {
        auto versionDialog = new VersionDialog(this);
        versionDialog->CheckVersionsList(false);
    }

    if (noGUI && noInstanceRunning)
        StartGameWithArgs({});
}

void BBLauncher::BBSelectButton_isPressed() {
    QString QBBInstallLoc = "";
    QBBInstallLoc = QFileDialog::getExistingDirectory(
        this, "Select Bloodborne install location (ex. CUSA03173, CUSA00900", QDir::currentPath());
    if (QBBInstallLoc != "") {
        Common::game_serial = QBBInstallLoc.last(9).toStdString();
        if (std::find(BBSerialList.begin(), BBSerialList.end(), Common::game_serial) !=
            BBSerialList.end()) {
            ui->ExeLabel->setText(QBBInstallLoc);
            Common::installPath = Common::PathFromQString(QBBInstallLoc);
            Config::SaveLauncherSettings();
        } else {
            QMessageBox::warning(
                this, "Install Location not valid",
                "Select valid BB Install folder starting with CUSA (ex: CUSA03173, CUSA00900)");
        }
    }
}

void BBLauncher::ShadSelectButton_isPressed() {
    auto BuildWindow = new VersionDialog(this);

    connect(BuildWindow, &QDialog::finished, this, [this]() {
        QString shadLabelString;
        Common::PathToQString(shadLabelString, Common::shadPs4Executable);
        ui->ShadLabel->setText(shadLabelString);
    });

    BuildWindow->exec();
}

void BBLauncher::GetShadExecutable() {
    Common::shadPs4Executable = "";
    Config::SaveLauncherSettings();

    QMessageBox::warning(
        this, "No shadPS4.exe found",
        "Select ShadPS4 executable using the 'Manage builds'button before using BB Launcher.");
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

    // Releases older than 0.9.0 will need to use the game serial as save folder
    std::string savePath =
        Config::isReleaseOlder(9) ? Common::game_serial : PSFdata::getSavePath(Common::installPath);
    std::filesystem::path save_dir = Common::GetSaveDir() / "1" / savePath / "SPRJ0005";

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
    QString SoundhackSetting = "60 FPS sound fix: " + QVariant(SoundFixEnabled).toString();
    QString BackupEnableSetting =
        "Back up saves enabled: " + QVariant(BackupSaveEnabled).toString();

    QString AutoUpdateBblauncherSetting =
        "Auto-update BBLauncher: " + QVariant(AutoUpdateEnabled).toString();
    QString AutoUpdateShadSetting =
        "Auto-update shadPS4 pre-release: " + QVariant(AutoUpdateShadEnabled).toString();
    QString AutoUpdateVersionsSetting =
        "Auto-update shadPS4 version list: " + QVariant(AutoUpdateVersionsEnabled).toString();

    QString Location = PortableFolderinLauncherFolder ? "Launcher Folder" : "Build Folder";
    QString PortableFolderSetting = "Portable Folder Location: " + Location;

    QString BackupIntSetting;
    QString BackupNumSetting;

    if (BackupSaveEnabled) {
        BackupIntSetting = "Backup interval: " + QString::number(BackupInterval);
        BackupNumSetting = "Backup copies: " + QString::number(BackupNumber);
    } else {
        BackupIntSetting = "Backup interval: disabled";
        BackupNumSetting = "Backup copies: disabled";
    }

    QStringList SettingStrings = {SoundhackSetting,      PortableFolderSetting,
                                  BackupEnableSetting,   BackupIntSetting,
                                  BackupNumSetting,      AutoUpdateBblauncherSetting,
                                  AutoUpdateShadSetting, AutoUpdateVersionsSetting};

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
        std::filesystem::path savePath =
            Common::GetSaveDir() / "1" / Common::game_serial / "SPRJ0005";
        if (Common::game_serial == "CUSA03173")
            savePath = Common::GetSaveDir() / "1" / "CUSA00207" / "SPRJ0005";

        if (Common::game_serial == "CUSA03023")
            savePath = Common::GetSaveDir() / "1" / "CUSA01363" / "SPRJ0005";

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

    // Releases older than 0.11.0 will active patch file appended
    bool usePatchFile = false;
    Config::Build CurrentBuild = Config::GetCurrentBuildInfo();
    if (CurrentBuild.type == "Release") {
        static QRegularExpression versionRegex(R"(v\.?(\d+)\.(\d+)\.(\d+))");
        QRegularExpressionMatch match = versionRegex.match(QString::fromStdString(CurrentBuild.id));
        if (match.hasMatch()) {
            int major = match.captured(1).toInt();
            int minor = match.captured(2).toInt();
            int patch = match.captured(3).toInt();

            if (major > 0)
                usePatchFile = true;
            if (major == 0 && minor < 11)
                usePatchFile = true;
        }
    }

    QStringList gameArgs{"--game", QString::fromStdWString(path.wstring())};
    QStringList patchArgs{"--patch", getPatchFile()};
    QStringList final_args;

    if (!usePatchFile) {
        final_args.append(args);
        final_args.append(gameArgs);
    } else {
        final_args.append(args);
        final_args.append(gameArgs);
        final_args.append(patchArgs);
    }

    QString workDir = fileInfo.absolutePath();
    m_ipc_client->startEmulator(fileInfo, final_args, workDir);

    Config::GameRunning = true;
}

QString BBLauncher::getPatchFile() {
    QString patchDir;
    Common::PathToQString(patchDir, (Common::GetShadUserDir() / "patches"));
    QDir dir(patchDir);
    QStringList folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString patchFile = "";

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
        const QString serial = QString::fromStdString(Common::game_serial);
        QString selectedFileName;

        for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it) {
            const QString filePath = it.key();
            const QJsonArray idsArray = it.value().toArray();
            if (idsArray.contains(QJsonValue(serial))) {
                selectedFileName = filePath;
                break;
            }
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

        while (!xmlReader.atEnd()) {
            xmlReader.readNext();

            if (!xmlReader.isStartElement()) {
                continue;
            }

            if (xmlReader.name() == QStringLiteral("Metadata")) {
                isEnabled = false;
                for (const QXmlStreamAttribute& attr : xmlReader.attributes()) {
                    if (attr.name() == QStringLiteral("isEnabled")) {
                        isEnabled = (attr.value().toString() == "true");
                        if (isEnabled) {
                            patchFile = filePath;
                            break;
                        }
                    }
                }
            }

            if (patchFile != "")
                break;
        }

        if (patchFile != "")
            break;
    }

    return patchFile;
}

BBLauncher::~BBLauncher() {
    delete ui;
}
