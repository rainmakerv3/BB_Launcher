#include <filesystem>
#include <QDialog>

namespace Ui {
class LauncherSettings;
}

void LoadLauncherSettings();
void CreateSettingsFile();
void SetTheme(std::string theme);

const QStringList BackupNumList = {"1", "2", "3", "4", "5"};
const QStringList BackupFreqList = {"5", "10", "15", "20", "25", "30"};

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
    void SetLauncherDefaults();
    void SaveAndCloseLauncherSettings();
    void SaveLauncherSettings();

private:
    Ui::LauncherSettings* ui;
    void OnBackupStateChanged();
};
