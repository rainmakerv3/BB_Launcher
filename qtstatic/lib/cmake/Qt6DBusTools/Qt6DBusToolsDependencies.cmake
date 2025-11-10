# Copyright (C) 2024 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Find "ModuleTools" dependencies, which are other ModuleTools packages.
set(Qt6DBusTools_FOUND TRUE)

set(__qt_DBusTools_tool_third_party_deps "")
_qt_internal_find_third_party_dependencies("DBusTools" __qt_DBusTools_tool_third_party_deps)

set(__qt_DBusTools_tool_deps "Qt6CoreTools\;6.10.0")
_qt_internal_find_tool_dependencies("DBusTools" __qt_DBusTools_tool_deps)
