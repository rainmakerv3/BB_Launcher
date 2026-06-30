// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <vector>

#include "DrawParam.h"

namespace fs = std::filesystem;

namespace FileHelper {

DrawParam::DrawParam(std::vector<char> data, ModMerger* parent) : BBFormat(parent) {
    bigEndian = false;
    ReadDrawParam(data);
}

bool DrawParam::ReadDrawParam(std::vector<char> data) {
    if (data.empty()) {
        sendLog("ERROR: empty drawparam input data", LogFormat::BoldRed);
        return false;
    }

    istream = std::make_unique<std::stringstream>(std::string(data.begin(), data.end()),
                                                  std::ios_base::in | std::ios_base::binary);

    int intBuffer = 0;
    std::string strBuffer;

    GetStr(strBuffer, 8);
    debugLog("MAGIC: " + strBuffer);

    if (!strBuffer.contains("filt")) {
        sendLog("ERROR invalid param header: " + strBuffer, LogFormat::BoldRed);
        return false;
    }

    GetInt32(intBuffer);
    debugLog("Game: " + std::to_string(intBuffer));

    if (intBuffer != 3) {
        sendLog("ERROR invalid game ID: " + std::to_string(intBuffer), LogFormat::BoldRed);
        return false;
    }

    GetByte(intBuffer);
    debugLog("Zero check: " + std::to_string(intBuffer));

    // handle if not 0?

    GetByte(unk0D);
    debugLog("Unk0D: " + std::to_string(unk0D));

    GetInt16(intBuffer);
    debugLog("Zero check: " + std::to_string(intBuffer));

    // handle if not 0?

    int groupCount = 0;
    GetInt32(groupCount);
    debugLog("group count: " + std::to_string(groupCount));

    GetInt32(unk14);
    debugLog("unk14: " + std::to_string(unk14));

    GetInt32(intBuffer);
    debugLog("header check: " + std::to_string(intBuffer));
    // or assert targetValue == 0x40, 0x50, 0x54 (BB always 0x50?)

    Offsets offsets;
    GetInt32(offsets.groupHeaders);
    GetInt32(offsets.paramHeaderOffsets);
    GetInt32(offsets.paramHeaders);
    GetInt32(offsets.values);
    GetInt32(offsets.valueIDs);
    GetInt32(offsets.unk2);

    debugLog("offsets groupheader: " + std::to_string(offsets.groupHeaders));
    debugLog("offsets ParamHeaderOffsets: " + std::to_string(offsets.paramHeaderOffsets));
    debugLog("offsets param headers: " + std::to_string(offsets.paramHeaders));
    debugLog("offsets values: " + std::to_string(offsets.values));
    debugLog("offsets ValueIDs: " + std::to_string(offsets.valueIDs));
    debugLog("offsets Unk2: " + std::to_string(offsets.unk2));

    int unk3Count = 0;
    GetInt32(unk3Count);
    debugLog("unk3Count: " + std::to_string(unk3Count));

    GetInt32(offsets.unk3);
    debugLog("offsets unk3: " + std::to_string(offsets.unk3));

    GetInt32(offsets.unk3ValueIDs);
    debugLog("offsets unk3 value ids: " + std::to_string(offsets.unk3ValueIDs));

    GetFloat(unk40);
    debugLog("unk40: " + std::format("{:.3f}", unk40));

    GetInt32(offsets.commentOffsetsOffsets);
    debugLog("offsets.CommentOffsetsOffsets: " + std::to_string(offsets.commentOffsetsOffsets));

    GetInt32(offsets.commentOffsets);
    debugLog("offsets.CommentOffsets: " + std::to_string(offsets.commentOffsets));

    GetInt32(offsets.comments);
    debugLog("offsets.Comments: " + std::to_string(offsets.comments));

    debugLog("");

    for (int i = 0; i < groupCount; ++i) {
        ParamGroup group;
        int groupHeaderOffset = 0;
        GetInt32(groupHeaderOffset);
        debugLog("Group header offset: " + std::to_string(groupHeaderOffset));

        StepIn(offsets.groupHeaders + groupHeaderOffset, *istream);
        {
            int paramCount = 0;
            GetInt32(paramCount);
            debugLog(std::format("Group: {} Param count: {}", i, paramCount));

            int paramHeaderOffsetsOffset = 0;
            GetInt32(paramHeaderOffsetsOffset);
            debugLog(
                std::format("Group: {} Param headeroffsetoffset: {}", i, paramHeaderOffsetsOffset));

            group.name1 = ReadUtf16String();
            group.name2 = ReadUtf16String();

            debugLog("groupName1: " + group.name1);
            debugLog("groupName2: " + group.name2);

            StepIn(offsets.paramHeaderOffsets + paramHeaderOffsetsOffset, *istream);
            {
                for (int i = 0; i < paramCount; ++i) {
                    Param param;
                    int paramHeaderOffset = 0;
                    GetInt32(paramHeaderOffset);
                    debugLog("param header offset: " + std::to_string(paramHeaderOffset));

                    StepIn(offsets.paramHeaders + paramHeaderOffset, *istream);
                    {
                        int valuesOffset = 0;
                        GetInt32(valuesOffset);
                        debugLog("valuesoffset: " + std::to_string(valuesOffset));

                        int valueIdsOffset = 0;
                        GetInt32(valueIdsOffset);
                        debugLog("valueIdsOffset: " + std::to_string(valueIdsOffset));

                        int intType = 0;
                        GetByte(intType);
                        param.type = static_cast<ParamType>(intType);
                        debugLog("type: " + std::to_string(intType));

                        int valueCount = 0;
                        GetByte(valueCount);
                        debugLog("valueCount: " + std::to_string(valueCount));

                        istream->ignore(2); // or check zero

                        param.name1 = ReadUtf16String();
                        param.name2 = ReadUtf16String();

                        debugLog("paramName1: " + param.name1);
                        debugLog("paramName2: " + param.name2);

                        StepIn(offsets.values + valuesOffset, *istream);
                        {
                            {
                                for (int i = 0; i < valueCount; ++i) {
                                    ParamValue value;
                                    std::vector<char> byteVec;
                                    std::vector<float> floatVec;
                                    bool b;
                                    int intVal;
                                    float f;

                                    std::string s = "";

                                    switch (param.type) {
                                    case Byte:
                                        GetBytes(byteVec, 1);
                                        value.value = byteVec;

                                        s = "byte";
                                        break;
                                    case Short:
                                        GetInt16(intVal);
                                        value.value = intVal;

                                        s = std::to_string(std::get<int>(value.value));
                                        break;
                                    case IntA:
                                        GetInt32(intVal);
                                        value.value = intVal;

                                        s = std::to_string(std::get<int>(value.value));
                                        break;
                                    case BoolA:
                                        GetByte(b);
                                        value.value = b;

                                        s = std::to_string(std::get<bool>(value.value));
                                        break;
                                    case IntB:
                                        GetInt32(intVal);
                                        value.value = intVal;

                                        s = std::to_string(std::get<int>(value.value));
                                        break;
                                    case Float:
                                        GetFloat(f);
                                        value.value = f;

                                        s = std::format("{:.3f}", std::get<float>(value.value));
                                        break;
                                    case BoolB:
                                        GetByte(b);
                                        value.value = b;

                                        s = std::to_string(std::get<bool>(value.value));
                                        break;
                                    case Float2:
                                        GetFloat(f);
                                        floatVec.push_back(f);
                                        GetFloat(f);
                                        floatVec.push_back(f);

                                        value.value = floatVec;
                                        istream->ignore(8); // or assert 0

                                        for (const auto& ent : floatVec) {
                                            s = s + std::format("{:.3f}", ent) + " ";
                                        }
                                        break;
                                    case Float3:
                                        GetFloat(f);
                                        floatVec.push_back(f);
                                        GetFloat(f);
                                        floatVec.push_back(f);
                                        GetFloat(f);
                                        floatVec.push_back(f);

                                        value.value = floatVec;
                                        istream->ignore(4); // or assert 0

                                        for (const auto& ent : floatVec) {
                                            s = s + std::format("{:.3f}", ent) + " ";
                                        }
                                        break;
                                    case Float4:
                                        GetFloat(f);
                                        floatVec.push_back(f);
                                        GetFloat(f);
                                        floatVec.push_back(f);
                                        GetFloat(f);
                                        floatVec.push_back(f);
                                        GetFloat(f);
                                        floatVec.push_back(f);

                                        value.value = floatVec;

                                        for (const auto& ent : floatVec) {
                                            s = s + std::format("{:.3f}", ent) + " ";
                                        }
                                        break;
                                    case Byte4:
                                        GetBytes(byteVec, 4);
                                        value.value = byteVec;

                                        s = "byteVector";
                                        break;
                                    }

                                    param.values.push_back(value);
                                    debugLog("Added value: " + s);
                                }
                            }
                        }
                        debugLog("");
                        StepOut(*istream);

                        StepIn(offsets.valueIDs + valueIdsOffset, *istream);
                        {
                            for (int i = 0; i < valueCount; ++i) {
                                GetInt32(param.values[i].id);
                                debugLog("Value ID: " + std::to_string(param.values[i].id));
                            }
                        }
                        StepOut(*istream);
                    }
                    StepOut(*istream);

                    group.params.push_back(param);
                    debugLog("");
                }
            }
            StepOut(*istream);
        }
        StepOut(*istream);
        groups.push_back(group);
        debugLog("");
    }

    istream->seekg(offsets.unk2);
    unkBlock2.resize(offsets.unk3 - offsets.unk2);
    istream->read(unkBlock2.data(), offsets.unk3 - offsets.unk2);

    debugLog("unk2 block saved size: " + std::to_string(unkBlock2.size()));

    istream->seekg(offsets.unk3);
    for (int i = 0; i < unk3Count; ++i) {
        Unk3 u;
        int count;
        int valueIdsOffset;

        GetInt32(u.groupIndex);
        GetInt32(count);
        GetInt32(valueIdsOffset);

        debugLog("unk3 group index: " + std::to_string(u.groupIndex));
        debugLog("unk3 group count: " + std::to_string(count));
        debugLog("unk3 group valueidoffset: " + std::to_string(valueIdsOffset));

        StepIn(offsets.unk3ValueIDs + valueIdsOffset, *istream);
        for (int j = 0; j < count; ++j) {
            int id = 0;
            GetInt32(id);
            u.valueIDs.push_back(id);

            debugLog("unk3 value id: " + std::to_string(id));
        }
        StepOut(*istream);

        unk3s.push_back(u);
    }

    istream->seekg(offsets.commentOffsetsOffsets);
    std::vector<int> commentOffsetsOffsets;
    for (int i = 0; i < groupCount; ++i) {
        int off = 0;
        GetInt32(off);
        commentOffsetsOffsets.push_back(off);

        debugLog("group: " + std::to_string(i) + " comment offset: " + std::to_string(off));
    }

    int commentOffsetsLength = offsets.comments - offsets.commentOffsets;
    debugLog("comment offset length: " + std::to_string(commentOffsetsLength));

    for (int i = 0; i < groupCount; ++i) {
        int commentCount = i != groupCount - 1
                               ? (commentOffsetsOffsets[i + 1] - commentOffsetsOffsets[i]) / 4
                               : (commentOffsetsLength - commentOffsetsOffsets[i]) / 4;

        debugLog("group: " + std::to_string(i) + " comment count: " + std::to_string(commentCount));
        istream->seekg(offsets.commentOffsets + commentOffsetsOffsets[i]);

        for (int j = 0; j < commentCount; ++j) {
            int32_t commentOffset = 0;
            GetInt32(commentOffset);

            StepIn(offsets.comments + commentOffset, *istream);
            std::string comment = ReadUtf16String();
            StepOut(*istream);

            groups[i].comments.push_back(comment);
            debugLog("group: " + std::to_string(i) + " comment: " + comment);
        }
    }

    istream.reset();
    sendLog("Drawparam loaded");
    return true;
}

DrawParam::~DrawParam() {}

} // namespace FileHelper
