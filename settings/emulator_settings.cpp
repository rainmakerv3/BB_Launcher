// SPDX-FileCopyrightText: Copyright 2025-2026 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <fstream>
#include <iomanip>
#include <iostream>

#include "config.h"
#include "emulator_settings.h"

using json = nlohmann::json;

std::shared_ptr<EmulatorSettings> EmulatorSettings::s_instance = nullptr;
std::mutex EmulatorSettings::s_mutex;

namespace nlohmann {
template <>
struct adl_serializer<std::filesystem::path> {
    static void to_json(json& j, const std::filesystem::path& p) {
        j = p.string();
    }
    static void from_json(const json& j, std::filesystem::path& p) {
        p = j.get<std::string>();
    }
};
} // namespace nlohmann

// --------------------
// Print summary
// --------------------
void EmulatorSettings::PrintChangedSummary(const std::vector<std::string>& changed) {
    if (changed.empty()) {
        std::cout << "[Settings] No game-specific overrides applied\n";
        return;
    }
    std::cout << "[Settings] Game-specific overrides applied:\n";
    for (const auto& k : changed)
        std::cout << "    * " << k << "\n";
}

// --------------------
// ctor/dtor + singleton
// --------------------
EmulatorSettings::EmulatorSettings() {
    // Load();
}
EmulatorSettings::~EmulatorSettings() {
    Save();
}

std::shared_ptr<EmulatorSettings> EmulatorSettings::GetInstance() {
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_instance)
        s_instance = std::make_shared<EmulatorSettings>();
    return s_instance;
}

void EmulatorSettings::SetInstance(std::shared_ptr<EmulatorSettings> instance) {
    std::lock_guard<std::mutex> lock(s_mutex);
    s_instance = instance;
}

// --------------------
// General helpers
// --------------------
bool EmulatorSettings::AddGameInstallDir(const std::filesystem::path& dir, bool enabled) {
    for (const auto& d : m_general.install_dirs.value)
        if (d.path == dir)
            return false;
    m_general.install_dirs.value.push_back({dir, enabled});
    return true;
}

std::vector<std::filesystem::path> EmulatorSettings::GetGameInstallDirs() const {
    std::vector<std::filesystem::path> out;
    for (const auto& d : m_general.install_dirs.value)
        if (d.enabled)
            out.push_back(d.path);
    return out;
}

const std::vector<GameInstallDir>& EmulatorSettings::GetAllGameInstallDirs() const {
    return m_general.install_dirs.value;
}

void EmulatorSettings::SetAllGameInstallDirs(const std::vector<GameInstallDir>& dirs) {
    m_general.install_dirs.value = dirs;
}

void EmulatorSettings::RemoveGameInstallDir(const std::filesystem::path& dir) {
    auto iterator =
        std::find_if(m_general.install_dirs.value.begin(), m_general.install_dirs.value.end(),
                     [&dir](const GameInstallDir& install_dir) { return install_dir.path == dir; });
    if (iterator != m_general.install_dirs.value.end()) {
        m_general.install_dirs.value.erase(iterator);
    }
}

void EmulatorSettings::SetGameInstallDirEnabled(const std::filesystem::path& dir, bool enabled) {
    auto iterator =
        std::find_if(m_general.install_dirs.value.begin(), m_general.install_dirs.value.end(),
                     [&dir](const GameInstallDir& install_dir) { return install_dir.path == dir; });
    if (iterator != m_general.install_dirs.value.end()) {
        iterator->enabled = enabled;
    }
}

void EmulatorSettings::SetGameInstallDirs(const std::vector<std::filesystem::path>& dirs_config) {
    m_general.install_dirs.value.clear();
    for (const auto& dir : dirs_config) {
        m_general.install_dirs.value.push_back({dir, true});
    }
}

const std::vector<bool> EmulatorSettings::GetGameInstallDirsEnabled() {
    std::vector<bool> enabled_dirs;
    for (const auto& dir : m_general.install_dirs.value) {
        enabled_dirs.push_back(dir.enabled);
    }
    return enabled_dirs;
}

std::filesystem::path EmulatorSettings::GetHomeDir() {
    if (m_general.home_dir.value.empty()) {
        return Common::GetShadUserDir() / "home";
    }
    return m_general.home_dir.value;
}

void EmulatorSettings::SetHomeDir(const std::filesystem::path& dir) {
    m_general.home_dir.value = dir;
}

std::filesystem::path EmulatorSettings::GetSysModulesDir() {
    if (m_general.sys_modules_dir.value.empty()) {
        return Common::GetShadUserDir() / "sys_modules";
    }
    return m_general.sys_modules_dir.value;
}

void EmulatorSettings::SetSysModulesDir(const std::filesystem::path& dir) {
    m_general.sys_modules_dir.value = dir;
}

