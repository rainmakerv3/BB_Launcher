# Copyright (C) 2025 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(TARGET WebView2::WebView2)
    set(WebView2_FOUND TRUE)
    return()
endif()

function(get_cpu_arch result arch)
    set(arm64List arm64 ARM64 aarch64)
    set(x64List x86_64 AMD64 x86_64h)
    if(arch IN_LIST x64List)
        set(${result} "x64" PARENT_SCOPE)
    elseif(arch IN_LIST arm64List)
        set(${result} "arm64" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unknown architecture: ${arch}")
    endif()
endfunction()

get_cpu_arch(webview2_sdk_arch ${CMAKE_SYSTEM_PROCESSOR})

if(NOT DEFINED WEBVIEW2_SDK_ROOT)
    if(DEFINED ENV{WEBVIEW2_SDK_ROOT})
        file(TO_NATIVE_PATH "$ENV{WEBVIEW2_SDK_ROOT}" sdk_path)
        if(NOT EXISTS ${sdk_path})
            message(FATAL_ERROR "WEBVIEW2_SDK_ROOT set to non-existing ${sdk_path} path")
        endif()
        set(WEBVIEW2_SDK_ROOT "${sdk_path}" CACHE STRING "")
        unset(sdk_path)
    endif()
endif()

find_path(WEBVIEW2_INCLUDE_DIR
          NAMES WebView2.h
          PATHS "${WEBVIEW2_SDK_ROOT}/build/native/include")
find_library(WEBVIEW2_LIBRARY
          NAMES WebView2LoaderStatic.lib
          PATHS "${WEBVIEW2_SDK_ROOT}/build/native/${webview2_sdk_arch}")

if(WEBVIEW2_LIBRARY AND WEBVIEW2_INCLUDE_DIR)
    set(WEBVIEW2_LIBRARY_DIR "${WEBVIEW2_SDK_ROOT}/build/native/${webview2_sdk_arch}" CACHE STRING "")
    add_library(WebView2::WebView2 UNKNOWN IMPORTED)
    set_target_properties(WebView2::WebView2 PROPERTIES
        IMPORTED_LOCATION ${WEBVIEW2_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${WEBVIEW2_INCLUDE_DIR}
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WebView2 REQUIRED_VARS
    WEBVIEW2_LIBRARY
    WEBVIEW2_INCLUDE_DIR
)
