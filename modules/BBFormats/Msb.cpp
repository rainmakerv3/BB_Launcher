// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <regex>
#include <vector>

#include "Msb.h"
#include "modules/ModMerger.h"

namespace fs = std::filesystem;

namespace FileHelper {

Msb::Msb(std::vector<char>& data, ModMerger* parent) : BBFormat(parent) {
    bigEndian = false;
    ReadMsb(data);
}

bool Msb::ReadMsb(std::vector<char>& data) {
    if (data.empty()) {
        sendLog("ERROR: empty fmg input data", LogFormat::BoldRed);
        return false;
    }

    istream = std::make_unique<std::stringstream>(std::string(data.begin(), data.end()),
                                                  std::ios_base::in | std::ios_base::binary);

    std::string strBuffer = "";
    int intBuffer = 0;

    GetStr(strBuffer, 4);
    // debugLog("MAGIC: " + strBuffer);

    if (!strBuffer.contains("MSB")) {
        sendLog("ERROR invalid param header: " + strBuffer, LogFormat::BoldRed);
        return false;
    }

    GetInt32(intBuffer);
    // debugLog("1 check : " + std::to_string(intBuffer));

    GetInt32(intBuffer);
    // debugLog("16 check : " + std::to_string(intBuffer));

    GetByte(intBuffer); // bigendian, should be false
    // debugLog("zero check : " + std::to_string(intBuffer));

    GetByte(intBuffer); // bigbigendian, should be false
    // debugLog("zero check : " + std::to_string(intBuffer));

    GetByte(intBuffer); // encoding, should be true
    // debugLog("1 check : " + std::to_string(intBuffer));

    GetByte(intBuffer); // 64 bit should be 255
    // debugLog("255 check : " + std::to_string(intBuffer));

    std::vector<int64_t> modelOffsets;
    int64_t eventsOffset;
    GetSectionOffsets(modelOffsets, eventsOffset);

    istream->seekg(eventsOffset);
    std::vector<int64_t> eventOffsets;
    int64_t regionsOffset;
    GetSectionOffsets(eventOffsets, regionsOffset);

    istream->seekg(regionsOffset);
    std::vector<int64_t> regionOffsets;
    int64_t partsOffset;
    GetSectionOffsets(regionOffsets, partsOffset);

    istream->seekg(partsOffset);
    std::vector<int64_t> partOffsets;
    int64_t int64Buffer;
    GetSectionOffsets(partOffsets, int64Buffer);
    // debugLog("zeroCheck: " + std::to_string(int64Buffer));

    for (const auto& off : modelOffsets) {
        Model model;
        istream->seekg(off);
        int64_t start = static_cast<int64_t>(istream->tellg());

        int64_t nameOffset;
        GetInt64(nameOffset);
        // debugLog("nameOffset: " + std::to_string(nameOffset));

        GetInt32(model.type);
        // debugLog("type: " + std::to_string(type));

        GetInt32(intBuffer); // type id
        // debugLog("typeid: " + std::to_string(intBuffer));

        int64_t sibOffset;
        GetInt64(sibOffset);
        // debugLog("sibOffset: " + std::to_string(sibOffset));

        GetInt32(model.instanceCount);
        // debugLog("model.instanceCount: " + std::to_string(model.instanceCount));

        GetInt32(intBuffer);
        // debugLog("zero check: " + std::to_string(intBuffer));

        GetInt32(intBuffer);
        // debugLog("zero check: " + std::to_string(intBuffer));

        GetInt32(intBuffer);
        // debugLog("zero check: " + std::to_string(intBuffer));

        // assert name/sib offsets are not 0?

        istream->seekg(start + nameOffset);
        model.name = ReadUtf16String();
        // debugLog("model.name: " + model.name);

        model.sibPath = ReadUtf16String();
        // debugLog("model.sibPath: " + model.sibPath);

        models.push_back(model);
    }

    for (const auto& off : eventOffsets) {
        Event event;
        istream->seekg(off);
        int64_t start = static_cast<int64_t>(istream->tellg());

        int64_t nameOffset;
        GetInt64(nameOffset);
        // debugLog("nameOffset: " + std::to_string(nameOffset));

        GetInt32(event.id);
        // debugLog("event.id: " + std::to_string(event.id));

        GetInt32(event.type);
        // debugLog("event.type: " + std::to_string(event.type));

        GetInt32(intBuffer); // type id
        // debugLog("typeid: " + std::to_string(intBuffer));

        GetInt32(intBuffer);
        // debugLog("zero check: " + std::to_string(intBuffer));

        int64_t entityDataOffset;
        GetInt64(entityDataOffset);
        // debugLog("entityDataOffset: " + std::to_string(entityDataOffset));

        int64_t typeDataOffset;
        GetInt64(typeDataOffset);
        // debugLog("typeDataOffset: " + std::to_string(typeDataOffset));

        // original asserts name/entity offsets can't be zero and xor assrts typedata offset is not
        // 0 when type is 0xFFFFFF

        istream->seekg(start + nameOffset);
        event.name = ReadUtf16String();

        istream->seekg(start + entityDataOffset);
        GetInt32(event.partIndex);
        // debugLog("event.partIndex: " + std::to_string(event.partIndex));

        GetInt32(event.regionIndex);
        // debugLog("event.regionIndex: " + std::to_string(event.regionIndex));

        GetInt32(event.entityId);
        // debugLog("event.entityId: " + std::to_string(event.entityId));

        GetByte(event.unkE0C);
        // debugLog("event.unkE0C: " + std::to_string(event.unkE0C));

        GetByte(event.unkE0D);
        // debugLog("event.unkE0D: " + std::to_string(event.unkE0D));

        GetByte(event.unkE0E);
        // debugLog("event.unkE0E: " + std::to_string(event.unkE0E));

        GetByte(event.unkE0F);
        // debugLog("event.unkE0F: " + std::to_string(event.unkE0F));

        if (event.type < 18) {
            istream->seekg(start + typeDataOffset);
            GetBytes(event.typeData, GetEventTypeDataLength(event.type));
        }

        events.push_back(event);
    }

    /* duplication test - event names
    std::unordered_set<std::string> evnames;
    for (const auto& event : events) {
        if (!evnames.insert(event.name).second) {
            sendLog("duplicate event name found: " + event.name, LogFormat::BoldRed);
        }
    }
    */

    for (const auto& off : regionOffsets) {
        Region region;
        istream->seekg(off);
        int64_t start = static_cast<int64_t>(istream->tellg());
        debugLog("regionstartoffset: " + std::to_string(start));

        int64_t nameOffset;
        GetInt64(nameOffset);
        // debugLog("nameOffset: " + std::to_string(nameOffset));

        GetInt32(intBuffer);
        // debugLog("zero check: " + std::to_string(intBuffer));

        GetInt32(intBuffer); // type id
        // debugLog("id2: " + std::to_string(intBuffer));

        GetInt32(region.shapeType);
        // debugLog("type: " + std::to_string(shapeType));

        GetVector3(region.position);
        // debugLog("region.position.x: " + std::to_string(region.position.x));
        //  debugLog("region.position.y: " + std::to_string(region.position.y));
        //  debugLog("region.position.z: " + std::to_string(region.position.z));

        GetVector3(region.rotation);
        // debugLog("region.rotation.x: " + std::to_string(region.rotation.x));
        // debugLog("region.rotation.y: " + std::to_string(region.rotation.y));
        // debugLog("region.rotation.z: " + std::to_string(region.rotation.z));

        GetInt32(intBuffer);
        // debugLog("zero check: " + std::to_string(intBuffer));

        int64_t unkOffsetA;
        GetInt64(unkOffsetA);
        // debugLog("unkOffsetA: " + std::to_string(unkOffsetA));

        int64_t unkOffsetB;
        GetInt64(unkOffsetB);
        // debugLog("unkOffsetB: " + std::to_string(unkOffsetB));

        int64_t shapeDataOffset;
        GetInt64(shapeDataOffset);
        // debugLog("shapeDataOffset: " + std::to_string(shapeDataOffset));

        int64_t entityDataOffset;
        GetInt64(entityDataOffset);
        // debugLog("entityDataOffset: " + std::to_string(entityDataOffset));

        // original asserts nameoffset, unkoffsets can't be 0, and also xor asserts shapedata is not
        // 0 when shapetype is 0 or FFFFFF

        istream->seekg(start + nameOffset);
        region.name = ReadUtf16String();
        // debugLog("region.name: " + region.name);

        istream->seekg(start + unkOffsetA);
        GetInt16(intBuffer);
        // debugLog("zero check: " + std::to_string(intBuffer));

        istream->seekg(start + unkOffsetB);
        GetInt16(intBuffer);
        // debugLog("zero check: " + std::to_string(intBuffer));

        if (region.shapeType < 7 && region.shapeType != 0) {
            istream->seekg(start + shapeDataOffset);
            GetBytes(region.shapeData, GetShapeDataLength(region.shapeType));
        }

        istream->seekg(start + entityDataOffset);
        GetInt32(region.entityId);
        // debugLog("region.entityId: " + std::to_string(region.entityId));

        regions.push_back(region);
    }

    for (const auto& off : partOffsets) {
        Part part;
        istream->seekg(off);
        int64_t start = static_cast<int64_t>(istream->tellg());
        // debugLog("partOffsetstart: " + std::to_string(start), LogFormat::BoldGreen);

        int64_t descOffset;
        GetInt64(descOffset);
        // debugLog("descOffset: " + std::to_string(descOffset));

        int64_t nameOffset;
        GetInt64(nameOffset);
        // debugLog("nameOffset: " + std::to_string(nameOffset));

        GetInt32(part.instanceId);
        // debugLog("part.instanceId: " + std::to_string(part.instanceId));

        GetInt32(part.type);
        // debugLog("type: " + std::to_string(part.type));

        GetInt32(intBuffer); // type id
        // debugLog("typeid: " + std::to_string(intBuffer));

        GetInt32(part.modelIndex);
        // debugLog("part.modelIndex: " + std::to_string(part.modelIndex));

        int64_t sibOffset;
        GetInt64(sibOffset);
        // debugLog("sibOffset: " + std::to_string(sibOffset));

        GetVector3(part.position);
        // debugLog("part.position.x: " + std::to_string(part.position.x));
        // debugLog("part.position.y: " + std::to_string(part.position.y));
        // debugLog("part.position.z: " + std::to_string(part.position.z));

        GetVector3(part.rotation);
        // debugLog("part.rotation.x: " + std::to_string(part.rotation.x));
        debugLog("part.rotation.y: " + std::to_string(part.rotation.y));
        debugLog("part.rotation.z: " + std::to_string(part.rotation.z));

        GetVector3(part.scale);
        // debugLog("part.scale.x: " + std::to_string(part.scale.x));
        // debugLog("part.scale.y: " + std::to_string(part.scale.y));
        // debugLog("part.scale.z: " + std::to_string(part.scale.z));

        GetBytes(part.groupData, 96);

        GetInt32(intBuffer);
        // debugLog("zero check: " + std::to_string(intBuffer));

        int64_t entityDataOffset;
        GetInt64(entityDataOffset);
        // debugLog("entityDataOffset: " + std::to_string(entityDataOffset));

        int64_t typeDataOffset;
        GetInt64(typeDataOffset);
        // debugLog("typeDataOffset: " + std::to_string(typeDataOffset));

        int64_t gparamOffset;
        GetInt64(gparamOffset);
        // debugLog("gparamOffset: " + std::to_string(gparamOffset));

        int64_t sceneGparamOffset;
        GetInt64(sceneGparamOffset);
        // debugLog("sceneGparamOffset: " + std::to_string(sceneGparamOffset));

        // original has various asserts similar to earlier sections

        istream->seekg(start + descOffset);
        part.desc = ReadUtf16String();
        // debugLog("part.desc: " + part.desc);

        istream->seekg(start + nameOffset);
        part.name = ReadUtf16String();
        // debugLog("part.name: " + part.name);

        istream->seekg(start + sibOffset);
        part.sibPath = ReadUtf16String();
        // debugLog("part.sibPath: " + part.sibPath);

        istream->seekg(start + entityDataOffset);
        GetInt32(part.entityId);
        // debugLog("part.entityId: " + std::to_string(part.entityId));

        GetByte(part.unkE04);
        // debugLog("part.unkE04: " + std::to_string(part.unkE04));

        GetByte(part.unkE05);
        // debugLog("part.unkE05: " + std::to_string(part.unkE05));

        GetByte(part.unkE06);
        // debugLog("part.unkE06: " + std::to_string(part.unkE06));

        GetByte(part.unkE07);
        // debugLog("part.unkE07: " + std::to_string(part.unkE07));

        GetInt32(intBuffer);
        // debugLog("zero check: " + std::to_string(intBuffer));

        GetByte(part.lanternId);
        // debugLog("part.lanternId: " + std::to_string(part.lanternId));

        GetByte(part.lodParamId);
        // debugLog("part.lodParamId: " + std::to_string(part.lodParamId));

        GetByte(part.unkE0E);
        // debugLog("part.unkE0E: " + std::to_string(part.unkE0E));

        GetByte(part.unkE0F);
        // debugLog("part.unkE0F: " + std::to_string(part.unkE0F));

        if (part.type < 12) {
            istream->seekg(start + typeDataOffset);
            GetBytes(part.typeData, GetPartTypeDataLength(part.type));
        }

        bool hasGparamConfig = false;
        if (part.type == 0 || part.type == 1 || part.type == 2 || part.type == 5 ||
            part.type == 9 || part.type == 10)
            hasGparamConfig = true;

        if (hasGparamConfig) {
            istream->seekg(start + gparamOffset);
            GetBytes(part.gParamConfig, 32);
        }

        if (part.type == 5) { // only this has has scene param config
            istream->seekg(start + sceneGparamOffset);
            GetBytes(part.sceneParamConfig, 80);
        }

        parts.push_back(part);
    }

    DisambiguateNames(models);
    DisambiguateNames(regions);
    DisambiguateNames(parts);

    for (Event& event : events) {
        int pIndex = event.partIndex;
        if (pIndex >= 0 && pIndex < static_cast<int>(parts.size())) {
            event.partName = parts[pIndex].name;
        } else {
            event.partName = "";
        }

        int rIndex = event.regionIndex;
        if (rIndex >= 0 && rIndex < static_cast<int>(regions.size())) {
            event.regionName = regions[rIndex].name;
        } else {
            event.regionName = "";
        }
    }

    for (Part& part : parts) {
        int index = part.modelIndex;
        if (index >= 0 && index < static_cast<int>(models.size())) {
            part.modelName = models[index].name;
        } else {
            part.modelName = ""; // Or clear it if pIndex is -1
        }
    }

    istream.reset();
    return true;
}

void Msb::GetSectionOffsets(std::vector<int64_t>& sectionOffsets, int64_t& nextParamOffset) {
    GetInt32(version); // should be 3?
    // debugLog("version: " + std::to_string(version));

    int offsetCount;
    GetInt32(offsetCount);
    // debugLog("offsetCount: " + std::to_string(offsetCount));

    int64_t nameOffset;
    GetInt64(nameOffset);
    // debugLog("nameOffset: " + std::to_string(nameOffset));

    for (int i = 0; i < offsetCount - 1; i++) {
        int64_t entryOffset;
        GetInt64(entryOffset);
        // debugLog("entryOffset: " + std::to_string(entryOffset));
        sectionOffsets.push_back(entryOffset);
    }

    GetInt64(nextParamOffset);
    // debugLog("nextParamOffset: " + std::to_string(nextParamOffset));

    StepIn(nameOffset, *istream);
    std::string name = ReadUtf16String();
    // debugLog("name: " + name);
    StepOut(*istream);
}

bool Msb::RepackMsb(std::vector<char>& outputData) {
    ostream = std::make_unique<std::stringstream>(std::ios_base::out | std::ios_base::binary);
    std::string strBuffer;

    for (auto& model : models) {
        model.instanceCount = std::count_if(parts.begin(), parts.end(), [&model](const Part& part) {
            return part.modelName == model.name;
        });
    }

    std::unordered_map<std::string, int> modelLookup;
    for (int i = 0; i < models.size(); ++i) {
        modelLookup[models[i].name] = i;
    }

    std::unordered_map<std::string, int> partsLookup;
    for (int i = 0; i < parts.size(); ++i) {
        partsLookup[parts[i].name] = i;
    }

    std::unordered_map<std::string, int> regionsLookup;
    for (int i = 0; i < regions.size(); ++i) {
        regionsLookup[regions[i].name] = i;
    }

    for (auto& evt : events) {
        auto it = partsLookup.find(evt.partName);
        evt.partIndex = (it != partsLookup.end()) ? it->second : -1;

        auto it2 = regionsLookup.find(evt.regionName);
        evt.regionIndex = (it2 != regionsLookup.end()) ? it2->second : -1;
    }

    for (auto& part : parts) {
        auto it = modelLookup.find(part.modelName);
        part.modelIndex = (it != modelLookup.end()) ? it->second : -1;
    }

    std::sort(models.begin(), models.end(),
              [](const Model& a, const Model& b) { return a.type < b.type; });

    std::sort(events.begin(), events.end(),
              [](const Event& a, const Event& b) { return a.type < b.type; });

    std::sort(parts.begin(), parts.end(),
              [](const Part& a, const Part& b) { return a.type < b.type; });

    strBuffer = "MSB ";
    WriteStr(strBuffer, 4);

    WriteInt32(1);
    WriteInt32(0x10);
    WriteByte(false);
    WriteByte(false);
    WriteByte(1);
    WriteByte(0xFF);

    //////////////// MODELS
    WriteInt32(version);
    WriteInt32(static_cast<int>(models.size() + 1));
    ReserveBytes("ParamNameOffset", 8);
    for (int i = 0; i < models.size(); i++)
        ReserveBytes("EntryOffset" + std::to_string(i), 8);
    ReserveBytes("NextParamOffset", 8);

    FillReservedInt64("ParamNameOffset", static_cast<int64_t>(ostream->tellp()));
    WriteUtf16String("MODEL_PARAM_ST");
    PadStream(8);

    int modelCatId = 0;
    uint modelType = 100; // dummy value
    for (int i = 0; i < models.size(); i++) {
        Model& model = models[i];
        int64_t start = static_cast<int64_t>(ostream->tellp());
        FillReservedInt64("EntryOffset" + std::to_string(i), start);

        if (model.type != modelType) {
            modelCatId = 0;
            modelType = model.type;
        }

        ReserveBytes("NameOffset", 8);
        WriteInt32(model.type);
        WriteInt32(modelCatId);
        ReserveBytes("SibOffset", 8);
        WriteInt32(model.instanceCount);
        WriteInt32(0);
        WriteInt32(0);
        WriteInt32(0);

        FillReservedInt64("NameOffset", static_cast<int64_t>(ostream->tellp()) - start);
        WriteUtf16String(ReambiguateName(model.name));
        FillReservedInt64("SibOffset", static_cast<int64_t>(ostream->tellp()) - start);
        WriteUtf16String(model.sibPath);
        PadStream(8);

        modelCatId += 1;
    }

    FillReservedInt64("NextParamOffset", static_cast<int64_t>(ostream->tellp()));

    //////////////// EVENTS
    WriteInt32(version);
    WriteInt32(static_cast<int>(events.size() + 1));
    ReserveBytes("ParamNameOffset", 8);
    for (int i = 0; i < events.size(); i++)
        ReserveBytes("EntryOffset" + std::to_string(i), 8);
    ReserveBytes("NextParamOffset", 8);

    FillReservedInt64("ParamNameOffset", static_cast<int64_t>(ostream->tellp()));
    WriteUtf16String("EVENT_PARAM_ST");
    PadStream(8);

    int eventCatId = 0;
    uint eventType = 100; // dummy value
    for (int i = 0; i < events.size(); i++) {
        Event& event = events[i];
        int64_t start = static_cast<int64_t>(ostream->tellp());
        FillReservedInt64("EntryOffset" + std::to_string(i), start);

        if (event.type != eventType) {
            eventCatId = 0;
            eventType = event.type;
        }

        ReserveBytes("NameOffset", 8);
        WriteInt32(event.id);
        WriteInt32(event.type);
        WriteInt32(eventCatId);
        WriteInt32(0);

        ReserveBytes("EntityDataOffset", 8);
        ReserveBytes("TypeDataOffset", 8);
        FillReservedInt64("NameOffset", static_cast<int64_t>(ostream->tellp()) - start);
        WriteUtf16String(event.name);
        PadStream(8);

        FillReservedInt64("EntityDataOffset", static_cast<int64_t>(ostream->tellp()) - start);
        WriteInt32(event.partIndex);
        WriteInt32(event.regionIndex);
        WriteInt32(event.entityId);

        WriteByte(event.unkE0C);
        WriteByte(event.unkE0D);
        WriteByte(event.unkE0E);
        WriteByte(event.unkE0F);

        if (event.type < 18) {
            FillReservedInt64("TypeDataOffset", static_cast<int64_t>(ostream->tellp()) - start);
            ostream->write(event.typeData.data(), event.typeData.size());
        } else {
            FillReservedInt64("TypeDataOffset", 0);
        }

        eventCatId += 1;
    }

    FillReservedInt64("NextParamOffset", static_cast<int64_t>(ostream->tellp()));

    //////////////// REGIONS
    WriteInt32(version);
    WriteInt32(static_cast<int>(regions.size() + 1));
    ReserveBytes("ParamNameOffset", 8);
    for (int i = 0; i < regions.size(); i++)
        ReserveBytes("EntryOffset" + std::to_string(i), 8);
    ReserveBytes("NextParamOffset", 8);

    FillReservedInt64("ParamNameOffset", static_cast<int64_t>(ostream->tellp()));
    WriteUtf16String("POINT_PARAM_ST");
    PadStream(8);

    for (int i = 0; i < regions.size(); i++) {
        Region& region = regions[i];
        int64_t start = static_cast<int64_t>(ostream->tellp());
        FillReservedInt64("EntryOffset" + std::to_string(i), start);

        ReserveBytes("NameOffset", 8);
        WriteInt32(0);
        WriteInt32(i);
        WriteInt32(region.shapeType);
        WriteVector3(region.position);
        WriteVector3(region.rotation);
        WriteInt32(0);

        ReserveBytes("UnkOffsetA", 8);
        ReserveBytes("UnkOffsetB", 8);
        ReserveBytes("ShapeDataOffset", 8);
        ReserveBytes("EntityDataOffset", 8);

        FillReservedInt64("NameOffset", static_cast<int64_t>(ostream->tellp()) - start);
        WriteUtf16String(ReambiguateName(region.name));
        PadStream(4);

        FillReservedInt64("UnkOffsetA", static_cast<int64_t>(ostream->tellp()) - start);
        WriteInt16(0);
        PadStream(4);

        FillReservedInt64("UnkOffsetB", static_cast<int64_t>(ostream->tellp()) - start);
        WriteInt16(0);
        PadStream(8);

        if (region.shapeType < 7 && region.shapeType != 0) {
            FillReservedInt64("ShapeDataOffset", static_cast<int64_t>(ostream->tellp()) - start);
            ostream->write(region.shapeData.data(), region.shapeData.size());
        } else {
            FillReservedInt64("ShapeDataOffset", 0);
        }

        FillReservedInt64("EntityDataOffset", static_cast<int64_t>(ostream->tellp()) - start);
        WriteInt32(region.entityId);
        PadStream(8);
    }

    FillReservedInt64("NextParamOffset", static_cast<int64_t>(ostream->tellp()));

    //////////////// PARTS
    WriteInt32(version);
    WriteInt32(static_cast<int>(parts.size() + 1));
    ReserveBytes("ParamNameOffset", 8);
    for (int i = 0; i < parts.size(); i++)
        ReserveBytes("EntryOffset" + std::to_string(i), 8);
    ReserveBytes("NextParamOffset", 8);

    FillReservedInt64("ParamNameOffset", static_cast<int64_t>(ostream->tellp()));
    WriteUtf16String("PARTS_PARAM_ST");
    PadStream(8);

    int partsCatId = 0;
    uint partType = 100; // dummy value
    for (int i = 0; i < parts.size(); i++) {
        Part& part = parts[i];
        int64_t start = static_cast<int64_t>(ostream->tellp());
        FillReservedInt64("EntryOffset" + std::to_string(i), start);

        if (part.type != partType) {
            partsCatId = 0;
            partType = part.type;
        }

        ReserveBytes("DescOffset", 8);
        ReserveBytes("NameOffset", 8);
        WriteInt32(part.instanceId);
        WriteInt32(part.type);

        if (part.type > 11) {
            WriteInt32(0);
        } else {
            WriteInt32(partsCatId);
        }

        WriteInt32(part.modelIndex);
        ReserveBytes("SibOffset", 8);
        WriteVector3(part.position);
        WriteVector3(part.rotation);
        WriteVector3(part.scale);

        ostream->write(part.groupData.data(), part.groupData.size());
        WriteInt32(0);

        ReserveBytes("EntityDataOffset", 8);
        ReserveBytes("TypeDataOffset", 8);
        ReserveBytes("GparamOffset", 8);
        ReserveBytes("SceneGparamOffset", 8);

        int64_t stringsStart = static_cast<int64_t>(ostream->tellp());
        FillReservedInt64("DescOffset", static_cast<int64_t>(ostream->tellp()) - start);
        WriteUtf16String(part.desc);
        FillReservedInt64("NameOffset", static_cast<int64_t>(ostream->tellp()) - start);
        WriteUtf16String(ReambiguateName(part.name));
        FillReservedInt64("SibOffset", static_cast<int64_t>(ostream->tellp()) - start);
        WriteUtf16String(part.sibPath);

        int64_t patternPos = static_cast<int64_t>(ostream->tellp()) - stringsStart;
        if (patternPos <= 0x38) {
            std::vector<char> zeros(0x3C - patternPos, 0x00);
            ostream->write(zeros.data(), zeros.size());
        } else {
            PadStream(8);
        }

        FillReservedInt64("EntityDataOffset", static_cast<int64_t>(ostream->tellp()) - start);
        WriteInt32(part.entityId);
        WriteByte(part.unkE04);
        WriteByte(part.unkE05);
        WriteByte(part.unkE06);
        WriteByte(part.unkE07);

        WriteInt32(0);
        WriteByte(part.lanternId);
        WriteByte(part.lodParamId);
        WriteByte(part.unkE0E);
        WriteByte(part.unkE0F);

        if (part.type != 0)
            PadStream(8);

        if (part.type < 12) {
            FillReservedInt64("TypeDataOffset", static_cast<int64_t>(ostream->tellp()) - start);
            ostream->write(part.typeData.data(), part.typeData.size());
        } else {
            FillReservedInt64("TypeDataOffset", 0);
        }
        PadStream(8);

        bool hasGparamConfig = false;
        if (part.type == 0 || part.type == 1 || part.type == 2 || part.type == 5 ||
            part.type == 9 || part.type == 10)
            hasGparamConfig = true;

        if (hasGparamConfig) {
            FillReservedInt64("GparamOffset", static_cast<int64_t>(ostream->tellp()) - start);
            ostream->write(part.gParamConfig.data(), part.gParamConfig.size());
        } else {
            FillReservedInt64("GparamOffset", 0);
        }

        if (part.type == 5) {
            FillReservedInt64("SceneGparamOffset", static_cast<int64_t>(ostream->tellp()) - start);
            ostream->write(part.sceneParamConfig.data(), part.sceneParamConfig.size());
        } else {
            FillReservedInt64("SceneGparamOffset", 0);
        }

        partsCatId += 1;
    }

    FillReservedInt64("NextParamOffset", 0);

    auto& stringStream = static_cast<std::stringstream&>(*ostream);
    std::string str = stringStream.str();
    outputData = std::vector<char>(str.begin(), str.end());

    /* tests
    std::ofstream outFile("test.msb", std::ios::out | std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(outputData.data(), outputData.size());
        outFile.close();
    }
    */

    ostream.reset();
    sendLog("MSB Repacking complete\n");
    return true;
}

bool Msb::HandleConflict(std::vector<char>& mod1Data, std::vector<char>& mod2Data) {
    Msb mod1Msb = Msb(mod1Data, merger);
    Msb mod2Msb = Msb(mod2Data, merger);

    const std::vector<Model> mod1Models = mod1Msb.models;
    const std::vector<Model> mod2Models = mod2Msb.models;

    const std::vector<Event> mod1Events = mod1Msb.events;
    const std::vector<Event> mod2Events = mod2Msb.events;

    const std::vector<Region> mod1Regions = mod1Msb.regions;
    const std::vector<Region> mod2Regions = mod2Msb.regions;

    const std::vector<Part> mod1Parts = mod1Msb.parts;
    const std::vector<Part> mod2Parts = mod2Msb.parts;

    bool Success = true;

    if (!MergeCollection(models, mod1Models, mod2Models, "model"))
        return false;
    if (!MergeCollection(parts, mod1Parts, mod2Parts, "part"))
        return false;
    if (!MergeCollection(regions, mod1Regions, mod2Regions, "region"))
        return false;
    if (!MergeCollection(events, mod1Events, mod2Events, "event"))
        return false;

    return true;
}

template <typename T>
bool Msb::MergeCollection(std::vector<T>& baseItems, const std::vector<T>& mod1Items,
                          const std::vector<T>& mod2Items, const std::string& typeName) {
    // Build O(1) hash maps for instantaneous lookups
    std::unordered_map<std::string, T> mod1Map;
    for (const auto& item : mod1Items)
        mod1Map[item.Identifier()] = item;

    std::unordered_map<std::string, T> mod2Map;
    for (const auto& item : mod2Items)
        mod2Map[item.Identifier()] = item;

    for (auto it = baseItems.begin(); it != baseItems.end();) {
        T& item = *it;
        std::string key = item.Identifier();

        std::optional<T> mod1Item = GetSameItem(key, mod1Map);
        std::optional<T> mod2Item = GetSameItem(key, mod2Map);

        bool mod1Modified = mod1Item && (mod1Item.value() != item);
        bool mod2Modified = mod2Item && (mod2Item.value() != item);

        if (!mod1Item && !mod2Item) {
            it = baseItems.erase(it);
            continue;
        }

        if ((!mod1Item && !mod2Modified) || (!mod2Item && !mod1Modified)) {
            it = baseItems.erase(it);
            continue;
        }

        if (mod1Modified && mod2Modified) {
            if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                QMetaObject::invokeMethod(merger, &ModMerger::OpenPriorityDialog,
                                          Qt::BlockingQueuedConnection);
                if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                    return false;
                }
            }

            if (merger->GetModPriority() == ModMerger::ModPriority::Mod1) {
                item = mod1Item.value();
                sendLog("Unresolvable conflict in " + typeName + ": " + item.name +
                            ", using data from prioritized mod: " + merger->Mod1Name(),
                        LogFormat::Yellow);
            } else if (merger->GetModPriority() == ModMerger::ModPriority::Mod2) {
                item = mod2Item.value();
                sendLog("Unresolvable conflict in " + typeName + ": " + item.name +
                            ", using data from prioritized mod: " + merger->Mod2Name(),
                        LogFormat::Yellow);
            }
        } else if (mod1Modified && !mod2Modified) {
            item = mod1Item.value();
            sendLog("Merging modified " + typeName + ": " + item.name +
                    " from mod: " + merger->Mod1Name());
        } else if (mod2Modified && !mod1Modified) {
            item = mod2Item.value();
            sendLog("Merging modified " + typeName + ": " + item.name +
                    " from mod: " + merger->Mod2Name());
        }