// --------------------
// Save
// --------------------
bool EmulatorSettings::Save(const std::string& serial) const {
    try {
        if (!serial.empty()) {
            const std::filesystem::path cfgDir = Common::GetShadUserDir() / "custom_configs";
            std::filesystem::create_directories(cfgDir);
            const std::filesystem::path path = cfgDir / (serial + ".json");

            json j = json::object();

            // Only write overrideable fields for each group
            json generalObj = json::object();
            for (auto& item : m_general.GetOverrideableFields()) {
                json whole = m_general;
                if (whole.contains(item.key))
                    generalObj[item.key] = whole[item.key];
            }
            j["General"] = generalObj;

            // Debug
            json debugObj = json::object();
            for (auto& item : m_debug.GetOverrideableFields()) {
                json whole = m_debug;
                if (whole.contains(item.key))
                    debugObj[item.key] = whole[item.key];
            }
            j["Debug"] = debugObj;

            // Input
            json inputObj = json::object();
            for (auto& item : m_input.GetOverrideableFields()) {
                json whole = m_input;
                if (whole.contains(item.key))
                    inputObj[item.key] = whole[item.key];
            }
            j["Input"] = inputObj;

            // Audio
            json audioObj = json::object();
            for (auto& item : m_audio.GetOverrideableFields()) {
                json whole = m_audio;
                if (whole.contains(item.key))
                    audioObj[item.key] = whole[item.key];
            }
            j["Audio"] = audioObj;

            // GPU
            json gpuObj = json::object();
            for (auto& item : m_gpu.GetOverrideableFields()) {
                json whole = m_gpu;
                if (whole.contains(item.key))
                    gpuObj[item.key] = whole[item.key];
            }
            j["GPU"] = gpuObj;

            // Vulkan
            json vulkanObj = json::object();
            for (auto& item : m_vulkan.GetOverrideableFields()) {
                json whole = m_vulkan;
                if (whole.contains(item.key))
                    vulkanObj[item.key] = whole[item.key];
            }
            j["Vulkan"] = vulkanObj;

            std::ofstream out(path);
            if (!out.is_open()) {
                std::cerr << "Failed to open file for writing: " << path << std::endl;
                return false;
            }
            out << std::setw(4) << j;
            out.flush();
            if (out.fail()) {
                std::cerr << "Failed to write settings to: " << path << std::endl;
                return false;
            }
            return true;
        } else {
            const std::filesystem::path path = Common::GetShadUserDir() / "config.json";
            json j;
            j["General"] = m_general;
            j["Debug"] = m_debug;
            j["Input"] = m_input;
            j["Audio"] = m_audio;
            j["GPU"] = m_gpu;
            j["Vulkan"] = m_vulkan;
            // j["Users"] = m_userManager.GetUsers();

            std::ofstream out(path);
            if (!out.is_open()) {
                std::cerr << "Failed to open file for writing: " << path << std::endl;
                return false;
            }
            out << std::setw(4) << j;
            out.flush();
            if (out.fail()) {
                std::cerr << "Failed to write settings to: " << path << std::endl;
                return false;
            }
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saving settings: " << e.what() << std::endl;
        return false;
    }
}

// --------------------
// Load
// --------------------
bool EmulatorSettings::Load(const std::string& serial) {
    try {
        // If serial is empty, load ONLY global settings
        if (serial.empty()) {
            const std::filesystem::path userDir = Common::GetShadUserDir();
            const std::filesystem::path configPath = userDir / "config.json";

            std::cout << "[EmulatorSettings] Loading global settings from: " << configPath
                      << std::endl;

            // Load global config if exists
            if (std::ifstream globalIn{configPath}; globalIn.good()) {
                json gj;
                globalIn >> gj;

                std::cout << "[EmulatorSettings] Global config JSON size: " << gj.size()
                          << std::endl;

                if (gj.contains("General")) {
                    json current = m_general;
                    current.update(gj.at("General"));
                    m_general = current.get<GeneralSettings>();
                    std::cout << "[EmulatorSettings] Loaded General settings" << std::endl;
                }
                if (gj.contains("Debug")) {
                    json current = m_debug;
                    current.update(gj.at("Debug"));
                    m_debug = current.get<DebugSettings>();
                    std::cout << "[EmulatorSettings] Loaded Debug settings" << std::endl;
                }
                if (gj.contains("Input")) {
                    json current = m_input;
                    current.update(gj.at("Input"));
                    m_input = current.get<InputSettings>();
                    std::cout << "[EmulatorSettings] Loaded Input settings" << std::endl;
                }
                if (gj.contains("Audio")) {
                    json current = m_audio;
                    current.update(gj.at("Audio"));
                    m_audio = current.get<AudioSettings>();
                    std::cout << "[EmulatorSettings] Loaded Audio settings" << std::endl;
                }
                if (gj.contains("GPU")) {
                    json current = m_gpu;
                    current.update(gj.at("GPU"));
                    m_gpu = current.get<GPUSettings>();
                    std::cout << "[EmulatorSettings] Loaded GPU settings" << std::endl;
                }
                if (gj.contains("Vulkan")) {
                    json current = m_vulkan;
                    current.update(gj.at("Vulkan"));
                    m_vulkan = current.get<VulkanSettings>();
                    std::cout << "[EmulatorSettings] Loaded Vulkan settings" << std::endl;
                }
                if (gj.contains("Users")) {
                    // TODO
                    // m_userManager.GetUsers() = gj.at("Users").get<Users>();
                    std::cout << "[EmulatorSettings] Loaded Users" << std::endl;
                }
            } else {
                std::cout << "[EmulatorSettings] Global config not found, setting defaults"
                          << std::endl;
                SetDefaultValues();
                // TODO
                // ensure a default user exists
                // if (m_userManager.GetUsers().user.empty())
                //  m_userManager.GetUsers().user = m_userManager.CreateDefaultUser();
                Save();
            }

            return true;
        }
        // If serial is provided, ONLY apply game-specific overrides
        // WITHOUT reloading global settings!
        else {
            std::cout << "[EmulatorSettings] Applying game-specific overrides for: " << serial
                      << std::endl;

            const std::filesystem::path gamePath =
                Common::GetShadUserDir() / "custom_configs" / (serial + ".json");

            std::cout << "[EmulatorSettings] Game config path: " << gamePath << std::endl;

            if (!std::filesystem::exists(gamePath)) {
                std::cout << "[EmulatorSettings] No game-specific config found" << std::endl;
                return false;
            }

            std::ifstream in(gamePath);
            if (!in.is_open()) {
                std::cerr << "[EmulatorSettings] Failed to open game config file" << std::endl;
                return false;
            }

            json gj;
            in >> gj;

            std::cout << "[EmulatorSettings] Game config JSON: " << gj.dump(2) << std::endl;

            std::vector<std::string> changed;

            if (gj.contains("General")) {
                std::cout << "[EmulatorSettings] Applying General overrides" << std::endl;
                ApplyGroupOverrides<GeneralSettings>(m_general, gj.at("General"), changed);
            }
            if (gj.contains("Debug")) {
                std::cout << "[EmulatorSettings] Applying Debug overrides" << std::endl;
                ApplyGroupOverrides<DebugSettings>(m_debug, gj.at("Debug"), changed);
            }
            if (gj.contains("Input")) {
                std::cout << "[EmulatorSettings] Applying Input overrides" << std::endl;
                ApplyGroupOverrides<InputSettings>(m_input, gj.at("Input"), changed);
            }
            if (gj.contains("Audio")) {
                std::cout << "[EmulatorSettings] Applying Audio overrides" << std::endl;
                ApplyGroupOverrides<AudioSettings>(m_audio, gj.at("Audio"), changed);
            }
            if (gj.contains("GPU")) {
                std::cout << "[EmulatorSettings] Applying GPU overrides" << std::endl;
                ApplyGroupOverrides<GPUSettings>(m_gpu, gj.at("GPU"), changed);

                // Debug: Print specific GPU values
                auto gpuJson = gj["GPU"];
                if (gpuJson.contains("fsr_enabled")) {
                    std::cout << "[EmulatorSettings] GPU/fsr_enabled JSON: "
                              << gpuJson["fsr_enabled"].dump() << std::endl;
                }
                if (gpuJson.contains("rcas_enabled")) {
                    std::cout << "[EmulatorSettings] GPU/rcas_enabled JSON: "
                              << gpuJson["rcas_enabled"].dump() << std::endl;
                }
            }
            if (gj.contains("Vulkan")) {
                std::cout << "[EmulatorSettings] Applying Vulkan overrides" << std::endl;
                ApplyGroupOverrides<VulkanSettings>(m_vulkan, gj.at("Vulkan"), changed);
            }

            PrintChangedSummary(changed);

            // Debug: Print current values after applying overrides
            std::cout << "[EmulatorSettings] After applying overrides:" << std::endl;
            std::cout << "  FSR Enabled: " << IsFsrEnabled() << std::endl;
            std::cout << "  RCAS Enabled: " << IsRcasEnabled() << std::endl;
            std::cout << "  Volume: " << GetVolumeSlider() << std::endl;
            std::cout << "  Show Splash: " << IsShowSplash() << std::endl;

            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading settings: " << e.what() << std::endl;
        return false;
    }
}

void EmulatorSettings::SetDefaultValues() {
    m_general = GeneralSettings{};
    m_debug = DebugSettings{};
    m_input = InputSettings{};
    m_audio = AudioSettings{};
    m_gpu = GPUSettings{};
    m_vulkan = VulkanSettings{};
}

std::vector<std::string> EmulatorSettings::GetAllOverrideableKeys() const {
    std::vector<std::string> keys;

    auto addKeys = [&keys](const std::vector<OverrideItem>& items) {
        for (const auto& item : items) {
            keys.push_back(item.key);
        }
    };

    addKeys(m_general.GetOverrideableFields());
    addKeys(m_debug.GetOverrideableFields());
    addKeys(m_input.GetOverrideableFields());
    addKeys(m_audio.GetOverrideableFields());
    addKeys(m_gpu.GetOverrideableFields());
    addKeys(m_vulkan.GetOverrideableFields());

    return keys;
}
