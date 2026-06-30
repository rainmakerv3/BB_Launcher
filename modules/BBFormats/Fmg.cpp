// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <vector>

#include "Fmg.h"
#include "modules/ModMerger.h"

namespace fs = std::filesystem;

namespace FileHelper {

Fmg::Fmg(std::vector<char> data, ModMerger* parent) : BBFormat(parent) {
    bigEndian = false;
    ReadFmg(data);
}

bool Fmg::ReadFmg(std::vector<char> data) {
    if (data.empty()) {
        sendLog("ERROR: empty fmg input data", LogFormat::BoldRed);
        return false;
    }

    istream = std::make_unique<std::stringstream>(std::string(data.begin(), data.end()),
                                                  std::ios_base::in | std::ios_base::binary);

    std::string strBuffer = "";
    int intBuffer = 0;

    GetByte(intBuffer);
    debugLog("md5: " + std::to_string(intBuffer));
    if (intBuffer != 0) {
        md5 = true;
        istream->seekg(17); // md5 bytes exist, must be skipped
    }

    GetByte(bigEndian);
    debugLog("bigEndian: " + std::to_string(bigEndian));

    GetByte(intBuffer);
    debugLog("version: " + std::to_string(intBuffer));

    if (intBuffer != 2) {
        sendLog("ERROR: Unexpected FMG version: " + std::to_string(intBuffer), LogFormat::BoldRed);
    }

    istream->ignore(1); // or assert 0

    GetInt32(intBuffer);
    debugLog("filesize: " + std::to_string(intBuffer));

    GetByte(unicode);
    debugLog("unicode: " + std::to_string(unicode)); // should be 1?

    istream->ignore(3); // or assert 0

    int entryCount;
    GetInt32(entryCount);
    debugLog("entryCount: " + std::to_string(entryCount));

    GetInt32(intBuffer);
    debugLog("stringCount: " + std::to_string(intBuffer));

    istream->ignore(4); // or assert 0xFF

    uint64_t stringOffsetOffset;
    GetInt64(stringOffsetOffset);
    debugLog("stringOffsetOffset: " + std::to_string(stringOffsetOffset));

    if (md5) {
        stringOffsetOffset += 16;
    }

    istream->ignore(8); // or assert 0

    for (int i = 0; i < entryCount; i++) {
        int offsetIndex;
        GetInt32(offsetIndex);
        debugLog("offsetIndex: " + std::to_string(offsetIndex));

        int firstID;
        GetInt32(firstID);
        debugLog("firstID: " + std::to_string(firstID));

        int lastID;
        GetInt32(lastID);
        debugLog("lastID: " + std::to_string(lastID));

        istream->ignore(4); // or assert 0

        StepIn(stringOffsetOffset + offsetIndex * 8, *istream);
        for (int j = 0; j < lastID - firstID + 1; j++) {
            uint64_t stringOffset;
            GetInt64(stringOffset);
            debugLog("stringOffset: " + std::to_string(stringOffset));
            if (md5)
                stringOffset += 16;

            FmgEntry entry;
            entry.id = firstID + j;
            debugLog("entry.id: " + std::to_string(entry.id));

            if (stringOffset > 0) {
                if (unicode) {
                    StepIn(stringOffset, *istream);
                    entry.text = ReadUtf16String();
                    debugLog("entry.text: " + entry.text);
                    StepOut(*istream);
                } else {
                    sendLog("ERROR unexpected fmg string encoding", LogFormat::BoldRed);
                    return false;
                }
            }

            fmgEntries.push_back(entry);
        }
        StepOut(*istream);
    }

    istream.reset();
    return true;
}

bool Fmg::RepackFmg(std::vector<char>& outputData) {
    ostream = std::make_unique<std::stringstream>(std::ios_base::out | std::ios_base::binary);
    std::string strBuffer;

    WriteByte(0);
    WriteByte(0); // big endian = false
    WriteByte(2); // version
    WriteByte(0);

    ReserveBytes("FileSize", 4);
    WriteByte(1); // unicode = true
    WriteByte(0);
    WriteByte(0);
    WriteByte(0);

    ReserveBytes("GroupCount", 4);
    WriteInt32(static_cast<int>(fmgEntries.size()));
    WriteInt32(0xFF);

    ReserveBytes("StringOffsets", 8);
    WriteInt64(static_cast<uint64_t>(0));

    int entryCount = 0;
    std::sort(fmgEntries.begin(), fmgEntries.end(),
              [](const auto& e1, const auto& e2) { return e1.id < e2.id; });

    for (int i = 0; i < fmgEntries.size(); i++) {
        WriteInt32(i);
        WriteInt32(fmgEntries[i].id);

        while (i < fmgEntries.size() - 1 && fmgEntries[i + 1].id == fmgEntries[i].id + 1) {
            i++;
        }

        WriteInt32(fmgEntries[i].id);
        WriteInt32(0);

        entryCount++;
    }

    FillReservedInt32("GroupCount", entryCount);
    FillReservedInt64("StringOffsets", static_cast<uint64_t>(ostream->tellp()));

    for (int i = 0; i < fmgEntries.size(); i++) {
        strBuffer = "StringOffset" + std::to_string(i);
        ReserveBytes(strBuffer, 8);
    }

    for (int i = 0; i < fmgEntries.size(); i++) {
        strBuffer = "StringOffset" + std::to_string(i);

        if (!fmgEntries[i].text.empty()) {
            FillReservedInt64(strBuffer, static_cast<uint64_t>(ostream->tellp()));
            if (unicode) {
                WriteUtf16String(fmgEntries[i].text);
            } else {
                // hardcoded not to encounter, should be ok
            }
        } else {
            FillReservedInt64(strBuffer, static_cast<uint64_t>(0));
        }
    }

    FillReservedInt32("FileSize", static_cast<int>(ostream->tellp()));

    if (md5) {
        sendLog("ERROR: unexpected md5 file encountered");
        return false;
    }

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
    sendLog("FMG Repacking complete\n");
    return true;
}

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

Fmg::~Fmg() {}

} // namespace FileHelper
