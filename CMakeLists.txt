cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(BB_Launcher LANGUAGES CXX)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(APPLE)
    enable_language(OBJC)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 14)
endif()

option(FORCE_UAC "Requires running as Admin on Windows" ON)
option(USE_WEBENGINE "Use WebEngine to enable downloading non-premium mods on Linux" ON)

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

if (ARCHITECTURE STREQUAL "x86_64")
    # Target x86-64-v3 CPU architecture as this is a good balance between supporting performance critical
    # instructions like AVX2 and maintaining support for older CPUs.
    add_compile_options(-march=x86-64-v3)
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

if(WIN32 AND NOT CMAKE_PREFIX_PATH)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/DetectQtInstallation.cmake")
endif ()

if (WIN32)
    set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/qtstatic")
    set(Qt6_DIR "CMAKE_CURRENT_SOURCE_DIR}/qtstatic/lib/cmake/Qt6")
    set(Qt6Widgets_DIR "CMAKE_CURRENT_SOURCE_DIR}/qtstatic/lib/cmake/Qt6Widgets")
    set(Qt6Network_DIR "CMAKE_CURRENT_SOURCE_DIR}/qtstatic/lib/cmake/Qt6Network")
    set(WEBVIEW2_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/WebView2SDK/build/native/x64/WebView2LoaderStatic.lib")
    set(WEBVIEW2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/WebView2SDK/build/native/include")
endif()

find_package(toml11 4.2.0 CONFIG)
find_package(pugixml 1.14 CONFIG)
find_package(ZLIB 1.3 MODULE)
find_package(fmt 10.2.0 CONFIG)
find_package(SDL3 3.1.2 CONFIG)
find_package(VulkanHeaders 1.4.324 CONFIG)
if (APPLE)
    find_package(date 3.0.1 CONFIG)
endif()

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR NOT MSVC)
    find_package(cryptopp 8.9.0 MODULE)
endif()

add_subdirectory(externals)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Quick QuickWidgets WebView WebSockets Concurrent)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND USE_WEBENGINE)
    add_definitions(-DUSE_WEBENGINE)
    find_package(Qt6 REQUIRED COMPONENTS WebEngineCore WebEngineWidgets WebChannel)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

qt_add_resources(RESOURCE_FILES dist/BBLauncher.qrc)

set(PROJECT_SOURCES
    main.cpp
    modules/bblauncher.cpp
    modules/bblauncher.h
    modules/bblauncher.ui
    modules/Common.cpp
    modules/Common.h
    modules/ModDownloader.h
    modules/ModDownloader.cpp
    modules/ModDownloader.ui
    modules/ModManager.h
    modules/ModManager.cpp
    modules/ModManager.ui
    modules/PkgExtractor.h
    modules/PkgExtractor.cpp
    modules/QAnsiTextEdit.cpp
    modules/QAnsiTextEdit.h
    modules/RunGuard.cpp
    modules/RunGuard.h
    modules/SaveManager.h
    modules/SaveManager.cpp
    modules/SaveManager.ui
    modules/scope_exit.h
    modules/TrophyManager.cpp
    modules/TrophyManager.h
    modules/TrophyManager.ui
    modules/version_dialog.cpp
    modules/version_dialog.h
    modules/version_dialog.ui
    modules/ipc/ipc_client.cpp
    modules/ipc/ipc_client.h
    modules/PkgDeps/crypto.cpp
    modules/PkgDeps/crypto.h
    modules/PkgDeps/loader.cpp
    modules/PkgDeps/loader.h
    modules/PkgDeps/pfs.h
    modules/PkgDeps/pkg.cpp
    modules/PkgDeps/pkg.h
    modules/PkgDeps/pkg_type.cpp
    modules/PkgDeps/pkg_type.h
    modules/PkgDeps/types.h
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
    settings/PSF/psf.cpp
    settings/PSF/psf.h
    settings/formatting.h
    settings/config.cpp
    settings/config.h
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
    settings/RightClickButton.cpp    
    settings/RightClickButton.h  
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

qt_add_executable(BB_Launcher
    ${PROJECT_SOURCES}
)

if (WIN32)
    target_sources(BB_Launcher PRIVATE dist/bblauncher.rc)
    target_include_directories(BB_Launcher PRIVATE
        qtstatic/include/QtWebView/6.10.0/QtWebView/private
    )
endif()

target_include_directories(BB_Launcher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(BB_Launcher PRIVATE Qt6::Widgets Qt6::Network Qt6::Quick Qt6::QuickWidgets Qt6::WebView Qt6::WebSockets Qt6::Concurrent)
target_link_libraries(BB_Launcher PRIVATE fmt::fmt toml11::toml11 pugixml::pugixml SDL3::SDL3 Vulkan::Headers qmicroz volk_headers nlohmann_json::nlohmann_json cryptopp::cryptopp ZLIB::ZLIB)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND USE_WEBENGINE)
    target_link_libraries(BB_Launcher PRIVATE Qt6::WebEngineCore Qt6::WebEngineWidgets Qt6::WebChannel)
endif()

if (APPLE)
    target_link_libraries(BB_Launcher PRIVATE date::date-tz)
endif()

set_target_properties(BB_Launcher PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING "5.1"
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_ICON_FILE "BBIcon.icns"
    WIN32_EXECUTABLE TRUE
)

set_source_files_properties(dist/BBIcon.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)

if (WIN32 AND FORCE_UAC) 
    SET_TARGET_PROPERTIES(BB_Launcher PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")
    add_definitions(-DFORCE_UAC)
endif()

install(TARGETS BB_Launcher BUNDLE DESTINATION .)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES "dist/BBLauncher.desktop" DESTINATION "share/applications")
    install(FILES "dist/BBIcon.png" DESTINATION "share/icons/hicolor/512x512/apps")
endif()
