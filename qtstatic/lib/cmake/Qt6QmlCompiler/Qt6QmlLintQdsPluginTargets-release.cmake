#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QmlLintQdsPlugin" for configuration "Release"
set_property(TARGET Qt6::QmlLintQdsPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QmlLintQdsPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/qmllint/qdslintplugin.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QmlLintQdsPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QmlLintQdsPlugin "${_IMPORT_PREFIX}/plugins/qmllint/qdslintplugin.lib" )

# Import target "Qt6::QmlLintQdsPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QmlLintQdsPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QmlLintQdsPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/qmllint/objects-Release/QmlLintQdsPlugin_init/QmlLintQdsPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QmlLintQdsPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QmlLintQdsPlugin_init "${_IMPORT_PREFIX}/plugins/qmllint/objects-Release/QmlLintQdsPlugin_init/QmlLintQdsPlugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
