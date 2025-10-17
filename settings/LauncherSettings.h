// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDialog>
#include <SDL3/SDL_gamepad.h>

namespace Ui {
class LauncherSettings;
}

class LauncherSettings : public QDialog {
    Q_OBJECT

public:
    explicit LauncherSettings(QWidget* parent = nullptr);
    ~LauncherSettings();

public slots:

private slots:
    void SetLauncherDefaults();
    void SaveAndCloseLauncherSettings();
    void SaveLauncherSettings();

private:
    Ui::LauncherSettings* ui;
    void OnBackupStateChanged();
    void CreateShortcut();
    bool convertPngToIco(const QString& pngFilePath, const QString& icoFilePath);

#ifdef Q_OS_WIN
    bool createShortcutWin(const QString& linkPath, const QString& iconPath,
                           const QString& exePath);
#else
    bool createShortcutLinux(const QString& linkPath, const std::string& name,
                             const QString& iconPath);
#endif

    const QStringList BackupNumList = {"1", "2", "3", "4", "5"};
    const QStringList BackupFreqList = {"5", "10", "15", "20", "25", "30"};
};

namespace GamepadSelect {

int GetIndexfromGUID(SDL_JoystickID* gamepadIDs, int gamepadCount, std::string GUID);
std::string GetGUIDString(SDL_JoystickID* gamepadIDs, int index);
std::string GetSelectedGamepad();
void SetSelectedGamepad(std::string GUID);

} // namespace GamepadSelect
