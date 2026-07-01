// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "BBFormats.h"

namespace FileHelper {

class Esd : public BBFormat {
    Q_OBJECT

public:
    explicit Esd(std::vector<char> data, ModMerger* parent);
    ~Esd() override;

    bool ReadEsd(std::vector<char> data);
    bool RepackEsd(std::vector<char>& outputData);
    // bool HandleConflict(const std::vector<char>& mod1Data, const std::vector<char>& mod2Data);

private:
    // std::optional<FmgEntry> GetSameEntry(const int& id, const std::vector<FmgEntry>&
    // otherEntries);

    bool longFormat;
    int gameNumber = 2;
    int Unk70, Unk74, Unk78, Unk7C;
    std::string name = "";
};

} // namespace FileHelper
