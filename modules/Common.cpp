// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QCoreApplication>
#include <QProcessEnvironment>

#include "Common.h"
#include "scope_exit.h"
#include "settings/config.h"

#ifdef Q_OS_MAC
#include <CoreFoundation/CFBundle.h>
#include <dlfcn.h>
#include <sys/param.h>
#endif

#ifndef MAX_PATH
#ifdef _WIN32
#include <Shlobj.h>
#include <windows.h>
// This is the maximum number of UTF-16 code units permissible in Windows file paths
#define MAX_PATH 260
#else
// This is the maximum number of UTF-8 code units permissible in all other OSes' file paths
#define MAX_PATH 1024
#endif
#endif

namespace Common {

std::string game_serial;
std::filesystem::path installPath;
std::filesystem::path installUpdatePath;
std::filesystem::path shadPs4Executable;

const char VERSION[] = "Release12.04";

std::filesystem::path GetCurrentPath(bool getLinuxFileName) {
    std::filesystem::path currentPath;
#if defined(__APPLE__)
    currentPath = Common::GetBundleParentDirectory();

#elif defined(linux)
    char* appdir_env = std::getenv("APPDIR");
    QString appPath;

    if (appdir_env != nullptr) {
        appPath = QProcessEnvironment::systemEnvironment().value(QStringLiteral("APPIMAGE"));
        getLinuxFileName ? currentPath = Common::PathFromQString(appPath)
                         : currentPath = Common::PathFromQString(appPath).parent_path();
    } else {
        getLinuxFileName ? appPath = QCoreApplication::applicationFilePath()
                         : appPath = QCoreApplication::applicationDirPath();
        currentPath = Common::PathFromQString(appPath);
    }

#else
    currentPath = std::filesystem::current_path();
#endif
    return currentPath;
}

std::filesystem::path GetShadUserDir() {
    std::filesystem::path userPath = Config::PortableFolderinLauncherFolder
                                         ? Common::GetCurrentPath()
                                         : Common::shadPs4Executable.parent_path();
    auto user_dir = userPath / "user";

    if (!std::filesystem::exists(user_dir)) {
#ifdef __APPLE__
        user_dir =
            std::filesystem::path(getenv("HOME")) / "Library" / "Application Support" / "shadPS4";
#elif defined(__linux__)
        const char* xdg_data_home = getenv("XDG_DATA_HOME");
        if (xdg_data_home != nullptr && strlen(xdg_data_home) > 0) {
            user_dir = std::filesystem::path(xdg_data_home) / "shadPS4";
        } else {
            user_dir = std::filesystem::path(getenv("HOME")) / ".local" / "share" / "shadPS4";
        }
#elif _WIN32
        TCHAR appdata[MAX_PATH] = {0};
        SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appdata);

        // Releases older than 0.7.0 do have global user folder
        if (!Config::isReleaseOlder(7))
            user_dir = std::filesystem::path(appdata) / "shadPS4";
#endif
    }
    return user_dir;
}

std::filesystem::path GetBBLFilesPath() {
    std::filesystem::path path;

#if defined(__APPLE__)
    path = std::filesystem::path(getenv("HOME")) / "Library" / "Application Support" / "BBLauncher";
#elif defined(__linux__)
    const char* xdg_data_home = getenv("XDG_DATA_HOME");
    if (xdg_data_home != nullptr && strlen(xdg_data_home) > 0) {
        path = std::filesystem::path(xdg_data_home) / "BBLauncher";
    } else {
        path = std::filesystem::path(getenv("HOME")) / ".local" / "share" / "BBLauncher";
    }
#else
    path = std::filesystem::current_path() / "BBLauncher";
#endif

    return path;
}

std::filesystem::path GetSaveDir() {
    std::filesystem::path path = Config::externalSaveDir;

    // Releases older than 0.6.0 do have have configurable save folder
    bool noConfigurableSaveFolder = Config::isReleaseOlder(6);
    if (Config::externalSaveDir == "" || noConfigurableSaveFolder) {
        path = Common::GetShadUserDir() / "savedata";
    }

    return path;
}

void PathToQString(QString& result, const std::filesystem::path& path) {
#ifdef _WIN32
    result = QString::fromStdWString(path.wstring());
#else
    result = QString::fromStdString(path.string());
#endif
}

std::filesystem::path PathFromQString(const QString& path) {
#ifdef _WIN32
    return std::filesystem::path(path.toStdWString());
#else
    return std::filesystem::path(path.toStdString());
#endif
}

std::string PathToU8(const std::filesystem::path& path) {
    const auto u8_string = path.u8string();
    return std::string{u8_string.begin(), u8_string.end()};
}

#ifdef __APPLE__
using IsTranslocatedURLFunc = Boolean (*)(CFURLRef path, bool* isTranslocated,
                                          CFErrorRef* __nullable error);
using CreateOriginalPathForURLFunc = CFURLRef __nullable (*)(CFURLRef translocatedPath,
                                                             CFErrorRef* __nullable error);

static CFURLRef UntranslocateBundlePath(const CFURLRef bundle_path) {
    if (void* security_handle =
            dlopen("/System/Library/Frameworks/Security.framework/Security", RTLD_LAZY)) {
        SCOPE_EXIT {
            dlclose(security_handle);
        };

        const auto IsTranslocatedURL = reinterpret_cast<IsTranslocatedURLFunc>(
            dlsym(security_handle, "SecTranslocateIsTranslocatedURL"));
        const auto CreateOriginalPathForURL = reinterpret_cast<CreateOriginalPathForURLFunc>(
            dlsym(security_handle, "SecTranslocateCreateOriginalPathForURL"));

        bool is_translocated = false;
        if (IsTranslocatedURL && CreateOriginalPathForURL &&
            IsTranslocatedURL(bundle_path, &is_translocated, nullptr) && is_translocated) {
            return CreateOriginalPathForURL(bundle_path, nullptr);
        }
    }
    return nullptr;
}

std::filesystem::path GetBundleParentDirectory() {
    if (CFBundleRef bundle_ref = CFBundleGetMainBundle()) {
        if (CFURLRef bundle_url_ref = CFBundleCopyBundleURL(bundle_ref)) {
            SCOPE_EXIT {
                CFRelease(bundle_url_ref);
            };

            CFURLRef untranslocated_url_ref = UntranslocateBundlePath(bundle_url_ref);
            SCOPE_EXIT {
                if (untranslocated_url_ref) {
                    CFRelease(untranslocated_url_ref);
                }
            };

            char app_bundle_path[MAXPATHLEN];
            if (CFURLGetFileSystemRepresentation(
                    untranslocated_url_ref ? untranslocated_url_ref : bundle_url_ref, true,
                    reinterpret_cast<u8*>(app_bundle_path), sizeof(app_bundle_path))) {
                std::filesystem::path bundle_path{app_bundle_path};
                return bundle_path.parent_path();
            }
        }
    }
    return std::filesystem::current_path();
}

#endif

} // namespace Common
