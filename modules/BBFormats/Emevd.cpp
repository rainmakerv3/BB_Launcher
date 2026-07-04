// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <vector>

#include "Emevd.h"
#include "modules/ModMerger.h"

namespace fs = std::filesystem;

namespace FileHelper {

Emevd::Emevd(std::vector<char>& data, ModMerger* parent) : BBFormat(parent) {
    bigEndian = false;
    ReadEmevd(data);
}

bool Emevd::ReadEmevd(std::vector<char>& data) {
    if (data.empty()) {
        sendLog("ERROR: empty fmg input data", LogFormat::BoldRed);
        return false;
    }

    istream = std::make_unique<std::stringstream>(std::string(data.begin(), data.end()),
                                                  std::ios_base::in | std::ios_base::binary);

    std::string strBuffer = "";
    int intBuffer = 0;
    int64_t int64Buffer = 0;

    GetStr(strBuffer, 4);
    debugLog("MAGIC: " + strBuffer);

    if (!strBuffer.contains("EVD")) {
        sendLog("ERROR invalid emevd header: " + strBuffer, LogFormat::BoldRed);
        return false;
    }

    GetByte(intBuffer); // bigendian, should be 0
    debugLog("bigEndian: " + std::to_string(intBuffer));

    char sByte;
    istream->read(&sByte, 1);              // signed byte
    int is64Bit = static_cast<int>(sByte); // is64bit, should be -1, assert 0/-1?
    debugLog("is64bit: " + std::to_string(is64Bit));

    GetByte(intBuffer); // unk06 should be 0
    debugLog("unk06: " + std::to_string(intBuffer));

    GetByte(intBuffer); // unk07 should be 0
    debugLog("unk07: " + std::to_string(intBuffer));

    GetInt32(intBuffer); // version should be 0xCC / 204
    debugLog("version: " + std::to_string(intBuffer));

    GetInt32(intBuffer);
    debugLog("fileSize: " + std::to_string(intBuffer));

    Offsets offsets;

    int64_t eventCount = 0;
    GetInt64(eventCount);
    debugLog("eventCount: " + std::to_string(eventCount));

    GetInt64(offsets.events);
    debugLog("offsets.events: " + std::to_string(offsets.events));

    GetInt64(int64Buffer);
    debugLog("instruction count: " + std::to_string(int64Buffer));

    GetInt64(offsets.instructions);
    debugLog("offsets.instructions: " + std::to_string(offsets.instructions));

    GetInt64(int64Buffer); // should be 0
    debugLog("zero check: " + std::to_string(int64Buffer));

    istream->ignore(8); // unknown struct offset

    GetInt64(int64Buffer);
    debugLog("layer count: " + std::to_string(int64Buffer));

    GetInt64(offsets.layers);
    debugLog("offsets.layers: " + std::to_string(offsets.layers));

    GetInt64(int64Buffer);
    debugLog("parameter count: " + std::to_string(int64Buffer));

    GetInt64(offsets.parameters);
    debugLog("offsets.parameters: " + std::to_string(offsets.parameters));

    int64_t linkedFileCount = 0;
    GetInt64(linkedFileCount);
    debugLog("linkedFileCount: " + std::to_string(linkedFileCount));

    GetInt64(offsets.linkedFiles);
    debugLog("offsets.linkedFiles: " + std::to_string(offsets.linkedFiles));

    GetInt64(int64Buffer);
    debugLog("arg data length: " + std::to_string(int64Buffer));

    GetInt64(offsets.arguments);
    debugLog("offsets.arguments: " + std::to_string(offsets.arguments));

    int64_t stringsLength = 0;
    GetInt64(stringsLength);
    debugLog("stringsLength: " + std::to_string(stringsLength));

    GetInt64(offsets.strings);
    debugLog("offsets.strings: " + std::to_string(offsets.strings));

    istream->seekg(offsets.events);
    for (int i = 0; i < eventCount; i++) {
        Event ev;

        GetInt64(ev.id);
        debugLog("ev.id: " + std::to_string(ev.id));

        int64_t instructionCount = 0;
        GetInt64(instructionCount);
        debugLog("instructionCount: " + std::to_string(instructionCount));

        int64_t instructionsOffset = 0;
        GetInt64(instructionsOffset);
        debugLog("instructionOffset: " + std::to_string(instructionsOffset));

        int64_t parameterCount = 0;
        GetInt64(parameterCount);
        debugLog("parameterCount: " + std::to_string(parameterCount));

        int64_t parametersOffset = 0;
        GetInt64(parametersOffset);
        debugLog("parametersOffset: " + std::to_string(parametersOffset));

        GetInt32(intBuffer);
        ev.restBehavior = static_cast<RestBehaviorType>(intBuffer);
        debugLog("ev.restBehavior: " + std::to_string(intBuffer));

        GetInt32(intBuffer);
        debugLog("zero check: " + std::to_string(intBuffer));

        if (instructionCount > 0) {
            StepIn(offsets.instructions + instructionsOffset, *istream);
            {
                for (int j = 0; j < instructionCount; j++) {
                    Instruction inst;
                    GetInt32(inst.bank);
                    debugLog("inst.bank: " + std::to_string(inst.bank));

                    GetInt32(inst.id);
                    debugLog("inst.id: " + std::to_string(inst.id));

                    int64_t argsLength = 0;
                    GetInt64(argsLength);
                    debugLog("argsLength: " + std::to_string(argsLength));

                    int64_t argsOffset = 0;
                    GetInt64(argsOffset);
                    debugLog("argsOffset: " + std::to_string(argsOffset));

                    int layerOffset = 0;
                    GetInt32(layerOffset);
                    debugLog("layerOffset: " + std::to_string(layerOffset));

                    istream->ignore(4); // or assert 0

                    if (argsLength > 0) {
                        StepIn(offsets.arguments + argsOffset, *istream);
                        GetBytes(inst.argData, static_cast<int>(argsLength));
                        StepOut(*istream);
                    } else {
                        inst.argData = {};
                    }

                    if (layerOffset != -1) {
                        StepIn(offsets.layers + layerOffset, *istream);
                        {
                            uint buf = 0;
                            GetInt32(buf);
                            inst.layer = buf;
                            debugLog("inst.layer " + std::to_string(inst.layer.value()));

                            // br.AssertVarint(0); not doing the asserts
                            // br.AssertVarint(-1);
                            // br.AssertVarint(1);
                        }
                        StepOut(*istream);
                    }

                    ev.instructions.push_back(inst);
                }
            }
            StepOut(*istream);
        }

        if (parameterCount > 0) {
            StepIn(offsets.parameters + parametersOffset, *istream);
            {
                for (int j = 0; j < parameterCount; j++) {
                    Parameter p;

                    GetInt64(p.instructionIndex);
                    debugLog("p.instructionIndex: " + std::to_string(p.instructionIndex));

                    GetInt64(p.targetStartByte);
                    debugLog("p.targetStartByte: " + std::to_string(p.targetStartByte));

                    GetInt64(p.sourceStartByte);
                    debugLog("p.sourceStartByte: " + std::to_string(p.sourceStartByte));

                    GetInt32(p.byteCount);
                    debugLog("p.byteCount: " + std::to_string(p.byteCount));

                    GetInt32(p.unkID);
                    debugLog("p.unkID: " + std::to_string(p.unkID));

                    ev.parameters.push_back(p);
                }
            }
            StepOut(*istream);
        }

        events.push_back(ev);
    }

    istream->seekg(offsets.linkedFiles);
    for (int i = 0; i < linkedFileCount; i++) {
        int64_t off;
        GetInt64(off);
        debugLog("linkedfileoffset: " + std::to_string(off));

        linkedData.linkedFileOffsets.push_back(off);
    }

    istream->seekg(offsets.strings);
    GetBytes(linkedData.stringData, stringsLength);

    istream.reset();
    return true;
}

bool Emevd::RepackEmevd(std::vector<char>& outputData) {
    ostream = std::make_unique<std::stringstream>(std::ios_base::out | std::ios_base::binary);
    std::string strBuffer;

    strBuffer = "EVD";
    WriteStr(strBuffer, 4);

    WriteByte(0);
    WriteByte(-1);
    WriteByte(0);
    WriteByte(0);

    WriteInt32(204);
    ReserveBytes("FileSize", 4);

    Offsets offsets;

    WriteInt64(static_cast<int64_t>(events.size()));
    ReserveBytes("EventsOffset", 8);

    int64_t instCount = 0;
    int64_t layersCount = 0;
    int64_t parametersCount = 0;

    for (const Event& ev : events) {
        parametersCount += ev.parameters.size();
        instCount += ev.instructions.size();
        for (const Instruction& inst : ev.instructions) {
            if (inst.layer.has_value()) {
                layersCount += 1;
            }
        }
    }
    WriteInt64(instCount);
    ReserveBytes("InstructionsOffset", 8);

    WriteInt64(static_cast<int64_t>(0));
    ReserveBytes("Offset3", 8);
    WriteInt64(layersCount);
    ReserveBytes("LayersOffset", 8);

    WriteInt64(parametersCount);
    ReserveBytes("ParametersOffset", 8);
    WriteInt64(static_cast<int64_t>(linkedData.linkedFileOffsets.size()));
    ReserveBytes("LinkedFilesOffset", 8);

    ReserveBytes("ArgumentsLength", 8);
    ReserveBytes("ArgumentsOffset", 8);
    WriteInt64(static_cast<int64_t>(linkedData.stringData.size()));
    ReserveBytes("StringsOffset", 8);

    offsets.events = static_cast<int64_t>(ostream->tellp());
    FillReservedInt64("EventsOffset", offsets.events);

    for (int i = 0; i < events.size(); i++) {
        Event ev = events[i];

        WriteInt64(ev.id);
        WriteInt64(static_cast<int64_t>(ev.instructions.size()));

        std::string evInstOff = std::format("Event{}InstrsOffset", i);
        ReserveBytes(evInstOff, 8);

        WriteInt64(static_cast<int64_t>(ev.parameters.size()));

        std::string evParamsOff = std::format("Event{}ParamsOffset", i);
        ReserveBytes(evParamsOff, 4);
        WriteInt32(0);

        WriteInt32(static_cast<uint>(ev.restBehavior));
        WriteInt32(0);
    }

    offsets.instructions = static_cast<int64_t>(ostream->tellp());
    FillReservedInt64("InstructionsOffset", offsets.instructions);

    for (int i = 0; i < events.size(); i++) {
        const Event& ev = events[i];

        int64_t currentPos = static_cast<int64_t>(ostream->tellp());
        int64_t instrsOffset = ev.instructions.size() > 0 ? currentPos - offsets.instructions : -1;

        std::string evInstOff = std::format("Event{}InstrsOffset", i);
        FillReservedInt64(evInstOff, instrsOffset);

        for (int j = 0; j < ev.instructions.size(); j++) {
            const Instruction& inst = ev.instructions[j];

            WriteInt32(inst.bank);
            WriteInt32(inst.id);
            WriteInt64(static_cast<int64_t>(inst.argData.size()));

            std::string evInstArgOff = std::format("Event{}Instr{}ArgsOffset", i, j);
            ReserveBytes(evInstArgOff, 4);
            WriteInt32(0);

            std::string evInstLayerOff = std::format("Event{}Instr{}LayerOffset", i, j);
            ReserveBytes(evInstLayerOff, 4);
            WriteInt32(0);
        }
    }

    offsets.layers = static_cast<int64_t>(ostream->tellp());
    FillReservedInt64("Offset3", offsets.layers); // seems to always be equal to layers?
    FillReservedInt64("LayersOffset", offsets.layers);

    for (int i = 0; i < events.size(); i++) {
        const Event& ev = events[i];
        for (int j = 0; j < ev.instructions.size(); j++) {
            const Instruction& inst = ev.instructions[j];

            std::string evInstLayerOff = std::format("Event{}Instr{}LayerOffset", i, j);
            if (inst.layer.has_value()) {
                FillReservedInt32(evInstLayerOff, static_cast<uint>(ostream->tellp()) -
                                                      static_cast<uint>(offsets.layers));
                WriteInt32(2);
                WriteInt32(inst.layer.value());
                WriteInt64(static_cast<int64_t>(0));
                WriteInt64(static_cast<int64_t>(-1));
                WriteInt64(static_cast<int64_t>(1));
            } else {
                FillReservedInt32(evInstLayerOff, 0xFFFFFFFF);
            }
        }
    }

    offsets.arguments = static_cast<int64_t>(ostream->tellp());
    FillReservedInt64("ArgumentsOffset", offsets.arguments);

    for (int i = 0; i < events.size(); i++) {
        const Event& ev = events[i];
        for (int j = 0; j < ev.instructions.size(); j++) {
            const Instruction& inst = ev.instructions[j];

            int32_t currentPos = static_cast<int32_t>(ostream->tellp());
            int32_t argsOffset =
                inst.argData.size() > 0 ? currentPos - static_cast<int32_t>(offsets.arguments) : -1;
            std::string evInstArgOff = std::format("Event{}Instr{}ArgsOffset", i, j);
            FillReservedInt32(evInstArgOff, argsOffset);

            ostream->write(inst.argData.data(), inst.argData.size());
            PadStream(4);
        }
    }

    // padding
    uint32_t current_pos = static_cast<uint32_t>(ostream->tellp());
    uint32_t relative_offset = current_pos - static_cast<uint32_t>(offsets.arguments);
    uint32_t padding = (0x10 - (relative_offset % 0x10)) % 0x10;

    for (uint32_t i = 0; i < padding; ++i) {
        ostream->put(0x00);
    }

    FillReservedInt64("ArgumentsLength",
                      static_cast<int64_t>(ostream->tellp()) - offsets.arguments);

    offsets.parameters = static_cast<int64_t>(ostream->tellp());
    FillReservedInt64("ParametersOffset", static_cast<int64_t>(ostream->tellp()));

    for (int i = 0; i < events.size(); i++) {
        const Event& ev = events[i];

        std::string evParamsOff = std::format("Event{}ParamsOffset", i);
        int32_t paramsOffset =
            ev.parameters.size() > 0 ? static_cast<int>(ostream->tellp()) - offsets.parameters : -1;
        FillReservedInt32(evParamsOff, paramsOffset);

        for (int j = 0; j < ev.parameters.size(); j++) {
            const Parameter& par = ev.parameters[j];

            WriteInt64(par.instructionIndex);
            WriteInt64(par.targetStartByte);
            WriteInt64(par.sourceStartByte);
            WriteInt32(par.byteCount);
            WriteInt32(par.unkID);
        }
    }

    offsets.linkedFiles = static_cast<int64_t>(ostream->tellp());
    FillReservedInt64("LinkedFilesOffset", offsets.linkedFiles);
    for (const auto& offset : linkedData.linkedFileOffsets) {
        WriteInt64(offset);
    }

    offsets.strings = static_cast<int64_t>(ostream->tellp());
    FillReservedInt64("StringsOffset", offsets.strings);
    ostream->write(linkedData.stringData.data(), linkedData.stringData.size());

    FillReservedInt32("FileSize", static_cast<uint>(ostream->tellp()));

    auto& stringStream = static_cast<std::stringstream&>(*ostream);
    std::string str = stringStream.str();
    outputData = std::vector<char>(str.begin(), str.end());

    /* tests
    std::ofstream outFile("test.emevd", std::ios::out | std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(outputData.data(), outputData.size());
        outFile.close();
    }
    */

    ostream.reset();
    sendLog("EMEVD Repacking complete\n");
    return true;
}

bool Emevd::HandleConflict(std::vector<char>& mod1Data, std::vector<char>& mod2Data) {
    Emevd mod1Emevd = Emevd(mod1Data, merger);
    const std::vector mod1events = mod1Emevd.events;

    Emevd mod2Emevd = Emevd(mod2Data, merger);
    const std::vector mod2events = mod2Emevd.events;

    for (auto& event : events) {
        std::optional mod1event = GetSameEvent(event.id, mod1events);
        std::optional mod2event = GetSameEvent(event.id, mod2events);

        bool mod1Modified = mod1event && (mod1event.value() != event);
        bool mod2Modified = mod2event && (mod2event.value() != event);

        if (mod1Modified && mod2Modified) {
            if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                QMetaObject::invokeMethod(merger, &ModMerger::OpenPriorityDialog,
                                          Qt::BlockingQueuedConnection);
                if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                    return false;
                }
            }

            if (merger->GetModPriority() == ModMerger::ModPriority::Mod1) {
                event = mod1event.value();
                sendLog("Unresolvable conflict, in event: " + mod1event.value().name +
                            " using data from prioritized mod: " + merger->Mod1Name(),
                        LogFormat::Yellow);
            } else if (merger->GetModPriority() == ModMerger::ModPriority::Mod2) {
                event = mod2event.value();
                sendLog("Unresolvable conflict, in event: " + mod2event.value().name +
                            " using data from prioritized mod: " + merger->Mod2Name(),
                        LogFormat::Yellow);
            }
        } else if (mod1Modified && !mod2Modified) {
            event = mod1event.value();
            sendLog("Merging modified event structure: " + std::to_string(event.id) +
                    " from mod: " + merger->Mod1Name());
        } else if (mod2Modified && !mod1Modified) {
            event = mod2event.value();
            sendLog("Merging modified event structure: " + std::to_string(event.id) +
                    " from mod: " + merger->Mod2Name());
        }
    }

    std::unordered_set<int> existingIds;
    for (const auto& ev : events) {
        existingIds.insert(ev.id);
    }

    for (const auto& event : mod1events) {
        if (existingIds.find(event.id) == existingIds.end()) {
            events.push_back(event);
            sendLog("New custom event added id: " + std::to_string(event.id) +
                    " from mod: " + merger->Mod1Name());
        }
    }
    for (const auto& event : mod2events) {
        if (existingIds.find(event.id) == existingIds.end()) {
            events.push_back(event);
            sendLog("New custom event added id: " + std::to_string(event.id) +
                    " from mod: " + merger->Mod2Name());
        }
    }

    bool mod1LinkedDataModified = linkedData != mod1Emevd.linkedData;
    bool mod2LinkedDataModified = linkedData != mod2Emevd.linkedData;

    if (mod1LinkedDataModified && mod2LinkedDataModified) {
        sendLog("ERROR :Mods with conflicting linked data offset currently cannot be merged. "
                "Aborting...",
                LogFormat::BoldRed);
        return false;
    } else if (mod1LinkedDataModified && !mod2LinkedDataModified) {
        linkedData = mod1Emevd.linkedData;
        sendLog("Merging modified linked data offsets from Mod: " + merger->Mod1Name());
    } else if (mod2LinkedDataModified && !mod1LinkedDataModified) {
        linkedData = mod2Emevd.linkedData;
        sendLog("Merging modified linked data offsets from Mod: " + merger->Mod2Name());
    }

    return true;
}

std ::optional<Emevd::Event> Emevd::GetSameEvent(const int& id,
                                                 const std::vector<Event>& otherEntries) {
    std::optional<Event> result = std::nullopt;
    for (const auto& otherEntry : otherEntries) {
        if (otherEntry.id == id) {
            return result.emplace(otherEntry);
        }
    }

    return result;
}

Emevd::~Emevd() {}

} // namespace FileHelper
