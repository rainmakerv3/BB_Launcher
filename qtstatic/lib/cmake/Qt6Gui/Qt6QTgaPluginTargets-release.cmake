#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QTgaPlugin" for configuration "Release"
set_property(TARGET Qt6::QTgaPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QTgaPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/imageformats/qtga.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QTgaPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QTgaPlugin "${_IMPORT_PREFIX}/plugins/imageformats/qtga.lib" )

# Import target "Qt6::QTgaPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QTgaPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QTgaPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/imageformats/objects-Release/QTgaPlugin_init/QTgaPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QTgaPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QTgaPlugin_init "${_IMPORT_PREFIX}/plugins/imageformats/objects-Release/QTgaPlugin_init/QTgaPlugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
