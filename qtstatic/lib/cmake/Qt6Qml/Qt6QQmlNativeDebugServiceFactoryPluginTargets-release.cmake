#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QQmlNativeDebugServiceFactoryPlugin" for configuration "Release"
set_property(TARGET Qt6::QQmlNativeDebugServiceFactoryPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QQmlNativeDebugServiceFactoryPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/qmltooling/qmldbg_nativedebugger.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QQmlNativeDebugServiceFactoryPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QQmlNativeDebugServiceFactoryPlugin "${_IMPORT_PREFIX}/plugins/qmltooling/qmldbg_nativedebugger.lib" )

# Import target "Qt6::QQmlNativeDebugServiceFactoryPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QQmlNativeDebugServiceFactoryPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QQmlNativeDebugServiceFactoryPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/qmltooling/objects-Release/QQmlNativeDebugServiceFactoryPlugin_init/QQmlNativeDebugServiceFactoryPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QQmlNativeDebugServiceFactoryPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QQmlNativeDebugServiceFactoryPlugin_init "${_IMPORT_PREFIX}/plugins/qmltooling/objects-Release/QQmlNativeDebugServiceFactoryPlugin_init/QQmlNativeDebugServiceFactoryPlugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
