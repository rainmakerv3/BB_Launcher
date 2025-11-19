#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QWebpPlugin" for configuration "Release"
set_property(TARGET Qt6::QWebpPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QWebpPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/imageformats/qwebp.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QWebpPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QWebpPlugin "${_IMPORT_PREFIX}/plugins/imageformats/qwebp.lib" )

# Import target "Qt6::QWebpPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QWebpPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QWebpPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/imageformats/objects-Release/QWebpPlugin_init/QWebpPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QWebpPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QWebpPlugin_init "${_IMPORT_PREFIX}/plugins/imageformats/objects-Release/QWebpPlugin_init/QWebpPlugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
