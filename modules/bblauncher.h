// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>
#include <QMainWindow>

namespace Ui {
class BBLauncher;
}

void PathToQString(QString& result, const std::filesystem::path& path);
std::filesystem::path PathFromQString(const QString& path);
std::string PathToU8(const std::filesystem::path& path);
std::filesystem::path GetShadUserDir();

extern std::string game_serial;
extern std::filesystem::path installPath;
extern std::string installPathString;
extern std::filesystem::path EbootPath;
extern std::filesystem::path SaveDir;
extern char VERSION[];
extern std::filesystem::path shadPs4Executable;

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
    void SaveConfigOption(std::string configKey, std::string configValue);
    bool CheckBBInstall();
    void UpdateSettingsList();
    void UpdateModList();
    void GetShadExecutable();

    const std::vector<std::string> BBSerialList = {"CUSA03173", "CUSA00900", "CUSA00208",
                                                   "CUSA00207", "CUSA01363", "CUSA03023"};
};
