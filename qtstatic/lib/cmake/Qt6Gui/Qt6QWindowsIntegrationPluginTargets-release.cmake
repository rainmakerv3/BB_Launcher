#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QWindowsIntegrationPlugin" for configuration "Release"
set_property(TARGET Qt6::QWindowsIntegrationPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QWindowsIntegrationPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/platforms/qwindows.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QWindowsIntegrationPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QWindowsIntegrationPlugin "${_IMPORT_PREFIX}/plugins/platforms/qwindows.lib" )

# Import target "Qt6::QWindowsIntegrationPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QWindowsIntegrationPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QWindowsIntegrationPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/platforms/objects-Release/QWindowsIntegrationPlugin_init/QWindowsIntegrationPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QWindowsIntegrationPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QWindowsIntegrationPlugin_init "${_IMPORT_PREFIX}/plugins/platforms/objects-Release/QWindowsIntegrationPlugin_init/QWindowsIntegrationPlugin_init.cpp.obj" )

# Import target "Qt6::QWindowsIntegrationPlugin_resources_1" for configuration "Release"
set_property(TARGET Qt6::QWindowsIntegrationPlugin_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QWindowsIntegrationPlugin_resources_1 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/lib/objects-Release/QWindowsIntegrationPlugin_resources_1/.qt/rcc/qrc_openglblacklists_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QWindowsIntegrationPlugin_resources_1 )
list(APPEND _cmake_import_check_files_for_Qt6::QWindowsIntegrationPlugin_resources_1 "${_IMPORT_PREFIX}/lib/objects-Release/QWindowsIntegrationPlugin_resources_1/.qt/rcc/qrc_openglblacklists_init.cpp.obj" )

# Import target "Qt6::QWindowsIntegrationPlugin_resources_2" for configuration "Release"
set_property(TARGET Qt6::QWindowsIntegrationPlugin_resources_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QWindowsIntegrationPlugin_resources_2 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/lib/objects-Release/QWindowsIntegrationPlugin_resources_2/.qt/rcc/qrc_cursors_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QWindowsIntegrationPlugin_resources_2 )
list(APPEND _cmake_import_check_files_for_Qt6::QWindowsIntegrationPlugin_resources_2 "${_IMPORT_PREFIX}/lib/objects-Release/QWindowsIntegrationPlugin_resources_2/.qt/rcc/qrc_cursors_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
