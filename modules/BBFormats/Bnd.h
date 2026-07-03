// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>

#include "BBFormats.h"

namespace FileHelper {

class Bnd : public BBFormat {
    Q_OBJECT

public:
    explicit Bnd(std::vector<char>& data, ModMerger* parent);
    ~Bnd() override;

    bool UnpackBnd(std::vector<char>& data);
    bool RepackBnd(std::vector<char>& outputData);

    struct BinderFile {
        /// Flags indicating compression, and possibly other things.
        int flagsValue;
        int id;
        std::string name;
        uint64_t compressedSize;
        uint64_t uncompressedSize;
        uint64_t dataOffsetLong;
        uint dataOffset;
        std::vector<char> data;
    };

    // original class info
    std::vector<BinderFile> files;
    std::optional<BinderFile> GetSameFile(const int& id,
                                          const std::vector<BinderFile> otherBinderFiles);

private:
    // everything marked Flag** is unknown
    enum class Format {
        None = 0,
        BigEndian = 0b00000001,   // File is big-endian regardless of the big-endian byte.
        IDs = 0b00000010,         // Files have ID numbers.
        Names1 = 0b00000100,      /// Files have name strings
        Names2 = 0b00001000,      // Files have name strings; Names1 may or may not be set
        LongOffsets = 0b00010000, // File data offsets are 64-bit.
        Compression = 0b00100000, // Files may be compressed.
        Flag6 = 0b01000000,
        Flag7 = 0b10000000,
    };

    enum class FileFlags {
        None = 0,
        Compressed = 0b00000001,
        Flag1 = 0b00000010,
        Flag2 = 0b00000100,
        Flag3 = 0b00001000,
        Flag4 = 0b00010000,
        Flag5 = 0b00100000,
        Flag6 = 0b01000000,
        Flag7 = 0b10000000,
    };

    std::string FindCommonBndRootPath(const std::vector<BinderFile>& files);
    std::string DateToBinderTimestamp();
    uint8_t ReverseBits(uint8_t value);
    int ReadFormat(bool bitBigEndian);
    void WriteFormat(int value);
    int ReadFileFlags(bool bitBigEndian);
    void WriteFileFlags(int value);
    std::vector<Bnd::Format> GetFormatFlags(int value);
    std::vector<Bnd::FileFlags> GetFileFlags(int value);
    uint GetBND4FileHeaderSize(int formatInt);

    // original class info
    std::string version;
    int format;
    bool unk04;
    bool unk05;
    bool bitBigEndian;
    bool unicode;
    int extended;
    std::filesystem::path rootPath;

    // other private members
    std::filesystem::path origPath;
    std::filesystem::path extractedFolder;
    bool hasBinderCompression;
    bool hasBinderLongOffsets;
    bool hasBinderIDs;
    bool hasBinderNames;
    bool binderFormatEqualsNames1;
};

} // namespace FileHelper
