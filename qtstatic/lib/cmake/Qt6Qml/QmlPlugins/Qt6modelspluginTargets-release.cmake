#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::modelsplugin" for configuration "Release"
set_property(TARGET Qt6::modelsplugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::modelsplugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/qml/QtQml/Models/modelsplugin.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::modelsplugin )
list(APPEND _cmake_import_check_files_for_Qt6::modelsplugin "${_IMPORT_PREFIX}/qml/QtQml/Models/modelsplugin.lib" )

# Import target "Qt6::modelsplugin_init" for configuration "Release"
set_property(TARGET Qt6::modelsplugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::modelsplugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQml/Models/objects-Release/modelsplugin_init/modelsplugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::modelsplugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::modelsplugin_init "${_IMPORT_PREFIX}/qml/QtQml/Models/objects-Release/modelsplugin_init/modelsplugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
