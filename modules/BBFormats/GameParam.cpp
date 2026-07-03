// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <vector>

#include "GameParam.h"
#include "modules/ModMerger.h"

namespace fs = std::filesystem;

namespace FileHelper {

GameParam::GameParam(std::vector<char>& data, const std::string& name, ModMerger* parent)
    : fileName(name), BBFormat(parent) {
    bigEndian = false;
    ReadGameParam(data);
}

bool GameParam::ReadGameParam(std::vector<char>& data) {
    if (data.empty()) {
        sendLog("ERROR: empty fmg input data", LogFormat::BoldRed);
        return false;
    }

    /*
    def = GetParamDef(fileName);
    if (!def.loaded) {
        sendLog("ERROR: no paramdef found for " + fileName, LogFormat::BoldRed);
        return false;
    }
    */

    istream = std::make_unique<std::stringstream>(std::string(data.begin(), data.end()),
                                                  std::ios_base::in | std::ios_base::binary);

    std::string strBuffer = "";
    int intBuffer = 0;

    istream->seekg(0x2C);
    GetByte(bigEndian); // assert 0/0xFF?
    debugLog("bigEndian: " + std::to_string(bigEndian));

    GetByte(format2D);
    debugLog("format2D: " + std::to_string(format2D)); // might just hardcode to 4?
    std::vector<GameParam::FormatFlags1> flags1 = GetFormatFlags1(format2D);
    auto has_flag = [flags1](FormatFlags1 flag) -> bool {
        return std::find(flags1.begin(), flags1.end(), flag) != flags1.end();
    };

    bool flags1HasFlag01 = has_flag(FormatFlags1::Flag01);
    debugLog("flags1HasFlag01: " + std::to_string(flags1HasFlag01));

    bool flags1HasIntDataOffset = has_flag(FormatFlags1::IntDataOffset);
    debugLog("flags1HasIntDataOffset: " + std::to_string(flags1HasIntDataOffset));

    bool flags1HasOffsetParamType = has_flag(FormatFlags1::OffsetParamType);
    debugLog("flags1HasOffsetParamType: " + std::to_string(flags1HasOffsetParamType));

    bool flags1HasLongDataOffset = has_flag(FormatFlags1::LongDataOffset);
    debugLog("flags1HasLongDataOffset: " + std::to_string(flags1HasLongDataOffset));

    GetByte(format2E);
    bool paramUnicode = IsUnicodeNames(format2E);
    debugLog("paramUnicode: " + std::to_string(paramUnicode));

    GetByte(paramDefFormatVersion);
    debugLog("paramdefformatversion: " + std::to_string(intBuffer)); // assert 0/0xFF?

    istream->seekg(0);

    // The strings offset in the header is highly unreliable; only use it as a last resort
    uint actualStringsOffset = 0;
    uint stringsOffset;
    GetInt32(stringsOffset);
    debugLog("stringsOffset: " + std::to_string(stringsOffset));

    if (flags1HasFlag01 && flags1HasIntDataOffset || flags1HasLongDataOffset) {
        GetInt16(intBuffer);
        debugLog("zero check: " + std::to_string(intBuffer));
    } else {
        sendLog("ERROR: param flag types", LogFormat::BoldRed);
        return false;
    }

    GetInt16(unk06);
    debugLog("unk06: " + std::to_string(unk06));

    GetInt16(paramDefDataVersion);
    debugLog("def.dataVersion: " + std::to_string(paramDefDataVersion));

    int rowCount;
    GetInt16(rowCount);
    debugLog("rowCount: " + std::to_string(rowCount));

    if (flags1HasOffsetParamType) {
        sendLog("ERROR: unexpected offset param type", LogFormat::BoldRed);
        return false;
    } else {
        GetStr(paramType, 0x20);
    }

    debugLog("paramType: " + paramType);
    istream->ignore(4); // Format

    uint64_t dataStartHeader = -1;
    if (flags1HasFlag01 && flags1HasIntDataOffset) {
        sendLog("ERROR: unexpected param format encountered", LogFormat::BoldRed);
        return false;
    } else if (flags1HasLongDataOffset) {
        GetInt64(dataStartHeader);
        debugLog("dataStartHeader: " + std::to_string(dataStartHeader));
        istream->ignore(8); // or assert 0
    }

    uint64_t rowsStart = static_cast<uint64_t>(istream->tellg());
    debugLog("rowsStart: " + std::to_string(rowsStart));
    auto GetRowDataOffset = [this, flags1HasLongDataOffset](uint64_t position) -> uint64_t {
        if (flags1HasLongDataOffset) {
            uint64_t offset;
            StepIn(static_cast<std::streamoff>(istream->tellg()) + 8, *istream);
            GetInt64(offset);
            StepOut(*istream);
            return offset;
        } else {
            sendLog("ERROR: unexpected param w/ int offsets encountered", LogFormat::BoldRed);
            return false;
        }
    };

    bool unnamedRows = false;
    bool headerlessRows = false;
    uint64_t rowDataOffset1 = GetRowDataOffset(rowsStart);
    debugLog("rowDataOffset1: " + std::to_string(rowDataOffset1));

    uint64_t rowsSize = rowDataOffset1 - rowsStart;
    int rowHeaderSize = 12;

    if (rowsSize < (rowCount * rowHeaderSize)) {
        unnamedRows = true;
        rowHeaderSize = 8;
    }

    if (dataStartHeader != -1) {
        if (rowsStart == dataStartHeader || (rowsStart + rowHeaderSize) > dataStartHeader) {
            headerlessRows = true;
            unnamedRows = true;
        }
    }

    debugLog("unnamedRows: " + std::to_string(unnamedRows));       // should be 0
    debugLog("headerlessRows: " + std::to_string(headerlessRows)); // should be 0

    if (headerlessRows) {
        sendLog("ERROR: unexpected param w/ headerless rows encountered", LogFormat::BoldRed);
        return false;
    } else {
        for (int i = 0; i < rowCount; i++) {
            GameParam::Row row;
            uint64_t nameOffset;
            if (flags1HasLongDataOffset) {
                GetInt32(row.id);
                debugLog("row.id: " + std::to_string(row.id));

                istream->ignore(4); // or assert 0
                GetInt64(row.dataOffset);
                debugLog("row " + std::to_string(row.id) +
                         " dataoffset: " + std::to_string(row.dataOffset));

                if (!unnamedRows) {
                    GetInt64(nameOffset);
                } else {
                    sendLog("ERROR: param w/ nameless rows encountered", LogFormat::BoldRed);
                    return false;
                }
            } else {
                sendLog("ERROR: param type with no long offsets", LogFormat::BoldRed);
                return false;
            }

            if (!unnamedRows && nameOffset != 0 && nameOffset != data.size()) {
                if (actualStringsOffset == 0 || nameOffset < actualStringsOffset) {
                    actualStringsOffset = nameOffset;
                    debugLog("actualStringsOffset: " + std::to_string(actualStringsOffset));
                }

                StepIn(nameOffset, *istream);
                if (paramUnicode) {
                    row.name = ReadUtf16String();
                } else {
                    std::getline(*istream, row.name, '\0');
                }
                StepOut(*istream);

                debugLog("row.name: " + row.name);
            }

            rows.push_back(row);
        }

        if (data.size() > 1) {
            detectedSize = rows[1].dataOffset - rows[0].dataOffset;
        } else if (rows.size() == 1) {
            detectedSize = (actualStringsOffset == 0 ? stringsOffset : actualStringsOffset) -
                           rows[0].dataOffset;
        } else {
            detectedSize = -1;
        }

        debugLog("detectedSize: " + std::to_string(detectedSize));
    }

    // for now process row data as a whole, so we don't need to read/write the individual cells lol
    for (int i = 0; i < rowCount; i++) {
        istream->seekg(rows[i].dataOffset);
        GetBytes(rows[i].data, detectedSize);
    }

    sendLog("Game Param loaded: " + fileName);
    istream.reset();
    return true;
}

bool GameParam::RepackGameParam(std::vector<char>& outputData) {
    ostream = std::make_unique<std::stringstream>(std::ios_base::out | std::ios_base::binary);
    std::string strBuffer;

    std::vector<GameParam::FormatFlags1> flags1 = GetFormatFlags1(format2D);
    auto has_flag = [flags1](FormatFlags1 flag) -> bool {
        return std::find(flags1.begin(), flags1.end(), flag) != flags1.end();
    };

    bool flags1HasFlag01 = has_flag(FormatFlags1::Flag01);
    bool flags1HasIntDataOffset = has_flag(FormatFlags1::IntDataOffset);
    bool flags1HasOffsetParamType = has_flag(FormatFlags1::OffsetParamType);
    bool flags1HasLongDataOffset = has_flag(FormatFlags1::LongDataOffset);
    bool paramUnicode = IsUnicodeNames(format2E);

    ReserveBytes("StringsOffset", 4);

    if (flags1HasFlag01 && flags1HasIntDataOffset || flags1HasLongDataOffset) {
        WriteInt16(0);
    } else {
        sendLog("ERROR: unexpected param format encountered", LogFormat::BoldRed);
        return false;
    }

    WriteInt16(unk06);
    WriteInt16(paramDefDataVersion);
    WriteInt16(rows.size());

    if (flags1HasOffsetParamType) {
        sendLog("ERROR: unexpected offset type param encountered", LogFormat::BoldRed);
        return false;
    } else {
        /*
        if (HeaderlessRows) {
            padding = 0x20;
        } */
        WriteStr(paramType, 0x20);
    }

    WriteByte(0x00); // should be 0xFF if BigEndian, but that shouldn't be
    WriteByte(format2D);
    WriteByte(format2E);
    WriteByte(paramDefFormatVersion);

    if (flags1HasFlag01 && flags1HasIntDataOffset) {
        sendLog("ERROR: unexpected param format encountered", LogFormat::BoldRed);
        return false;
    } else if (flags1HasLongDataOffset) {
        ReserveBytes("DataStart", 8);
        WriteInt64(static_cast<uint64_t>(0));
    }

    for (int i = 0; i < rows.size(); i++) {
        if (flags1HasLongDataOffset) {
            WriteInt32(rows[i].id);
            WriteInt32(0);

            strBuffer = "RowOffset" + std::to_string(i);
            ReserveBytes(strBuffer, 8);

            strBuffer = "NameOffset" + std::to_string(i);
            ReserveBytes(strBuffer, 8);
        } else {
            sendLog("ERROR: unexpected param format encountered", LogFormat::BoldRed);
            return false;
        }
    }

    if (format2D == 1) {
        sendLog("ERROR: unexpected param format encountered", LogFormat::BoldRed);
        return false;
    }

    if (flags1HasFlag01 && flags1HasIntDataOffset) {
        sendLog("ERROR: unexpected param format encountered", LogFormat::BoldRed);
        return false;
    } else if (flags1HasLongDataOffset) {
        FillReservedInt64("DataStart", static_cast<uint64_t>(ostream->tellp()));
    } else {
        sendLog("ERROR: unexpected param format encountered", LogFormat::BoldRed);
        return false;
    }

    for (int i = 0; i < rows.size(); i++) {
        if (flags1HasLongDataOffset) {
            strBuffer = "RowOffset" + std::to_string(i);
            FillReservedInt64(strBuffer, static_cast<uint64_t>(ostream->tellp()));
            ostream->write(rows[i].data.data(), detectedSize);
        } else {
            sendLog("ERROR: unexpected param format encountered", LogFormat::BoldRed);
            return false;
        }
    }

    FillReservedInt32("StringsOffset", static_cast<uint>(ostream->tellp()));

    if (flags1HasOffsetParamType) {
        sendLog("ERROR: unexpected param format encountered", LogFormat::BoldRed);
        return false;
    }

    WriteInt16(0);

    for (int i = 0; i < rows.size(); i++) {
        strBuffer = "NameOffset" + std::to_string(i);
        FillReservedInt64(strBuffer, static_cast<uint64_t>(ostream->tellp()));

        if (paramUnicode) {
            WriteUtf16String(rows[i].name);
        } else {
            ostream->write(rows[i].name.data(), rows[i].name.size());
            char null = '\0';
            ostream->write(&null, 1);
        }
    }

    // BB sometimes (but not always) include some useless padding here
    WriteInt16(0);

    auto& stringStream = static_cast<std::stringstream&>(*ostream);
    std::string str = stringStream.str();
    outputData = std::vector<char>(str.begin(), str.end());

    /* tests
    std::ofstream outFile("test.param", std::ios::out | std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(outputData.data(), outputData.size());
        outFile.close();
    }
    */

    ostream.reset();
    sendLog("Param Repacking complete: " + fileName + "\n");
    return true;
}

bool GameParam::HandleConflict(std::vector<char>& mod1Data, std::vector<char>& mod2Data) {
    GameParam mod1Prm = GameParam(mod1Data, fileName, merger);
    const std::vector<Row> mod1Rows = mod1Prm.rows;

    GameParam mod2Prm = GameParam(mod2Data, fileName, merger);
    const std::vector<Row> mod2Rows = mod2Prm.rows;

    for (int i = 0; i < rows.size(); i++) {
        bool mod1Modified = false;
        bool mod2Modified = false;

        std::optional<Row> mod1entry;
        std::optional<Row> mod2entry;

        mod1entry = GetSameRow(rows[i].id, mod1Rows);
        mod2entry = GetSameRow(rows[i].id, mod2Rows);

        mod1Modified = mod1entry && (mod1entry->data != rows[i].data);
        mod2Modified = mod2entry && (mod2entry->data != rows[i].data);

        if (mod1Modified && mod2Modified) {
            if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                QMetaObject::invokeMethod(merger, &ModMerger::OpenPriorityDialog,
                                          Qt::BlockingQueuedConnection);
                if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                    return false;
                }
            }

            if (merger->GetModPriority() == ModMerger::ModPriority::Mod1) {
                rows[i].data = mod1entry->data;
                sendLog("Unresolvable conflict in row: " + std::to_string(mod1entry->id) +
                            ", using row data from prioritized mod: " + merger->Mod1Name(),
                        LogFormat::Yellow);
            } else if (merger->GetModPriority() == ModMerger::ModPriority::Mod2) {
                rows[i].data = mod2entry->data;
                sendLog("Unresolvable conflict in row: " + std::to_string(mod2entry->id) +
                            ", using row data from prioritized mod: " + merger->Mod2Name(),
                        LogFormat::Yellow);
            }
        }

