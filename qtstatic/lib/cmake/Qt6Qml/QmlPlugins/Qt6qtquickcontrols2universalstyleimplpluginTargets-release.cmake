#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::qtquickcontrols2universalstyleimplplugin" for configuration "Release"
set_property(TARGET Qt6::qtquickcontrols2universalstyleimplplugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qtquickcontrols2universalstyleimplplugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Universal/impl/qtquickcontrols2universalstyleimplplugin.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::qtquickcontrols2universalstyleimplplugin )
list(APPEND _cmake_import_check_files_for_Qt6::qtquickcontrols2universalstyleimplplugin "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Universal/impl/qtquickcontrols2universalstyleimplplugin.lib" )

# Import target "Qt6::qtquickcontrols2universalstyleimplplugin_init" for configuration "Release"
set_property(TARGET Qt6::qtquickcontrols2universalstyleimplplugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qtquickcontrols2universalstyleimplplugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Universal/impl/objects-Release/qtquickcontrols2universalstyleimplplugin_init/qtquickcontrols2universalstyleimplplugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::qtquickcontrols2universalstyleimplplugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::qtquickcontrols2universalstyleimplplugin_init "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Universal/impl/objects-Release/qtquickcontrols2universalstyleimplplugin_init/qtquickcontrols2universalstyleimplplugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
