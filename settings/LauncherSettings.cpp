// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QMessageBox>
#include <QPushButton>
#include "LauncherSettings.h"
#include "modules/bblauncher.h"
#include "settings/ui_LauncherSettings.h"
#include "settings/updater/BuildInfo.h"
#include "settings/updater/CheckUpdate.h"
#include "toml.hpp"

std::filesystem::path SettingsPath = std::filesystem::current_path() / "BBLauncher";
std::filesystem::path SettingsFile = SettingsPath / "LauncherSettings.toml";

std::string theme = "Dark";
bool SoundFixEnabled = true;
bool BackupSaveEnabled = false;
int BackupInterval = 10;
int BackupNumber = 2;
bool AutoUpdateEnabled = false;

namespace toml {

template <typename TC, typename K>
std::filesystem::path find_fs_path_or(const basic_value<TC>& v, const K& ky,
                                      std::filesystem::path opt) {
    try {
        auto str = find<std::string>(v, ky);
        if (str.empty()) {
            return opt;
        }
        std::u8string u8str{(char8_t*)&str.front(), (char8_t*)&str.back() + 1};
        return std::filesystem::path{u8str};
    } catch (...) {
        return opt;
    }
}

} // namespace toml

LauncherSettings::LauncherSettings(QWidget* parent)
    : QDialog(parent), ui(new Ui::LauncherSettings) {
    ui->setupUi(this);

    ui->BackupIntervalComboBox->addItems(BackupFreqList);
    ui->BackupNumberComboBox->addItems(BackupNumList);

    if (theme == "Dark") {
        ui->DarkThemeRadioButton->setChecked(true);
    } else {
        ui->LightThemeRadioButton->setChecked(true);
    }

    ui->UpdateCheckBox->setChecked(AutoUpdateEnabled);
    ui->SoundFixCheckBox->setChecked(SoundFixEnabled);
    ui->BackupSaveCheckBox->setChecked(BackupSaveEnabled);
    ui->BackupIntervalComboBox->setCurrentText(QString::number(BackupInterval));
    ui->BackupNumberComboBox->setCurrentText(QString::number(BackupNumber));
    OnBackupStateChanged();

    connect(ui->UpdateButton, &QPushButton::clicked, this, []() {
        auto checkUpdate = new CheckUpdate(true);
        checkUpdate->exec();
    });

    connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::pressed, this,
            &LauncherSettings::SaveAndCloseLauncherSettings);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::pressed, this,
            &LauncherSettings::SaveLauncherSettings);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::pressed, this,
            &LauncherSettings::SetLauncherDefaults);

    connect(ui->BackupSaveCheckBox, &QCheckBox::stateChanged, this,
            &LauncherSettings::OnBackupStateChanged);
}

void SetTheme(std::string theme) {
    QPalette themePalette;
    if (theme == "Dark") {
        themePalette.setColor(QPalette::Window, QColor(50, 50, 50));
        themePalette.setColor(QPalette::WindowText, Qt::white);
        themePalette.setColor(QPalette::Base, QColor(20, 20, 20));
        themePalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        themePalette.setColor(QPalette::ToolTipBase, Qt::white);
        themePalette.setColor(QPalette::ToolTipText, Qt::white);
        themePalette.setColor(QPalette::Text, Qt::white);
        themePalette.setColor(QPalette::Button, QColor(53, 53, 53));
        themePalette.setColor(QPalette::ButtonText, Qt::white);
        themePalette.setColor(QPalette::BrightText, Qt::red);
        themePalette.setColor(QPalette::Link, QColor(42, 130, 218));
        themePalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        themePalette.setColor(QPalette::HighlightedText, Qt::black);
    } else if (theme == "Light") {
        themePalette.setColor(QPalette::Window, QColor(240, 240, 240));   // Light gray
        themePalette.setColor(QPalette::WindowText, Qt::black);           // Black
        themePalette.setColor(QPalette::Base, QColor(230, 230, 230, 80)); // Grayish
        themePalette.setColor(QPalette::ToolTipBase, Qt::black);          // Black
        themePalette.setColor(QPalette::ToolTipText, Qt::black);          // Black
        themePalette.setColor(QPalette::Text, Qt::black);                 // Black
        themePalette.setColor(QPalette::Button, QColor(240, 240, 240));   // Light gray
        themePalette.setColor(QPalette::ButtonText, Qt::black);           // Black
        themePalette.setColor(QPalette::BrightText, Qt::red);             // Red
        themePalette.setColor(QPalette::Link, QColor(42, 130, 218));      // Blue
        themePalette.setColor(QPalette::Highlight, QColor(42, 130, 218)); // Blue
        themePalette.setColor(QPalette::HighlightedText, Qt::white);      // White
    }
    qApp->setPalette(themePalette);
}

void LauncherSettings::SetLauncherDefaults() {
    ui->DarkThemeRadioButton->setChecked(true);
    ui->SoundFixCheckBox->setChecked(true);
    ui->BackupIntervalComboBox->setCurrentText("10");
    ui->BackupNumberComboBox->setCurrentText("2");
}

