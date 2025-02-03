// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <filesystem>
#include <QDialog>

namespace Ui {
class LauncherSettings;
}

namespace Config {

void SaveConfigPath(std::string configKey, std::filesystem::path path);
void LoadLauncherSettings();
void CreateSettingsFile();
void SetTheme(std::string theme);
std::filesystem::path GetFoolproofKbmConfigFile(const std::string& game_id);
std::string_view GetDefaultKeyboardConfig();
void SaveUnifiedControl(bool setting);

extern std::string theme;
extern bool SoundFixEnabled;
extern bool BackupSaveEnabled;
extern int BackupInterval;
extern int BackupNumber;
extern bool AutoUpdateEnabled;
extern bool UnifiedInputConfig;

} // namespace Config

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

    const QStringList BackupNumList = {"1", "2", "3", "4", "5"};
    const QStringList BackupFreqList = {"5", "10", "15", "20", "25", "30"};
};
