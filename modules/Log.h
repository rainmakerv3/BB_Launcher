// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <source_location>
#include <string>

namespace Log {

enum Type : int { Information, Warning, Error, Critical };

void LogMsg(Log::Type type, const std::string& logMsg,
            const std::source_location location = std::source_location::current());

#define LogInfo(...) Log::LogMsg(Log::Type::Information, __VA_ARGS__)
#define LogWarning(...) Log::LogMsg(Log::Type::Warning, __VA_ARGS__)
#define LogError(...) Log::LogMsg(Log::Type::Error, __VA_ARGS__)
#define LogCritical(...) Log::LogMsg(Log::Type::Critical, __VA_ARGS__)

}; // namespace Log
