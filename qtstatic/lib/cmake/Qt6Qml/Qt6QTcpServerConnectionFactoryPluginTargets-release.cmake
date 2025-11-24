#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QTcpServerConnectionFactoryPlugin" for configuration "Release"
set_property(TARGET Qt6::QTcpServerConnectionFactoryPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QTcpServerConnectionFactoryPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/qmltooling/qmldbg_tcp.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QTcpServerConnectionFactoryPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QTcpServerConnectionFactoryPlugin "${_IMPORT_PREFIX}/plugins/qmltooling/qmldbg_tcp.lib" )

# Import target "Qt6::QTcpServerConnectionFactoryPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QTcpServerConnectionFactoryPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QTcpServerConnectionFactoryPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/qmltooling/objects-Release/QTcpServerConnectionFactoryPlugin_init/QTcpServerConnectionFactoryPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QTcpServerConnectionFactoryPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QTcpServerConnectionFactoryPlugin_init "${_IMPORT_PREFIX}/plugins/qmltooling/objects-Release/QTcpServerConnectionFactoryPlugin_init/QTcpServerConnectionFactoryPlugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
