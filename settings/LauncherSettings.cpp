// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QMessageBox>
#include <QPushButton>
#include "LauncherSettings.h"
#include "formatting.h"
#include "modules/Common.h"
#include "settings/ui_LauncherSettings.h"
#include "settings/updater/CheckUpdate.h"

std::string Config::theme = "Dark";
bool Config::SoundFixEnabled = true;
bool Config::BackupSaveEnabled = false;
int Config::BackupInterval = 10;
int Config::BackupNumber = 2;
bool Config::AutoUpdateEnabled = false;
bool Config::UnifiedInputConfig = true;

const std::filesystem::path SettingsFile = Common::BBLFilesPath / "LauncherSettings.toml";

LauncherSettings::LauncherSettings(QWidget* parent)
    : QDialog(parent), ui(new Ui::LauncherSettings) {
    ui->setupUi(this);
    using namespace Config;

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

void LauncherSettings::SetLauncherDefaults() {
    ui->DarkThemeRadioButton->setChecked(true);
    ui->SoundFixCheckBox->setChecked(true);
    ui->BackupIntervalComboBox->setCurrentText("10");
    ui->BackupNumberComboBox->setCurrentText("2");
}

void LauncherSettings::SaveLauncherSettings() {
    using namespace Config;
    toml::value data;
    std::error_code error;

    if (std::filesystem::exists(SettingsFile, error)) {
        try {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(SettingsFile, std::ios_base::binary);
            data = toml::parse(ifs, std::string{fmt::UTF(SettingsFile.filename().u8string()).data});
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

    data["Launcher"]["Theme"] = theme;
    data["Launcher"]["SoundFixEnabled"] = SoundFixEnabled;
    data["Launcher"]["AutoUpdateEnabled"] = AutoUpdateEnabled;

    data["Backups"]["BackupSaveEnabled"] = BackupSaveEnabled;
    data["Backups"]["BackupInterval"] = BackupInterval;
    data["Backups"]["BackupNumber"] = BackupNumber;

    std::ofstream file(SettingsFile, std::ios::binary);
    file << data;
    file.close();

    Config::SetTheme(theme);
}

void LauncherSettings::SaveAndCloseLauncherSettings() {
    SaveLauncherSettings();
    QWidget::close();
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

namespace Config {

void LoadLauncherSettings() {
    using namespace Config;

    if (!std::filesystem::exists(Common::BBLFilesPath)) {
        std::filesystem::create_directories(Common::BBLFilesPath);
        CreateSettingsFile();
    } else if (!std::filesystem::exists(SettingsFile)) {
        CreateSettingsFile();
    }

    toml::value data;
    try {
        std::ifstream ifs;
        ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        ifs.open(SettingsFile, std::ios_base::binary);
        data = toml::parse(ifs, std::string{fmt::UTF(SettingsFile.filename().u8string()).data});
    } catch (std::exception& ex) {
        QMessageBox::critical(NULL, "Filesystem error", ex.what());
        return;
    }

    theme = toml::find_or<std::string>(data, "Launcher", "Theme", "Dark");
    SoundFixEnabled = toml::find_or<bool>(data, "Launcher", "SoundFixEnabled", true);
    AutoUpdateEnabled = toml::find_or<bool>(data, "Launcher", "AutoUpdateEnabled", false);

    BackupSaveEnabled = toml::find_or<bool>(data, "Backups", "BackupSaveEnabled", false);
    BackupInterval = toml::find_or<int>(data, "Backups", "BackupInterval", 10);
    BackupNumber = toml::find_or<int>(data, "Backups", "BackupNumber", 2);

    if (data.contains("Launcher")) {
        const toml::value& launcher = data.at("Launcher");

        Common::installPath = toml::find_fs_path_or(launcher, "installPath", {});
        Common::shadPs4Executable = toml::find_fs_path_or(launcher, "shadPath", {});
    }

    if (std::filesystem::exists(Common::installPath)) {
        QString installPathString;
        Common::PathToQString(installPathString, Common::installPath);
        Common::game_serial = installPathString.last(9).toStdString();
    } else {
        Common::installPath = "";
    }

    Config::SetTheme(theme);

    std::filesystem::path shadConfigFile = Common::GetShadUserDir() / "config.toml";
    toml::value shadData;
    try {
        std::ifstream ifs;
        ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        ifs.open(shadConfigFile, std::ios_base::binary);
        shadData =
            toml::parse(ifs, std::string{fmt::UTF(shadConfigFile.filename().u8string()).data});
    } catch (std::exception& ex) {
        QMessageBox::critical(NULL, "Filesystem error", ex.what());
        return;
    }

    UnifiedInputConfig = toml::find_or<bool>(shadData, "Input,", "useUnifiedInputConfig", true);

    if (shadData.contains("GUI")) {
        const toml::value& GUI = shadData.at("GUI");
        Common::SaveDir = toml::find_fs_path_or(GUI, "saveDataPath", {});
        if (Common::SaveDir.empty()) {
            Common::SaveDir = Common::GetShadUserDir() / "savedata";
        }
    }
}

void CreateSettingsFile() {
    if (!std::filesystem::exists(Common::BBLFilesPath / "Mods")) {
        std::filesystem::create_directories(Common::BBLFilesPath / "Mods");
    }

    if (!std::filesystem::exists(Common::BBLFilesPath / "Mods-BACKUP")) {
        std::filesystem::create_directories(Common::BBLFilesPath / "Mods-BACKUP");
    }

    toml::value data;

    data["Launcher"]["installPath"] = "";
    data["Launcher"]["shadPath"] = "";
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

void SaveConfigPath(std::string configKey, std::filesystem::path path) {
    toml::value data;
    std::error_code error;

    if (std::filesystem::exists(SettingsFile, error)) {
        try {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(SettingsFile, std::ios_base::binary);
            data = toml::parse(ifs, std::string{fmt::UTF(path.filename().u8string()).data});
        } catch (const std::exception& ex) {
            QMessageBox::critical(NULL, "Filesystem error", ex.what());
            return;
        }
    } else {
        if (error) {
            QMessageBox::critical(NULL, "Filesystem error",
                                  QString::fromStdString(error.message()));
        }
    }

    data["Launcher"][configKey] = std::string{fmt::UTF(path.u8string()).data};

    std::ofstream file(SettingsFile, std::ios::binary);
    file << data;
    file.close();
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

std::string_view GetDefaultKeyboardConfig() {
    return R"(#Feeling lost? Check out the Help section!

#Keyboard bindings

triangle = f
circle = space
cross = e
square = r

pad_up = w, lalt
pad_up = mousewheelup
pad_down = s, lalt
pad_down = mousewheeldown
pad_left = a, lalt
pad_left = mousewheelleft
pad_right = d, lalt
pad_right = mousewheelright

l1 = rightbutton, lshift
r1 = leftbutton
l2 = rightbutton
r2 = leftbutton, lshift
l3 = x
r3 = q
r3 = middlebutton

options = escape
touchpad = g

key_toggle = i, lalt
mouse_to_joystick = right
mouse_movement_params = 0.5, 1, 0.125
leftjoystick_halfmode = lctrl

axis_left_x_minus = a
axis_left_x_plus = d
axis_left_y_minus = w
axis_left_y_plus = s

# Controller bindings

triangle = triangle
cross = cross
square = square
circle = circle

l1 = l1
l2 = l2
l3 = l3
r1 = r1
r2 = r2
r3 = r3

options = options
touchpad = back

pad_up = pad_up
pad_down = pad_down
pad_left = pad_left
pad_right = pad_right

axis_left_x = axis_left_x
axis_left_y = axis_left_y
axis_right_x = axis_right_x
axis_right_y = axis_right_y

# Range of deadzones: 1 (almost none) to 127 (max)
analog_deadzone = leftjoystick, 2
analog_deadzone = rightjoystick, 2
)";
}

void SaveUnifiedControl(bool setting) {
    using namespace Config;
    toml::value data;
    std::error_code error;
    std::filesystem::path ShadConfig = Common::GetShadUserDir() / "config.toml";

    if (std::filesystem::exists(ShadConfig, error)) {
        try {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(ShadConfig, std::ios_base::binary);
            data = toml::parse(ifs, std::string{fmt::UTF(ShadConfig.filename().u8string()).data});
        } catch (const std::exception& ex) {
            QMessageBox::critical(NULL, "Filesystem error", ex.what());
            return;
        }
    } else {
        if (error) {
            QMessageBox::critical(NULL, "Filesystem error",
                                  QString::fromStdString(error.message()));
        }
    }

    data["Input"]["useUnifiedInputConfig"] = setting;

    std::ofstream file(ShadConfig, std::ios::binary);
    file << data;
    file.close();

    Config::SetTheme(theme);
}

std::filesystem::path GetFoolproofKbmConfigFile(const std::string& game_id) {
    // Read configuration file of the game, and if it doesn't exist, generate it from default
    // If that doesn't exist either, generate that from getDefaultConfig() and try again
    // If even the folder is missing, we start with that.

    const auto config_dir = Common::GetShadUserDir() / "input_config";
    const auto config_file = config_dir / (game_id + ".ini");
    const auto default_config_file = config_dir / "default.ini";

    // Ensure the config directory exists
    if (!std::filesystem::exists(config_dir)) {
        std::filesystem::create_directories(config_dir);
    }

    // Check if the default config exists
    if (!std::filesystem::exists(default_config_file)) {
        // If the default config is also missing, create it from getDefaultConfig()
        const auto default_config = GetDefaultKeyboardConfig();
        std::ofstream default_config_stream(default_config_file);
        if (default_config_stream) {
            default_config_stream << default_config;
        }
    }

    // if empty, we only need to execute the function up until this point
    if (game_id.empty()) {
        return default_config_file;
    }

    // If game-specific config doesn't exist, create it from the default config
    if (!std::filesystem::exists(config_file)) {
        std::filesystem::copy(default_config_file, config_file);
    }
    return config_file;
}

} // namespace Config

LauncherSettings::~LauncherSettings() {
    delete ui;
}
