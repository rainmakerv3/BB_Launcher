// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <fstream>
#include <sstream>
#include <QTextBrowser>

class ModMerger;

namespace FileHelper {

class BBFormat : public QObject {
    Q_OBJECT

public:
    explicit BBFormat(ModMerger* parent = nullptr);
    ~BBFormat();

protected:
#ifndef DEBUG
    bool debugLogEnabled = false;
#else
    bool debugLogEnabled = true;
#endif

    enum class ModPriority : int { NotSet, Mod1, Mod2 };
    enum class LogFormat : int { Default, Yellow, BoldRed, BoldGreen };

    struct Vector3 {
        float x, y, z;

        bool operator==(const Vector3& other) const {
            const float diff = 0.0001f; // tolerance
            return std::abs(x - other.x) < diff && std::abs(y - other.y) < diff &&
                   std::abs(z - other.z) < diff;
        }
    };

    void sendLog(const std::string& log, LogFormat format = LogFormat::Default);
    void debugLog(const std::string& log, LogFormat format = LogFormat::Default);

    void StepIn(std::streamoff offset, std::istream& stream);
    void StepOut(std::istream& stream);

    void StepIn(std::streamoff offset, std::ostream& stream);
    void StepOut(std::ostream& stream);

    void GetStr(std::string& buffer, int length);
    void WriteStr(std::string buffer, int length);

    void GetInt64(int64_t& buffer);
    void GetInt64(uint64_t& buffer);
    void WriteInt64(int64_t buffer);
    void WriteInt64(uint64_t buffer);

    void GetInt32(int& buffer);
    void GetInt32(uint& buffer);
    void WriteInt32(int buffer);
    void WriteInt32(uint buffer);

    void GetInt16(int& buffer);
    void WriteInt16(int buffer);

    void GetFloat(float& buffer);
    void WriteFloat(float value);
    void GetBytes(std::vector<char>& buffer, int length);

    void GetByte(int& buffer);
    void GetByte(bool& buffer);
    void GetByte(char* buffer);
    void WriteByte(const int& buffer);
    void WriteByte(const bool& buffer);
    void WriteBytes(const int& buffer, const int& length);

    void GetVector3(Vector3& vec3);
    void WriteVector3(Vector3& vec3);

    void ReserveBytes(const std::string& name, const int& length);
    void FillReservedInt64(const std::string& name, const uint64_t& value);
    void FillReservedInt32(const std::string& name, const uint& value);
    void FillReservedInt32(const std::string& name, const int& value);

    std::string ReadUtf16String();
    void WriteUtf16String(const std::string& input);

    uint8_t ReverseBits(uint8_t value);
    void PadStream(uint64_t alignment);

    ModMerger* merger;

    bool bigEndian = false;
    std::unique_ptr<std::istream> istream = nullptr;
    std::unique_ptr<std::ostream> ostream = nullptr;

    std::vector<std::streamoff> steps = {};
    std::vector<std::pair<std::string, std::streamoff>> res = {};
};


} // namespace FileHelper
