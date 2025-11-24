# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

find_package(PkgConfig QUIET)
if (PkgConfig_FOUND)
    pkg_check_modules(Gconf gconf-2.0 IMPORTED_TARGET)

    if (TARGET PkgConfig::Gconf)
        mark_as_advanced(Gconf_LIBRARIES Gconf_INCLUDE_DIRS)
        if (NOT TARGET Gconf::Gconf)
            add_library(Gconf::Gconf INTERFACE IMPORTED)
            target_link_libraries(Gconf::Gconf INTERFACE PkgConfig::Gconf)
        endif()
    else()
        set(Gconf_FOUND 0)
    endif()
endif()
