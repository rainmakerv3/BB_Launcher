#include <fstream>
#include <thread>
#include <QFileDialog>
#include <QMessageBox>
#include "bblauncher.h"
#include "modules/ModManager.h"
#include "settings/LauncherSettings.h"
#include "settings/ShadSettings.h"
#include "settings/toml.hpp"
#include "ui_bblauncher.h"

std::string installPathString = "";
std::filesystem::path installPath = "";
std::filesystem::path PKGPath = "";
std::string game_serial = "";

BBLauncher::BBLauncher(QWidget* parent) : QMainWindow(parent), ui(new Ui::BBLauncher) {
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->statusBar()->setSizeGripEnabled(false);
    QApplication::setStyle("Fusion");

    // this->installEventFilter(this); if needed

    LoadLauncherSettings();
    UpdateSettingsList();
    UpdateModList();

    connect(ui->ExeSelectButton, &QPushButton::pressed, this,
            &BBLauncher::ExeSelectButton_isPressed);

    connect(ui->PatchesButton, &QPushButton::pressed, this, &BBLauncher::WIPButton_isPressed);
    connect(ui->LaunchButton, &QPushButton::pressed, this, &BBLauncher::LaunchButton_isPressed);
    connect(ui->TrophyButton, &QPushButton::pressed, this, &BBLauncher::WIPButton_isPressed);
    connect(ui->shadSettingsButton, &QPushButton::pressed, this, &BBLauncher::WIPButton_isPressed);
    connect(ui->SaveManagerButton, &QPushButton::pressed, this, &BBLauncher::WIPButton_isPressed);
    connect(ui->ModManagerButton, &QPushButton::pressed, this, [this]() {
        if (installPath == "") {
            QMessageBox::warning(this, "No Bloodborne install path selected",
                                 "Select Bloodborne Install Folder First before using Mod Manager");
            return;
        }
        ModManager* ModWindow = new ModManager(this);
        ModWindow->exec();
        UpdateModList();
    });
    /*connect(ui->shadSettingsButton, &QPushButton::pressed, this, [this]() {
        ShadSettings* ShadSettingsWindow = new ShadSettings(this);
        ShadSettingsWindow->exec();
    }); */
    connect(ui->LauncherSettingsButton, &QPushButton::pressed, this, [this]() {
        LauncherSettings* LauncherSettingsWindow = new LauncherSettings(this);
        LauncherSettingsWindow->exec();
        UpdateSettingsList();
    });
    ;

    if (installPathString == "") {
        const QString NoPathText = "no Bloodborne Folder selected (CUSA****)";
        ui->ExeLabel->setText(NoPathText);
    } else {
        ui->ExeLabel->setText(QString::fromStdString(installPathString));
    }
}

void BBLauncher::ExeSelectButton_isPressed() {
    QString QBBInstallLoc = "";
    QBBInstallLoc = QFileDialog::getExistingDirectory(
        this, "Select Bloodborne install location (ex. CUSA03173, CUSA00900", QDir::currentPath());
    if (QBBInstallLoc != "") {
        game_serial = QBBInstallLoc.last(9).toStdString();
        if (std::find(BBSerialList.begin(), BBSerialList.end(), game_serial) !=
            BBSerialList.end()) {
            ui->ExeLabel->setText(QBBInstallLoc);
            installPathString = QBBInstallLoc.toStdString();
            installPath = installPathString;
            PKGPath = installPath / "eboot.bin";
            SaveInstallLoc();
        } else {
            QMessageBox::warning(
                this, "Install Location not valid",
                "Select valid BB Install folder starting with CUSA (ex: CUSA03173, CUSA00900)");
        }
    }
}

void BBLauncher::WIPButton_isPressed() {
    QMessageBox::warning(this, "WIP", "Still working on this :)");
    UpdateSettingsList();
}

void BBLauncher::LaunchButton_isPressed() {
    if (installPath != "") {
        if (SoundFixEnabled) {
            std::filesystem::path savePath =
                GetShadUserDir() / "savedata" / "1" / game_serial / "SPRJ0005";
            if (std::filesystem::exists(savePath / "userdata0010.")) {
                std::ofstream savefile1;
                savefile1.open(savePath / "userdata0010.",
                               std::ios::in | std::ios::out | std::ios::binary);
                savefile1.seekp(0x204E);
                savefile1.put(0x1);
                savefile1.close();
            }
        }
        QMainWindow::hide();
        std::thread shadThread(startShad);
        shadThread.detach();
    } else {
        QMessageBox::warning(this, "No Bloodborne Install folder selected",
                             "Select valid Bloodborne Install folder first");
    }

    if (BackupSaveEnabled) {
        std::thread saveThread(StartBackupSave);
        saveThread.detach();
    }
}

void BBLauncher::startShad() {
    const std::string PKGLoc = (PKGPath).string();

#ifdef _WIN32
    const char* runBBshadPS4win = ("shadPS4.exe -g \"" + PKGLoc + "\"").c_str();
    std::system(runBBshadPS4win);
#elif defined(__linux__)
    if (std::filesystem::exists(std::filesystem::current_path() / "Shadps4-qt.AppImage")) {
        std::system("chmod +x Shadps4-qt.AppImage");
        const char* runBBshadPS4linux = ("./Shadps4-qt.AppImage -g \"" + PKGLoc + "\"").c_str();
        std::system(runBBshadPS4linux);
    } else if (std::filesystem::exists(std::filesystem::current_path() / "Shadps4-sdl.AppImage")) {
        std::system("chmod +x Shadps4-sdl.AppImage");
        const char* runBBshadPS4linux = ("./Shadps4-sdl.AppImage -g \"" + PKGLoc + "\"").c_str();
        std::system(runBBshadPS4linux);
    } else {
        const char* runBBshadPS4linux = ("./Shadps4 -g \"" + PKGLoc + "\"").c_str();
        std::system(runBBshadPS4linux);
    }
#elif defined(__APPLE__)
    const char* runBBshadPS4apple = ("open shadPS4 -g \"" + PKGLoc + "\"").c_str();
    std::system(runBBshadPS4apple);
#endif

    QApplication::quit();
}

