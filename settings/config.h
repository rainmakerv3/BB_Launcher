// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <filesystem>
#include <optional>
#include <SDL3/SDL_gamepad.h>

#include "modules/Common.h"

namespace Config {

struct ShadSettings {
    std::optional<std::string> defaultControllerID;
    std::optional<bool> useUnifiedInputConfig;
    std::optional<std::filesystem::path> dlcPath;
    std::optional<std::filesystem::path> savePath;
};

struct Build {
    std::string path;
    std::string type;
    std::string id;
    std::string modified;
};

void LoadSettings();
void CreateSettingsFile();
void SetTheme(std::string theme);
std::filesystem::path GetFoolproofKbmConfigFile(const std::string& game_id);
std::string_view GetDefaultKeyboardConfig();
void CreateSettingsFile();
bool isReleaseOlder(int minorVersion, int majorVersion = 0);
Build GetCurrentBuildInfo();
int GetDmemValue();

void SaveShadSettings(ShadSettings settings, bool is_game_specific = false);
void SaveLauncherSettings();

void CreateGSFile();
std::string GetLastModifiedString(const std::filesystem::path& path);

extern std::string theme;
extern bool PortableFolderinLauncherFolder;
extern bool SoundFixEnabled;
extern bool BackupSaveEnabled;
extern int BackupInterval;
extern int BackupNumber;
extern bool AutoUpdateEnabled;
extern bool UnifiedInputConfig;
extern std::string TrophyKey;
extern std::string DefaultControllerID;
extern std::string ApiKey;
extern std::filesystem::path SevenZipPath;

extern bool ShowEarnedTrophy;
extern bool ShowNotEarnedTrophy;
extern bool ShowHiddenTrophy;

extern std::string DefaultFolderString;
extern bool AutoUpdateVersionsEnabled;
extern bool AutoUpdateShadEnabled;
extern bool ShowChangeLog;

extern std::filesystem::path externalSaveDir;
extern std::filesystem::path dlcDir;
extern bool GameRunning;

const std::filesystem::path SettingsFile = Common::GetBBLFilesPath() / "LauncherSettings.toml";

} // namespace Config

namespace GamepadSelect {

int GetIndexfromGUID(SDL_JoystickID* gamepadIDs, int gamepadCount, std::string GUID);
std::string GetGUIDString(SDL_JoystickID* gamepadIDs, int index);
std::string GetSelectedGamepad();
void SetSelectedGamepad(std::string GUID);

} // namespace GamepadSelect
