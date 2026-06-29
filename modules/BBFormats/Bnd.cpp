// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Bnd.h"

namespace fs = std::filesystem;

namespace FileHelper {

Bnd::Bnd(std::vector<char> data, ModMerger* parent) : BBFormat(parent) {
    bigEndian = false;
    version = DateToBinderTimestamp();
    UnpackBnd(data);
}

bool Bnd::UnpackBnd(std::vector<char> data) {
    if (data.empty()) {
        sendLog("ERROR: empty bnd input data", LogFormat::BoldRed);
        return false;
    }

    istream = std::make_unique<std::stringstream>(std::string(data.begin(), data.end()),
                                                  std::ios_base::in | std::ios_base::binary);

    std::string strBuffer;
    int intBuffer = 0;
    int64_t int64Buffer = 0;

    if (!istream) {
        sendLog("ERROR: Failed to open BND data buffer", LogFormat::BoldRed);
        return false;
    }

    GetStr(strBuffer, 4);
    debugLog("MAGIC: " + strBuffer);

    if (!strBuffer.contains("BND4")) {
        sendLog("ERROR Invalid header magic: " + strBuffer, LogFormat::BoldRed);
        return false;
    }

    GetByte(unk04);
    debugLog("Unk04: " + std::to_string(unk04));

    GetByte(unk05);
    debugLog("Unk05: " + std::to_string(unk05));

    istream->ignore(3); // or assert 0;
    GetByte(bigEndian);
    debugLog("BigEndian: " + std::to_string(bigEndian));

    GetByte(bitBigEndian);
    bitBigEndian = !bitBigEndian;
    debugLog("BitBigEndian: " + std::to_string(bitBigEndian));

    istream->ignore(1); // or assert 0;

    int fileCount = 0;
    GetInt32(fileCount);
    debugLog("File count: " + std::to_string(fileCount));

    GetInt64(int64Buffer);
    debugLog("header size check: " + std::to_string(intBuffer)); // assert 64?

    GetStr(version, 8);
    debugLog("version: " + version);

    uint64_t fileHeaderSize;
    GetInt64(fileHeaderSize);
    debugLog("fileHeaderSize: " + std::to_string(fileHeaderSize));

    istream->ignore(8); // Headers end (includes hash table)

    GetByte(unicode);
    debugLog("Unicode: " + std::to_string(unicode));

    format = ReadFormat(bitBigEndian);
    debugLog("Format: " + std::to_string(format));

    std::vector<Format> formatVec = GetFormatFlags(format);
    auto has_flag = [formatVec](Format flag) -> bool {
        return std::find(formatVec.begin(), formatVec.end(), flag) != formatVec.end();
    };

    hasBinderCompression = has_flag(Format::Compression);
    hasBinderLongOffsets = has_flag(Format::LongOffsets);
    hasBinderIDs = has_flag(Format::IDs);
    hasBinderNames = has_flag(Format::Names1) || has_flag(Format::Names2);
    binderFormatEqualsNames1 = format == static_cast<int>(Format::Names1);

    debugLog("hasBinderCompression: " + std::to_string(hasBinderCompression));
    debugLog("hasBinderLongOffsets: " + std::to_string(hasBinderLongOffsets));
    debugLog("hasBinderIDs: " + std::to_string(hasBinderIDs));
    debugLog("hasBinderNames: " + std::to_string(hasBinderNames));
    debugLog("binderFormatEqualsNames1: " + std::to_string(binderFormatEqualsNames1));

    GetByte(extended);
    debugLog("Extended: " + std::to_string(extended)); // or assert (0, 1, 4, 0x80);

    istream->ignore(5); // or assert 0

    if (extended == 4) {
        // hash table value originally asserted, but I think I can just skip this
    } else {
        istream->ignore(8); // or assert 0
    }

    uint expectedHeaderSize = GetBND4FileHeaderSize(format);
    debugLog("expectedHeaderSize: " + std::to_string(expectedHeaderSize));

    if (fileHeaderSize != expectedHeaderSize) {
        sendLog("ERROR Header size does not match expected value", LogFormat::BoldRed);
        return false;
    }

    for (int i = 0; i < fileCount; i++) {
        BinderFile file;
        file.flagsValue = ReadFileFlags(bitBigEndian);
        debugLog("File Flags: " + std::to_string(file.flagsValue));

        istream->ignore(3); // or assert 0
        istream->ignore(4); // or assert -1

        GetInt64(file.compressedSize);
        debugLog("compressedSize: " + std::to_string(file.compressedSize));

        if (hasBinderCompression) {
            GetInt64(file.uncompressedSize);
            debugLog("uncompressedSize: " + std::to_string(file.uncompressedSize));
        }

        if (hasBinderLongOffsets) {
            GetInt64(file.dataOffsetLong);
            debugLog("dataOffset (64bit): " + std::to_string(file.dataOffsetLong));
        } else {
            GetInt32(file.dataOffset);
            debugLog("dataOffset (32bit): " + std::to_string(file.dataOffset));
        }

        file.id = -1;
        if (hasBinderIDs) {
            GetInt32(file.id);
            debugLog("ID: " + std::to_string(file.id));
        }

        if (hasBinderNames) {
            uint nameOffset;
            GetInt32(nameOffset);
            StepIn(nameOffset, *istream);

            if (unicode) {
                file.name = ReadUtf16String();
            } else {
                std::getline(*istream, file.name, '\0');
            }

            StepOut(*istream);

            debugLog("Name: " + file.name);
        }

        if (binderFormatEqualsNames1) {
            GetInt32(file.id);
            istream->ignore(4); // or assert 0
        }

        files.push_back(file);
    }

    rootPath = FindCommonBndRootPath(files);
    debugLog("base path found: " + rootPath.string());

    for (auto& file : files) {
        std::vector<FileFlags> flags = GetFileFlags(file.flagsValue);
        bool fileCompressed =
            std::find(flags.begin(), flags.end(), FileFlags::Compressed) != flags.end();

        hasBinderLongOffsets ? StepIn(file.dataOffsetLong, *istream)
                             : StepIn(file.dataOffset, *istream);
        if (fileCompressed) {
            sendLog("ERROR unexpected compressed bnd file encountered", LogFormat::BoldRed);
            return false;
        } else {
            file.data.resize(file.compressedSize);
            istream->read(file.data.data(), file.compressedSize);
        }
        StepOut(*istream);

        /* tests only
        std::string relativePathString = fs::relative(file.name, rootPath).string();
        fs::path destPath = Common::GetCurrentPath() / "test";
        destPath /= std::u8string_view(reinterpret_cast<const char8_t*>(relativePathString.data()),
                                       relativePathString.size());

        if (!fs::exists(destPath.parent_path()))
            fs::create_directories(destPath.parent_path());

        std::ofstream outFile(destPath, std::ios::out | std::ios::binary);
        if (outFile.is_open()) {
            outFile.write(file.data.data(), file.data.size());
            outFile.close();
            debugLog("File written: " + relativePathString);
        }
        */
    }

    istream.reset();
    sendLog("Bnd extraction completed");
    return true;
}

bool Bnd::RepackBnd(std::vector<char>& outputData) {
    ostream = std::make_unique<std::stringstream>(std::ios_base::out | std::ios_base::binary);
    std::string strBuffer;

    strBuffer = "BND4";
    WriteStr(strBuffer, 4);

    WriteByte(unk04);
    WriteByte(unk05);
    WriteByte(0);
    WriteByte(0);

    WriteByte(0);
    WriteByte(bigEndian);
    WriteByte(!bitBigEndian);
    WriteByte(0);

    WriteInt32(static_cast<int>(files.size()));
    WriteInt64(static_cast<int64_t>(0x40));
    WriteStr(version, 8);
    WriteInt64(static_cast<uint64_t>(GetBND4FileHeaderSize(format)));
    ReserveBytes("HeadersEnd", 8); // headers end

    WriteByte(unicode);
    WriteFormat(format);
    WriteByte(extended);
    WriteByte(static_cast<int>(0));

    WriteInt32(static_cast<int>(0));
    ReserveBytes("HashTableOffset", 8); // hash table offset

    for (int i = 0; i < files.size(); i++) {
        BinderFile file = files[i];

        WriteFileFlags(file.flagsValue);
        WriteByte(static_cast<int>(0));
        WriteByte(static_cast<int>(0));
        WriteByte(static_cast<int>(0));

        WriteInt32(static_cast<int>(-1));

        std::string resComp = "FileCompressedSize" + std::to_string(i);
        ReserveBytes(resComp, 8);

        if (hasBinderCompression) {
            std::string resUncomp = "FileUncompressedSize" + std::to_string(i);
            ReserveBytes(resUncomp, 8);
        }

        std::string resOff = "FileDataOffset" + std::to_string(i);
        if (hasBinderLongOffsets) {
            ReserveBytes(resOff, 8);
        } else {
            ReserveBytes(resOff, 4);
        }

        if (hasBinderIDs) {
            WriteInt32(file.id);
        }

        if (hasBinderNames) {
            std::string resNames = "FileNameOffset" + std::to_string(i);
            ReserveBytes(resNames, 4);
        }

        if (binderFormatEqualsNames1) {
            WriteInt32(file.id);
            WriteInt32(static_cast<int>(0));
        }
    }

    for (int i = 0; i < files.size(); i++) {
        BinderFile file = files[i];

        if (hasBinderNames) {
            int pos = static_cast<int>(ostream->tellp());
            std::string resNames = "FileNameOffset" + std::to_string(i);
            FillReservedInt32(resNames, pos);
            if (unicode) {
                WriteUtf16String(file.name);
            } else {
                WriteStr(file.name, file.name.size());
            }
        }
    }

    if (extended == 4) { // shouldn't be needed
        // bw.Pad(0x8);
        // bw.FillInt64("HashTableOffset", bw.Position);
        // BinderHashTable.Write(bw, fileHeaders);
        sendLog("ERROR Unhandled Extended4 binder type encountered", LogFormat::BoldRed);
        return false;
    } else {
        FillReservedInt64("HashTableOffset", static_cast<uint64_t>(0));
    }

    int endPos = static_cast<uint64_t>(ostream->tellp());
    FillReservedInt64("HeadersEnd", endPos);

    for (int i = 0; i < files.size(); i++) {
        BinderFile file = files[i];
        if (!file.data.empty()) {
            PadStream(0x10);
        }

        std::streamoff offset = ostream->tellp();
        ostream->write(file.data.data(), file.data.size());

        std::string resComp = "FileUncompressedSize" + std::to_string(i);
        FillReservedInt64(resComp, file.uncompressedSize);

        if (hasBinderCompression) {
            std::string resUncomp = "FileCompressedSize" + std::to_string(i);
            FillReservedInt64(resUncomp, file.compressedSize);
        }

        std::string resOff = "FileDataOffset" + std::to_string(i);
        if (hasBinderLongOffsets) {
            FillReservedInt64(resOff, static_cast<uint64_t>(offset));
        } else {
            FillReservedInt32(resOff, static_cast<int>(offset));
        }
    }

    auto& stringStream = static_cast<std::stringstream&>(*ostream);
    std::string str = stringStream.str();
    outputData = std::vector<char>(str.begin(), str.end());

    /* tests only
    std::ofstream outFile("test.bnd", std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(outputData.data(), outputData.size());
        outFile.close();
    }
    */

    ostream.reset();
    sendLog("BND4 Repacking complete\n");
    return true;
}

std::string Bnd::DateToBinderTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);

    std::tm tm_now;
