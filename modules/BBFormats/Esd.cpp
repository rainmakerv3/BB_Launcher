// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <ranges>
#include <vector>

#include "Esd.h"
// #include "modules/ModMerger.h"

namespace fs = std::filesystem;

namespace FileHelper {

Esd::Esd(std::vector<char> data, ModMerger* parent) : BBFormat(parent) {
    bigEndian = false;
    ReadEsd(data);
}

bool Esd::ReadEsd(std::vector<char> data) {
    if (data.empty()) {
        sendLog("ERROR: empty fmg input data", LogFormat::BoldRed);
        return false;
    }

    istream = std::make_unique<std::stringstream>(std::string(data.begin(), data.end()),
                                                  std::ios_base::in | std::ios_base::binary);
    std::string strBuffer = "";
    int intBuffer = 0;

    GetStr(strBuffer, 4);
    debugLog("Magic: " + strBuffer); // should be fsSL
    longFormat = strBuffer == "fsSL";
    if (!longFormat) {
        sendLog("ERROR: unexpected esd format", LogFormat::BoldRed);
        return false;
    }

    GetInt32(intBuffer);
    debugLog("1 check: " + std::to_string(intBuffer));

    GetInt32(gameNumber); // should be 2
    debugLog("gameNumber: " + std::to_string(gameNumber));

    GetInt32(intBuffer);
    debugLog("2 check: " + std::to_string(intBuffer));

    GetInt32(intBuffer);
    debugLog("84 check: " + std::to_string(intBuffer));

    int dataSize = 0;
    GetInt32(dataSize);
    debugLog("dataSize: " + std::to_string(dataSize));

    GetInt32(intBuffer);
    debugLog("6 check: " + std::to_string(intBuffer));

    GetInt32(intBuffer); // since longformat should be true, should be 72
    debugLog("72 check: " + std::to_string(intBuffer));

    GetInt32(intBuffer);
    debugLog("1 check: " + std::to_string(intBuffer));

    int stateGroupSize = 0;
    GetInt32(stateGroupSize); // since longformat should be true, should be 32
    debugLog("stateGroupSize: " + std::to_string(stateGroupSize));

    int stateGroupCount = 0;
    GetInt32(stateGroupCount);
    debugLog("stateGroupCount: " + std::to_string(stateGroupCount));

    int stateSize = 0;
    GetInt32(stateSize); // since longformat should be true, should be 72
    debugLog("stateSize: " + std::to_string(stateSize));

    int stateCount = 0;
    GetInt32(stateCount);
    debugLog("stateCount: " + std::to_string(stateCount));

    GetInt32(intBuffer); // since longformat should be true, should be 56
    debugLog("56 check: " + std::to_string(intBuffer));

    int conditionCount = 0;
    GetInt32(conditionCount);
    debugLog("conditionCount: " + std::to_string(conditionCount));

    GetInt32(intBuffer); // since longformat should be true, should be 24
    debugLog("24 check: " + std::to_string(intBuffer));

    int commandCallCount = 0;
    GetInt32(commandCallCount);
    debugLog("commandCallCount: " + std::to_string(commandCallCount));

    GetInt32(intBuffer); // since longformat should be true, should be 16
    debugLog("16 check: " + std::to_string(intBuffer));

    int commandArgCount = 0;
    GetInt32(commandArgCount);
    debugLog("commandArgCount: " + std::to_string(commandArgCount));

    int conditionOffsetsOffset = 0;
    GetInt32(conditionOffsetsOffset);
    debugLog("conditionOffsetsOffset: " + std::to_string(conditionOffsetsOffset));

    int conditionOffsetsCount = 0;
    GetInt32(conditionOffsetsCount);
    debugLog("conditionOffsetsCount: " + std::to_string(conditionOffsetsCount));

    int nameBlockOffset = 0;
    GetInt32(nameBlockOffset);
    debugLog("nameBlockOffset: " + std::to_string(nameBlockOffset));

    int nameLength = 0;
    GetInt32(nameLength);
    debugLog("nameLength: " + std::to_string(nameLength));

    int unkOffset1 = 0;
    GetInt32(unkOffset1);
    debugLog("unkOffset1: " + std::to_string(unkOffset1));

    GetInt32(intBuffer);
    debugLog("0 check: " + std::to_string(intBuffer));

    int unkOffset2 = 0;
    GetInt32(unkOffset2);
    debugLog("unkOffset2: " + std::to_string(unkOffset2));

    GetInt32(intBuffer);
    debugLog("0 check: " + std::to_string(intBuffer));

    int64_t dataStart = static_cast<uint64_t>(istream->tellg());
    debugLog("dataStart: " + std::to_string(dataStart));

    GetInt32(intBuffer);
    debugLog("1 check: " + std::to_string(intBuffer));

    GetInt32(Unk70);
    debugLog("Unk70: " + std::to_string(Unk70));

    GetInt32(Unk74);
    debugLog("Unk74: " + std::to_string(Unk74));

    GetInt32(Unk78);
    debugLog("Unk78: " + std::to_string(Unk78));

    GetInt32(Unk7C);
    debugLog("Unk7C: " + std::to_string(Unk7C));

    GetInt32(intBuffer);
    debugLog("0 check: " + std::to_string(intBuffer));

    int64_t stateGroupsOffset;
    GetInt64(stateGroupsOffset);

    int64_t int64Buffer;
    GetInt64(int64Buffer); // should be equal stateGroupCount
    debugLog("should be same as stateGroupCount: " + std::to_string(int64Buffer));

    int64_t nameOffset;
    GetInt64(nameOffset);
    debugLog("nameOffset: " + std::to_string(nameOffset));

    GetInt64(int64Buffer); // should be equal stateGroupCount
    debugLog("should be same as nameLength: " + std::to_string(nameLength));

    GetInt64(int64Buffer);
    debugLog("-1 check: " + std::to_string(int64Buffer));

    GetInt64(int64Buffer);
    debugLog("-1 check: " + std::to_string(int64Buffer));

    if (nameLength > 0) {
        StepIn(dataStart + nameOffset, *istream);
        name = ReadUtf16String();
        StepOut(*istream);
    }
    debugLog("name: " + name);

    std::map<int64_t, std::vector<int64_t>> stateGroupOffsets;
    for (int i = 0; i < stateGroupCount; i++) {
        int64_t id;
        GetInt64(id);
        debugLog("id: " + std::to_string(id));

        int64_t statesOffset;
        GetInt64(statesOffset);
        debugLog("statesOffset: " + std::to_string(statesOffset));

        int64_t stateCountinGroup;
        GetInt64(stateCountinGroup);
        debugLog("stateCountinGroup: " + std::to_string(stateCountinGroup));

        int64_t check;
        GetInt64(check); // should be equal to statesOffset
        debugLog("should be equal to statesoffset: " + std::to_string(check));

        std::vector<int64_t> offsets(stateCountinGroup);
        for (int j = 0; j < stateCountinGroup; j++)
            offsets[j] = statesOffset + j * stateSize;

        // Every state group with more than one state has a dummy state after the end
        // that's identical to the first state original assert it, but I won't

        if (stateGroupOffsets.contains(id)) {
            sendLog("ERROR: duplicate state id");
            return false;
        }
        stateGroupOffsets[id] = offsets;
    }

    std::map<int64_t, State> states;
    for (int i = 0; i < stateCount; i++) {
        State state;
        int64_t offset = static_cast<int64_t>(istream->tellg() - dataStart);

        GetInt64(state.id);
        debugLog("state.id: " + std::to_string(state.id));

        int64_t conditionOffsetsOffset;
        GetInt64(conditionOffsetsOffset);
        debugLog("conditionOffsetsOffset: " + std::to_string(conditionOffsetsOffset));
        int64_t conditionOffsetCount;
        GetInt64(conditionOffsetCount);
        debugLog("conditionOffsetCount: " + std::to_string(conditionOffsetCount));

        int64_t entryCommandsOffset;
        GetInt64(entryCommandsOffset);
        debugLog("entryCommandsOffset: " + std::to_string(entryCommandsOffset));
        int64_t entryCommandCount;
        GetInt64(entryCommandCount);
        debugLog("entryCommandCount: " + std::to_string(entryCommandCount));

        int64_t exitCommandsOffset;
        GetInt64(exitCommandsOffset);
        debugLog("exitCommandsOffset: " + std::to_string(exitCommandsOffset));
        int64_t exitCommandCount;
        GetInt64(exitCommandCount);
        debugLog("exitCommandCount: " + std::to_string(exitCommandCount));

        int64_t whileCommandsOffset;
        GetInt64(whileCommandsOffset);
        debugLog("whileCommandsOffset: " + std::to_string(whileCommandsOffset));
        int64_t whileCommandCount;
        GetInt64(whileCommandCount);
        debugLog("whileCommandCount: " + std::to_string(whileCommandCount));

        StepIn(0, *istream);
        {
            istream->seekg(dataStart + conditionOffsetsOffset);
            for (int j = 0; j < conditionOffsetCount; j++) {
                int64_t off;
                GetInt64(off);
                debugLog("conditionOffset: " + std::to_string(j) + +": " + std::to_string(off));
                state.conditionOffsets.push_back(off);
            }

            istream->seekg(dataStart + entryCommandsOffset);
            for (int j = 0; j < entryCommandCount; j++) {
                AddCommandCall(state.entryCommands, dataStart);
            }

            istream->seekg(dataStart + exitCommandsOffset);
            for (int j = 0; j < exitCommandCount; j++) {
                AddCommandCall(state.exitCommands, dataStart);
            }

            istream->seekg(dataStart + whileCommandsOffset);
            for (int j = 0; j < whileCommandCount; j++) {
                AddCommandCall(state.whileCommands, dataStart);
            }
        }
        StepOut(*istream);

        states[offset] = state;
    }

    std::map<int64_t, Condition> conditions;
    for (int i = 0; i < conditionCount; i++) {
        Condition cond;
        int64_t offset = static_cast<int64_t>(istream->tellg() - dataStart);

        GetInt64(cond.stateOffset);
        debugLog("cond.stateOffset: " + std::to_string(cond.stateOffset));

        int64_t passCommandsOffset;
        GetInt64(passCommandsOffset);
        debugLog("passCommandsOffset: " + std::to_string(passCommandsOffset));

        int64_t passCommandCount;
        GetInt64(passCommandCount);
        debugLog("passCommandCount: " + std::to_string(passCommandCount));

        int64_t conditionOffsetsOffset;
        GetInt64(conditionOffsetsOffset);
        debugLog("subconditionOffsetsOffset: " + std::to_string(conditionOffsetsOffset));

        int64_t conditionOffsetCount;
        GetInt64(conditionOffsetCount);
        debugLog("subconditionOffsetCount: " + std::to_string(conditionOffsetCount));

        int64_t evaluatorOffset;
        GetInt64(evaluatorOffset);
        debugLog("evaluatorOffset: " + std::to_string(evaluatorOffset));

        int64_t evaluatorLength;
        GetInt64(evaluatorLength);
        debugLog("evaluatorLength: " + std::to_string(evaluatorLength));

        StepIn(0, *istream);
        {
            istream->seekg(dataStart + passCommandsOffset);
            for (int j = 0; j < passCommandCount; j++) {
                AddCommandCall(cond.passCommands, dataStart);
            }

            istream->seekg(dataStart + conditionOffsetsOffset);
            for (int j = 0; j < conditionOffsetCount; j++) {
                int64_t off;
                GetInt64(off);
                debugLog("subconditionoffset: " + std::to_string(off));
                cond.conditionOffsets.push_back(off);
            }

            StepIn(0, *istream);
            GetBytes(cond.evaluator, evaluatorLength);
            StepOut(*istream);
        }
        StepOut(*istream);
        conditions[offset] = cond;
    }

    for (auto& pair : states) {
        State& state = pair.second;
        for (const auto& offset : state.conditionOffsets) {
            Condition cond = conditions[offset];
            state.conditions.push_back(cond);
        }
        state.conditionOffsets.clear();
    }

    std::map<int64_t, std::map<int64_t, int64_t>> groupedStateOffsets;
    for (const auto& stateGroupID : std::views::keys(stateGroupOffsets)) {
        std::vector<int64_t> stateOffsets = stateGroupOffsets[stateGroupID];
        std::map<int64_t, int64_t> stateIds;
        std::map<int64_t, State> stateGroup;

        if (!TakeStates(stateSize, stateOffsets, states, stateIds, stateGroup)) {
            return false;
        }

        stateGroups[stateGroupID] = stateGroup;
        groupedStateOffsets[stateGroupID] = stateIds;
        for (State& state : std::views::values(stateGroup)) {
            for (Condition& cond : state.conditions) {
                GetStateAndConditions(cond, stateIds, conditions);
            }
        }
    }

    std::vector<Condition> conds;
    for (const auto& group : stateGroups) {
        std::function<void(const Condition&)> AddCondition = [&](const Condition& cond) {
            auto it = std::find_if(conds.begin(), conds.end(),
                                   [&cond](const Condition& c) { return c == cond; });

            if (it == conds.end()) {
                conds.push_back(cond);
                for (const auto& sub : cond.subconditions) {
                    AddCondition(sub);
                }
            }
        };

        for (const auto& statePair : group.second) {
            const State& st = statePair.second;
            for (const Condition& cond : st.conditions) {
                AddCondition(cond);
            }
        }
    }

    sendLog("DERIVED COND COUNT: " + std::to_string(conds.size()), LogFormat::BoldRed);

    if (states.size() > 0) {
        sendLog("ERROR: Orphaned state/s left", LogFormat::BoldRed);
        return false;
    }

    istream.reset();
    return true;
}

bool Esd::RepackEsd(std::vector<char>& outputData) {
    ostream = std::make_unique<std::stringstream>(std::ios_base::in | std::ios_base::out |
                                                  std::ios_base::binary);
    ostream->exceptions(std::ios_base::failbit | std::ios_base::badbit);

    std::string strBuffer;
    int64_t int64Buffer;

    WriteStr("fsSL", 4);
    WriteInt32(1);
    WriteInt32(2);
    WriteInt32(2);
    WriteInt32(0x54);

    ReserveBytes("DataSize", 4);
    WriteInt32(6);
    WriteInt32(0x48);
    WriteInt32(1);
    WriteInt32(0x20);
    WriteInt32(static_cast<int>(stateGroups.size()));

    int stateSize = 0x48;
    WriteInt32(stateSize);

    int sTotal = 0;
    for (const auto& pair : stateGroups) {
        auto& sg = pair.second;
        sTotal += sg.size() + (sg.size() == 1 ? 0 : 1);
    }
    WriteInt32(sTotal);

    WriteInt32(0x38);
    ReserveBytes("ConditionCount", 4);
    WriteInt32(0x18);
    ReserveBytes("CommandCallCount", 4);
    WriteInt32(0x10);
    ReserveBytes("CommandArgCount", 4);
    ReserveBytes("ConditionOffsetsOffset", 4);
    ReserveBytes("ConditionOffsetsCount", 4);
    ReserveBytes("NameBlockOffset", 4);
    WriteInt32(static_cast<int>(name.size() + 1)); // since name can never be null
    ReserveBytes("UnkOffset1", 4);
    WriteInt32(0);
    ReserveBytes("UnkOffset2", 4);
    WriteInt32(0);

    const int64_t dataStart = static_cast<int64_t>(ostream->tellp());
    auto getDataOffset = [&dataStart, this]() {
        return static_cast<int64_t>(ostream->tellp()) - dataStart;
    };

    WriteInt32(1);
    WriteInt32(Unk70);
    WriteInt32(Unk74);
    WriteInt32(Unk78);
    WriteInt32(Unk7C);
    WriteInt32(0);

    ReserveBytes("StateGroupsOffset", 8);
    WriteInt64(static_cast<int64_t>(stateGroups.size()));
    ReserveBytes("NameOffset", 8);
    WriteInt64(static_cast<int64_t>(name.size() + 1));
    WriteInt64(static_cast<int64_t>(-1));
    WriteInt64(static_cast<int64_t>(-1));

    std::map<int64_t, std::vector<int64_t>> stateIDs;
    for (const auto& pair : stateGroups) {
        const auto& group = pair.second;
        const auto& groupID = pair.first;

        std::vector<int64_t> idsVec;
        idsVec.reserve(group.size());
        std::ranges::copy(group | std::views::keys, std::back_inserter(idsVec));

        stateIDs[groupID] = idsVec;
    }

    if (stateGroups.size() == 0) {
        FillReservedInt64("StateGroupsOffset", -1);
    } else {
        FillReservedInt64("StateGroupsOffset", getDataOffset());
        for (const auto& pair : stateGroups) {
            int64_t groupID = pair.first;
            WriteInt64(groupID);

            std::string off1name = "StateGroup" + std::to_string(groupID) + ":StatesOffset1";
            std::string off2name = "StateGroup" + std::to_string(groupID) + ":StatesOffset2";

            ReserveBytes(off1name, 8);
            WriteInt64(static_cast<int64_t>(pair.second.size()));
            ReserveBytes(off2name, 8);
        }
    }

    std::map<int64_t, std::map<int64_t, int64_t>> stateOffsets;
    std::vector<std::pair<int64_t, int64_t>> weirdStateOffsets;
    for (const auto& pair : stateGroups) {
        int64_t groupID = pair.first;

        std::string off1name = "StateGroup" + std::to_string(groupID) + ":StatesOffset1";
        std::string off2name = "StateGroup" + std::to_string(groupID) + ":StatesOffset2";

        FillReservedInt64(off1name, getDataOffset());
        FillReservedInt64(off2name, getDataOffset());

        int64_t firstStateOffset = getDataOffset();
        for (const auto& stateID : stateIDs[groupID]) {

            stateOffsets[groupID][stateID] = getDataOffset();

            const State& st = stateGroups[groupID][stateID];
            std::string stateCondOff = std::format("State{}-{}:ConditionsOffset", groupID, stateID);
            std::string stateEntryOff =
                std::format("State{}-{}:EntryCommandsOffset", groupID, stateID);
            std::string stateExitOff =
                std::format("State{}-{}:ExitCommandsOffset", groupID, stateID);
            std::string stateWhileOff =
                std::format("State{}-{}:WhileCommandsOffset", groupID, stateID);

            WriteInt64(stateID);
            ReserveBytes(stateCondOff, 8);
            WriteInt64(static_cast<int64_t>(st.conditions.size()));
            ReserveBytes(stateEntryOff, 8);
            WriteInt64(static_cast<int64_t>(st.entryCommands.size()));
            ReserveBytes(stateExitOff, 8);
            WriteInt64(static_cast<int64_t>(st.exitCommands.size()));
            ReserveBytes(stateWhileOff, 8);
            WriteInt64(static_cast<int64_t>(st.whileCommands.size()));
        }

        if (stateGroups[groupID].size() > 1) {
            weirdStateOffsets.push_back({firstStateOffset, static_cast<int64_t>(ostream->tellp())});
            std::vector<char> nullBytes(stateSize, 0);
            ostream->write(nullBytes.data(), nullBytes.size());
        }
    }

    std::map<int64_t, std::vector<Condition>> conditions;
    for (auto& group : stateGroups) {
        std::vector<Condition> conds;
        std::function<void(Condition&)> AddCondition = [&](Condition& cond) {
            auto it = std::find_if(conds.begin(), conds.end(),
                                   [&cond](Condition& c) { return c == cond; });

            if (it == conds.end()) {
                conds.push_back(cond);
                cond.condId = conds.size();
                for (auto& sub : cond.subconditions) {
                    AddCondition(sub);
                }
            }
        };

        std::map<int64_t, State> stateGroup = group.second;
        for (const auto& statePair : group.second) {
            State st = statePair.second;
            for (Condition& cond : st.conditions) {
                AddCondition(cond);
            }
        }

        int64_t groupID = group.first;
        conditions[groupID] = conds;
    }

    int totalCondCount = 0;
    for (const auto& group : conditions) {
        totalCondCount += group.second.size();
    }
    FillReservedInt32("ConditionCount", totalCondCount);

    std::map<int, int64_t> conditionOffsets;
    for (const auto& pair : stateGroups) {
        int64_t groupID = pair.first;

        for (int i = 0; i < conditions[groupID].size(); i++) {
            Condition cond = conditions[groupID][i];
            conditionOffsets[cond.condId] = getDataOffset();

            const auto& stateOffsetsGroup = stateOffsets[groupID];
            cond.targetStateId.has_value()
                ? WriteInt64(stateOffsetsGroup.at(cond.targetStateId.value()))
                : WriteInt64(static_cast<int64_t>(-1));

            std::string passCommOff = std::format("Condition{}-{}:PassCommandsOffset", groupID, i);
            std::string condOff = std::format("Condition{}-{}:ConditionsOffset", groupID, i);
            std::string evalOff = std::format("Condition{}-{}:EvaluatorOffset", groupID, i);

            ReserveBytes(passCommOff, 8);
            WriteInt64(static_cast<int64_t>(cond.passCommands.size()));
            ReserveBytes(condOff, 8);
            WriteInt64(static_cast<int64_t>(cond.subconditions.size()));
            ReserveBytes(evalOff, 8);
            WriteInt64(static_cast<int64_t>(cond.evaluator.size()));
        }
    }

    std::vector<CommandCall> commands;
    for (const auto& pair : stateGroups) {
        int64_t groupID = pair.first;

        for (const auto& stateID : stateIDs[groupID]) {
            const auto& st = stateGroups[groupID][stateID];
            std::string stateEntryOff =
                std::format("State{}-{}:EntryCommandsOffset", groupID, stateID);
            std::string stateExitOff =
                std::format("State{}-{}:ExitCommandsOffset", groupID, stateID);
            std::string stateWhileOff =
                std::format("State{}-{}:WhileCommandsOffset", groupID, stateID);

            if (st.entryCommands.size() == 0) {
                FillReservedInt64(stateEntryOff, -1);
            } else {
                FillReservedInt64(stateEntryOff, getDataOffset());

                for (const auto& command : st.entryCommands) {
                    WriteInt32(command.commandBank);
                    WriteInt32(command.commandId);

                    std::string commandOff = std::format("Command{}:ArgsOffset", commands.size());
                    ReserveBytes(commandOff, 8);
                    WriteInt64(static_cast<int64_t>(command.arguments.size()));
                    commands.push_back(command);
                }
            }

            if (st.exitCommands.size() == 0) {
                FillReservedInt64(stateExitOff, -1);
            } else {
                FillReservedInt64(stateExitOff, getDataOffset());

                for (const auto& command : st.exitCommands) {
                    WriteInt32(command.commandBank);
                    WriteInt32(command.commandId);

                    std::string commandOff = std::format("Command{}:ArgsOffset", commands.size());
                    ReserveBytes(commandOff, 8);
                    WriteInt64(static_cast<int64_t>(command.arguments.size()));
                    commands.push_back(command);
                }
            }

            if (st.whileCommands.size() == 0) {
                FillReservedInt64(stateWhileOff, -1);
            } else {
                FillReservedInt64(stateWhileOff, getDataOffset());

                for (const auto& command : st.whileCommands) {
                    WriteInt32(command.commandBank);
                    WriteInt32(command.commandId);

                    std::string commandOff = std::format("Command{}:ArgsOffset", commands.size());
                    ReserveBytes(commandOff, 8);
                    WriteInt64(static_cast<int64_t>(command.arguments.size()));
                    commands.push_back(command);
                }
            }
        }

        for (int i = 0; i < conditions[groupID].size(); i++) {
            const auto& cond = conditions[groupID][i];
            std::string passCommOff = std::format("Condition{}-{}:PassCommandsOffset", groupID, i);

            if (cond.passCommands.size() == 0) {
                FillReservedInt64(passCommOff, -1);
            } else {
                FillReservedInt64(passCommOff, getDataOffset());

                for (const auto& command : cond.passCommands) {
                    WriteInt32(command.commandBank);
                    WriteInt32(command.commandId);

                    std::string commandOff = std::format("Command{}:ArgsOffset", commands.size());
                    ReserveBytes(commandOff, 8);
                    WriteInt64(static_cast<int64_t>(command.arguments.size()));
                    commands.push_back(command);
                }
            }
        }
    }

    FillReservedInt32("CommandCallCount", static_cast<int>(commands.size()));

    int totalArgCount = 0;
    for (const auto& comm : commands) {
        totalArgCount += comm.arguments.size();
    }
    FillReservedInt32("CommandArgCount", totalArgCount);

    for (int i = 0; i < commands.size(); i++) {
        std::string commandOff = std::format("Command{}:ArgsOffset", i);
        FillReservedInt64(commandOff, getDataOffset());

        for (int j = 0; j < commands[i].arguments.size(); j++) {
            std::string argBytesOff = std::format("Command{}-{}:BytecodeOffset", i, j);
            ReserveBytes(argBytesOff, 8);
            WriteInt64(static_cast<int64_t>(commands[i].arguments[j].size()));
        }
    }

    FillReservedInt32("ConditionOffsetsOffset", static_cast<int>(getDataOffset()));
    int conditionOffsetsCount = 0;
    for (const auto& pair : stateGroups) {
        int64_t groupID = pair.first;

        for (const auto& stateID : stateIDs[groupID]) {
            std::string stateCondOff = std::format("State{}-{}:ConditionsOffset", groupID, stateID);
            FillReservedInt64(stateCondOff, getDataOffset());
            const auto& st = stateGroups[groupID][stateID];

            for (const auto& cond : st.conditions) {
                WriteInt64(conditionOffsets[cond.condId]);
                conditionOffsetsCount += 1;
            }
        }

        for (int i = 0; i < conditions[groupID].size(); i++) {
            const auto& cond = conditions[groupID][i];
            std::string condOff = std::format("Condition{}-{}:ConditionsOffset", groupID, i);

            if (cond.subconditions.size() == 0) {
                FillReservedInt64(condOff, -1);
            } else {
                FillReservedInt64(condOff, getDataOffset());

                for (const auto& subCon : cond.subconditions) {
                    WriteInt64(conditionOffsets[subCon.condId]);
                    conditionOffsetsCount += 1;
                }
            }
        }
    }
    FillReservedInt32("ConditionOffsetsCount", conditionOffsetsCount);

    for (const auto& pair : stateGroups) {
        int64_t groupID = pair.first;
        for (int i = 0; i < conditions[groupID].size(); i++) {
            const auto& cond = conditions[groupID][i];
            std::string evalOff = std::format("Condition{}-{}:EvaluatorOffset", groupID, i);

            FillReservedInt64(evalOff, getDataOffset());
            ostream->write(cond.evaluator.data(), cond.evaluator.size());
        }
    }

    for (int i = 0; i < commands.size(); i++) {
        for (int j = 0; j < commands[i].arguments.size(); j++) {
            std::string argBytesOff = std::format("Command{}-{}:BytecodeOffset", i, j);
            FillReservedInt64(argBytesOff, getDataOffset());
            ostream->write(commands[i].arguments[j].data(), commands[i].arguments[j].size());
        }
    }

    FillReservedInt32("NameBlockOffset", static_cast<int>(getDataOffset()));
    if (name == "") {
        FillReservedInt64("NameOffset", -1);
    } else {
        PadStream(2);
        FillReservedInt64("NameOffset", getDataOffset());
        WriteUtf16String(name);
    }

    FillReservedInt32("UnkOffset1", static_cast<int>(getDataOffset()));
    FillReservedInt32("UnkOffset2", static_cast<int>(getDataOffset()));
    FillReservedInt32("DataSize", static_cast<int>(getDataOffset()));

    PadStream(0x10);

    auto& ss = static_cast<std::stringstream&>(*ostream);
    ss.seekg(0, std::ios::end);

    std::vector<char> bytes(stateSize);
    for (const auto& offsets : weirdStateOffsets) {
        ss.seekg(offsets.first);
        ss.read(bytes.data(), stateSize);

        ss.seekp(offsets.second);
        ss.write(bytes.data(), stateSize);
    }

    ss.seekg(0, std::ios::end);
    auto totalSize = ss.tellg();

    outputData.resize(totalSize);
    ss.seekg(0, std::ios::beg);
    ss.read(outputData.data(), totalSize);

    /* tests */
    std::ofstream outFile("test.esd", std::ios::out | std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(outputData.data(), outputData.size());
        outFile.close();
    }

    ostream.reset();
    sendLog("ESD Repacking complete\n");
    return true;
}

/*

bool Fmg::HandleConflict(const std::vector<char>& mod1Data, const std::vector<char>& mod2Data) {
    Fmg mod1Fmg = Fmg(mod1Data, merger);
    const std::vector<FmgEntry> mod1Fmgs = mod1Fmg.fmgEntries;

    Fmg mod2Fmg = Fmg(mod2Data, merger);
    const std::vector<FmgEntry> mod2Fmgs = mod2Fmg.fmgEntries;

    for (int i = 0; i < fmgEntries.size(); i++) {
        bool mod1Modified = false;
        bool mod2Modified = false;

        std::optional<FmgEntry> mod1entry = GetSameEntry(fmgEntries[i].id, mod1Fmgs);
        std::optional<FmgEntry> mod2entry = GetSameEntry(fmgEntries[i].id, mod2Fmgs);

        mod1Modified = mod1entry && (mod1entry->text != fmgEntries[i].text);
        mod2Modified = mod2entry && (mod2entry->text != fmgEntries[i].text);

        if (mod1Modified && mod2Modified) {
            if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                merger->SetModPriority();
                if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                    return false;
                }
            }

            if (merger->GetModPriority() == ModMerger::ModPriority::Mod1) {
                fmgEntries[i].text = mod1entry->text;
                sendLog("Unresolvable conflict in id: " + std::to_string(mod1entry->id) +
                            ", using text: " + mod1entry->text +
                            " from prioritized mod: " + merger->Mod1Name(),
                        LogFormat::Yellow);
            } else if (merger->GetModPriority() == ModMerger::ModPriority::Mod2) {
                fmgEntries[i].text = mod2entry->text;
                sendLog("Unresolvable conflict in id: " + std::to_string(mod2entry->id) +
                            ", using text: " + mod2entry->text +
                            " from prioritized mod: " + merger->Mod2Name(),
                        LogFormat::Yellow);
            }
        }

        if (mod1Modified && !mod2Modified) {
            fmgEntries[i].text = mod1entry->text;
            sendLog("Merging id: " + std::to_string(mod1entry->id) + " text: " + mod1entry->text +
                    " from mod: " + merger->Mod1Name());
        } else if (mod2Modified && !mod1Modified) {
            fmgEntries[i].text = mod2entry->text;
            sendLog("Merging id: " + std::to_string(mod2entry->id) + " text: " + mod2entry->text +
                    " from mod: " + merger->Mod2Name());
        }
    }

    for (const auto& fmg : mod1Fmgs) {
        std::optional<FmgEntry> origEntry = GetSameEntry(fmg.id, fmgEntries);

        if (!origEntry.has_value()) {
            fmgEntries.push_back(fmg);
            sendLog("Fmg data added id: " + std::to_string(fmg.id) + " text: " + fmg.text);
        }
    }

    for (const auto& fmg : mod2Fmgs) {
        std::optional<FmgEntry> origEntry = GetSameEntry(fmg.id, fmgEntries);

        if (!origEntry.has_value()) {
            fmgEntries.push_back(fmg);
            sendLog("Fmg data added id: " + std::to_string(fmg.id) + " text: " + fmg.text);
        }
    }

    return true;
}

std::optional<Fmg::FmgEntry> Fmg::GetSameEntry(const int& id,
                                               const std::vector<Fmg::FmgEntry>& otherEntries) {
    std::optional<FmgEntry> result = std::nullopt;
    for (const auto& otherEntry : otherEntries) {
        if (otherEntry.id == id) {
            return result.emplace(otherEntry);
        }
    }

    return result;
}
*/

void Esd::AddCommandCall(std::vector<CommandCall>& callsVector, const uint64_t& dataStart) {
    CommandCall call;
    GetInt32(call.commandBank); // assert 1/3/5/7?
    debugLog("call.commandBank" + std::to_string(call.commandBank));

    GetInt32(call.commandId);
    debugLog("call.commandId" + std::to_string(call.commandId));

    int64_t argsOffset;
    GetInt64(argsOffset);
    debugLog("argsOffset" + std::to_string(argsOffset));

    int64_t argsCount;
    GetInt64(argsCount);
    debugLog("argsCount" + std::to_string(argsCount));

    StepIn(dataStart + argsOffset, *istream);
    {
        for (int i = 0; i < argsCount; i++) {
            int64_t argOffset;
            GetInt64(argOffset);
            debugLog("argOffset" + std::to_string(argOffset));

            int64_t argSize;
            GetInt64(argSize);
            debugLog("argSize" + std::to_string(argSize));

            StepIn(dataStart + argOffset, *istream);
            std::vector<char> buff(argSize);
            GetBytes(buff, argSize);
            call.arguments.push_back(buff);
            StepOut(*istream);
        }
    }
    StepOut(*istream);

    callsVector.push_back(call);
}

bool Esd::TakeStates(const int64_t& stateSize, std::vector<int64_t>& stateOffsets,
                     std::map<int64_t, State>& states, std::map<int64_t, int64_t>& stateIds,
                     std::map<int64_t, Esd::State>& stateGroup) {
    if (stateOffsets.size() > 1) {
        int64_t weirdStateOffset = stateOffsets[0] + stateSize * stateOffsets.size();
        if (!states.erase(weirdStateOffset)) {
            sendLog("ERROR: Weird state not found", LogFormat::BoldRed);
            return false;
        }
    }

    for (const auto& offset : stateOffsets) {
        State state = states[offset];
        if (stateGroup.contains(state.id)) {
            sendLog("ERROR: Duplicate state id when taking states", LogFormat::BoldRed);
            return false;
        }

        stateGroup[state.id] = state;
        states.erase(offset);
        stateIds[offset] = state.id;
    }

    stateOffsets.clear();
    return true;
}

bool Esd::GetStateAndConditions(Condition& cond, const std::map<int64_t, int64_t>& stateOffsets,
                                const std::map<int64_t, Condition>& conditionMap) {
    if (cond.stateOffset == -2) {
        debugLog("condition stateoffset already processed");
        return true;
    }

    if (cond.stateOffset == -1) {
        cond.targetStateId = std::nullopt;
    } else if (stateOffsets.contains(cond.stateOffset)) {
        cond.targetStateId = stateOffsets.at(cond.stateOffset);
    } else {
        sendLog("ERROR: Condition target state not found.", LogFormat::BoldRed);
        return false;
    }
    cond.stateOffset = -2;

    for (const auto& offset : cond.conditionOffsets) {
        Condition newcond = conditionMap.at(offset);
        cond.subconditions.push_back(newcond);
    }
    cond.conditionOffsets.clear();

    for (Condition& sub : cond.subconditions) {
        GetStateAndConditions(sub, stateOffsets, conditionMap);
    }
    return true;
}

Esd::~Esd() {}

} // namespace FileHelper
