// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>
#include <QString>

namespace Common {

void PathToQString(QString& result, const std::filesystem::path& path);
std::filesystem::path PathFromQString(const QString& path);
std::string PathToU8(const std::filesystem::path& path);
std::filesystem::path GetShadUserDir();

extern std::string game_serial;
extern std::filesystem::path installPath;
extern std::filesystem::path SaveDir;
extern std::filesystem::path shadPs4Executable;
extern char VERSION[];

#if defined(__APPLE__)
const std::filesystem::path BBLFilesPath =
    std::filesystem::path(getenv("HOME")) / "Library" / "Application Support" / "BBLauncher";
#else
const std::filesystem::path BBLFilesPath = std::filesystem::current_path() / "BBLauncher";
#endif

const std::filesystem::path ModPath = BBLFilesPath / "Mods";

} // namespace Common