#if defined(_MSC_VER)
    localtime_s(&tm_now, &time_now);
#else
    localtime_r(&time_now, &tm_now);
#endif

    // tm_year is years since 1900, tm_mon is months since Jan (0-11)
    int year = (1900 + tm_now.tm_year) - 2000;

    char month =
        static_cast<char>((tm_now.tm_mon + 1) + 'A'); // Assuming 'A' implies 1-based offset
    int day = tm_now.tm_mday;
    char hour = static_cast<char>(tm_now.tm_hour + 'A');
    int minute = tm_now.tm_min;

    // String formatting and padding to 8 bytes (equivalent to PadRight)
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << year << month << day << hour << std::setw(2)
        << minute;

    std::string result = oss.str();
    result.resize(8, '\0'); // Pads or truncates strictly to 8 chars using null terminators

    return result;
}

int Bnd::ReadFormat(bool bitBigEndian) {
    int rawFormat;
    GetByte(rawFormat);

    bool reverse = bitBigEndian || (((rawFormat & 1) != 0) && ((rawFormat & 0b10000000) == 0));
    return reverse ? rawFormat : ReverseBits(rawFormat);
}

void Bnd::WriteFormat(int value) {
    std::vector<Format> formats = GetFormatFlags(value);
    bool hasFlag6 = std::find(formats.begin(), formats.end(), Format::Flag6) != formats.end();

    bool reverse = bitBigEndian || (bigEndian && hasFlag6);
    int rawFormat = reverse ? value : ReverseBits(value);

    WriteByte(rawFormat);
}

