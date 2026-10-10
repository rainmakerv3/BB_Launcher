// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <map>

#include "Common.h"
#include "Log.h"

namespace LauncherLog {

const std::filesystem::path logFile = Common::GetBBLFilesPath() / "log.txt";
const std::map<LauncherLog::Type, std::string> typeMap = {
    {LauncherLog::Type::Information, "Information"},
    {LauncherLog::Type::Warning, "Warning"},
    {LauncherLog::Type::Error, "Error"},
    {LauncherLog::Type::Critical, "Critical"},
};

void LogMsg(LauncherLog::Type type, const std::string& logMsg,
            const std::source_location location) {
    std::string_view name = location.file_name();
    size_t last_slash = name.find_last_of("/\\"); // Handle both Unix and Windows separators
    if (last_slash != std::string_view::npos) {
        name.remove_prefix(last_slash + 1);
    }

    std::ofstream ofs(logFile, std::ios_base::out | std::ios_base::app);
    ofs << "[" << typeMap.at(type) << "] " << name << " line " << location.line() << ": " << logMsg
        << '\n';
    ofs.close();
}

}; // namespace LauncherLog
