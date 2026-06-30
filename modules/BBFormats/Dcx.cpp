// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <miniz.h>

#include "Dcx.h"
#include "modules/Common.h"

namespace fs = std::filesystem;

namespace FileHelper {

Dcx::Dcx(ModMerger* parent) : BBFormat(parent) {
    bigEndian = true;
}

bool Dcx::UnpackDcx(std::filesystem::path file, std::vector<char>& output) {
    if (!std::filesystem::exists(file)) {
        sendLog("ERROR File does not exist: " + Common::PathToU8(file), LogFormat::BoldRed);
        return false;
    }

    sendLog("Extracting Dcx: " + Common::PathToU8(file) + "...");
    origPath = file;
    istream = std::make_unique<std::ifstream>(file, std::ios::binary);
    uint32_t uintBuffer = 0;
    std::string strBuffer;

    if (!istream) {
        sendLog("Failed to open file: " + file.string());
        return false;
    }

    GetStr(strBuffer, 4);
    debugLog("MAGIC: " + strBuffer);

    if (!strBuffer.contains("DCX")) {
        sendLog("ERROR Invalid header magic: " + strBuffer, LogFormat::BoldRed);
        return false;
    }

    istream->seekg(0x28);
    GetStr(strBuffer, 4);
    debugLog("Format: " + strBuffer);

    if (!strBuffer.contains("DFLT")) {
        sendLog("ERROR Invalid compression: " + strBuffer, LogFormat::BoldRed);
        return false;
    }

    istream->seekg(0x4);
    GetInt32(compInfo.unk04);
    debugLog("Comp info unk04: " + std::to_string(compInfo.unk04));

    istream->seekg(0x10);
    GetInt32(compInfo.unk10);
    debugLog("Comp info unk10: " + std::to_string(compInfo.unk10));

    istream->seekg(0x14);
    GetInt32(compInfo.unk14);
    debugLog("Comp info unk14: " + std::to_string(compInfo.unk14));

    istream->seekg(0x30);
    GetByte(compInfo.unk30);
    debugLog("Comp info unk30: " + std::to_string(compInfo.unk30));

    istream->seekg(0x38);
    GetByte(compInfo.unk38);
    debugLog("Comp info unk38: " + std::to_string(compInfo.unk38));

    // reset position here
    istream->seekg(0x8);

    istream->ignore(4); // or assert 0x18
    istream->ignore(4); // or assert 0x24
    istream->ignore(8);

    GetStr(strBuffer, 4);
    debugLog("DCS check: " + strBuffer); // maybe assert if not DCS

    GetInt32(uintBuffer);
    debugLog("uncompressedSize: " + std::to_string(uintBuffer));

    GetInt32(uintBuffer);
    debugLog("compressedSize: " + std::to_string(uintBuffer));

    GetStr(strBuffer, 4);
    debugLog("DCP check: " + strBuffer); // maybe asset if not DCP

    GetStr(strBuffer, 4);
    debugLog("DFLT check: " + strBuffer); // maybe asset if not DFLT

    istream->ignore(4); // or assert 0x20
    istream->ignore(4); // unk30 value is here
    istream->ignore(4); // or assert 0x0
    istream->ignore(4); // unk38 value is here
    istream->ignore(4); // or assert 0x0
    istream->ignore(4); // or assert 0x00010100

    GetStr(strBuffer, 4);
    debugLog("DCA check: " + strBuffer); // maybe asset if not DCA

    GetInt32(uintBuffer);
    debugLog("compressedHeaderLength: " + std::to_string(uintBuffer));

    // Decompress
    mz_stream strm;
    strm.zalloc = nullptr;
    strm.zfree = nullptr;
    strm.opaque = nullptr;
    strm.avail_in = 0;
    strm.next_in = nullptr;

    if (mz_inflateInit2(&strm, 15) != MZ_OK) {
        debugLog("ERROR: Failed to initialize miniz inflate stream", LogFormat::BoldRed);
        return false;
    }

    constexpr size_t bufferSize = 32768;
    std::vector<uint8_t> inBuffer(bufferSize);
    std::vector<uint8_t> outBuffer(bufferSize);
    size_t fileSize = fs::file_size(file);
    int lastNotifiedPercent = 0;
    int ret;

    do {
        if (strm.avail_in == 0 && !istream->eof()) {
            istream->read(reinterpret_cast<char*>(inBuffer.data()), bufferSize);
            strm.avail_in = static_cast<uint32_t>(istream->gcount());
            strm.next_in = inBuffer.data();
        }

        strm.avail_out = bufferSize;
        strm.next_out = outBuffer.data();

        ret = mz_inflate(&strm, MZ_NO_FLUSH);

        if (ret == MZ_NEED_DICT || ret == MZ_DATA_ERROR || ret == MZ_MEM_ERROR) {
            mz_inflateEnd(&strm);
            sendLog("ERROR extraction failed due to corrupted stream data. Ret: " +
                        std::to_string(ret),
                    LogFormat::BoldRed);
            return false;
        }

        size_t decompressedChunkSize = bufferSize - strm.avail_out;
        if (decompressedChunkSize > 0) {
            output.insert(output.end(), outBuffer.begin(),
                          outBuffer.begin() + decompressedChunkSize);

            int currentPercent =
                static_cast<int>((static_cast<double>(strm.total_in) / fileSize) * 100);
            if (currentPercent >= lastNotifiedPercent + 10) {
                lastNotifiedPercent = currentPercent;
                sendLog("Extraction progress: " + std::to_string(lastNotifiedPercent) + "% done.",
                        LogFormat::Default);
            }
        }

    } while (ret != MZ_STREAM_END && (strm.avail_in > 0 || !istream->eof() || ret == MZ_OK));

    mz_inflateEnd(&strm);

    std::string extractedName = file.string();
    extractedName.erase(extractedName.length() - 4);
    extractedPath = extractedName;

    /* tests only
    std::ofstream outFile(extractedPath, std::ios::out | std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(output.data()),
                  output.size());
    */

    istream.reset();
    debugLog("decompressed bytes: " + std::to_string(output.size()));
    sendLog("Dcx extraction completed: " + Common::PathToU8(file) + "\n");
    return true;
}

bool Dcx::RepackDcx(std::vector<char> input) {
    if (input.empty()) {
        sendLog("ERROR: empty dcx input data", LogFormat::BoldRed);
        return false;
    }

    sendLog("Repacking Dcx: " + Common::PathToU8(origPath) + ", this may take a bit of time...");
    std::filesystem::remove(origPath);
    ostream = std::make_unique<std::ofstream>(origPath, std::ios::out | std::ios::binary);
    std::string strBuffer;

    strBuffer = "DCX";
    WriteStr(strBuffer, 4);

    WriteInt32(compInfo.unk04);
    WriteInt32(0x18);
    WriteInt32(0x24);

    WriteInt32(compInfo.unk10);
    WriteInt32(compInfo.unk14);

    strBuffer = "DCS";
    WriteStr(strBuffer, 4);

    WriteInt32(static_cast<uint>(input.size()));
    ReserveBytes("CompressedSize", 4);

    strBuffer = "DCP";
    WriteStr(strBuffer, 4);

    strBuffer = "DFLT";
    WriteStr(strBuffer, 4);

    WriteInt32(0x20);

    WriteByte(compInfo.unk30);
    WriteByte(0);
    WriteByte(0);
    WriteByte(0);

    WriteInt32(0);

    WriteByte(compInfo.unk38);
    WriteByte(0);
    WriteByte(0);
    WriteByte(0);

    WriteInt32(0);
    WriteInt32(0x00010100);

    strBuffer = "DCA";
    WriteStr(strBuffer, 4);
    WriteInt32(8);

    std::streamoff compressedStart = ostream->tellp();

    mz_stream stream;
    memset(&stream, 0, sizeof(stream));

    int status = mz_deflateInit2(&stream, 9, MZ_DEFLATED, 15, 9, MZ_DEFAULT_STRATEGY);
    if (status != MZ_OK) {
        sendLog("ERROR dcx deflate stream not initiated: " + std::to_string(status),
                LogFormat::BoldRed);
        return false;
    }

    constexpr size_t bufferSize = 32768;
    std::vector<uint8_t> outBuffer(bufferSize);
    size_t totalInputSize = input.size();
    size_t bytesProcessed = 0;
    int lastNotifiedPercent = 0;

    while (bytesProcessed < totalInputSize) {
        size_t currentChunkSize = std::min(bufferSize, totalInputSize - bytesProcessed);

        stream.next_in = reinterpret_cast<const uint8_t*>(input.data() + bytesProcessed);
        stream.avail_in = static_cast<unsigned int>(currentChunkSize);

        bool isLastChunk = (bytesProcessed + currentChunkSize == totalInputSize);
        int flush = isLastChunk ? MZ_FINISH : MZ_NO_FLUSH;

        do {
            stream.next_out = outBuffer.data();
            stream.avail_out = static_cast<unsigned int>(bufferSize);

            status = mz_deflate(&stream, flush);
            if (status != MZ_OK && status != MZ_STREAM_END) {
                mz_deflateEnd(&stream);
                sendLog("ERROR dcx compression failed: " + std::to_string(status),
                        LogFormat::BoldRed);
                return false;
            }

            size_t compressedChunkSize = bufferSize - stream.avail_out;
            if (compressedChunkSize > 0) {
                ostream->write(reinterpret_cast<const char*>(outBuffer.data()),
                               compressedChunkSize);
            }
        } while (stream.avail_out == 0);

        bytesProcessed += currentChunkSize;

        int currentPercent =
            static_cast<int>((static_cast<double>(bytesProcessed) / totalInputSize) * 100);
        if (currentPercent >= lastNotifiedPercent + 10) {
            lastNotifiedPercent += 10;
            sendLog("Compression progress: " + std::to_string(lastNotifiedPercent) + "% done.",
                    LogFormat::Default);
        }
    }

    mz_deflateEnd(&stream);

    WriteInt32(Adler32(input));
    FillReservedInt32("CompressedSize", static_cast<int>(ostream->tellp() - compressedStart));

    ostream.reset();
    sendLog("Dcx repacking completed: " + Common::PathToU8(origPath.string()) + "\n");
    return true;
}

uint32_t Dcx::Adler32(const std::vector<char>& data) {
    uint32_t adlerA = 1;
    uint32_t adlerB = 0;

    const uint8_t* udata = reinterpret_cast<const uint8_t*>(data.data());
    size_t length = data.size();
    for (size_t i = 0; i < length; ++i) {
        adlerA = (adlerA + udata[i]) % 65521;
        adlerB = (adlerB + adlerA) % 65521;
    }

    return (adlerB << 16) | adlerA;
}

Dcx::~Dcx() {}

} // namespace FileHelper
