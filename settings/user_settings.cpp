// SPDX-FileCopyrightText: Copyright 2026 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <map>

#include "modules/Common.h"
#include "modules/Log.h"
#include "settings/updater/BuildInfo.h"
#include "user_settings.h"

using json = nlohmann::json;

// Singleton storage
std::shared_ptr<UserSettingsImpl> UserSettingsImpl::s_instance = nullptr;
std::mutex UserSettingsImpl::s_mutex;

// Singleton
UserSettingsImpl::UserSettingsImpl() = default;

UserSettingsImpl::~UserSettingsImpl() {
    Save();
}

std::shared_ptr<UserSettingsImpl> UserSettingsImpl::GetInstance() {
    std::lock_guard lock(s_mutex);
    if (!s_instance)
        s_instance = std::make_shared<UserSettingsImpl>();
    return s_instance;
}

void UserSettingsImpl::SetInstance(std::shared_ptr<UserSettingsImpl> instance) {
    std::lock_guard lock(s_mutex);
    s_instance = std::move(instance);
}

bool UserSettingsImpl::Save() const {
    const auto path = Common::GetShadUserDir() / "users.json";
    try {
        json j;
        j["Users"] = m_userManager.GetUsers();
        j["Users"]["commit_hash"] = std::string(Build::Rev);

        std::ofstream out(path);
        if (!out) {
            LogError("Failed to open user settings for writing: " + path.string());
            return false;
        }
        out << std::setw(2) << j;
        return !out.fail();
    } catch (const std::exception& e) {
        std::string msg = "Error saving user settings: ";
        msg += e.what();
        LogError(msg);
        return false;
    }
}

bool UserSettingsImpl::Load() {
    const auto path = Common::GetShadUserDir() / "users.json";
    try {
        if (!std::filesystem::exists(path)) {
            // LOG_DEBUG(EmuSettings, "User settings file not found: {}", path.string());
            // Create default user if no file exists
            if (m_userManager.GetUsers().user.empty()) {
                m_userManager.GetUsers() = m_userManager.CreateDefaultUsers();
            }
            Save(); // Save default users
            return false;
        }

        std::ifstream in(path);
        if (!in) {
            LogError("Failed to open user settings: " + path.string());
            return false;
        }

        json j;
        in >> j;

        // Create a default Users object
        auto default_users = m_userManager.CreateDefaultUsers();

        // Convert default_users to json for merging
        json default_json;
        default_json["Users"] = default_users;

        // Merge the loaded json with defaults (preserves existing data, adds missing fields)
        if (j.contains("Users")) {
            json current = default_json["Users"];
            current.update(j["Users"]);
            m_userManager.GetUsers() = current.get<Users>();
        } else {
            m_userManager.GetUsers() = default_users;
        }

        if (m_userManager.GetUsers().commit_hash != Build::Rev) {
            Save();
        }

        // LOG_DEBUG(EmuSettings, "User settings loaded successfully");
        return true;
    } catch (const std::exception& e) {
        std::string msg = "Error loading user settings: ";
        msg += e.what();
        LogError(msg);

        // Fall back to defaults
        if (m_userManager.GetUsers().user.empty()) {
            m_userManager.GetUsers() = m_userManager.CreateDefaultUsers();
        }
        return false;
    }
}