int Bnd::ReadFileFlags(bool bitBigEndian) {
    int rawFlags;
    GetByte(rawFlags);

    bool reverse = bitBigEndian;
    return reverse ? rawFlags : ReverseBits(rawFlags);
}

void Bnd::WriteFileFlags(int value) {
    bool reverse = bitBigEndian;
    int rawFlags = reverse ? value : ReverseBits(value);
    WriteByte(rawFlags);
}

uint8_t Bnd::ReverseBits(uint8_t value) {
    return static_cast<uint8_t>(((value & 0b00000001) << 7) | ((value & 0b00000010) << 5) |
                                ((value & 0b00000100) << 3) | ((value & 0b00001000) << 1) |
                                ((value & 0b00010000) >> 1) | ((value & 0b00100000) >> 3) |
                                ((value & 0b01000000) >> 5) | ((value & 0b10000000) >> 7));
}

std::vector<Bnd::Format> Bnd::GetFormatFlags(int formatValue) {
    const Format allFlags[] = {Format::BigEndian, Format::IDs,         Format::Names1,
                               Format::Names2,    Format::LongOffsets, Format::Compression,
                               Format::Flag6,     Format::Flag7};
    std::vector<Format> activeFlags;

    for (Format flag : allFlags) {
        if ((formatValue & static_cast<int>(flag)) != 0) {
            activeFlags.push_back(flag);
        }
    }
    return activeFlags;
}

