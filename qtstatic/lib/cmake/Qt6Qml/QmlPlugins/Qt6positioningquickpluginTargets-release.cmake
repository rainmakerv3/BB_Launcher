#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::positioningquickplugin" for configuration "Release"
set_property(TARGET Qt6::positioningquickplugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::positioningquickplugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/qml/QtPositioning/positioningquickplugin.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::positioningquickplugin )
list(APPEND _cmake_import_check_files_for_Qt6::positioningquickplugin "${_IMPORT_PREFIX}/qml/QtPositioning/positioningquickplugin.lib" )

# Import target "Qt6::positioningquickplugin_init" for configuration "Release"
set_property(TARGET Qt6::positioningquickplugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::positioningquickplugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtPositioning/objects-Release/positioningquickplugin_init/positioningquickplugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::positioningquickplugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::positioningquickplugin_init "${_IMPORT_PREFIX}/qml/QtPositioning/objects-Release/positioningquickplugin_init/positioningquickplugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
