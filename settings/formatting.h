// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "toml.hpp"
#include <fmt/core.h>
#include <fmt/xchar.h>

namespace toml {
template <typename TC, typename K>
std::filesystem::path find_fs_path_or(const basic_value<TC>& v, const K& ky,
                                      std::filesystem::path opt) {
    try {
        auto str = find<std::string>(v, ky);
        if (str.empty()) {
            return opt;
        }
        std::u8string u8str{(char8_t*)&str.front(), (char8_t*)&str.back() + 1};
        return std::filesystem::path{u8str};
    } catch (...) {
        return opt;
    }
}
} // namespace toml

namespace fmt {
template <typename T = std::string_view>
struct UTF {
    T data;
    
    explicit UTF(const std::u8string_view view) {
        data = view.empty() ? T{} : T{(const char*)&view.front(), (const char*)&view.back() + 1};
    }
    
    explicit UTF(const std::u8string& str) : UTF(std::u8string_view{str}) {}
};
} // namespace fmt
