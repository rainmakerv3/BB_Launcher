#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::qtquickcontrols2imaginestyleimplplugin" for configuration "Release"
set_property(TARGET Qt6::qtquickcontrols2imaginestyleimplplugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qtquickcontrols2imaginestyleimplplugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Imagine/impl/qtquickcontrols2imaginestyleimplplugin.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::qtquickcontrols2imaginestyleimplplugin )
list(APPEND _cmake_import_check_files_for_Qt6::qtquickcontrols2imaginestyleimplplugin "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Imagine/impl/qtquickcontrols2imaginestyleimplplugin.lib" )

# Import target "Qt6::qtquickcontrols2imaginestyleimplplugin_init" for configuration "Release"
set_property(TARGET Qt6::qtquickcontrols2imaginestyleimplplugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qtquickcontrols2imaginestyleimplplugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Imagine/impl/objects-Release/qtquickcontrols2imaginestyleimplplugin_init/qtquickcontrols2imaginestyleimplplugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::qtquickcontrols2imaginestyleimplplugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::qtquickcontrols2imaginestyleimplplugin_init "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Imagine/impl/objects-Release/qtquickcontrols2imaginestyleimplplugin_init/qtquickcontrols2imaginestyleimplplugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
