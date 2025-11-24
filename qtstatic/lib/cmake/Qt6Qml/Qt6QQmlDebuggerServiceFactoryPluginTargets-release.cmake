#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QQmlDebuggerServiceFactoryPlugin" for configuration "Release"
set_property(TARGET Qt6::QQmlDebuggerServiceFactoryPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QQmlDebuggerServiceFactoryPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/qmltooling/qmldbg_debugger.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QQmlDebuggerServiceFactoryPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QQmlDebuggerServiceFactoryPlugin "${_IMPORT_PREFIX}/plugins/qmltooling/qmldbg_debugger.lib" )

# Import target "Qt6::QQmlDebuggerServiceFactoryPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QQmlDebuggerServiceFactoryPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QQmlDebuggerServiceFactoryPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/qmltooling/objects-Release/QQmlDebuggerServiceFactoryPlugin_init/QQmlDebuggerServiceFactoryPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QQmlDebuggerServiceFactoryPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QQmlDebuggerServiceFactoryPlugin_init "${_IMPORT_PREFIX}/plugins/qmltooling/objects-Release/QQmlDebuggerServiceFactoryPlugin_init/QQmlDebuggerServiceFactoryPlugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
