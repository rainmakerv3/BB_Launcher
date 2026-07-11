// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <ranges>
#include <vector>

#include "Esd.h"
#include "modules/ModMerger.h"

namespace fs = std::filesystem;

namespace FileHelper {

Esd::Esd(std::vector<char>& data, ModMerger* parent) : BBFormat(parent) {
    bigEndian = false;
    ReadEsd(data);
}

bool Esd::ReadEsd(std::vector<char>& data) {
    if (data.empty()) {
        sendLog("ERROR: empty esd input data", LogFormat::BoldRed);
        return false;
    }

    istream = std::make_unique<std::stringstream>(std::string(data.begin(), data.end()),
                                                  std::ios_base::in | std::ios_base::binary);
    std::string strBuffer = "";
    int intBuffer = 0;
    int64_t int64Buffer = 0;

    GetStr(strBuffer, 4);
    debugLog("Magic: " + strBuffer); // should be fsSL
    longFormat = strBuffer == "fsSL";
    if (!longFormat) {
        sendLog("ERROR: unexpected esd format", LogFormat::BoldRed);
        return false;
    }

    GetInt32(intBuffer);
    GetInt32(gameNumber); // should be 2
    GetInt32(intBuffer);  // should also be 2
    GetInt32(intBuffer);  // should be 84

    int dataSize = 0;
    GetInt32(dataSize);

    GetInt32(intBuffer); // should be 6
    GetInt32(intBuffer); // since longformat should be true, should be 72

    GetInt32(intBuffer); // should be 1

    int stateGroupSize = 0;
    GetInt32(stateGroupSize); // since longformat should be true, should be 32

    int stateGroupCount = 0;
    GetInt32(stateGroupCount);
    debugLog("stateGroupCount: " + std::to_string(stateGroupCount));

    int stateSize = 0;
    GetInt32(stateSize); // since longformat should be true, should be 72

    int stateCount = 0;
    GetInt32(stateCount);
    debugLog("stateCount: " + std::to_string(stateCount));

    GetInt32(intBuffer); // since longformat should be true, should be 56

    int conditionCount = 0;
    GetInt32(conditionCount);
    debugLog("conditionCount: " + std::to_string(conditionCount));

    GetInt32(intBuffer); // since longformat should be true, should be 24

    int commandCallCount = 0;
    GetInt32(commandCallCount);
    debugLog("commandCallCount: " + std::to_string(commandCallCount));

    GetInt32(intBuffer); // since longformat should be true, should be 16

    int commandArgCount = 0;
    GetInt32(commandArgCount);
    debugLog("commandArgCount: " + std::to_string(commandArgCount));

    int beginConditionOffsetsOffset = 0;
    GetInt32(beginConditionOffsetsOffset);
    debugLog("beginConditionOffsetsOffset: " + std::to_string(beginConditionOffsetsOffset));

    int totalConditionOffsetsCount = 0;
    GetInt32(totalConditionOffsetsCount);
    debugLog("totalConditionOffsetsCount: " + std::to_string(totalConditionOffsetsCount));

    int nameBlockOffset = 0;
    GetInt32(nameBlockOffset);

    int nameLength = 0;
    GetInt32(nameLength);

    int unkOffset1 = 0;
    GetInt32(unkOffset1);

    GetInt32(intBuffer);

    int unkOffset2 = 0;
    GetInt32(unkOffset2);
    GetInt32(intBuffer);

    int64_t dataStart = static_cast<uint64_t>(istream->tellg());

    GetInt32(intBuffer); // should be 1
    GetInt32(Unk70);
    GetInt32(Unk74);
    GetInt32(Unk78);
    GetInt32(Unk7C);
    GetInt32(intBuffer); // should be 0

    int64_t stateGroupsOffset;
    GetInt64(stateGroupsOffset);
    debugLog("stateGroupsOffset: " + std::to_string(stateGroupsOffset));

    GetInt64(int64Buffer); // should be equal stateGroupCount

    int64_t nameOffset;
    GetInt64(nameOffset);

    GetInt64(int64Buffer);

    GetInt64(int64Buffer); // should be -1
    GetInt64(int64Buffer); // should also be -1

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

        int64_t statesOffset;
        GetInt64(statesOffset);

        int64_t stateCountinGroup;
        GetInt64(stateCountinGroup);

        int64_t check;
        GetInt64(check); // should be equal to statesOffset

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

    debugLog("statesmap Offset: " + std::to_string(static_cast<uint64_t>(istream->tellg())));
    std::map<int64_t, State> states;
    for (int i = 0; i < stateCount; i++) {
        State state;
        int64_t offset = static_cast<int64_t>(istream->tellg()) - dataStart;

        GetInt64(state.id);

        int64_t conditionOffsetsOffset;
        GetInt64(conditionOffsetsOffset);

        int64_t conditionOffsetCount;
        GetInt64(conditionOffsetCount);

        int64_t entryCommandsOffset;
        GetInt64(entryCommandsOffset);

        int64_t entryCommandCount;
        GetInt64(entryCommandCount);

        int64_t exitCommandsOffset;
        GetInt64(exitCommandsOffset);

        int64_t exitCommandCount;
        GetInt64(exitCommandCount);

        int64_t whileCommandsOffset;
        GetInt64(whileCommandsOffset);

        int64_t whileCommandCount;
        GetInt64(whileCommandCount);

        StepIn(0, *istream);
        {
            istream->seekg(dataStart + conditionOffsetsOffset);
            for (int j = 0; j < conditionOffsetCount; j++) {
                int64_t off;
                GetInt64(off);
                state.conditionOffsets.push_back(off);
            }

            istream->seekg(dataStart + entryCommandsOffset);
            for (int j = 0; j < entryCommandCount; j++) {
                AddCommandCall(state.entryCommands, dataStart);
            }

            std::sort(state.entryCommands.begin(), state.entryCommands.end(),
                      [](const CommandCall& a, const CommandCall& b) {
                          return a.commandId < b.commandId;
                      });

            istream->seekg(dataStart + exitCommandsOffset);
            for (int j = 0; j < exitCommandCount; j++) {
                AddCommandCall(state.exitCommands, dataStart);
            }

            std::sort(state.exitCommands.begin(), state.exitCommands.end(),
                      [](const CommandCall& a, const CommandCall& b) {
                          return a.commandId < b.commandId;
                      });

            istream->seekg(dataStart + whileCommandsOffset);
            for (int j = 0; j < whileCommandCount; j++) {
                AddCommandCall(state.whileCommands, dataStart);
            }

            std::sort(state.whileCommands.begin(), state.whileCommands.end(),
                      [](const CommandCall& a, const CommandCall& b) {
                          return a.commandId < b.commandId;
                      });
        }
        StepOut(*istream);

        states[offset] = state;
    }

    debugLog("conditionsmap Offset: " + std::to_string(static_cast<uint64_t>(istream->tellg())));
    std::map<int64_t, Condition> conditions;
    for (int i = 0; i < conditionCount; i++) {
        Condition cond;
        int64_t offset = static_cast<int64_t>(istream->tellg()) - dataStart;
        GetInt64(cond.stateOffset);

        int64_t passCommandsOffset;
        GetInt64(passCommandsOffset);

        int64_t passCommandCount;
        GetInt64(passCommandCount);

        int64_t conditionOffsetsOffset;
        GetInt64(conditionOffsetsOffset);
        int64_t conditionOffsetCount;
        GetInt64(conditionOffsetCount);

        int64_t evaluatorOffset;
        GetInt64(evaluatorOffset);

        int64_t evaluatorLength;
        GetInt64(evaluatorLength);

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
                cond.conditionOffsets.push_back(off);
            }

            StepIn(dataStart + evaluatorOffset, *istream);
            GetBytes(cond.evaluator, evaluatorLength);
            StepOut(*istream);
        }
        StepOut(*istream);
        cond.condId = i;
        conditions[offset] = cond;
    }

    for (State& state : std::views::values(states)) {
        for (const auto& offset : state.conditionOffsets) {
            auto it = conditions.find(offset);
            if (it == conditions.end()) {
                sendLog("ERROR: State condition offset state not found.", LogFormat::BoldRed);
                return false;
            }
            Condition cond = conditions[offset];
            state.conditions.push_back(cond);
        }
        state.conditionOffsets.clear();
    }

    for (const auto& stateGroupID : std::views::keys(stateGroupOffsets)) {
        const std::vector<int64_t>& stateOffsets = stateGroupOffsets[stateGroupID];
        std::map<int64_t, int64_t> stateIds;
        std::map<int64_t, State> stateGroup;

        if (!TakeStates(stateSize, stateOffsets, states, stateIds, stateGroup)) {
            return false;
        }

        for (State& state : std::views::values(stateGroup)) {
            for (Condition& cond : state.conditions) {
                GetStateAndConditions(cond, stateIds, conditions);
            }
        }

        stateGroups[stateGroupID] = stateGroup;
    }

    int totalCondCount = 0;
    std::map<int64_t, std::vector<Condition>> conditionsMAP;
    for (auto& group : stateGroups) {
        std::vector<Condition> conds;
        std::function<void(Condition&)> AddCondition = [&](Condition& cond) {
            auto it = std::find_if(conds.begin(), conds.end(),
                                   [&cond](Condition& c) { return c == cond; });

            if (it == conds.end()) {
                conds.push_back(cond);
                for (auto& sub : cond.subconditions) {
                    AddCondition(sub);
                }
            }
        };

        for (auto& statePair : group.second) {
            State& st = statePair.second;
            for (Condition& cond : st.conditions) {
                AddCondition(cond);
            }
        }

        int64_t groupID = group.first;
        conditionsMAP[groupID] = conds;
        totalCondCount += conds.size();
    }

    debugLog("derived cond count 1: " + std::to_string(conditionsMAP.size()), LogFormat::Yellow);
    debugLog("derived cond count 2: " + std::to_string(totalCondCount), LogFormat::Yellow);

    if (states.size() > 0) {
        sendLog("ERROR: Orphaned state/s left", LogFormat::BoldRed);
        return false;
    }

    istream.reset();
    return true;
}

