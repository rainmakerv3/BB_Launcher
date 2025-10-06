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
std::string Config::TrophyKey = "";
bool Config::ShowEarnedTrophy = true;
bool Config::ShowNotEarnedTrophy = true;
bool Config::ShowHiddenTrophy = false;
std::string Config::UpdateChannel = "Nightly";
bool Config::AutoUpdateShadEnabled = false;
bool Config::CheckPortableSettings = true;
std::string Config::DefaultControllerID = "";

const std::filesystem::path SettingsFile = Common::BBLFilesPath / "LauncherSettings.toml";

static std::string SelectedGamepad = "";

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
    ui->PortableSettingsCheckBox->setChecked(CheckPortableSettings);
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
#if (QT_VERSION < QT_VERSION_CHECK(6, 7, 0))
    connect(ui->BackupSaveCheckBox, &QCheckBox::stateChanged, this,
            &LauncherSettings::OnBackupStateChanged);

#else
    connect(ui->BackupSaveCheckBox, &QCheckBox::checkStateChanged, this,
            &LauncherSettings::OnBackupStateChanged);
#endif
}

void LauncherSettings::SetLauncherDefaults() {
    ui->PortableSettingsCheckBox->setChecked(true);
    ui->UpdateCheckBox->setChecked(false);
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
    CheckPortableSettings = ui->PortableSettingsCheckBox->isChecked();

    data["Launcher"]["Theme"] = theme;
    data["Launcher"]["SoundFixEnabled"] = SoundFixEnabled;
    data["Launcher"]["AutoUpdateEnabled"] = AutoUpdateEnabled;
    data["Launcher"]["PortableSettings"] = CheckPortableSettings;

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

void CreateGSFile() {
    std::string filename = Common::game_serial + ".toml";
    std::filesystem::path gsConfig = Common::GetShadUserDir() / "custom_configs" / filename;

    toml::ordered_value data;

    data["General"]["extraDmemInMbytes"] = 0;

    std::ofstream file(gsConfig, std::ios::binary);
    file << data;
    file.close();
}

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
    CheckPortableSettings = toml::find_or<bool>(data, "Launcher", "PortableSettings", true);

    BackupSaveEnabled = toml::find_or<bool>(data, "Backups", "BackupSaveEnabled", false);
    BackupInterval = toml::find_or<int>(data, "Backups", "BackupInterval", 10);
    BackupNumber = toml::find_or<int>(data, "Backups", "BackupNumber", 2);

    ShowEarnedTrophy = toml::find_or<bool>(data, "Trophy", "ShowEarned", true);
    ShowNotEarnedTrophy = toml::find_or<bool>(data, "Trophy", "ShowUnearned", true);
    ShowHiddenTrophy = toml::find_or<bool>(data, "Trophy", "ShowHidden", false);

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

    if (std::filesystem::exists(Common::installPath.parent_path() /
                                (Common::game_serial + "-UPDATE"))) {

        Common::installUpdatePath =
            Common::installPath.parent_path() / (Common::game_serial + "-UPDATE");
    } else if (std::filesystem::exists(Common::installPath.parent_path() /
                                       (Common::game_serial + "-patch"))) {
        Common::installUpdatePath =
            Common::installPath.parent_path() / (Common::game_serial + "-patch");
    }

    Config::SetTheme(theme);

    std::filesystem::path shadConfigFile = Common::GetShadUserDir() / "config.toml";
    if (std::filesystem::exists(shadConfigFile)) {
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

        UnifiedInputConfig = toml::find_or<bool>(shadData, "Input", "useUnifiedInputConfig", true);
        TrophyKey = toml::find_or<std::string>(shadData, "Keys", "TrophyKey", "");
        DefaultControllerID =
            toml::find_or<std::string>(shadData, "General", "defaultControllerID", "");

        if (shadData.contains("GUI")) {
            const toml::value& GUI = shadData.at("GUI");
            Common::SaveDir = toml::find_fs_path_or(GUI, "saveDataPath", {});
            if (Common::SaveDir.empty()) {
                Common::SaveDir = Common::GetShadUserDir() / "savedata";
            }
        }
    }

    std::fstream file(Common::GetShadUserDir() / "qt_ui.ini");
    std::string line;
    std::vector<std::string> lines;
    int lineCount = 0;

    while (std::getline(file, line)) {
        lineCount++;

        if (line.contains("updateChannel")) {
            std::size_t equal_pos = line.find('=');
            UpdateChannel = line.substr(equal_pos + 1);
        }

        if (line.contains("checkForUpdates")) {
            std::size_t equal_pos = line.find('=');
            std::string updatesEnabled(line.substr(equal_pos + 1));
            AutoUpdateShadEnabled = updatesEnabled == "true" ? true : false;
        }
    }
    file.close();
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
    data["Launcher"]["PortableSettings"] = true;

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

void SaveTrophySettings(bool ShowEarned, bool ShowUnEarned, bool ShowHidden) {
    toml::value data;
    std::error_code error;

    if (std::filesystem::exists(SettingsFile, error)) {
        try {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(SettingsFile, std::ios_base::binary);
            data = toml::parse(ifs, std::string{fmt::UTF(SettingsFile.filename().u8string()).data});
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

    data["Trophy"]["ShowEarned"] = ShowEarned;
    data["Trophy"]["ShowUnearned"] = ShowUnEarned;
    data["Trophy"]["ShowHidden"] = ShowHidden;

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

void SaveInputSettings(bool unifiedControl, std::string defaultID) {
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

    data["Input"]["useUnifiedInputConfig"] = unifiedControl;
    if (defaultID != "noIDsave")
        data["General"]["defaultControllerID"] = defaultID;

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

namespace GamepadSelect {

int GetDefaultGamepad(SDL_JoystickID* gamepadIDs, int gamepadCount) {
    char GUIDbuf[33];
    if (Config::DefaultControllerID != "") {
        for (int i = 0; i < gamepadCount; i++) {
            SDL_GUIDToString(SDL_GetGamepadGUIDForID(gamepadIDs[i]), GUIDbuf, 33);
            std::string currentGUID = std::string(GUIDbuf);
            if (currentGUID == Config::DefaultControllerID) {
                return i;
            }
        }
    }
    return -1;
}

int GetIndexfromGUID(SDL_JoystickID* gamepadIDs, int gamepadCount, std::string GUID) {
    char GUIDbuf[33];
    for (int i = 0; i < gamepadCount; i++) {
        SDL_GUIDToString(SDL_GetGamepadGUIDForID(gamepadIDs[i]), GUIDbuf, 33);
        std::string currentGUID = std::string(GUIDbuf);
        if (currentGUID == GUID) {
            return i;
        }
    }
    return -1;
}

std::string GetGUIDString(SDL_JoystickID* gamepadIDs, int index) {
    char GUIDbuf[33];
    SDL_GUIDToString(SDL_GetGamepadGUIDForID(gamepadIDs[index]), GUIDbuf, 33);
    std::string GUID = std::string(GUIDbuf);
    return GUID;
}

std::string GetSelectedGamepad() {
    return SelectedGamepad;
}

void SetSelectedGamepad(std::string GUID) {
    SelectedGamepad = GUID;
}

} // namespace GamepadSelect
