#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QmlLintQuickPlugin" for configuration "Release"
set_property(TARGET Qt6::QmlLintQuickPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QmlLintQuickPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/qmllint/quicklintplugin.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QmlLintQuickPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QmlLintQuickPlugin "${_IMPORT_PREFIX}/plugins/qmllint/quicklintplugin.lib" )

# Import target "Qt6::QmlLintQuickPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QmlLintQuickPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QmlLintQuickPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/qmllint/objects-Release/QmlLintQuickPlugin_init/QmlLintQuickPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QmlLintQuickPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QmlLintQuickPlugin_init "${_IMPORT_PREFIX}/plugins/qmllint/objects-Release/QmlLintQuickPlugin_init/QmlLintQuickPlugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
