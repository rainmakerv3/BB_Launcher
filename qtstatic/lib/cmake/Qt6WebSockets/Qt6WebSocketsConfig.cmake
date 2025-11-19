# Copyright (C) 2024 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was QtModuleConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

cmake_minimum_required(VERSION 3.16...3.21)

include(CMakeFindDependencyMacro)

# Extra cmake code begin

# Extra cmake code end

# Find required dependencies, if any.
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/Qt6WebSocketsDependencies.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/Qt6WebSocketsDependencies.cmake")
    _qt_internal_suggest_dependency_debugging(WebSockets
        __qt_WebSockets_pkg ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE)
endif()

# If *ConfigDependencies.cmake exists, the variable value will be defined there.
# Don't override it in that case.
if(NOT DEFINED "Qt6WebSockets_FOUND")
    set("Qt6WebSockets_FOUND" TRUE)
endif()

if (NOT QT_NO_CREATE_TARGETS AND Qt6WebSockets_FOUND)
    include("${CMAKE_CURRENT_LIST_DIR}/Qt6WebSocketsTargets.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/Qt6WebSocketsAdditionalTargetInfo.cmake"
        OPTIONAL)
    include("${CMAKE_CURRENT_LIST_DIR}/Qt6WebSocketsExtraProperties.cmake"
        OPTIONAL)
endif()

# Find the private module counterpart.
set(__qt_WebSockets_always_load_private_module OFF)
include("${CMAKE_CURRENT_LIST_DIR}/Qt6WebSockets-build.cmake" OPTIONAL)
if (Qt6WebSockets_FOUND
        AND NOT Qt6WebSocketsPrivate_FOUND
        AND NOT FALSE
        AND (
            __qt_WebSockets_always_load_private_module
            OR DEFINED QT_REPO_MODULE_VERSION
            OR QT_FIND_PRIVATE_MODULES
        )
    )
    if("${_qt_cmake_dir}" STREQUAL "")
        set(_qt_cmake_dir "${QT_TOOLCHAIN_RELOCATABLE_CMAKE_DIR}")
    endif()
    set(__qt_use_no_default_path_for_qt_packages "NO_DEFAULT_PATH")
    if(QT_DISABLE_NO_DEFAULT_PATH_IN_QT_PACKAGES)
        set(__qt_use_no_default_path_for_qt_packages "")
    endif()
    find_package(Qt6WebSocketsPrivate "6.10.0" EXACT
        QUIET
        CONFIG
        PATHS
            ${QT_BUILD_CMAKE_PREFIX_PATH}
            "${CMAKE_CURRENT_LIST_DIR}/.."
            "${_qt_cmake_dir}"
            ${_qt_additional_packages_prefix_paths}
        ${__qt_use_no_default_path_for_qt_packages}
    )

    if(NOT Qt6WebSocketsPrivate_FOUND)
        get_property(Qt6WebSocketsPrivate_warning_shown GLOBAL PROPERTY
            Qt6WebSocketsPrivate_warning_shown
        )
        if(NOT Qt6WebSocketsPrivate_warning_shown)
            message(VERBOSE
                "The private module package 'Qt6WebSocketsPrivate' "
                "could not be found. It possibly needs to be installed separately with your "
                "package manager."
            )
            set_property(GLOBAL PROPERTY
                Qt6WebSocketsPrivate_warning_shown ON
            )
        endif()
    endif()
endif()
unset(__qt_WebSockets_always_load_private_module)

