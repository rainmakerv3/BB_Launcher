# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

find_package(PkgConfig QUIET)
if (PkgConfig_FOUND)
    pkg_check_modules(Gypsy gypsy IMPORTED_TARGET)

    if (TARGET PkgConfig::Gypsy)
        mark_as_advanced(Gypsy_LIBRARIES Gypsy_INCLUDE_DIRS)
        if (NOT TARGET Gypsy::Gypsy)
            add_library(Gypsy::Gypsy INTERFACE IMPORTED)
            target_link_libraries(Gypsy::Gypsy INTERFACE PkgConfig::Gypsy)
        endif()
    else()
        set(Gypsy_FOUND 0)
    endif()
endif()