void Esd::AddCommandCall(std::vector<CommandCall>& callsVector, const uint64_t& dataStart) {
    CommandCall call;

    GetInt32(call.commandBank); // assert 1/3/5/7?
    GetInt32(call.commandId);

    int64_t argsOffset;
    GetInt64(argsOffset);

    int64_t argsCount;
    GetInt64(argsCount);

    StepIn(dataStart + argsOffset, *istream);
    {
        for (int i = 0; i < argsCount; i++) {
            int64_t argOffset;
            GetInt64(argOffset);

            int64_t argSize;
            GetInt64(argSize);

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

bool Esd::TakeStates(const int64_t& stateSize, const std::vector<int64_t>& stateOffsets,
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
    name == "" ? WriteInt32(static_cast<int>(0)) : WriteInt32(static_cast<int>(name.size() + 1));
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
    name == "" ? WriteInt64(static_cast<int64_t>(0))
               : WriteInt64(static_cast<int64_t>(name.size() + 1));
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
    for (const auto& groupID : std::views::keys(stateGroups)) {
        std::string off1name = "StateGroup" + std::to_string(groupID) + ":StatesOffset1";
        std::string off2name = "StateGroup" + std::to_string(groupID) + ":StatesOffset2";

        FillReservedInt64(off1name, getDataOffset());
        FillReservedInt64(off2name, getDataOffset());

        int64_t firstStateOffset = static_cast<int64_t>(ostream->tellp());
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
                                   [&cond](Condition& c) { return c.condId == cond.condId; });

            if (it == conds.end()) {
                conds.push_back(cond);
                for (auto& sub : cond.subconditions) {
                    AddCondition(sub);
                }
            }
        };

        for (auto& statePair : group.second) {
            State& st = statePair.second;
            for (Condition& cond : st.conditions) {
                AddCondition(cond);
            }
        }

        int64_t groupID = group.first;
        conditions[groupID] = conds;
    }

    int totalCondCount = 0;
    for (const auto& conditions : std::views::values(conditions)) {
        totalCondCount += conditions.size();
    }

    FillReservedInt32("ConditionCount", totalCondCount);
    debugLog("totalCondCount: " + std::to_string(totalCondCount), LogFormat::BoldRed);

    std::map<int, int64_t> conditionOffsets;
    for (const auto& groupID : std::views::keys(stateGroups)) {
        for (int i = 0; i < conditions[groupID].size(); i++) {
            Condition& cond = conditions[groupID][i];
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
    for (const auto& groupID : std::views::keys(stateGroups)) {
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
    for (const auto& groupID : std::views::keys(stateGroups)) {
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
    for (const auto& groupID : std::views::keys(stateGroups)) {
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

    /* tests
    std::ofstream outFile("test.esd", std::ios::out | std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(outputData.data(), outputData.size());
        outFile.close();
    }
    */

    ostream.reset();
    sendLog("ESD Repacking complete\n");
    return true;
}

bool Esd::HandleConflict(std::vector<char>& mod1Data, std::vector<char>& mod2Data) {
    Esd mod1Esd = Esd(mod1Data, merger);
    const std::map<int64_t, std::map<int64_t, State>> mod1StateGroups = mod1Esd.stateGroups;

    Esd mod2Esd = Esd(mod2Data, merger);
    const std::map<int64_t, std::map<int64_t, State>> mod2StateGroups = mod2Esd.stateGroups;

    for (auto& groupPair : stateGroups) {
        auto& stateGroup = groupPair.second;
        const auto& stateGroupID = groupPair.first;

        std::optional<std::map<int64_t, State>> mod1group =
            GetSameStateGroup(stateGroupID, mod1StateGroups);
        std::optional<std::map<int64_t, State>> mod2group =
            GetSameStateGroup(stateGroupID, mod2StateGroups);

        for (auto& statePair : stateGroup) {
            const int stateID = statePair.first;
            State& st = statePair.second;

            bool mod1Modified = false;
            bool mod2Modified = false;

            std::optional<State> mod1st = GetSameState(stateID, mod1group);
            std::optional<State> mod2st = GetSameState(stateID, mod2group);

            mod1Modified = mod1st && (mod1st != st);
            mod2Modified = mod2st && (mod2st != st);

            if (mod1Modified && mod2Modified) {
                bool mergeSuccessful = true;
                State mergedState;
                mergedState.id = stateID;

                mergedState.entryCommands =
                    MergeCommands(st.entryCommands, mod1st->entryCommands, mod2st->entryCommands);
                mergedState.exitCommands =
                    MergeCommands(st.exitCommands, mod1st->exitCommands, mod2st->exitCommands);
                mergedState.whileCommands =
                    MergeCommands(st.whileCommands, mod1st->whileCommands, mod2st->whileCommands);
                mergedState.conditions = MergeConditions(st.conditions, mod1st->conditions,
                                                         mod2st->conditions, mergeSuccessful);

                if (mergeSuccessful) {
                    st = mergedState;
                    sendLog("Successfully deep-merged contents of State ID: " +
                            std::to_string(stateID));
                } else {
                    if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                        QMetaObject::invokeMethod(merger, &ModMerger::OpenPriorityDialog,
                                                  Qt::BlockingQueuedConnection);
                        if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                            return false;
                        }
                    }

                    if (merger->GetModPriority() == ModMerger::ModPriority::Mod1) {
                        st = mod1st.value();
                        sendLog("Logical conflict in state id: " + std::to_string(stateID) +
                                    ", fallback to prioritized mod data: " + merger->Mod1Name(),
                                LogFormat::Yellow);
                    } else if (merger->GetModPriority() == ModMerger::ModPriority::Mod2) {
                        st = mod2st.value();
                        sendLog("Logical conflict in state id: " + std::to_string(stateID) +
                                    ", fallback to prioritized mod data: " + merger->Mod2Name(),
                                LogFormat::Yellow);
                    }
                }
            } else if (mod1Modified && !mod2Modified) {
                st = mod1st.value();
                sendLog("Merging modified state: " + std::to_string(stateID) +
                        " from mod: " + merger->Mod1Name());
            } else if (mod2Modified && !mod1Modified) {
                st = mod2st.value();
                sendLog("Merging modified state: " + std::to_string(stateID) +
                        " from mod: " + merger->Mod2Name());
            }
        }

        std::unordered_set<int> existingIds;
        for (const auto& id : std::views::keys(stateGroup)) {
            existingIds.insert(id);
        }

        if (mod1group.has_value()) {
            for (const auto& stPair : mod1group.value()) {
                if (existingIds.find(stPair.first) == existingIds.end()) {
                    stateGroup[stPair.first] = stPair.second;
                    sendLog("New state added to group : " + std::to_string(stateGroupID) +
                            " state ID: " + std::to_string(stPair.first));
                }
            }
        }

        if (mod2group.has_value()) {
            for (const auto& stPair : mod2group.value()) {
                if (existingIds.find(stPair.first) == existingIds.end()) {
                    stateGroup[stPair.first] = stPair.second;
                    sendLog("New state added to group : " + std::to_string(stateGroupID) +
                            " state ID: " + std::to_string(stPair.first));
                }
            }
        }
    }

    std::unordered_set<int> existingGroupIds;
    for (const auto& id : std::views::keys(stateGroups)) {
        existingGroupIds.insert(id);
    }

    for (const auto& sg : mod1StateGroups) {
        if (existingGroupIds.find(sg.first) == existingGroupIds.end()) {
            stateGroups[sg.first] = sg.second;
            sendLog("New state group added id: " + std::to_string(sg.first));
        }
    }

    for (const auto& sg : mod2StateGroups) {
        if (existingGroupIds.find(sg.first) == existingGroupIds.end()) {
            stateGroups[sg.first] = sg.second;
            sendLog("New state group added id: " + std::to_string(sg.first));
        }
    }

    return true;
}

std::vector<Esd::CommandCall> Esd::MergeCommands(const std::vector<CommandCall>& vanilla,
                                                 const std::vector<CommandCall>& mod1,
                                                 const std::vector<CommandCall>& mod2) {
    if (mod1 == vanilla && mod2 != vanilla)
        return mod2;
    if (mod2 == vanilla && mod1 != vanilla)
        return mod1;

    std::vector<CommandCall> result = vanilla;

    for (const auto& cmd : mod1) {
        if (std::find(vanilla.begin(), vanilla.end(), cmd) == vanilla.end()) {
            result.push_back(cmd);
        }
    }

    for (const auto& cmd : mod2) {
        if (std::find(vanilla.begin(), vanilla.end(), cmd) == vanilla.end() &&
            std::find(result.begin(), result.end(), cmd) == result.end()) {
            result.push_back(cmd);
        }
    }
    return result;
}

std::vector<Esd::Condition> Esd::MergeConditions(const std::vector<Condition>& vanilla,
                                                 const std::vector<Condition>& mod1,
                                                 const std::vector<Condition>& mod2,
                                                 bool& success) {
    if (!success)
        return mod1;
    if (mod1 == vanilla && mod2 != vanilla)
        return mod2;
    if (mod2 == vanilla && mod1 != vanilla)
        return mod1;

    std::vector<Condition> result;

    size_t vanillaSize = vanilla.size();
    size_t maxSize = std::max(mod1.size(), mod2.size());

    // index matching does not work, not sure what to do...
    for (size_t i = 0; i < maxSize; ++i) {
        bool inVanilla = (i < vanillaSize);
        bool inMod1 = (i < mod1.size());
        bool inMod2 = (i < mod2.size());

        if (inVanilla) {
            const Condition& vanCond = vanilla[i];

            if (inMod1 && inMod2) {
                const Condition& c1 = mod1[i];
                const Condition& c2 = mod2[i];

                if (c1.evaluator != c2.evaluator && c1.evaluator != vanCond.evaluator &&
                    c2.evaluator != vanCond.evaluator) {
                    success = false;
                    return result;
                }

                if (c1.targetStateId != c2.targetStateId &&
                    c1.targetStateId != vanCond.targetStateId &&
                    c2.targetStateId != vanCond.targetStateId) {
                    success = false;
                    return result;
                }

                Condition mergedCond =
                    (c1.evaluator != vanCond.evaluator || c1.targetStateId != vanCond.targetStateId)
                        ? c1
                        : c2;

                mergedCond.passCommands =
                    MergeCommands(vanCond.passCommands, c1.passCommands, c2.passCommands);
                mergedCond.subconditions = MergeConditions(vanCond.subconditions, c1.subconditions,
                                                           c2.subconditions, success);

                result.push_back(mergedCond);
            } else if (inMod1) {
                result.push_back(mod1[i]);
            } else if (inMod2) {
                result.push_back(mod2[i]);
            }
        } else {
            if (inMod1 && inMod2) {
                if (mod1[i] == mod2[i]) {
                    result.push_back(mod1[i]);
                } else {
                    success = false;
                    return result;
                }
            } else if (inMod1) {
                result.push_back(mod1[i]);
            } else if (inMod2) {
                result.push_back(mod2[i]);
            }
        }
    }

    return result;
}

std::optional<Esd::State> Esd::GetSameState(
    const int& id, const std::optional<std::map<int64_t, State>>& otherStates) {
    if (!otherStates.has_value()) {
        return std::nullopt;
    }

    std::optional<State> result = std::nullopt;
    for (const auto& otherEntry : otherStates.value()) {
        if (otherEntry.second.id == id) {
            return result.emplace(otherEntry.second);
        }
    }

    return result;
}

std::optional<std::map<int64_t, Esd::State>> Esd::GetSameStateGroup(
    const int& id, const std::map<int64_t, std::map<int64_t, Esd::State>> otherGroups) {
    std::optional<std::map<int64_t, Esd::State>> result = std::nullopt;
    for (const auto& group : otherGroups) {
        if (group.first == id) {
            return result.emplace(group.second);
        }
    }

    return result;
}

Esd::~Esd() {}

} // namespace FileHelper
