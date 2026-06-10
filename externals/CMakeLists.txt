# SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later

set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)
set_directory_properties(PROPERTIES
    EXCLUDE_FROM_ALL ON
    SYSTEM ON
)

# fmtlib
if (NOT TARGET fmt::fmt)
    add_subdirectory(fmt)
endif()

# pugixml
if (NOT TARGET pugixml::pugixml)
    add_subdirectory(pugixml)
endif()

# Toml11
if (NOT TARGET toml11::toml11)
    add_subdirectory(toml11)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        get_target_property(_toml11_compile_options toml11 INTERFACE_COMPILE_OPTIONS)
        list(REMOVE_ITEM _toml11_compile_options "/Zc:preprocessor")
        set_target_properties(toml11 PROPERTIES INTERFACE_COMPILE_OPTIONS ${_toml11_compile_options})
      endif()
    endif()
endif()

# SDL3
if (NOT TARGET SDL3::SDL3)
    set(SDL_TEST_LIBRARY OFF)
    set(SDL_PIPEWIRE OFF)
    add_subdirectory(sdl3)
endif()

# microz
add_subdirectory(microz)

#nlohmann json 
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

#volk
add_subdirectory(volk)

# vulkan-headers
if (NOT TARGET Vulkan::Headers)
    set(VULKAN_HEADERS_ENABLE_MODULE OFF)
    add_subdirectory(vulkan-headers)
endif()

# CryptoPP
if (NOT TARGET cryptopp::cryptopp)
    set(CRYPTOPP_INSTALL OFF)
    set(CRYPTOPP_BUILD_TESTING OFF)
    set(CRYPTOPP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cryptopp)
    # cryptopp instruction set checks do not account for added compile options,
    # so disable extensions in the library config to match our chosen target CPU.
    set(CRYPTOPP_DISABLE_AESNI ON)
    add_subdirectory(cryptopp-cmake)
    file(COPY cryptopp DESTINATION cryptopp FILES_MATCHING PATTERN "*.h")
    # remove externals/cryptopp from include directories because it contains a conflicting zlib.h file
    set_target_properties(cryptopp PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/cryptopp")
endif()

# liblzma
set(ENABLE_NLS OFF CACHE BOOL "Disable NLS" FORCE)
add_subdirectory(xz)

# libarchive
set(ENABLE_NETTLE OFF CACHE BOOL "Disable nettle" FORCE)
set(ENABLE_OPENSSL OFF CACHE BOOL "Disable openssl" FORCE)
set(ENABLE_TEST OFF CACHE BOOL "Disable libarchive tests" FORCE)
set(ENABLE_ACL OFF CACHE BOOL "Disable ACL" FORCE)
set(LIBLZMA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/xz/src/liblzma/api")
set(LIBLZMA_LIBRARY liblzma)
set(LIBLZMA_FOUND TRUE CACHE BOOL "Force LibLZMA found fallback" FORCE)
set(LIBLZMA_HAS_AUTO_DECODER TRUE CACHE BOOL "Bypass check" FORCE)
set(LIBLZMA_HAS_EASY_ENCODER TRUE CACHE BOOL "Bypass check" FORCE)
set(LIBLZMA_HAS_LZMA_PRESET TRUE CACHE BOOL "Bypass check" FORCE)
if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(HAVE_WCSCPY 1 CACHE INTERNAL "Define to 1 if you have the wcscpy function" FORCE)
endif()
add_subdirectory(libarchive)

if (APPLE)
     # date
    if (NOT TARGET date::date-tz)
        option(BUILD_TZ_LIB "" ON)
        option(USE_SYSTEM_TZ_DB "" ON)
        add_subdirectory(date)
    endif()
endif()
