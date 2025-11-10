#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QModernWindowsStylePlugin" for configuration "Release"
set_property(TARGET Qt6::QModernWindowsStylePlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QModernWindowsStylePlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/styles/qmodernwindowsstyle.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QModernWindowsStylePlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QModernWindowsStylePlugin "${_IMPORT_PREFIX}/plugins/styles/qmodernwindowsstyle.lib" )

# Import target "Qt6::QModernWindowsStylePlugin_init" for configuration "Release"
set_property(TARGET Qt6::QModernWindowsStylePlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QModernWindowsStylePlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/styles/objects-Release/QModernWindowsStylePlugin_init/QModernWindowsStylePlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QModernWindowsStylePlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QModernWindowsStylePlugin_init "${_IMPORT_PREFIX}/plugins/styles/objects-Release/QModernWindowsStylePlugin_init/QModernWindowsStylePlugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
