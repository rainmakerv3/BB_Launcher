// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>
#include <QMainWindow>

#include "modules/ipc/ipc_client.h"

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
    void PrintLog(QString entry, std::string type);

private:
    static void StartBackupSave();
    bool CheckBBInstall();
    void UpdateSettingsList();
    void UpdateModList();
    void GetShadExecutable();
    QIcon RecolorIcon(const QIcon& icon, bool isWhite);
    void UpdateIcons();
    void RunGame();
    void RestartEmulator();
    void StartGameWithArgs(QStringList args);
    void StartEmulator(std::filesystem::path path, QStringList args);
    std::vector<MemoryPatcher::PendingPatch> readPatches(std::string gameSerial,
                                                         std::string appVersion);
    QString getPatchFile();

    Ui::BBLauncher* ui;
    std::shared_ptr<IpcClient> m_ipc_client = std::make_shared<IpcClient>();

    std::filesystem::path shadPs4Directory;
    bool noGUIset;
    bool noinstancerunning;
    bool is_paused;

    const std::vector<std::string> BBSerialList = {"CUSA03173", "CUSA00900", "CUSA00208",
                                                   "CUSA00207", "CUSA01363", "CUSA03023",
                                                   "CUSA00299", "CUSA03014"};
};
