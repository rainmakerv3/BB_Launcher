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
        std::vector<std::vector<char>> arguments;

        bool operator==(const CommandCall&) const = default;
    };

    struct Condition {
        int condId; // for map only >.<
        int64_t stateOffset;
        std::optional<int64_t> targetStateId = std::nullopt;
        std::vector<char> evaluator;
        std::vector<CommandCall> passCommands;
        std::vector<Condition> subconditions;
        std::vector<int64_t> conditionOffsets;

        bool operator==(const Condition& other) const { // everything exc. the map Id
            return stateOffset == other.stateOffset && targetStateId == other.targetStateId &&
                   evaluator == other.evaluator && passCommands == other.passCommands &&
                   subconditions == other.subconditions &&
                   conditionOffsets == other.conditionOffsets;
        }
    };

    struct State { /// <summary>
        int64_t id;
        std::vector<CommandCall> entryCommands;
        std::vector<CommandCall> exitCommands;
        std::vector<CommandCall> whileCommands;
        std::vector<int64_t> conditionOffsets;
        std::vector<Condition> conditions;

        bool operator==(const State&) const = default;
    };

public:
    explicit Esd(std::vector<char>& data, ModMerger* parent);
    ~Esd() override;

    bool ReadEsd(std::vector<char>& data);
    bool RepackEsd(std::vector<char>& outputData);
    bool HandleConflict(std::vector<char>& mod1Data, std::vector<char>& mod2Data);

private:
    std::vector<CommandCall> MergeCommands(const std::vector<CommandCall>& vanilla,
                                           const std::vector<CommandCall>& mod1,
                                           const std::vector<CommandCall>& mod2);
    std::vector<Condition> MergeConditions(const std::vector<Condition>& vanilla,
                                           const std::vector<Condition>& mod1,
                                           const std::vector<Condition>& mod2, bool& success);

    std::optional<State> GetSameState(const int& id,
                                      const std::optional<std::map<int64_t, State>>& otherStates);
    std::optional<std::map<int64_t, State>> GetSameStateGroup(
        const int& id, const std::map<int64_t, std::map<int64_t, State>> stateGroups);

    void AddCommandCall(std::vector<CommandCall>& callsVector, const uint64_t& dataStart);
    bool TakeStates(const int64_t& stateSize, const std::vector<int64_t>& stateOffsets,
                    std::map<int64_t, State>& states, std::map<int64_t, int64_t>& stateIds,
                    std::map<int64_t, Esd::State>& stateGroup);
    bool GetStateAndConditions(Condition& condition, const std::map<int64_t, int64_t>& stateOffsets,
                               const std::map<int64_t, Condition>& conditionMap);

    bool longFormat;
    int gameNumber = 2;
    int Unk70, Unk74, Unk78, Unk7C;
    std::string name = "";
    std::map<int64_t, std::map<int64_t, State>> stateGroups;
};

} // namespace FileHelper
