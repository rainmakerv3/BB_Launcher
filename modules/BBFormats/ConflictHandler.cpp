// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ConflictHandler.h"
#include "Tpf.h"

namespace fs = std::filesystem;

namespace FileHelper {

ConflictHandler::ConflictHandler(const std::string& fileType, ModMerger* merger)
    : BBFormat(merger) {
    type = fileType;
}

bool ConflictHandler::HandleItemConflict(std::vector<char>& origData,
                                         const std::vector<char>& mod1Data,
                                         const std::vector<char>& mod2Data) {
    if (type.contains("TPF")) {
        Tpf origTpf(origData, merger);
        if (!origTpf.HandleConflict(mod1Data, mod2Data)) {
            return false;
        } else {
            origData.clear();
            origTpf.RepackTpf(origData);
        }
    }

    return true;
}

ConflictHandler::~ConflictHandler() {}

} // namespace FileHelper
