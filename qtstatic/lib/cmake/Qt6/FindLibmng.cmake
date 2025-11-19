# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(TARGET Libmng::Libmng)
    set(Libmng_FOUND TRUE)
    return()
endif()

find_package(PkgConfig QUIET)

if(PkgConfig_FOUND)
    pkg_check_modules(Libmng IMPORTED_TARGET libmng)

    if(TARGET PkgConfig::Libmng)
        add_library(Libmng::Libmng INTERFACE IMPORTED)
        target_link_libraries(Libmng::Libmng INTERFACE PkgConfig::Libmng)
        set(Libmng_FOUND TRUE)
    endif()
endif()

if(NOT TARGET Libmng::Libmng)
    find_library(LIBMNG_LIBRARY NAMES mng)
    find_path(LIBMNG_INCLUDE_DIR libmng.h)

    if(LIBMNG_LIBRARY AND LIBMNG_INCLUDE_DIR)
        add_library(Libmng::Libmng UNKNOWN IMPORTED)
        set_target_properties(Libmng::Libmng PROPERTIES
            IMPORTED_LOCATION ${LIBMNG_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${LIBMNG_INCLUDE_DIR}
        )
    endif()

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Libmng REQUIRED_VARS
        LIBMNG_LIBRARY
        LIBMNG_INCLUDE_DIR
    )
endif()
