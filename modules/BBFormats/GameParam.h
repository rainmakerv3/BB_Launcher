// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "BBFormats.h"

#include "BBFormats.h"

namespace FileHelper {

class GameParam : public BBFormat {
    Q_OBJECT

    /*
    using cellValue =
        std::variant<int, float, double, std::string, std::vector<std::string>, std::vector<int>>;

    enum class DefType : int {
        s8,      // Signed 1-byte integer.
        u8,      // Unsigned 1-byte integer.
        s16,     // Signed 2-byte integer.
        u16,     // Unsigned 2-byte integer.
        s32,     // Signed 4-byte integer.
        u32,     // Unsigned 4-byte integer
        b32,     //  4-byte integer representing a boolean (why? lol)
        f32,     // Single-precision floating point value
        angle32, // Single-precision floating point value representing an angle
        f64,     // Double-precision floating point value
        dummy8,  // Byte or array of bytes used for padding or placeholding
        fixstr,  // Fixed-width Shift-JIS string.
        fixstrW, // Fixed-width UTF-16 string.
    };


     struct Field {
         // DefType defType;
         std::string fieldName;
         int arrayLength = 1;
         int bitSize = -1;
         int sortId;
         // std::vector<cellValue> defaultValue;
         // defaultThreshold?
     };

      struct ParamDef {
          bool bigEndian = false;
          bool uniCode = 1;
          int formatVersion = 201;
          bool loaded = true;
          std::vector<Field> paramFields;
      };

       */

    enum class FormatFlags1 : int {
        None = 0,                     // No flags set.
        Flag01 = 0b00000001,          // Unknown.
        IntDataOffset = 0b00000010,   // Expanded header with 32-bit data offset.
        LongDataOffset = 0b00000100,  // Expanded header with 64-bit data offset.
        Flag08 = 0b00001000,          // Unused?
        Flag10 = 0b00010000,          // Unused?
        Flag20 = 0b00100000,          // Unused?
        Flag40 = 0b01000000,          // Unused?
        OffsetParamType = 0b10000000, // Type is written separately instead of fixed-width
    };

    enum class FormatFlags2 : int {
        None = 0,                     // No flags set.
        UnicodeRowNames = 0b00000001, // Row names are written as UTF-16.
        Flag02 = 0b00000010,          // Unknown.
        Flag04 = 0b00000100,          // Unknown.
        Flag08 = 0b00001000,          // Unused?
        Flag10 = 0b00010000,          // Unused?
        Flag20 = 0b00100000,          // Unused?
        Flag40 = 0b01000000,          // Unused?
        Flag80 = 0b10000000,          // Unused?
    };

    struct Row {
        int id;
        std::string name;
        uint64_t dataOffset;
        std::vector<char> data;
        // std::vector<cellValue> values;
    };

public:
    explicit GameParam(std::vector<char> data, const std::string& fileName, ModMerger* parent);
    ~GameParam() override;

    // ParamDef GetParamDef(const std::string& filename); // GameParamDef.cpp, big hardcodes
    bool ReadGameParam(std::vector<char> data);
    bool RepackGameParam(std::vector<char>& outputData);
    bool HandleConflict(const std::vector<char>& mod1Data, const std::vector<char>& mod2Data);

private:
    std::vector<GameParam::FormatFlags1> GetFormatFlags1(int flagsValue);
    bool IsUnicodeNames(int flags2Value);
    std::optional<Row> GetSameRow(const int& id, const std::vector<Row>& otherRows);

    // ParamDef def;
    int format2D = 4; // hardcode?
    int format2E;
    int paramDefFormatVersion = 0;
    int paramDefDataVersion = 1;
    int unk06;
    int detectedSize;
    std::string fileName;
    std::string paramType;
    std::vector<Row> rows;
};

} // namespace FileHelper
