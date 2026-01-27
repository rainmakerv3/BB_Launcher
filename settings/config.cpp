// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QMessageBox>
#include <QRegularExpression>

#include "config.h"
#include "formatting.h"

std::filesystem::path Config::SevenZipPath;
std::string Config::ApiKey = "";
std::string Config::theme = "Dark";
bool Config::SoundFixEnabled = true;
bool Config::AutoUpdateEnabled = false;
bool Config::PortableFolderinLauncherFolder = false;

bool Config::BackupSaveEnabled = false;
int Config::BackupInterval = 10;
int Config::BackupNumber = 2;

bool Config::ShowEarnedTrophy = true;
bool Config::ShowNotEarnedTrophy = true;
bool Config::ShowHiddenTrophy = false;

bool Config::AutoUpdateVersionsEnabled = true;
bool Config::AutoUpdateShadEnabled = false;
bool Config::ShowChangeLog = true;
std::string Config::DefaultFolderString = "";

std::string Config::TrophyKey = "";
bool Config::UnifiedInputConfig = true;
std::string Config::DefaultControllerID = "";

std::filesystem::path Config::externalSaveDir;
bool Config::GameRunning = false;
static std::string SelectedGamepad = "";

#if __APPLE__
#include <date/date.h>
#include <date/tz.h>
#include <date/tz_private.h>
#endif