        ++it;
    }

    std::unordered_set<std::string> baseIdentifiers;
    for (const auto& item : baseItems) {
        baseIdentifiers.insert(item.Identifier());
    }

    std::unordered_set<std::string> brandNewKeys;
    for (const auto& [key, _] : mod1Map) {
        if (!baseIdentifiers.contains(key))
            brandNewKeys.insert(key);
    }

    for (const auto& [key, _] : mod2Map) {
        if (!baseIdentifiers.contains(key))
            brandNewKeys.insert(key);
    }

    for (const auto& key : brandNewKeys) {
        bool inMod1 = mod1Map.contains(key);
        bool inMod2 = mod2Map.contains(key);

        if (inMod1 && inMod2) {
            const T& item1 = mod1Map[key];
            const T& item2 = mod2Map[key];

            if (item1 == item2) {
                baseItems.push_back(item1);
                sendLog("Brand new " + typeName + " added (identical in both mods): " + item1.name);
            } else {
                if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                    QMetaObject::invokeMethod(merger, &ModMerger::OpenPriorityDialog,
                                              Qt::BlockingQueuedConnection);
                    if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                        return false;
                    }
                }

                if (merger->GetModPriority() == ModMerger::ModPriority::Mod1) {
                    baseItems.push_back(item1);
                    sendLog("New asset conflict in " + typeName + ": " + item1.name +
                                ", prioritizing Mod 1 data: " + merger->Mod1Name(),
                            LogFormat::Yellow);
                } else {
                    baseItems.push_back(item2);
                    sendLog("New asset conflict in " + typeName + ": " + item2.name +
                                ", prioritizing Mod 2 data: " + merger->Mod2Name(),
                            LogFormat::Yellow);
                }
            }
        } else if (inMod1) {
            baseItems.push_back(mod1Map[key]);
            sendLog("Brand new " + typeName + " added from Mod 1: " + mod1Map[key].name);
        } else if (inMod2) {
            baseItems.push_back(mod2Map[key]);
            sendLog("Brand new " + typeName + " added from Mod 2: " + mod2Map[key].name);
        }
    }
    return true;
}

