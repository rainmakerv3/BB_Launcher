// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>
#include <QMainWindow>

void PathToQString(QString& result, const std::filesystem::path& path);

QT_BEGIN_NAMESPACE
namespace Ui {
class BBLauncher;
}
QT_END_NAMESPACE

const std::vector<std::string> BBSerialList = {"CUSA03173", "CUSA00900", "CUSA00208",
                                               "CUSA00207", "CUSA01363", "CUSA03023"};

extern std::filesystem::path userPath;
extern std::string game_serial;
extern std::filesystem::path installPath;
extern std::string installPathString;
extern std::filesystem::path EbootPath;

void StartBackupSave();
std::filesystem::path GetShadUserDir();

class BBLauncher : public QMainWindow {
    Q_OBJECT

public:
    BBLauncher(QWidget* parent = nullptr);
    ~BBLauncher();
    // bool eventFilter(QObject* obj, QEvent* event);

public slots:
    void UpdateSettingsList();
    void UpdateModList();

private slots:
    void ExeSelectButton_isPressed();
    void WIPButton_isPressed();
    void LaunchButton_isPressed();
    static void startShad();
    void SaveInstallLoc();
    void CheckBBInstall();

private:
    Ui::BBLauncher* ui;
};
