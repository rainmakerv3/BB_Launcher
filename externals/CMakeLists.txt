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

if (APPLE)
     # date
    if (NOT TARGET date::date-tz)
        option(BUILD_TZ_LIB "" ON)
        option(USE_SYSTEM_TZ_DB "" ON)
        add_subdirectory(date)
    endif()
endif()