std::string Msb::ReambiguateName(const std::string& name) {
    std::regex pattern(R"( \{\d+\})");
    return std::regex_replace(name, pattern, "");
}

int Msb::GetShapeDataLength(int shapeType) {
    switch (shapeType) {
    case 1: // Circle
        return 4;
    case 2: // Sphere
        return 4;
    case 3: // Cylinder
        return 8;
    case 4: // Rectangle
        return 8;
    case 5: // Box
        return 12;
    case 6: // Composite, should not be in BB
    default:
        return 0;
    }
};

int Msb::GetPartTypeDataLength(int partType) {
    switch (partType) {
    case 0: // MapPiece
        return 8;
    case 1: // Object
        return 32;
    case 2: // Enemy
        return 64;
    case 4: // Player
        return 16;
    case 5: // Collision
        return 24;
    case 8: // NavMesh
        return 8;
    case 9: // DummyObject
        return 32;
    case 10: // DummyEnemy
        return 64;
    case 11: // ConnectCollision
        return 16;
    default:
        return 0;
    }
};

int Msb::GetEventTypeDataLength(int type) {
    switch (type) {
    case 1: // Sound
        return 8;
    case 2: // SFX
        return 8;
    case 4: // Treasure
        return 80;
    case 5: // Generator
        return 256;
    case 6: // Message
        return 8;
    case 7: // ObjAct
        return 24;
    case 8: // SpawnPoint
        return 16;
    case 9: // MapOffset
        return 16;
    case 10: // Navmesh
        return 16;
    case 11: // Environment
        return 32;
    case 13: // WindSFX
        return 16;
    case 14: // PatrolInfo
        return 80;
    case 15: // DarkLock
        return 16;
    case 16: // PlatoonInfo
        return 144;
    case 17: // MultiSummon
        return 16;
    default:
        return 0;
    }
};

Msb::~Msb() {}

} // namespace FileHelper