void LoadLauncherSettings() {
    if (!std::filesystem::exists(SettingsPath)) {
        std::filesystem::create_directory(SettingsPath);
        CreateSettingsFile();
    } else if (!std::filesystem::exists(SettingsFile)) {
        CreateSettingsFile();
    }

    toml::value data;
    try {
        std::ifstream ifs;
        ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        ifs.open(SettingsFile, std::ios_base::binary);
        data = toml::parse(SettingsFile);
    } catch (std::exception& ex) {
        QMessageBox::critical(NULL, "Filesystem error", ex.what());
        return;
    }

    installPathString = toml::find_or<std::string>(data, "Launcher", "installPath", "");
    game_serial = QString::fromStdString(installPathString).last(9).toStdString();
    installPath = installPathString;
    EbootPath = installPath / "eboot.bin";

    theme = toml::find_or<std::string>(data, "Launcher", "Theme", "Dark");
    SoundFixEnabled = toml::find_or<bool>(data, "Launcher", "SoundFixEnabled", true);
    AutoUpdateEnabled = toml::find_or<bool>(data, "Launcher", "AutoUpdateEnabled", false);

    BackupSaveEnabled = toml::find_or<bool>(data, "Backups", "BackupSaveEnabled", false);
    BackupInterval = toml::find_or<int>(data, "Backups", "BackupInterval", 10);
    BackupNumber = toml::find_or<int>(data, "Backups", "BackupNumber", 2);
    shadPs4Executable =
        std::filesystem::path(toml::find_or<std::string>(data, "Launcher", "shadPath", ""));

    SetTheme(theme);

    toml::value shadData;
    try {
        std::ifstream ifs;
        ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        ifs.open(GetShadUserDir() / "config.toml", std::ios_base::binary);
        shadData = toml::parse(GetShadUserDir() / "config.toml");
    } catch (std::exception& ex) {
        QMessageBox::critical(NULL, "Filesystem error", ex.what());
        return;
    }

    if (shadData.contains("GUI")) {
        const toml::value& GUI = shadData.at("GUI");
        SaveDir = toml::find_fs_path_or(GUI, "saveDataPath", {});
        if (SaveDir.empty()) {
            SaveDir = GetShadUserDir() / "savedata";
        }
    }
}

void LauncherSettings::SaveLauncherSettings() {
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

    if (ui->DarkThemeRadioButton->isChecked()) {
        theme = "Dark";
    } else {
        theme = "Light";
    }

    SoundFixEnabled = ui->SoundFixCheckBox->isChecked();
    BackupSaveEnabled = ui->BackupSaveCheckBox->isChecked();
    BackupInterval = ui->BackupIntervalComboBox->currentText().toInt();
    BackupNumber = ui->BackupNumberComboBox->currentText().toInt();
    AutoUpdateEnabled = ui->UpdateCheckBox->isChecked();

    data["Launcher"]["installPath"] = installPathString;
    data["Launcher"]["Theme"] = theme;
    data["Launcher"]["SoundFixEnabled"] = SoundFixEnabled;
    data["Launcher"]["AutoUpdateEnabled"] = AutoUpdateEnabled;

    data["Backups"]["BackupSaveEnabled"] = BackupSaveEnabled;
    data["Backups"]["BackupInterval"] = BackupInterval;
    data["Backups"]["BackupNumber"] = BackupNumber;

    std::ofstream file(SettingsFile, std::ios::binary);
    file << data;
    file.close();

    SetTheme(theme);
}

void LauncherSettings::SaveAndCloseLauncherSettings() {
    SaveLauncherSettings();
    QWidget::close();
}

void CreateSettingsFile() {
    if (!std::filesystem::exists(SettingsPath / "Mods")) {
        std::filesystem::create_directories(SettingsPath / "Mods");
    }

    if (!std::filesystem::exists(SettingsPath / "Mods-BACKUP")) {
        std::filesystem::create_directories(SettingsPath / "Mods-BACKUP");
    }

    toml::value data;

    data["Launcher"]["installPath"] = "";
    data["Launcher"]["Theme"] = "Dark";
    data["Launcher"]["SoundFixEnabled"] = true;
    data["Launcher"]["AutoUpdateEnabled"] = false;

    data["Backups"]["BackupSaveEnabled"] = false;
    data["Backups"]["BackupInterval"] = 10;
    data["Backups"]["BackupNumber"] = 2;

    std::ofstream file(SettingsFile, std::ios::binary);
    file << data;
    file.close();
}

void LauncherSettings::OnBackupStateChanged() {
    if (ui->BackupSaveCheckBox->isChecked()) {
        ui->BackupIntervalLabel->show();
        ui->BackupIntervalComboBox->show();
        ui->BackupNumberLabel->show();
        ui->BackupNumberComboBox->show();
    } else {
        ui->BackupIntervalLabel->hide();
        ui->BackupIntervalComboBox->hide();
        ui->BackupNumberLabel->hide();
        ui->BackupNumberComboBox->hide();
    }
}

LauncherSettings::~LauncherSettings() {
    delete ui;
}
