#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::qtquickshapesdesignhelpersplugin" for configuration "Release"
set_property(TARGET Qt6::qtquickshapesdesignhelpersplugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qtquickshapesdesignhelpersplugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Shapes/DesignHelpers/qtquickshapesdesignhelpersplugin.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::qtquickshapesdesignhelpersplugin )
list(APPEND _cmake_import_check_files_for_Qt6::qtquickshapesdesignhelpersplugin "${_IMPORT_PREFIX}/qml/QtQuick/Shapes/DesignHelpers/qtquickshapesdesignhelpersplugin.lib" )

# Import target "Qt6::qtquickshapesdesignhelpersplugin_init" for configuration "Release"
set_property(TARGET Qt6::qtquickshapesdesignhelpersplugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qtquickshapesdesignhelpersplugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Shapes/DesignHelpers/objects-Release/qtquickshapesdesignhelpersplugin_init/qtquickshapesdesignhelpersplugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::qtquickshapesdesignhelpersplugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::qtquickshapesdesignhelpersplugin_init "${_IMPORT_PREFIX}/qml/QtQuick/Shapes/DesignHelpers/objects-Release/qtquickshapesdesignhelpersplugin_init/qtquickshapesdesignhelpersplugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