        if (mod1Modified && !mod2Modified) {
            rows[i].data = mod1entry->data;
            sendLog("Merging row: " + std::to_string(mod1entry->id) +
                    " from mod: " + merger->Mod1Name());
        } else if (mod2Modified && !mod1Modified) {
            rows[i].data = mod2entry->data;
            sendLog("Merging row: " + std::to_string(mod2entry->id) +
                    " from mod: " + merger->Mod2Name());
        }
    }

    bool rowAdded = false;
    std::unordered_set<int> existingIds;
    for (const auto& row : rows) {
        existingIds.insert(row.id);
    }

    for (const auto& row : mod1Rows) {
        if (existingIds.find(row.id) == existingIds.end()) {
            rowAdded = true;
            rows.push_back(row);
            existingIds.insert(row.id);
            sendLog("New row added id: " + std::to_string(row.id) +
                    " from mod: " + merger->Mod1Name());
        }
    }

    for (const auto& row : mod2Rows) {
        if (existingIds.find(row.id) == existingIds.end()) {
            rowAdded = true;
            rows.push_back(row);
            sendLog("New row added id: " + std::to_string(row.id) +
                    " from mod: " + merger->Mod2Name());
        }
    }

    if (rowAdded) {
        std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) { return a.id < b.id; });
    }

    return true;
}

