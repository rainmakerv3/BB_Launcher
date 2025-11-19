# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# We can't create the same interface imported target multiple times, CMake will complain if we do
# that. This can happen if the find_package call is done in multiple different subdirectories.
if(TARGET WrapJasper::WrapJasper)
    set(WrapJasper_FOUND TRUE)
    return()
endif()

set(WrapJasper_FOUND OFF)
find_package(Jasper)

if(Jasper_FOUND)
    set(WrapJasper_FOUND ON)

    # Upstream package does not provide targets, only variables. So define a target.
    add_library(WrapJasper::WrapJasper INTERFACE IMPORTED)
    target_link_libraries(WrapJasper::WrapJasper INTERFACE ${JASPER_LIBRARIES})
    target_include_directories(WrapJasper::WrapJasper INTERFACE ${JASPER_INCLUDE_DIR})
endif()
