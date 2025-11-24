#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QQmlInspectorServiceFactoryPlugin" for configuration "Release"
set_property(TARGET Qt6::QQmlInspectorServiceFactoryPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QQmlInspectorServiceFactoryPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/qmltooling/qmldbg_inspector.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QQmlInspectorServiceFactoryPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QQmlInspectorServiceFactoryPlugin "${_IMPORT_PREFIX}/plugins/qmltooling/qmldbg_inspector.lib" )

# Import target "Qt6::QQmlInspectorServiceFactoryPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QQmlInspectorServiceFactoryPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QQmlInspectorServiceFactoryPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/qmltooling/objects-Release/QQmlInspectorServiceFactoryPlugin_init/QQmlInspectorServiceFactoryPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QQmlInspectorServiceFactoryPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QQmlInspectorServiceFactoryPlugin_init "${_IMPORT_PREFIX}/plugins/qmltooling/objects-Release/QQmlInspectorServiceFactoryPlugin_init/QQmlInspectorServiceFactoryPlugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
