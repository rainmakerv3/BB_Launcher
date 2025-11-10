#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QWindowsDirect2DIntegrationPlugin" for configuration "Release"
set_property(TARGET Qt6::QWindowsDirect2DIntegrationPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QWindowsDirect2DIntegrationPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/platforms/qdirect2d.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QWindowsDirect2DIntegrationPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QWindowsDirect2DIntegrationPlugin "${_IMPORT_PREFIX}/plugins/platforms/qdirect2d.lib" )

# Import target "Qt6::QWindowsDirect2DIntegrationPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QWindowsDirect2DIntegrationPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QWindowsDirect2DIntegrationPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/platforms/objects-Release/QWindowsDirect2DIntegrationPlugin_init/QWindowsDirect2DIntegrationPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QWindowsDirect2DIntegrationPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QWindowsDirect2DIntegrationPlugin_init "${_IMPORT_PREFIX}/plugins/platforms/objects-Release/QWindowsDirect2DIntegrationPlugin_init/QWindowsDirect2DIntegrationPlugin_init.cpp.obj" )

# Import target "Qt6::QWindowsDirect2DIntegrationPlugin_resources_1" for configuration "Release"
set_property(TARGET Qt6::QWindowsDirect2DIntegrationPlugin_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QWindowsDirect2DIntegrationPlugin_resources_1 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/lib/objects-Release/QWindowsDirect2DIntegrationPlugin_resources_1/.qt/rcc/qrc_openglblacklists_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QWindowsDirect2DIntegrationPlugin_resources_1 )
list(APPEND _cmake_import_check_files_for_Qt6::QWindowsDirect2DIntegrationPlugin_resources_1 "${_IMPORT_PREFIX}/lib/objects-Release/QWindowsDirect2DIntegrationPlugin_resources_1/.qt/rcc/qrc_openglblacklists_init.cpp.obj" )

# Import target "Qt6::QWindowsDirect2DIntegrationPlugin_resources_2" for configuration "Release"
set_property(TARGET Qt6::QWindowsDirect2DIntegrationPlugin_resources_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QWindowsDirect2DIntegrationPlugin_resources_2 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/lib/objects-Release/QWindowsDirect2DIntegrationPlugin_resources_2/.qt/rcc/qrc_cursors_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QWindowsDirect2DIntegrationPlugin_resources_2 )
list(APPEND _cmake_import_check_files_for_Qt6::QWindowsDirect2DIntegrationPlugin_resources_2 "${_IMPORT_PREFIX}/lib/objects-Release/QWindowsDirect2DIntegrationPlugin_resources_2/.qt/rcc/qrc_cursors_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
