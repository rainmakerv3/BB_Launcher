// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <iostream>
#include <vector>

#include "BBFormats.h"
#include "modules/ModMerger.h"

namespace fs = std::filesystem;
namespace FileHelper {

BBFormat::BBFormat(ModMerger* modMerger) : QObject(modMerger), merger(modMerger) {}

void BBFormat::sendLog(const std::string& logMsg, LogFormat format) {
    QString msg = QString::fromStdString(logMsg);
    merger->Log(msg, static_cast<int>(format));
}

void BBFormat::debugLog(const std::string& logMsg, LogFormat format) {
    if (debugLogEnabled) {
        QString msg = QString::fromStdString(logMsg);
        merger->Log(msg, static_cast<int>(format));
    }
}

void BBFormat::PadStream(uint64_t alignment) {
    uint64_t currentPos = ostream->tellp();
    uint64_t remainder = currentPos % alignment;

    if (remainder != 0) {
        uint64_t paddingNeeded = alignment - remainder;
        std::vector<char> paddingBytes(paddingNeeded, 0x00);
        ostream->write(paddingBytes.data(), paddingBytes.size());
    }
}

std::string BBFormat::ReadUtf16String() {
    std::u16string u16;
    char16_t ch;

    while (istream->read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
        if (ch == 0) {
            break;
        }
        u16.push_back(ch);
    }

    std::string utf8;
    for (size_t i = 0; i < u16.size(); ++i) {
        uint32_t cp = u16[i];

        if (cp >= 0xD800 && cp <= 0xDBFF) {
            if (i + 1 < u16.size()) {
                uint32_t low = u16[i + 1];
                if (low >= 0xDC00 && low <= 0xDFFF) {
                    cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                    ++i;
                }
            }
        }

        if (cp <= 0x7F) {
            utf8 += static_cast<char>(cp);
        } else if (cp <= 0x7FF) {
            utf8 += static_cast<char>(0xC0 | ((cp >> 6) & 0x1F));
            utf8 += static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp <= 0xFFFF) {
            utf8 += static_cast<char>(0xE0 | ((cp >> 12) & 0x0F));
            utf8 += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            utf8 += static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp <= 0x10FFFF) {
            utf8 += static_cast<char>(0xF0 | ((cp >> 18) & 0x07));
            utf8 += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            utf8 += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            utf8 += static_cast<char>(0x80 | (cp & 0x3F));
        }
    }

    return utf8;
}

void BBFormat::WriteUtf16String(const std::string& input) {
    std::vector<char16_t> u16;
    for (size_t i = 0; i < input.size();) {
        uint32_t cp = 0;
        uint8_t b1 = static_cast<uint8_t>(input[i]);

        if (b1 <= 0x7F) {
            cp = b1;
            i += 1;
        } else if ((b1 & 0xE0) == 0xC0) {
            if (i + 1 >= input.size())
                break;
            uint8_t b2 = static_cast<uint8_t>(input[i + 1]);
            cp = ((b1 & 0x1F) << 6) | (b2 & 0x3F);
            i += 2;
        } else if ((b1 & 0xF0) == 0xE0) {
            if (i + 2 >= input.size())
                break;
            uint8_t b2 = static_cast<uint8_t>(input[i + 1]);
            uint8_t b3 = static_cast<uint8_t>(input[i + 2]);
            cp = ((b1 & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
            i += 3;
        } else if ((b1 & 0xF8) == 0xF0) {
            if (i + 3 >= input.size())
                break;
            uint8_t b2 = static_cast<uint8_t>(input[i + 1]);
            uint8_t b3 = static_cast<uint8_t>(input[i + 2]);
            uint8_t b4 = static_cast<uint8_t>(input[i + 3]);
            cp = ((b1 & 0x07) << 18) | ((b2 & 0x3F) << 12) | ((b3 & 0x3F) << 6) | (b4 & 0x3F);
            i += 4;
        } else {
            i += 1;
            continue;
        }

        if (cp <= 0xFFFF) {
            u16.push_back(static_cast<char16_t>(cp));
        } else if (cp <= 0x10FFFF) {
            cp -= 0x10000;
            uint16_t highSurrogate = 0xD800 + ((cp >> 10) & 0x03FF);
            uint16_t lowSurrogate = 0xDC00 + (cp & 0x03FF);

            u16.push_back(static_cast<char16_t>(highSurrogate));
            u16.push_back(static_cast<char16_t>(lowSurrogate));
        }
    }

    u16.push_back(0); // null terminator

    ostream->write(reinterpret_cast<const char*>(u16.data()), u16.size() * sizeof(char16_t));
}

void BBFormat::GetStr(std::string& buffer, int length) {
    buffer.resize(length);
    istream->read(buffer.data(), length);

    buffer.erase(std::remove(buffer.begin(), buffer.end(), '\0'), buffer.end());
}

void BBFormat::WriteStr(std::string buffer, int length) {
    buffer.resize(length);
    ostream->write(buffer.data(), length);
}

void BBFormat::GetInt64(int64_t& buffer) {
    buffer = 0;
    istream->read(reinterpret_cast<char*>(&buffer), 8);

    if (bigEndian) {
        buffer = std::byteswap(buffer);
    }
}

void BBFormat::GetInt64(uint64_t& buffer) {
    buffer = 0;
    istream->read(reinterpret_cast<char*>(&buffer), 8);

    if (bigEndian) {
        buffer = std::byteswap(buffer);
    }
}

void BBFormat::WriteInt64(int64_t buffer) {
    if (bigEndian) {
        buffer = std::byteswap(buffer);
    }

    ostream->write(reinterpret_cast<const char*>(&buffer), 8);
}

void BBFormat::WriteInt64(uint64_t buffer) {
    if (bigEndian) {
        buffer = std::byteswap(buffer);
    }

    ostream->write(reinterpret_cast<const char*>(&buffer), 8);
}

void BBFormat::GetInt32(int& buffer) {
    buffer = 0;
    istream->read(reinterpret_cast<char*>(&buffer), 4);

    if (bigEndian) {
        buffer = std::byteswap(buffer);
    }
}

void BBFormat::GetInt32(uint& buffer) {
    buffer = 0;
    istream->read(reinterpret_cast<char*>(&buffer), 4);

    if (bigEndian) {
        buffer = std::byteswap(buffer);
    }
}

void BBFormat::WriteInt32(int buffer) {
    if (bigEndian) {
        buffer = std::byteswap(buffer);
    }

    ostream->write(reinterpret_cast<const char*>(&buffer), 4);
}

void BBFormat::WriteInt32(uint buffer) {
    if (bigEndian) {
        buffer = std::byteswap(buffer);
    }

    ostream->write(reinterpret_cast<const char*>(&buffer), 4);
}

void BBFormat::GetInt16(int& buffer) {
    buffer = 0;
    istream->read(reinterpret_cast<char*>(&buffer), 2);

    if (bigEndian) {
        buffer = std::byteswap(buffer);
    }
}

void BBFormat::WriteInt16(int buffer) {
    if (bigEndian) {
        buffer = std::byteswap(buffer);
    }

    ostream->write(reinterpret_cast<const char*>(&buffer), 2);
}

void BBFormat::GetFloat(float& buffer) {
    int intBuf = 0;
    istream->read(reinterpret_cast<char*>(&intBuf), 4);

    if (bigEndian) {
        intBuf = std::byteswap(intBuf);
    }

    buffer = std::bit_cast<float>(intBuf);
}

void BBFormat::GetBytes(std::vector<char>& buffer, int length) {
    buffer.resize(length);
    istream->read(reinterpret_cast<char*>(buffer.data()), buffer.size());
}

void BBFormat::GetByte(int& buffer) {
    buffer = 0;
    istream->read(reinterpret_cast<char*>(&buffer), 1);
}

void BBFormat::GetByte(bool& buffer) {
    buffer = false;
    istream->read(reinterpret_cast<char*>(&buffer), 1);
}

void BBFormat::GetByte(char* buffer) {
    buffer[0] = 0;
    istream->read(buffer, 1);
}

void BBFormat::ReserveBytes(const std::string& name, const int& length) {
    auto it = std::find_if(res.begin(), res.end(),
                           [&name](const auto& pair) { return pair.first == name; });
    if (it != res.end()) {
        sendLog("ERROR: attemping to reserve already reserved value");
    }

    std::pair<std::string, std::streamoff> entry;
    entry.first = name;
    entry.second = ostream->tellp();
    res.push_back(entry);

    for (int i = 0; i < length; ++i) {
        ostream->write("\xFE", 1);
    }
}

void BBFormat::FillReservedInt64(const std::string& name, const uint64_t& value) {
    std::streamoff reservedOffset = 0;
    auto it = std::find_if(res.begin(), res.end(),
                           [&name](const auto& pair) { return pair.first == name; });

    if (it != res.end()) {
        reservedOffset = it->second;
        res.erase(it);
    } else {
        sendLog("ERROR: reserved offset not found, will likely cause errors. Name: " + name);
    }

    StepIn(reservedOffset, *ostream);
    WriteInt64(value);
    StepOut(*ostream);
}

void BBFormat::FillReservedInt32(const std::string& name, const int& value) {
    std::streamoff reservedOffset = 0;
    auto it = std::find_if(res.begin(), res.end(),
                           [&name](const auto& pair) { return pair.first == name; });

    if (it != res.end()) {
        reservedOffset = it->second;
        res.erase(it);
    } else {
        sendLog("ERROR: reserved offset not found, will likely cause errors. Name: " + name);
    }

    StepIn(reservedOffset, *ostream);
    WriteInt32(value);
    StepOut(*ostream);
}

void BBFormat::FillReservedInt32(const std::string& name, const uint& value) {
    std::streamoff reservedOffset = 0;
    auto it = std::find_if(res.begin(), res.end(),
                           [&name](const auto& pair) { return pair.first == name; });

    if (it != res.end()) {
        reservedOffset = it->second;
        res.erase(it);
    } else {
        sendLog("ERROR: reserved offset not found, will likely cause errors. Name: " + name);
    }

    StepIn(reservedOffset, *ostream);
    WriteInt32(value);
    StepOut(*ostream);
}

void BBFormat::WriteByte(const bool& buffer) {
    ostream->write(reinterpret_cast<const char*>(&buffer), 1);
}

void BBFormat::WriteByte(const int& buffer) {
    ostream->write(reinterpret_cast<const char*>(&buffer), 1);
}

void BBFormat::WriteBytes(const int& buffer, const int& length) {
    ostream->write(reinterpret_cast<const char*>(&buffer), length);
}

void BBFormat::StepIn(std::streamoff offset, std::istream& stream) {
    steps.push_back(stream.tellg());
    stream.seekg(offset, std::ios::beg);
}

void BBFormat::StepOut(std::istream& stream) {
    if (!steps.empty()) {
        std::streampos previous_pos = steps.back();
        steps.pop_back();
        stream.seekg(previous_pos, std::ios::beg);
    }
}

void BBFormat::StepIn(std::streamoff offset, std::ostream& stream) {
    steps.push_back(stream.tellp());
    stream.seekp(offset, std::ios::beg);
}

void BBFormat::StepOut(std::ostream& stream) {
    if (!steps.empty()) {
        std::streampos previous_pos = steps.back();
        steps.pop_back();
        stream.seekp(previous_pos, std::ios::beg);
    }
}

BBFormat::~BBFormat() {}

} // namespace  FileHelper
