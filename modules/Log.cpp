// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <map>

#include "Common.h"
#include "Log.h"

namespace Log {

const std::filesystem::path logFile = Common::GetBBLFilesPath() / "log.txt";
const std::map<Log::Type, std::string> typeMap = {
    {Log::Type::Information, "Information"},
    {Log::Type::Warning, "Warning"},
    {Log::Type::Error, "Error"},
    {Log::Type::Critical, "Critical"},
};

void LogMsg(Log::Type type, const std::string& logMsg, const std::source_location location) {
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

}; // namespace Log