std::vector<Bnd::FileFlags> Bnd::GetFileFlags(int flagsValue) {
    const FileFlags allFlags[] = {FileFlags::Compressed, FileFlags::Flag1, FileFlags::Flag2,
                                  FileFlags::Flag3,      FileFlags::Flag4, FileFlags::Flag5,
                                  FileFlags::Flag6,      FileFlags::Flag7};
    std::vector<FileFlags> activeFlags;

    for (FileFlags flag : allFlags) {
        if ((flagsValue & static_cast<int>(flag)) != 0) {
            activeFlags.push_back(flag);
        }
    }
    return activeFlags;
}

uint Bnd::GetBND4FileHeaderSize(int formatInt) {
    std::vector<Format> format = GetFormatFlags(formatInt);
    auto has_flag = [&format](Format flag) -> bool {
        return std::find(format.begin(), format.end(), flag) != format.end();
    };

    return 0x10 + (has_flag(Format::LongOffsets) ? 8 : 4) +
           (has_flag(Format::Compression) ? 8 : 0) + (has_flag(Format::IDs) ? 4 : 0) +
           ((has_flag(Format::Names1) || has_flag(Format::Names2)) ? 4 : 0) +
           (formatInt == static_cast<int>(Format::Names1) ? 8 : 0);
}

std::string Bnd::FindCommonBndRootPath(const std::vector<BinderFile>& files) {
    std::vector<std::string> paths;
    for (const auto& file : files) {
        paths.push_back(file.name);
    }

    std::string root = "";
    if (paths.empty()) {
        return root;
    }

    size_t minLen = paths[0].length();
    for (const auto& s : paths) {
        minLen = std::min(minLen, s.length());
    }

    size_t commonLen = 0;
    for (size_t i = 0; i < minLen; ++i) {
        char c = paths[0][i];
        bool allMatch = true;

        for (const auto& s : paths) {
            if (s[i] != c) {
                allMatch = false;
                break;
            }
        }

        if (!allMatch) {
            break;
        }
        commonLen++;
    }

    std::string rootPath = paths[0].substr(0, commonLen);
    size_t lastBackslash = rootPath.find_last_of('\\');
    size_t lastSlash = rootPath.find_last_of('/');

    size_t rootPathIndex = std::string::npos;
    if (lastBackslash != std::string::npos && lastSlash != std::string::npos) {
        rootPathIndex = std::max(lastBackslash, lastSlash);
    } else if (lastBackslash != std::string::npos) {
        rootPathIndex = lastBackslash;
    } else {
        rootPathIndex = lastSlash;
    }

    if (!rootPath.empty() && rootPathIndex != std::string::npos) {
        root = rootPath.substr(0, rootPathIndex);
    }

    return root;
}

std::optional<Bnd::BinderFile> Bnd::GetSameFile(
    const int& id, const std::vector<Bnd::BinderFile> otherBinderFiles) {
    std::optional<Bnd::BinderFile> bFile = std::nullopt;
    for (const auto& file : otherBinderFiles) {
        if (file.id == id) {
            return bFile.emplace(file);
        }
    }

    return bFile;
}

Bnd::~Bnd() {}

} // namespace FileHelper
