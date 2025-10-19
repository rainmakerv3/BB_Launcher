// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <filesystem>
#include <SDL3/SDL_gamepad.h>

#include "modules/Common.h"

namespace Config {

void SaveConfigPath(std::string configKey, std::filesystem::path path);
void LoadLauncherSettings();
void CreateSettingsFile();
void SetTheme(std::string theme);
std::filesystem::path GetFoolproofKbmConfigFile(const std::string& game_id);
std::string_view GetDefaultKeyboardConfig();
void SaveInputSettings(bool unifiedControl, std::string defaultID);
void SaveTrophySettings(bool ShowEarned, bool ShowUnEarned, bool ShowHidden);
void SaveBuild(std::string build, std::string branch, std::string modified);
void CreateGSFile();
std::string GetLastModifiedString(const std::filesystem::path& path);

extern std::string theme;
extern bool SoundFixEnabled;
extern bool BackupSaveEnabled;
extern int BackupInterval;
extern int BackupNumber;
extern bool AutoUpdateEnabled;
extern bool CheckPortableSettings;
extern bool UnifiedInputConfig;
extern std::string TrophyKey;
extern std::string UpdateChannel;
extern bool AutoUpdateShadEnabled;
extern std::string DefaultControllerID;

extern std::string LastBuildHash;
extern std::string LastBuildBranch;
extern std::string LastBuildModified;

extern bool ShowEarnedTrophy;
extern bool ShowNotEarnedTrophy;
extern bool ShowHiddenTrophy;

extern bool GameRunning;

const std::filesystem::path SettingsFile = Common::GetBBLFilesPath() / "LauncherSettings.toml";

} // namespace Config

namespace GamepadSelect {

int GetIndexfromGUID(SDL_JoystickID* gamepadIDs, int gamepadCount, std::string GUID);
std::string GetGUIDString(SDL_JoystickID* gamepadIDs, int index);
std::string GetSelectedGamepad();
void SetSelectedGamepad(std::string GUID);

} // namespace GamepadSelect
