#include <filesystem>
#include <QDialog>

namespace Ui {
class LauncherSettings;
}

void LoadLauncherSettings();
void CreateSettingsFile();
void SetTheme(std::string theme);

const std::vector<std::string> BBSerialList = {"CUSA03173", "CUSA00900", "CUSA00299", "CUSA00207"};

extern std::filesystem::path SettingsPath;
extern std::filesystem::path SettingsFile;

extern std::string theme;
extern bool SoundFixEnabled;
extern bool BackupSaveEnabled;
extern int BackupInterval;
extern int BackupNumber;

class LauncherSettings : public QDialog {
    Q_OBJECT

public:
    explicit LauncherSettings(QWidget* parent = nullptr);
    ~LauncherSettings();

public slots:
    void SetDefaultLauncherDefaults();
    void SaveAndCloseLauncherSettings();
    void SaveLauncherSettings();

private:
    Ui::LauncherSettings* ui;
    void OnBackupStateChanged();
};
