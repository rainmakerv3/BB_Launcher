// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "BBFormats.h"

namespace FileHelper {

class ConflictHandler : public BBFormat {
    Q_OBJECT

public:
    explicit ConflictHandler(const std::string& type, ModMerger* merger);
    ~ConflictHandler() override;

    bool HandleItemConflict(std::vector<char>& origData, const std::vector<char>& mod1Data,
                            const std::vector<char>& mod2Data);
    bool HandleBinderConflict(std::vector<char>& origData, const std::vector<char>& mod1Data,
                              const std::vector<char>& mod2Data);

private:
    std::string type;
};

} // namespace FileHelper
