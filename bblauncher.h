#pragma once

#include <filesystem>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class BBLauncher;
}
QT_END_NAMESPACE

extern std::filesystem::path userPath;
extern std::string game_serial;
extern std::filesystem::path installPath;
extern std::string installPathString;
extern std::filesystem::path PKGPath;

void StartBackupSave();

class BBLauncher : public QMainWindow {
    Q_OBJECT

public:
    BBLauncher(QWidget* parent = nullptr);
    ~BBLauncher();

private slots:
    void ExeSelectButton_isPressed();
    void ModManagerButton_isPressed();
    void LaunchButton_isPressed();
    static void startShad();
    void SaveInstallLoc();

private:
    Ui::BBLauncher* ui;
};
