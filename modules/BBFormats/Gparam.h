// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "BBFormats.h"

namespace FileHelper {

class GParam : public BBFormat {
    Q_OBJECT

private:
    enum ParamType : int {
        Byte = 1,
        Short = 2,
        IntA = 3,
        BoolA = 5,
        IntB = 7,
        Float = 9,
        BoolB = 11, // 0x0B
        /// Two floats and 8 unused bytes.
        Float2 = 12, // 0x0C
        /// Three floats and 4 unused bytes.
        Float3 = 13, // 0x0D
        /// Four floats.
        Float4 = 14, // 0x0E
        /// Four bytes, used for BGRA.
        Byte4 = 15, // 0x0F
    };

    struct Offsets {
        int32_t groupHeaders;
        int32_t paramHeaderOffsets;
        int32_t paramHeaders;
        int32_t values;
        int32_t valueIDs;
        int32_t unk2;
        int32_t unk3;
        int32_t unk3ValueIDs;
        int32_t commentOffsetsOffsets;
        int32_t commentOffsets;
        int32_t comments;
    };

    struct ParamValue {
        int id;
        std::variant<bool, int, std::vector<char>, float, std::vector<float>> value;
    };

    struct Param {
        std::string name1;
        std::string name2;
        ParamType type;
        std::vector<ParamValue> values;
    };

    struct ParamGroup {
        std::string name1;
        std::string name2;
        std::vector<std::string> comments;
        std::vector<Param> params;
    };

    struct Unk3 {
        int groupIndex;
        std::vector<int> valueIDs;
    };

    bool unk0D;
    int unk14;
    float unk40;
    float unk50;
    std::vector<ParamGroup> groups;
    std::vector<char> unkBlock2;
    std::vector<Unk3> unk3s;

public:
    explicit GParam(std::vector<char> data, ModMerger* parent);
    ~GParam() override;

    bool ReadGParam(std::vector<char> data);
};

} // namespace FileHelper
