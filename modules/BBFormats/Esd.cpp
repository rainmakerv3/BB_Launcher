// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

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

    std::unordered_map<int64_t, std::vector<int64_t>> stateGroupOffsetMap(stateGroupCount);
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

        if (stateGroupOffsetMap.contains(id)) {
            sendLog("ERROR: duplicate state id");
            return false;
        }
        stateGroupOffsetMap[id] = offsets;
    }

    std::unordered_map<int64_t, State> stateMap(stateCount);
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

        stateMap[offset] = state;
        states.push_back(state);
    }

    std::unordered_map<int64_t, Condition> conditionMap(conditionCount);
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
        debugLog("conditionOffsetsOffset: " + std::to_string(conditionOffsetsOffset));

        int64_t conditionOffsetCount;
        GetInt64(conditionOffsetCount);
        debugLog("conditionOffsetCount: " + std::to_string(conditionOffsetCount));

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
                debugLog("conditionoffset: " + std::to_string(off));
                cond.conditionOffsets.push_back(off);
            }

            StepIn(0, *istream);
            GetBytes(cond.evaluator, evaluatorLength);
            StepOut(*istream);
        }
        StepOut(*istream);

        conditionMap[offset] = cond;
    }

    for (auto& state : states) {
        // getconditions
    }

    /* holy crap T_T
             StateGroups = new Dictionary<long, Dictionary<long, State>>(stateGroupCount);
             var groupedStateOffsets = new Dictionary<long, Dictionary<long, long>>();
             foreach (long stateGroupID in stateGroupOffsets.Keys)
             {
                 long[] stateOffsets = stateGroupOffsets[stateGroupID];
                 Dictionary<long, State> stateGroup = TakeStates(stateSize, stateOffsets, states,
     out Dictionary<long, long> stateIDs); StateGroups[stateGroupID] = stateGroup;
                 groupedStateOffsets[stateGroupID] = stateIDs;

                  foreach (State state in stateGroup.Values)
                      foreach (Condition condition in state.Conditions)
                          condition.GetStateAndConditions(stateIDs, conditions);
              }
    */
    istream.reset();
    return true;
}

bool Esd::RepackEsd(std::vector<char>& outputData) {
    ostream = std::make_unique<std::stringstream>(std::ios_base::out | std::ios_base::binary);
    std::string strBuffer;

    // start here

    auto& stringStream = static_cast<std::stringstream&>(*ostream);
    std::string str = stringStream.str();
    outputData = std::vector<char>(str.begin(), str.end());

    /* tests
    std::ofstream outFile("test.fmg", std::ios::out | std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(outputData.data(), outputData.size());
        outFile.close();
    }
    */

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
            GetBytes(call.arguments, argSize);
            StepOut(*istream);
        }
    }
    StepOut(*istream);

    callsVector.push_back(call);
}

Esd::~Esd() {}

} // namespace FileHelper
