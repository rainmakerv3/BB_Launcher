// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>
#include <QMainWindow>

namespace Ui {
class BBLauncher;
}

class BBLauncher : public QMainWindow {
    Q_OBJECT

public:
    BBLauncher(bool noGUI, bool noInstanceRunning, QWidget* parent = nullptr);
    ~BBLauncher();

    bool canLaunch = true;
    // bool eventFilter(QObject* obj, QEvent* event);

public slots:

private slots:
    void ExeSelectButton_isPressed();
    void ShadSelectButton_isPressed();
    void WIPButton_isPressed();
    void LaunchButton_isPressed(bool noGUIset);

private:
    Ui::BBLauncher* ui;
    std::filesystem::path shadPs4Directory;
    bool noGUIset;
    bool noinstancerunning;
    static void StartBackupSave();
    bool CheckBBInstall();
    void UpdateSettingsList();
    void UpdateModList();
    void GetShadExecutable();
    QIcon RecolorIcon(const QIcon& icon, bool isWhite);
    void UpdateIcons();

    const std::vector<std::string> BBSerialList = {"CUSA03173", "CUSA00900", "CUSA00208",
                                                   "CUSA00207", "CUSA01363", "CUSA03023"};
};
