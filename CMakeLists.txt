cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(APPLE)
    enable_language(OBJC)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 14)
endif()

project(BB_Launcher LANGUAGES CXX)

option(FORCE_UAC "Requires running as Admin on Windows" ON)

# First, determine whether to use CMAKE_OSX_ARCHITECTURES or CMAKE_SYSTEM_PROCESSOR.
if (APPLE AND CMAKE_OSX_ARCHITECTURES)
    set(BASE_ARCHITECTURE "${CMAKE_OSX_ARCHITECTURES}")
else()
    set(BASE_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
endif()

# Next, match common architecture strings down to a known common value.
if (BASE_ARCHITECTURE MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    set(ARCHITECTURE "x86_64")
elseif (BASE_ARCHITECTURE MATCHES "(aarch64)|(AARCH64)|(arm64)|(ARM64)")
    set(ARCHITECTURE "arm64")
else()
    message(FATAL_ERROR "Unsupported CPU architecture: ${BASE_ARCHITECTURE}")
endif()

if (APPLE AND ARCHITECTURE STREQUAL "x86_64" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    # Exclude ARM homebrew path to avoid conflicts when cross compiling.
    list(APPEND CMAKE_IGNORE_PREFIX_PATH "/opt/homebrew")

    # Need to reconfigure pkg-config to use the right architecture library paths.
    # It's not ideal to override these but otherwise the build breaks just by having pkg-config installed.
    set(ENV{PKG_CONFIG_DIR} "")
    set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_SYSROOT}/usr/lib/pkgconfig:${CMAKE_SYSROOT}/usr/share/pkgconfig:${CMAKE_SYSROOT}/usr/local/lib/pkgconfig:${CMAKE_SYSROOT}/usr/local/share/pkgconfig")
    set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})
endif()

# Setup a custom clang-format target (if clang-format can be found) that will run
# against all the src files. This should be used before making a pull request.

if (CLANG_FORMAT)
    set(SRCS ${PROJECT_SOURCE_DIR)
    set(CCOMMENT "Running clang format against all the .h and .cpp files in src/")
    if (WIN32)
        add_custom_target(clang-format
            COMMAND powershell.exe -Command "Get-ChildItem '${SRCS}/*' -Include *.cpp,*.h,*.mm -Recurse | Foreach {&'${CLANG_FORMAT}' -i $_.fullname}"
            COMMENT ${CCOMMENT})
    else()
        add_custom_target(clang-format
            COMMAND find ${SRCS} -iname *.h -o -iname *.cpp -o -iname *.mm | xargs ${CLANG_FORMAT} -i
            COMMENT ${CCOMMENT})
    endif()
    unset(SRCS)
    unset(CCOMMENT)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# generate git revision information
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/settings/updater/cmake-modules/")
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REF_SPEC GIT_REV)
git_describe(GIT_DESC --always --long --dirty)
git_branch_name(GIT_BRANCH)
string(TIMESTAMP BUILD_DATE "%Y-%m-%d %H:%M:%S")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/settings/updater/BuildInfo.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/settings/updater/BuildInfo.cpp" @ONLY)

find_package(toml11 4.2.0 CONFIG)
find_package(pugixml 1.14 CONFIG)
find_package(fmt 10.2.0 CONFIG)
find_package(SDL3 3.1.2 CONFIG)
if (APPLE)
    find_package(date 3.0.1 CONFIG)
endif()

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR NOT MSVC)
    find_package(cryptopp 8.9.0 MODULE)
endif()

add_subdirectory(externals)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

qt_add_resources(RESOURCE_FILES dist/BBLauncher.qrc)

if (UNIX AND NOT APPLE)
    find_package(OpenSSL REQUIRED)
endif()

set(PROJECT_SOURCES
    main.cpp
    modules/bblauncher.cpp
    modules/bblauncher.h
    modules/bblauncher.ui
    modules/Common.cpp
    modules/Common.h
    modules/ModManager.h
    modules/ModManager.cpp
    modules/ModManager.ui
    modules/SaveManager.h
    modules/RunGuard.cpp
    modules/RunGuard.h
    modules/SaveManager.cpp
    modules/SaveManager.ui
    modules/scope_exit.h
    modules/TrophyManager.cpp
    modules/TrophyManager.h
    modules/TrophyManager.ui
    modules/ipc/ipc_client.cpp
    modules/ipc/ipc_client.h
    modules/TrophyDeps/aes.h
    modules/TrophyDeps/concepts.h
    modules/TrophyDeps/enum.h
    modules/TrophyDeps/io_file.cpp
    modules/TrophyDeps/io_file.h
    modules/TrophyDeps/keys.h
    modules/TrophyDeps/nt_api.cpp
    modules/TrophyDeps/nt_api.h
    modules/TrophyDeps/trp.cpp
    modules/TrophyDeps/trp.h
    settings/formatting.h
    settings/control_settings.cpp
    settings/control_settings.h
    settings/control_settings.ui
    settings/hotkeys.cpp
    settings/hotkeys.h
    settings/hotkeys.ui
    settings/kbm_config_dialog.cpp
    settings/kbm_config_dialog.h
    settings/kbm_gui.cpp
    settings/kbm_gui.h
    settings/kbm_gui.ui
    settings/kbm_help_dialog.cpp
    settings/kbm_help_dialog.h
    settings/LauncherSettings.h
    settings/LauncherSettings.cpp
    settings/LauncherSettings.ui
    settings/ShadCheatsPatches.cpp    
    settings/ShadCheatsPatches.h    
    settings/ShadSettings.h
    settings/ShadSettings.cpp
    settings/ShadSettings.ui
    settings/updater/BuildInfo.h
    settings/updater/CheckUpdate.cpp
    settings/updater/CheckUpdate.h
    dist/BBIcon.icns
    ${CMAKE_CURRENT_BINARY_DIR}/settings/updater/BuildInfo.cpp
    ${RESOURCE_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BB_Launcher
        ${PROJECT_SOURCES}
        ${RESOURCEFOLDER}
    )
else()
    if(ANDROID)
        add_library(BB_Launcher SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(BB_Launcher
            ${PROJECT_SOURCES}
        )
    endif()
endif()

if (WIN32)
    target_sources(BB_Launcher PRIVATE dist/bblauncher.rc)
endif()

target_include_directories(BB_Launcher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BB_Launcher PRIVATE Qt6::Widgets Qt6::Network)
target_link_libraries(BB_Launcher PRIVATE fmt::fmt toml11::toml11 pugixml::pugixml SDL3::SDL3)

if (APPLE)
    target_link_libraries(BB_Launcher PRIVATE date::date-tz)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BB_Launcher)
endif()

set_target_properties(BB_Launcher PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING "5.1"
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_ICON_FILE "BBIcon.icns"
#   WIN32_EXECUTABLE TRUE
)

set_source_files_properties(dist/BBIcon.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)

if (WIN32 AND FORCE_UAC) 
    SET_TARGET_PROPERTIES(BB_Launcher PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")
    add_definitions(-DFORCE_UAC)
endif()

if (UNIX AND NOT APPLE)
    target_link_libraries(BB_Launcher PRIVATE ${OPENSSL_LIBRARIES})
endif()

install(TARGETS BB_Launcher BUNDLE DESTINATION .)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES "dist/BBLauncher.desktop" DESTINATION "share/applications")
    install(FILES "dist/libssl.so.3" DESTINATION "usr/lib")
    install(FILES "dist/BBIcon.png" DESTINATION "share/icons/hicolor/512x512/apps")
endif()
