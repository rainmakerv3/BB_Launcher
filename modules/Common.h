// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <bit>
#include <concepts>
#include <filesystem>
#include <QString>

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

namespace Common {

void PathToQString(QString& result, const std::filesystem::path& path);
std::filesystem::path PathFromQString(const QString& path);
std::string PathToU8(const std::filesystem::path& path);
std::filesystem::path GetShadUserDir();
std::filesystem::path GetCurrentPath();
std::filesystem::path GetBBLFilesPath();

extern std::string game_serial;
extern std::filesystem::path installPath;
extern std::filesystem::path installUpdatePath;
extern std::filesystem::path SaveDir;
extern std::filesystem::path shadPs4Executable;

extern const char VERSION[];

const std::filesystem::path ModPath = GetBBLFilesPath() / "Mods";
std::filesystem::path GetBundleParentDirectory();

// Endian.h
template <typename T>
using NativeEndian = T;

template <std::integral T>
class SwappedEndian {
public:
    const T& Raw() const {
        return data;
    }

    T Swap() const {
        return std::byteswap(data);
    }

    void FromRaw(const T& value) {
        data = value;
    }

    void FromSwap(const T& value) {
        data = std::byteswap(value);
    }

    operator const T() const {
        return Swap();
    }

    template <typename T1>
    explicit operator const SwappedEndian<T1>() const {
        SwappedEndian<T1> res;
        if (sizeof(T1) < sizeof(T)) {
            res.FromRaw(Raw() >> ((sizeof(T) - sizeof(T1)) * 8));
        } else if (sizeof(T1) > sizeof(T)) {
            res.FromSwap(Swap());
        } else {
            res.FromRaw(Raw());
        }
        return res;
    }

    SwappedEndian<T>& operator=(const T& right) {
        FromSwap(right);
        return *this;
    }
    SwappedEndian<T>& operator=(const SwappedEndian<T>& right) = default;

    template <typename T1>
    SwappedEndian<T>& operator+=(T1 right) {
        return *this = T(*this) + right;
    }
    template <typename T1>
    SwappedEndian<T>& operator-=(T1 right) {
        return *this = T(*this) - right;
    }
    template <typename T1>
    SwappedEndian<T>& operator*=(T1 right) {
        return *this = T(*this) * right;
    }
    template <typename T1>
    SwappedEndian<T>& operator/=(T1 right) {
        return *this = T(*this) / right;
    }
    template <typename T1>
    SwappedEndian<T>& operator%=(T1 right) {
        return *this = T(*this) % right;
    }
    template <typename T1>
    SwappedEndian<T>& operator&=(T1 right) {
        return *this = T(*this) & right;
    }
    template <typename T1>
    SwappedEndian<T>& operator|=(T1 right) {
        return *this = T(*this) | right;
    }
    template <typename T1>
    SwappedEndian<T>& operator^=(T1 right) {
        return *this = T(*this) ^ right;
    }
    template <typename T1>
    SwappedEndian<T>& operator<<=(T1 right) {
        return *this = T(*this) << right;
    }
    template <typename T1>
    SwappedEndian<T>& operator>>=(T1 right) {
        return *this = T(*this) >> right;
    }

    template <typename T1>
    SwappedEndian<T>& operator+=(const SwappedEndian<T1>& right) {
        return *this = Swap() + right.Swap();
    }
    template <typename T1>
    SwappedEndian<T>& operator-=(const SwappedEndian<T1>& right) {
        return *this = Swap() - right.Swap();
    }
    template <typename T1>
    SwappedEndian<T>& operator*=(const SwappedEndian<T1>& right) {
        return *this = Swap() * right.Swap();
    }
    template <typename T1>
    SwappedEndian<T>& operator/=(const SwappedEndian<T1>& right) {
        return *this = Swap() / right.Swap();
    }
    template <typename T1>
    SwappedEndian<T>& operator%=(const SwappedEndian<T1>& right) {
        return *this = Swap() % right.Swap();
    }
    template <typename T1>
    SwappedEndian<T>& operator&=(const SwappedEndian<T1>& right) {
        return *this = Raw() & right.Raw();
    }
    template <typename T1>
    SwappedEndian<T>& operator|=(const SwappedEndian<T1>& right) {
        return *this = Raw() | right.Raw();
    }
    template <typename T1>
    SwappedEndian<T>& operator^=(const SwappedEndian<T1>& right) {
        return *this = Raw() ^ right.Raw();
    }

    template <typename T1>
    SwappedEndian<T> operator&(const SwappedEndian<T1>& right) const {
        return SwappedEndian<T>{Raw() & right.Raw()};
    }
    template <typename T1>
    SwappedEndian<T> operator|(const SwappedEndian<T1>& right) const {
        return SwappedEndian<T>{Raw() | right.Raw()};
    }
    template <typename T1>
    SwappedEndian<T> operator^(const SwappedEndian<T1>& right) const {
        return SwappedEndian<T>{Raw() ^ right.Raw()};
    }

    template <typename T1>
    bool operator==(T1 right) const {
        return (T1)Swap() == right;
    }
    template <typename T1>
    bool operator!=(T1 right) const {
        return !(*this == right);
    }
    template <typename T1>
    bool operator>(T1 right) const {
        return (T1)Swap() > right;
    }
    template <typename T1>
    bool operator<(T1 right) const {
        return (T1)Swap() < right;
    }
    template <typename T1>
    bool operator>=(T1 right) const {
        return (T1)Swap() >= right;
    }
    template <typename T1>
    bool operator<=(T1 right) const {
        return (T1)Swap() <= right;
    }

    template <typename T1>
    bool operator==(const SwappedEndian<T1>& right) const {
        return Raw() == right.Raw();
    }
    template <typename T1>
    bool operator!=(const SwappedEndian<T1>& right) const {
        return !(*this == right);
    }
    template <typename T1>
    bool operator>(const SwappedEndian<T1>& right) const {
        return (T1)Swap() > right.Swap();
    }
    template <typename T1>
    bool operator<(const SwappedEndian<T1>& right) const {
        return (T1)Swap() < right.Swap();
    }
    template <typename T1>
    bool operator>=(const SwappedEndian<T1>& right) const {
        return (T1)Swap() >= right.Swap();
    }
    template <typename T1>
    bool operator<=(const SwappedEndian<T1>& right) const {
        return (T1)Swap() <= right.Swap();
    }

    SwappedEndian<T> operator++(int) {
        SwappedEndian<T> res = *this;
        *this += 1;
        return res;
    }
    SwappedEndian<T> operator--(int) {
        SwappedEndian<T> res = *this;
        *this -= 1;
        return res;
    }
    SwappedEndian<T>& operator++() {
        *this += 1;
        return *this;
    }
    SwappedEndian<T>& operator--() {
        *this -= 1;
        return *this;
    }

private:
    T data;
};

template <typename T>
using BigEndian =
    std::conditional_t<std::endian::native == std::endian::big, NativeEndian<T>, SwappedEndian<T>>;

} // namespace Common

using u32_be = Common::BigEndian<u32>;
using u64_be = Common::BigEndian<u64>;
