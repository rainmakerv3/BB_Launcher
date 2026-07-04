// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "BBFormats.h"

namespace FileHelper {

class Emevd : public BBFormat {
    Q_OBJECT

    enum RestBehaviorType : int { Default = 0, Restart = 1, End = 2 };

    struct Parameter {
        int64_t instructionIndex;
        int64_t targetStartByte;
        int64_t sourceStartByte;
        int byteCount;
        int unkID;

        bool operator==(const Parameter&) const = default;
    };

    struct Instruction {
        int bank;
        int id;
        std::vector<char> argData;
        std::optional<uint> layer = std::nullopt;

        bool operator==(const Instruction&) const = default;
    };

    struct Event {
        int64_t id;
        RestBehaviorType restBehavior;
        std::string name;
        std::vector<Parameter> parameters;
        std::vector<Instruction> instructions;

        bool operator==(const Event&) const = default;
    };

    struct Offsets {
        int64_t events;
        int64_t instructions;
        int64_t layers;
        int64_t parameters;
        int64_t linkedFiles;
        int64_t arguments;
        int64_t strings;
    };

    struct LinkedData {
        std::vector<int64_t> linkedFileOffsets;
        std::vector<char> stringData;

        bool operator==(const LinkedData&) const = default;
    };

public:
    explicit Emevd(std::vector<char>& data, ModMerger* parent);
    ~Emevd() override;

    bool ReadEmevd(std::vector<char>& data);
    bool RepackEmevd(std::vector<char>& outputData);
    bool HandleConflict(std::vector<char>& mod1Data, std::vector<char>& mod2Data);

private:
    enum class DiffOp { Match, Insert, Delete, Modify };

    struct DiffElement {
        DiffOp op;
        size_t vanillaIdx;
        size_t modIdx;
    };

    std::optional<Event> GetSameEvent(const int& id, const std::vector<Event>& otherEvents);
    std::vector<Event> events;
    LinkedData linkedData;
};

} // namespace FileHelper
