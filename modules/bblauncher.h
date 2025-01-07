#pragma once

#include <filesystem>
#include <QMainWindow>

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
extern std::filesystem::path PKGPath;

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

private:
    Ui::BBLauncher* ui;
};