namespace Config {

void CreateGSFile() {
    std::string filename = Common::game_serial + ".toml";
    std::filesystem::path gsConfig = Common::GetShadUserDir() / "custom_configs" / filename;

    if (!std::filesystem::exists(gsConfig.parent_path()))
        std::filesystem::create_directories(gsConfig.parent_path());

    toml::ordered_value data;

    data["General"]["extraDmemInMbytes"] = 0;

    std::ofstream file(gsConfig, std::ios::binary);
    file << data;
    file.close();
}

void LoadSettings() {
    using namespace Config;

    if (!std::filesystem::exists(Common::GetBBLFilesPath())) {
        std::filesystem::create_directories(Common::GetBBLFilesPath());
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

    ApiKey = toml::find_or<std::string>(data, "Launcher", "ApiKey", "");
    theme = toml::find_or<std::string>(data, "Launcher", "Theme", "Dark");
    SoundFixEnabled = toml::find_or<bool>(data, "Launcher", "SoundFixEnabled", true);
    AutoUpdateEnabled = toml::find_or<bool>(data, "Launcher", "AutoUpdateEnabled", false);
    PortableFolderinLauncherFolder =
        toml::find_or<bool>(data, "Launcher", "PortableFolderinLauncherFolder", false);

    BackupSaveEnabled = toml::find_or<bool>(data, "Backups", "BackupSaveEnabled", false);
    BackupInterval = toml::find_or<int>(data, "Backups", "BackupInterval", 10);
    BackupNumber = toml::find_or<int>(data, "Backups", "BackupNumber", 2);

    ShowEarnedTrophy = toml::find_or<bool>(data, "Trophy", "ShowEarned", true);
    ShowNotEarnedTrophy = toml::find_or<bool>(data, "Trophy", "ShowUnearned", true);
    ShowHiddenTrophy = toml::find_or<bool>(data, "Trophy", "ShowHidden", false);

    DefaultFolderString = toml::find_or<std::string>(data, "shadUpdater", "DefaultFolder", "");
    AutoUpdateVersionsEnabled =
        toml::find_or<bool>(data, "shadUpdater", "AutoUpdateVersionsEnabled", true);
    AutoUpdateShadEnabled =
        toml::find_or<bool>(data, "shadUpdater", "AutoUpdateShadEnabled", false);

    if (data.contains("Launcher")) {
        const toml::value& launcher = data.at("Launcher");

        SevenZipPath = toml::find_fs_path_or(launcher, "SevenZipPath", {});
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
            Config::externalSaveDir = toml::find_fs_path_or(GUI, "saveDataPath", {});
        }
    }
}

void CreateSettingsFile() {
    if (!std::filesystem::exists(Common::GetBBLFilesPath() / "Mods")) {
        std::filesystem::create_directories(Common::GetBBLFilesPath() / "Mods");
    }

    if (!std::filesystem::exists(Common::GetBBLFilesPath() / "Mods-BACKUP")) {
        std::filesystem::create_directories(Common::GetBBLFilesPath() / "Mods-BACKUP");
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

void SaveShadSettings(ShadSettings settings, bool is_game_specific) {
    using namespace Config;

    std::filesystem::path ShadConfig =
        is_game_specific
            ? Common::GetShadUserDir() / "custom_configs" / (Common::game_serial + ".toml")
            : Common::GetShadUserDir() / "config.toml";
    toml::value data = toml::parse(ShadConfig);
    std::error_code error;

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

    // global only
    if (!is_game_specific) {
        if (settings.useUnifiedInputConfig.has_value())
            data["Input"]["useUnifiedInputConfig"] = settings.useUnifiedInputConfig.value();

        if (settings.defaultControllerID.has_value())
            data["General"]["defaultControllerID"] = settings.defaultControllerID.value();
    }

    // game-specific only

    if (settings.isPSNSignedIn.has_value())
        data["General"]["isPSNSignedIn"] = settings.isPSNSignedIn.value();

    if (settings.isConnectedToNetwork.has_value())
        data["General"]["isConnectedToNetwork"] = settings.isConnectedToNetwork.value();

    if (settings.httpHostOverride.has_value())
        data["General"]["httpHostOverride"] = settings.httpHostOverride.value();

    // common

    std::ofstream file(ShadConfig, std::ios::binary);
    file << data;
    file.close();

    SetTheme(theme);
}

void SaveLauncherSettings() {
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

    data["Launcher"]["SevenZipPath"] = std::string{fmt::UTF(SevenZipPath.u8string()).data};
    data["Launcher"]["ApiKey"] = ApiKey;
    data["Launcher"]["Theme"] = theme;
    data["Launcher"]["SoundFixEnabled"] = SoundFixEnabled;
    data["Launcher"]["AutoUpdateEnabled"] = AutoUpdateEnabled;
    data["Launcher"]["installPath"] = std::string{fmt::UTF(Common::installPath.u8string()).data};
    data["Launcher"]["shadPath"] = std::string{fmt::UTF(Common::shadPs4Executable.u8string()).data};
    data["Launcher"]["PortableFolderinLauncherFolder"] = PortableFolderinLauncherFolder;

    data["Backups"]["BackupSaveEnabled"] = BackupSaveEnabled;
    data["Backups"]["BackupInterval"] = BackupInterval;
    data["Backups"]["BackupNumber"] = BackupNumber;

    data["Trophy"]["ShowEarned"] = Config::ShowEarnedTrophy;
    data["Trophy"]["ShowUnearned"] = Config::ShowNotEarnedTrophy;
    data["Trophy"]["ShowHidden"] = Config::ShowHiddenTrophy;

    data["shadUpdater"]["DefaultFolder"] = DefaultFolderString;
    data["shadUpdater"]["AutoUpdateVersionsEnabled"] = AutoUpdateVersionsEnabled;
    data["shadUpdater"]["AutoUpdateShadEnabled"] = AutoUpdateShadEnabled;

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

std::string GetLastModifiedString(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path))
        return "";

    std::chrono::time_point shadWriteTime = std::filesystem::last_write_time(path);

#if __APPLE__
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(shadWriteTime.time_since_epoch());
    auto time = date::sys_time<std::chrono::seconds>{sec};
    std::string shadModifiedStringUTC = date::format("{:%F %T}", time);
#else
    auto shadTimePoint = std::chrono::clock_cast<std::chrono::system_clock>(shadWriteTime);
    std::string shadModifiedStringUTC = std::format("{:%F %T}", shadWriteTime);
#endif

    return shadModifiedStringUTC;
}

Build GetCurrentBuildInfo() {
    toml::value data;
    std::error_code error;

    if (std::filesystem::exists(Config::SettingsFile, error)) {
        try {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(Config::SettingsFile, std::ios_base::binary);
            data = toml::parse(
                ifs, std::string{fmt::UTF(Config::SettingsFile.filename().u8string()).data});
        } catch (const std::exception& ex) {
            QMessageBox::critical(nullptr, "Filesystem error", ex.what());
            return {};
        }
    } else {
        if (error) {
            QMessageBox::critical(nullptr, "Filesystem error",
                                  QString::fromStdString(error.message()));
        }
    }

    if (!data.contains("Builds")) {
        // QMessageBox::critical(nullptr, "Error", "No saved build data found");
        return {};
    }

    int buildCounter = 1;
    while (true) {
        const auto arr =
            toml::find_or<toml::array>(data.at("Builds"), std::to_string(buildCounter), {});

        if (arr.empty())
            break;

        Config::Build build;
        build.path = arr[0].as_string();
        build.type = arr[1].as_string();
        build.id = arr[2].as_string();
        build.modified = arr[3].as_string();

        if (build.path == Common::shadPs4Executable)
            return build;

        buildCounter += 1;
    }

    return {};
}

bool isReleaseOlder(int minorVersion, int MajorVersion) {
    bool isOlder = false;
    Config::Build CurrentBuild = Config::GetCurrentBuildInfo();

    if (CurrentBuild.type == "Release") {
        static QRegularExpression versionRegex(R"(v\.?(\d+)\.(\d+)\.(\d+))");
        QRegularExpressionMatch match = versionRegex.match(QString::fromStdString(CurrentBuild.id));
        if (match.hasMatch()) {
            int major = match.captured(1).toInt();
            int minor = match.captured(2).toInt();
            int patch = match.captured(3).toInt();

            if (major > MajorVersion)
                isOlder = true;
            if (major == MajorVersion && minor < minorVersion)
                isOlder = true;
        }
    }

    return isOlder;
}

} // namespace Config

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
