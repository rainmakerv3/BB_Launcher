# Copyright (C) 2024 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

set(QWindowsIntegrationPlugin_FOUND TRUE)

# note: _third_party_deps example: "ICU\\;FALSE\\;1.0\\;i18n uc data;ZLIB\\;FALSE\\;\\;"
set(__qt_QWindowsIntegrationPlugin_third_party_deps "")
_qt_internal_find_third_party_dependencies("QWindowsIntegrationPlugin" __qt_QWindowsIntegrationPlugin_third_party_deps)
unset(__qt_QWindowsIntegrationPlugin_third_party_deps)

set(__qt_use_no_default_path_for_qt_packages "NO_DEFAULT_PATH")
if(QT_DISABLE_NO_DEFAULT_PATH_IN_QT_PACKAGES)
    set(__qt_use_no_default_path_for_qt_packages "")
endif()

# note: target_deps example: "Qt6Core\;5.12.0;Qt6Gui\;5.12.0"
set(__qt_QWindowsIntegrationPlugin_target_deps "Qt6Core\;6.10.0;Qt6Gui\;6.10.0;Qt6OpenGL\;6.10.0")
set(__qt_QWindowsIntegrationPlugin_find_dependency_paths "${CMAKE_CURRENT_LIST_DIR}/..;${_qt_cmake_dir}")
_qt_internal_find_qt_dependencies("QWindowsIntegrationPlugin" __qt_QWindowsIntegrationPlugin_target_deps
                                  __qt_QWindowsIntegrationPlugin_find_dependency_paths)
unset(__qt_QWindowsIntegrationPlugin_target_deps)
unset(__qt_QWindowsIntegrationPlugin_find_dependency_paths)

if(__qt_${target}_missing_deps)
    set(QWindowsIntegrationPlugin_FOUND FALSE)
endif()
