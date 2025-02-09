# SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later

set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)
set_directory_properties(PROPERTIES
    EXCLUDE_FROM_ALL ON
    SYSTEM ON
)

# CryptoPP
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND MSVC)
    # If it is clang and MSVC we will add a static lib
    # CryptoPP
    add_subdirectory(cryptoppwin)
    target_include_directories(cryptoppwin INTERFACE cryptoppwin/include)
else()
    # CryptoPP
    if (NOT TARGET cryptopp::cryptopp)
        set(CRYPTOPP_INSTALL OFF)
        set(CRYPTOPP_BUILD_TESTING OFF)
        set(CRYPTOPP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cryptopp)
        add_subdirectory(cryptopp-cmake)
        file(COPY cryptopp DESTINATION cryptopp FILES_MATCHING PATTERN "*.h")
        # remove externals/cryptopp from include directories because it contains a conflicting zlib.h file
        set_target_properties(cryptopp PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/cryptopp")
    endif()
endif()

# pugixml
if (NOT TARGET pugixml::pugixml)
    add_subdirectory(pugixml)
endif()