// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>

#include "modules/QAnsiTextEdit.h"
#include "modules/ipc/ipc_client.h"
#include "settings/emulator_settings.h"
#include "settings/user_settings.h"

namespace Ui {
class BBLauncher;
}

class BBLauncher : public QMainWindow {
    Q_OBJECT

public:
    BBLauncher(bool noGUI, bool noInstanceRunning, QWidget* parent = nullptr);
    ~BBLauncher();

    bool canLaunch = true;

public slots:

private slots:
    void BBSelectButton_isPressed();
    void ShadSelectButton_isPressed();
    void onGameClosed();
    void PrintLog(QString entry);
    void OpenFolders();

private:
    QVBoxLayout* createIconTextButtonLayout(const QString& resourcePath, const QString& buttonText,
                                            QPushButton* button);
    static void StartBackupSave();
    bool CheckBBInstall();
    void UpdatePatchesList();
    void UpdateModList();
    void LogSettings();
    void GetShadExecutable();
    QIcon RecolorIcon(const QIcon& icon, bool isWhite);
    void UpdateIcons();
    void RunGame();
    void RestartEmulator();
    void StartGameWithArgs(QStringList args);
    void StartEmulator(std::filesystem::path path, QStringList args);
    std::vector<MemoryPatcher::PendingPatch> readPatches(std::string gameSerial,
                                                         std::string appVersion);

    Ui::BBLauncher* ui;
    QAnsiTextEdit* logDisplay;
    std::shared_ptr<IpcClient> m_ipc_client = std::make_shared<IpcClient>();
    std::shared_ptr<EmulatorSettingsImpl> m_emu_settings = std::make_shared<EmulatorSettingsImpl>();
    std::shared_ptr<UserSettingsImpl> m_user_settings = std::make_shared<UserSettingsImpl>();

    std::filesystem::path shadPs4Directory;
    bool noGUIset;
    bool noinstancerunning;
    bool is_paused;

    QPushButton* modManagerButton = new QPushButton(this);
    QPushButton* modDownloaderButton = new QPushButton(this);
    QPushButton* patchesButton = new QPushButton(this);
    QPushButton* shadSettingsButton = new QPushButton(this);
    QPushButton* launcherSettingsButton = new QPushButton(this);
    QPushButton* saveViewerButton = new QPushButton(this);

    QPushButton* launchButton = new QPushButton(this);
    QPushButton* stopButton = new QPushButton(this);
    QPushButton* restartButton = new QPushButton(this);
    QPushButton* fullscreenButton = new QPushButton(this);

    const std::vector<std::string> BBSerialList = {"CUSA03173", "CUSA00900", "CUSA00208",
                                                   "CUSA00207", "CUSA01363", "CUSA03023",
                                                   "CUSA00299", "CUSA03014"};
};
