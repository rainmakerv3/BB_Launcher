// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "BBFormats.h"

namespace FileHelper {

class Fmg : public BBFormat {
    Q_OBJECT

public:
    explicit Fmg(std::vector<char>& data, ModMerger* parent);
    ~Fmg() override;

    bool ReadFmg(std::vector<char>& data);
    bool RepackFmg(std::vector<char>& outputData);
    bool HandleConflict(std::vector<char>& mod1Data, std::vector<char>& mod2Data);

private:
    struct FmgEntry {
        int id;
        std::string text;
    };

    std::optional<FmgEntry> GetSameEntry(const int& id, const std::vector<FmgEntry>& otherEntries);

    int fmgversion = 2; // should always be 2 for BB
    bool unicode = true;
    bool md5 = false;
    bool reuseOffsets = false; // not sure, but false seems to work for BB
    std::vector<FmgEntry> fmgEntries;
};

} // namespace FileHelper