void StartBackupSave() {
    const std::filesystem::path BackupPath =
        std::filesystem::current_path() / "BBLauncher" / "SaveBackups";

    if (!std ::filesystem::exists(BackupPath)) {
        std::filesystem::create_directory(BackupPath);
    }

    for (int i = 1; i < BackupNumber + 1; i++) {
        std::string backupstring = "BACKUP" + std::to_string(i);
        auto backup_dircreate = BackupPath / backupstring;
        if (!std ::filesystem::exists(backup_dircreate)) {
            std::filesystem::create_directory(backup_dircreate);
        }
    }

    const auto save_dir = GetShadUserDir() / "savedata" / "1" / game_serial;
    const auto backup_dir = BackupPath / "BACKUP1";

    while (true) {
        std::this_thread::sleep_for(std::chrono::minutes(BackupInterval));

        if (BackupNumber > 1) {
            const std::string lastDirstring = "BACKUP" + std::to_string(BackupNumber);
            const auto lastdir = BackupPath / lastDirstring;
            std::filesystem::remove_all(lastdir);
            for (int i = BackupNumber - 1; i > 0; i--) {
                std::string sourceString = "BACKUP" + std::to_string(i);
                std::string destString = "BACKUP" + std::to_string(i + 1);
                const auto sourceDir = BackupPath / sourceString;
                const auto destDir = BackupPath / destString;
                try {
                    std::filesystem::rename(sourceDir, destDir);
                } catch (std::exception& ex) {
                    // handle?;
                }
            }
        }

        try {
            std::filesystem::copy(save_dir, backup_dir,
                                  std::filesystem::copy_options::overwrite_existing |
                                      std::filesystem::copy_options::recursive);
        } catch (std::exception& ex) {
            // handle?;
        }
    }
}

void BBLauncher::SaveInstallLoc() {
    toml::value data;
    std::error_code error;

    if (std::filesystem::exists(SettingsFile, error)) {
        try {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(SettingsFile, std::ios_base::binary);
            data = toml::parse(SettingsFile);
        } catch (const std::exception& ex) {
            QMessageBox::critical(this, "Filesystem error", ex.what());
            return;
        }
    } else {
        if (error) {
            QMessageBox::critical(this, "Filesystem error",
                                  QString::fromStdString(error.message()));
        }
    }

    data["Launcher"]["installPath"] = installPathString;

    std::ofstream file(SettingsFile, std::ios::binary);
    file << data;
    file.close();
}

void BBLauncher::UpdateSettingsList() {
    QString SoundhackSetting = "60 FPS sound fix = " + QVariant(SoundFixEnabled).toString();
    QString ThemeSetting = "Selected Theme = " + QString::fromStdString(theme);
    QString BackupEnableSetting =
        "Back up saves enabled = " + QVariant(BackupSaveEnabled).toString();

    QString BackupIntSetting;
    QString BackupNumSetting;

    if (BackupSaveEnabled) {
        BackupIntSetting = "Backup Interval = " + QString::number(BackupInterval);
        BackupNumSetting = "Backup Copies = " + QString::number(BackupNumber);
    } else {
        BackupIntSetting = "Backup Interval = disabled";
        BackupNumSetting = "Backup Copies = disabled";
    }

    QStringList SettingStrings = {SoundhackSetting, ThemeSetting, BackupEnableSetting,
                                  BackupIntSetting, BackupNumSetting};

    ui->SettingList->clear();
    ui->SettingList->addItems(SettingStrings);
}

void BBLauncher::UpdateModList() {
    std::vector<std::string> ActiveModList;
    std::string line;
    int lineCount = 0;
    std::ifstream ActiveFile(ModPath / "ActiveMods.txt", std::ios::binary);

    ui->ModList->clear();

    while (std::getline(ActiveFile, line)) {
        lineCount++;
        ActiveModList.push_back(line);
    }
    ActiveFile.close();

    QStringList ActiveModStringList;

    if (ActiveModList.size() != 0) {
        for_each(ActiveModList.begin(), ActiveModList.end(),
                 [&](std::string s) { ActiveModStringList.append(QString::fromStdString(s)); });
    } else {
        const QString NoModMsg =
            "No mods active.\n\nPlace mods in the (BB Launcher exe folder)/BBLauncher/Mods and "
            "activate them using the Mod Manager button";
        ActiveModStringList.append(NoModMsg);
    }

    ui->ModList->addItems(ActiveModStringList);
}

// bool BBLauncher::eventFilter(QObject* obj, QEvent* event) {}

std::filesystem::path GetShadUserDir() {

    auto user_dir = std::filesystem::current_path() / "user";
    if (!std::filesystem::exists(user_dir)) {
#ifdef __APPLE__
        user_dir =
            std::filesystem::path(getenv("HOME")) / "Library" / "Application Support" / "shadPS4";
#elif defined(__linux__)
        const char* xdg_data_home = getenv("XDG_DATA_HOME");
        if (xdg_data_home != nullptr && strlen(xdg_data_home) > 0) {
            user_dir = std::filesystem::path(xdg_data_home) / "shadPS4";
        } else {
            user_dir = std::filesystem::path(getenv("HOME")) / ".local" / "share" / "shadPS4";
        }
#endif
    }
    return user_dir;
}

BBLauncher::~BBLauncher() {
    delete ui;
}