if (NOT QT_NO_CREATE_TARGETS AND Qt6WebSockets_FOUND)
    # DEPRECATED
    # Provide old style variables for includes, compile definitions, etc.
    # These variables are deprecated and only provided on a best-effort basis to facilitate porting.
    # Consider using target_link_libraries(app PRIVATE Qt6WebSockets) instead.
    set(Qt6WebSockets_LIBRARIES "Qt6::WebSockets")

    get_target_property(_Qt6WebSockets_OWN_INCLUDE_DIRS
                        Qt6::WebSockets INTERFACE_INCLUDE_DIRECTORIES)
    if(NOT _Qt6WebSockets_OWN_INCLUDE_DIRS)
        set(_Qt6WebSockets_OWN_INCLUDE_DIRS "")
    endif()

    if(TARGET Qt6::WebSocketsPrivate)
        get_target_property(_Qt6WebSockets_OWN_PRIVATE_INCLUDE_DIRS
                            Qt6::WebSocketsPrivate INTERFACE_INCLUDE_DIRECTORIES)
        if(NOT _Qt6WebSockets_OWN_PRIVATE_INCLUDE_DIRS)
            set(_Qt6WebSockets_OWN_PRIVATE_INCLUDE_DIRS "")
        endif()
    endif()

    get_target_property(Qt6WebSockets_DEFINITIONS
                        Qt6::WebSockets INTERFACE_COMPILE_DEFINITIONS)
    if(NOT Qt6WebSockets_DEFINITIONS)
        set(Qt6WebSockets_DEFINITIONS "")
    else()
        set(updated_defs "")
        foreach(def IN LISTS  Qt6WebSockets_DEFINITIONS)
             if(def MATCHES "^[A-Za-z_]")
                 list(APPEND updated_defs "-D${def}")
             else()
                 list(APPEND updated_defs "${def}")
             endif()
        endforeach()
        set(Qt6WebSockets_DEFINITIONS "${updated_defs}")
        unset(updated_defs)
    endif()

    get_target_property(Qt6WebSockets_COMPILE_DEFINITIONS
                        Qt6::WebSockets INTERFACE_COMPILE_DEFINITIONS)
    if(NOT Qt6WebSockets_COMPILE_DEFINITIONS)
        set(Qt6WebSockets_COMPILE_DEFINITIONS "")
    endif()

    set(Qt6WebSockets_INCLUDE_DIRS
        ${_Qt6WebSockets_OWN_INCLUDE_DIRS})

    set(Qt6WebSockets_PRIVATE_INCLUDE_DIRS
        ${_Qt6WebSockets_OWN_PRIVATE_INCLUDE_DIRS})

    foreach(_module_dep ${_Qt6WebSockets_MODULE_DEPENDENCIES})
        if(_module_dep MATCHES ".+Private$")
            set(_private_suffix "Private")
        else()
            set(_private_suffix "")
        endif()
        list(APPEND Qt6WebSockets${_private_suffix}_INCLUDE_DIRS
             ${Qt6${_module_dep}_INCLUDE_DIRS})
        list(APPEND Qt6WebSockets${_private_suffix}_PRIVATE_INCLUDE_DIRS
             ${Qt6${_module_dep}_PRIVATE_INCLUDE_DIRS})
        if(_private_suffix)
            list(APPEND Qt6WebSockets_PRIVATE_INCLUDE_DIRS
                ${Qt6${_module_dep}_PRIVATE_INCLUDE_DIRS})
        endif()
        list(APPEND Qt6WebSockets${_private_suffix}_DEFINITIONS
             ${Qt6${_module_dep}_DEFINITIONS})
        list(APPEND Qt6WebSockets${_private_suffix}_COMPILE_DEFINITIONS
             ${Qt6${_module_dep}_COMPILE_DEFINITIONS})
    endforeach()
    unset(_private_suffix)

    list(REMOVE_DUPLICATES Qt6WebSockets_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt6WebSockets_PRIVATE_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt6WebSockets_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt6WebSockets_COMPILE_DEFINITIONS)
endif()

if (TARGET Qt6::WebSockets)
    qt_make_features_available(Qt6::WebSockets)

    foreach(extra_cmake_include )
        include("${CMAKE_CURRENT_LIST_DIR}/${extra_cmake_include}")
    endforeach()

    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/Qt6WebSocketsPlugins.cmake")
        include("${CMAKE_CURRENT_LIST_DIR}/Qt6WebSocketsPlugins.cmake")
    endif()

    if(NOT "WebSockets" IN_LIST QT_ALL_MODULES_FOUND_VIA_FIND_PACKAGE)
        list(APPEND QT_ALL_MODULES_FOUND_VIA_FIND_PACKAGE "WebSockets")
        list(APPEND QT_ALL_MODULES_VERSIONED_FOUND_VIA_FIND_PACKAGE
            "Qt6::WebSockets")
    endif()

    get_target_property(_qt_module_target_type "Qt6::WebSockets" TYPE)
    if(NOT _qt_module_target_type STREQUAL "INTERFACE_LIBRARY")
        get_target_property(_qt_module_plugin_types
                            Qt6::WebSockets MODULE_PLUGIN_TYPES)
        if(_qt_module_plugin_types)
            foreach(_qt_module_plugin_type IN LISTS _qt_module_plugin_types)
                if(NOT "${_qt_module_plugin_type}"
                    IN_LIST QT_ALL_PLUGIN_TYPES_FOUND_VIA_FIND_PACKAGE)
                    list(APPEND QT_ALL_PLUGIN_TYPES_FOUND_VIA_FIND_PACKAGE
                    "${_qt_module_plugin_type}")
                endif()
            endforeach()
            unset(_qt_module_plugin_type)
        endif()
    endif()

    # Load Module's BuildInternals should any exist
    if (Qt6BuildInternals_DIR AND
        EXISTS "${CMAKE_CURRENT_LIST_DIR}/Qt6WebSocketsBuildInternals.cmake")
        include("${CMAKE_CURRENT_LIST_DIR}/Qt6WebSocketsBuildInternals.cmake")
    endif()

    if(NOT QT_NO_CREATE_VERSIONLESS_TARGETS)
        if(CMAKE_VERSION VERSION_LESS 3.18 OR QT_USE_OLD_VERSION_LESS_TARGETS)
            include("${CMAKE_CURRENT_LIST_DIR}/Qt6WebSocketsVersionlessTargets.cmake")
        else()
            include("${CMAKE_CURRENT_LIST_DIR}/Qt6WebSocketsVersionlessAliasTargets.cmake")
        endif()
    endif()
else()

    set(Qt6WebSockets_FOUND FALSE)
    if(NOT DEFINED Qt6WebSockets_NOT_FOUND_MESSAGE)
        set(Qt6WebSockets_NOT_FOUND_MESSAGE
            "Target \"Qt6::WebSockets\" was not found.")

        if(QT_NO_CREATE_TARGETS)
            string(APPEND Qt6WebSockets_NOT_FOUND_MESSAGE
                "Possibly due to QT_NO_CREATE_TARGETS being set to TRUE and thus "
                "${CMAKE_CURRENT_LIST_DIR}/Qt6WebSocketsTargets.cmake was not "
                "included to define the target.")
        endif()
    endif()
endif()
