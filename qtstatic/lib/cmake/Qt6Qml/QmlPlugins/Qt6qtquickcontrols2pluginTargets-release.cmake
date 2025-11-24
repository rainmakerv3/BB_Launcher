#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::qtquickcontrols2plugin" for configuration "Release"
set_property(TARGET Qt6::qtquickcontrols2plugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qtquickcontrols2plugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Controls/qtquickcontrols2plugin.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::qtquickcontrols2plugin )
list(APPEND _cmake_import_check_files_for_Qt6::qtquickcontrols2plugin "${_IMPORT_PREFIX}/qml/QtQuick/Controls/qtquickcontrols2plugin.lib" )

# Import target "Qt6::qtquickcontrols2plugin_init" for configuration "Release"
set_property(TARGET Qt6::qtquickcontrols2plugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qtquickcontrols2plugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Controls/objects-Release/qtquickcontrols2plugin_init/qtquickcontrols2plugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::qtquickcontrols2plugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::qtquickcontrols2plugin_init "${_IMPORT_PREFIX}/qml/QtQuick/Controls/objects-Release/qtquickcontrols2plugin_init/qtquickcontrols2plugin_init.cpp.obj" )

# Import target "Qt6::qtquickcontrols2plugin_resources_1" for configuration "Release"
set_property(TARGET Qt6::qtquickcontrols2plugin_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qtquickcontrols2plugin_resources_1 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/lib/objects-Release/qtquickcontrols2plugin_resources_1/.qt/rcc/qrc_indirectBasic_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::qtquickcontrols2plugin_resources_1 )
list(APPEND _cmake_import_check_files_for_Qt6::qtquickcontrols2plugin_resources_1 "${_IMPORT_PREFIX}/lib/objects-Release/qtquickcontrols2plugin_resources_1/.qt/rcc/qrc_indirectBasic_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
