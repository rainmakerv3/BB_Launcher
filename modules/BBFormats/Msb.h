// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "BBFormats.h"

namespace FileHelper {

class Msb : public BBFormat {
    Q_OBJECT

    struct Model {
        std::string name; // unique identifier, disambiguated
        std::string sibPath;
        int instanceCount;
        uint type;

        std::string Identifier() const {
            return name;
        }

        bool operator==(const Model&) const = default;
    };

    struct Event {
        std::string name;
        int id; // unique identifier
        uint type;
        std::string partName;
        int partIndex;
        std::string regionName;
        int regionIndex;
        int entityId;
        int unkE0C;
        int unkE0D;
        int unkE0E;
        int unkE0F;
        std::vector<char> typeData = {};

        std::string Identifier() const {
            return std::to_string(id);
        }

        // part and region index removed from equality
        bool operator==(const Event& other) const {
            return std::tie(name, id, type, partName, regionName, entityId, unkE0C, unkE0D, unkE0E,
                            unkE0F, typeData) ==
                   std::tie(other.name, other.id, other.type, other.partName, other.regionName,
                            other.entityId, other.unkE0C, other.unkE0D, other.unkE0E, other.unkE0F,
                            other.typeData);
        }
    };

    struct Region {
        std::string name;
        Vector3 position;
        Vector3 rotation;
        int entityId;
        uint shapeType;

        std::vector<char> shapeData = {};

        std::string Identifier() const {
            return name;
        }

        bool operator==(const Region&) const = default;
    };

    struct Part {         // so tedious T_T
        std::string name; // unique identifier, disambiguated
        std::string desc; // can be empty
        uint type;
        int instanceId;
        std::string modelName;
        int modelIndex;
        std::string sibPath;
        Vector3 position;
        Vector3 rotation;
        Vector3 scale;
        std::vector<char> groupData = {}; // drawg/display/backread groups
        int entityId;
        int unkE04;
        int unkE05;
        int unkE06;
        int unkE07;
        int lanternId;
        int lodParamId;
        int unkE0E;
        int unkE0F;
        std::vector<char> typeData = {};
        std::vector<char> gParamConfig = {};
        std::vector<char> sceneParamConfig = {};

        std::string Identifier() const {
            return name;
        }

        // remove model index removed from equality
        bool operator==(const Part& other) const {
            return std::tie(name, desc, type, instanceId, modelName, sibPath, position, rotation,
                            scale, groupData, entityId, unkE04, unkE05, unkE06, unkE07, lanternId,
                            lodParamId, unkE0E, unkE0F, typeData, gParamConfig, sceneParamConfig) ==
                   std::tie(other.name, other.desc, other.type, other.instanceId, other.modelName,
                            other.sibPath, other.position, other.rotation, other.scale,
                            other.groupData, other.entityId, other.unkE04, other.unkE05,
                            other.unkE06, other.unkE07, other.lanternId, other.lodParamId,
                            other.unkE0E, other.unkE0F, other.typeData, other.gParamConfig,
                            other.sceneParamConfig);
        }
    };

public:
    explicit Msb(std::vector<char>& data, ModMerger* parent);
    ~Msb() override;

    bool ReadMsb(std::vector<char>& data);
    bool RepackMsb(std::vector<char>& outputData);
    bool HandleConflict(std::vector<char>& mod1Data, std::vector<char>& mod2Data);

private:
    template <typename T>
    std::optional<T> GetSameItem(const std::string& key,
                                 const std::unordered_map<std::string, T>& lookupMap) {
        auto it = lookupMap.find(key);
        if (it != lookupMap.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    template <typename T>
    static void DisambiguateNames(std::vector<T>& entries, const std::string& className = "") {
        bool ambiguous;
        do {
            ambiguous = false;
            std::unordered_map<std::string, int> nameCounts;
            nameCounts[""] = 0;

            for (auto& entry : entries) {
                std::string name = entry.name;

                if (nameCounts.find(name) == nameCounts.end() && !name.empty()) {
                    nameCounts[name] = 1;
                } else {
                    ambiguous = true;
                    nameCounts[name]++;
                    entry.name = className + name + " {" + std::to_string(nameCounts[name]) + "}";
                }
            }
        } while (ambiguous);
    }

    template <typename T>
    bool MergeCollection(std::vector<T>& baseItems, const std::vector<T>& mod1Items,
                         const std::vector<T>& mod2Items, const std::string& typeName);

    std::string ReambiguateName(const std::string& name);
    void GetSectionOffsets(std::vector<int64_t>& sectionOffsets, int64_t& nextParamOffet);

    int GetShapeDataLength(int shapeType);
    int GetPartTypeDataLength(int partType);
    int GetEventTypeDataLength(int type);

    int version;
    std::vector<Model> models;
    std::vector<Part> parts;
    std::vector<Event> events;
    std::vector<Region> regions; // Also called points
};

} // namespace FileHelper
