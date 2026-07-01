// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "BBFormats.h"

namespace FileHelper {

class Esd : public BBFormat {
    Q_OBJECT

    struct CommandCall {
        int commandBank; // Should be 1, 5, 6, or 7.
        int commandId;
        std::vector<char> arguments;
    };

    struct Condition {
        int64_t stateOffset;
        std::optional<int64_t> targetState = std::nullopt;
        std::vector<char> evaluator;
        std::vector<CommandCall> passCommands;
        std::vector<Condition> subconditions;
        std::vector<int64_t> conditionOffsets;
    };

    struct State { /// <summary>
        int64_t id;
        std::vector<CommandCall> entryCommands;
        std::vector<CommandCall> exitCommands;
        std::vector<CommandCall> whileCommands;
        std::vector<int64_t> conditionOffsets;
    };

public:
    explicit Esd(std::vector<char> data, ModMerger* parent);
    ~Esd() override;

    bool ReadEsd(std::vector<char> data);
    bool RepackEsd(std::vector<char>& outputData);
    // bool HandleConflict(const std::vector<char>& mod1Data, const std::vector<char>& mod2Data);

private:
    // std::optional<FmgEntry> GetSameEntry(const int& id, const std::vector<FmgEntry>&
    // otherEntries);

    void AddCommandCall(std::vector<CommandCall>& callsVector, const uint64_t& dataStart);

    bool longFormat;
    int gameNumber = 2;
    int Unk70, Unk74, Unk78, Unk7C;
    std::string name = "";
    std::vector<State> states;
};

} // namespace FileHelper
