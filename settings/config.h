// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <filesystem>
#include <optional>
#include <SDL3/SDL_gamepad.h>

#include "modules/Common.h"

namespace Config {

struct ShadSettings {
    std::optional<std::string> defaultControllerID;
    std::optional<bool> useUnifiedInputConfig;
};

void LoadSettings();
void CreateSettingsFile();
void SetTheme(std::string theme);
std::filesystem::path GetFoolproofKbmConfigFile(const std::string& game_id);
std::string_view GetDefaultKeyboardConfig();

void SaveShadSettings(ShadSettings settings, bool is_game_specific = false);
void SaveLauncherSettings();

void CreateGSFile();
std::string GetLastModifiedString(const std::filesystem::path& path);

extern std::string theme;
extern bool SoundFixEnabled;
extern bool BackupSaveEnabled;
extern int BackupInterval;
extern int BackupNumber;
extern bool AutoUpdateEnabled;
extern bool UnifiedInputConfig;
extern std::string TrophyKey;
extern std::string DefaultControllerID;

extern std::string LastBuildId;
extern std::string LastBuildType;
extern std::string LastBuildModified;

extern bool ShowEarnedTrophy;
extern bool ShowNotEarnedTrophy;
extern bool ShowHiddenTrophy;

extern std::string DefaultFolderString;
extern std::string UpdateChannel;
extern bool AutoUpdateShadEnabled;
extern bool ShowChangeLog;

extern bool GameRunning;

const std::filesystem::path SettingsFile = Common::GetBBLFilesPath() / "LauncherSettings.toml";

} // namespace Config

namespace GamepadSelect {

int GetIndexfromGUID(SDL_JoystickID* gamepadIDs, int gamepadCount, std::string GUID);
std::string GetGUIDString(SDL_JoystickID* gamepadIDs, int index);
std::string GetSelectedGamepad();
void SetSelectedGamepad(std::string GUID);

} // namespace GamepadSelect
