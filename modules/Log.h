// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <source_location>
#include <string>

namespace LauncherLog {

enum Type : int { Information, Warning, Error, Critical };

void LogMsg(LauncherLog::Type type, const std::string& logMsg,
            const std::source_location location = std::source_location::current());

#define LogInfo(...) LauncherLog::LogMsg(LauncherLog::Type::Information, __VA_ARGS__)
#define LogWarning(...) LauncherLog::LogMsg(LauncherLog::Type::Warning, __VA_ARGS__)
#define LogError(...) LauncherLog::LogMsg(LauncherLog::Type::Error, __VA_ARGS__)
#define LogCritical(...) LauncherLog::LogMsg(LauncherLog::Type::Critical, __VA_ARGS__)

}; // namespace LauncherLog