std::optional<GameParam::Row> GameParam::GetSameRow(const int& id,
                                                    const std::vector<GameParam::Row>& otherRows) {
    std::optional<Row> result = std::nullopt;
    for (const auto& otherRow : otherRows) {
        if (otherRow.id == id) {
            return result.emplace(otherRow);
        }
    }

    return result;
}

std::vector<GameParam::FormatFlags1> GameParam::GetFormatFlags1(int flags1Value) {
    const FormatFlags1 allFlags[] = {FormatFlags1::None,           FormatFlags1::IntDataOffset,
                                     FormatFlags1::LongDataOffset, FormatFlags1::Flag08,
                                     FormatFlags1::Flag10,         FormatFlags1::Flag20,
                                     FormatFlags1::Flag40,         FormatFlags1::OffsetParamType};
    std::vector<FormatFlags1> formatFlags1;

    for (FormatFlags1 flag : allFlags) {
        if ((flags1Value & static_cast<int>(flag)) != 0) {
            formatFlags1.push_back(flag);
        }
    }
    return formatFlags1;
}

bool GameParam::IsUnicodeNames(int flags2Value) {
    const FormatFlags2 allFlags[] = {
        FormatFlags2::None,   FormatFlags2::UnicodeRowNames, FormatFlags2::Flag02,
        FormatFlags2::Flag04, FormatFlags2::Flag08,          FormatFlags2::Flag10,
        FormatFlags2::Flag20, FormatFlags2::Flag40,          FormatFlags2::Flag80};

    if ((flags2Value & static_cast<int>(FormatFlags2::UnicodeRowNames)) != 0) {
        return true;
    }

    return false;
}

GameParam::~GameParam() {}

} // namespace FileHelper
