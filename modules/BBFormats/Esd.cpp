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
    debugLog("stateGroupCount: " + std::to_string(stateCount));

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

        int64_t stateCount;
        GetInt64(stateCount);
        debugLog("stateCount: " + std::to_string(stateCount));

        int64_t check;
        GetInt64(check); // should be equal to statesOffset
        debugLog("should be equal to statesoffset: " + std::to_string(check));

        std::vector<int64_t> offsets(stateCount);
        for (int i = 0; i < stateCount; i++)
            offsets[i] = statesOffset + i * stateSize;

        // Every state group with more than one state has a dummy state after the end
        // that's identical to the first state original assert it, but I won't

        if (stateGroupOffsetMap.contains(id)) {
            sendLog("ERROR: duplicate state id");
            return false;
        }
        stateGroupOffsetMap[id] = offsets;
    }

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

Esd::~Esd() {}

} // namespace FileHelper
