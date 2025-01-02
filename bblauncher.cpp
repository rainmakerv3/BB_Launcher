#include <fstream>
#include <thread>
#include <QFileDialog>
#include <QMessageBox>
#include "bblauncher.h"
#include "settings/LauncherSettings.h"
#include "settings/toml.hpp"
#include "ui_bblauncher.h"

std::filesystem::path userPath = std::filesystem::current_path() / "user";
std::string installPathString = "";
std::filesystem::path installPath = "";
std::filesystem::path PKGPath = "";
std::string game_serial = "";

BBLauncher::BBLauncher(QWidget* parent) : QMainWindow(parent), ui(new Ui::BBLauncher) {
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
    QApplication::setStyle("Fusion");

    // this->installEventFilter(this); if needed

    LoadLauncherSettings();
    UpdateSettingsList();

    // Update ModList();

    connect(ui->ExeSelectButton, &QPushButton::pressed, this,
            &BBLauncher::ExeSelectButton_isPressed);

    connect(ui->shadSettingsButton, &QPushButton::pressed, this,
            &BBLauncher::ModManagerButton_isPressed);
    connect(ui->ModManagerButton, &QPushButton::pressed, this,
            &BBLauncher::ModManagerButton_isPressed);
    connect(ui->LaunchButton, &QPushButton::pressed, this, &BBLauncher::LaunchButton_isPressed);
    connect(ui->TrophyButton, &QPushButton::pressed, this, &BBLauncher::ModManagerButton_isPressed);
    connect(ui->SaveManagerButton, &QPushButton::pressed, this,
            &BBLauncher::ModManagerButton_isPressed);
    connect(ui->PatchesButton, &QPushButton::pressed, this,
            &BBLauncher::ModManagerButton_isPressed);
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

void BBLauncher::ModManagerButton_isPressed() {
    QMessageBox::warning(this, "WIP", "Still working on this :)");
    UpdateSettingsList();
}

void BBLauncher::LaunchButton_isPressed() {
    if (game_serial != "") {
        if (SoundFixEnabled) {
            std::filesystem::path savePath = userPath / "savedata" / "1" / game_serial / "SPRJ0005";
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
    const char* runBBshadPS4 = ("shadPS4.exe -g \"" + PKGLoc + "\"").c_str();
    std::system(runBBshadPS4);

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

    const auto save_dir = userPath / "savedata" / "1" / game_serial;
    const auto backup_dir = BackupPath / "BACKUP1";

    while (true) {
        std::this_thread::sleep_for(std::chrono::minutes(1)); // Interval

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
                    // handle
                }
            }
        }

        try {
            std::filesystem::copy(save_dir, backup_dir,
                                  std::filesystem::copy_options::overwrite_existing |
                                      std::filesystem::copy_options::recursive);
        } catch (std::exception& ex) {
            // handle
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
            // handle
            return;
        }
    } else {
        if (error) {
            // handle
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

// bool BBLauncher::eventFilter(QObject* obj, QEvent* event) {}

BBLauncher::~BBLauncher() {
    delete ui;
}
