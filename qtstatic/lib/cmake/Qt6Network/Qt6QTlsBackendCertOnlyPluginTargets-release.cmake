#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QTlsBackendCertOnlyPlugin" for configuration "Release"
set_property(TARGET Qt6::QTlsBackendCertOnlyPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QTlsBackendCertOnlyPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/tls/qcertonlybackend.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QTlsBackendCertOnlyPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QTlsBackendCertOnlyPlugin "${_IMPORT_PREFIX}/plugins/tls/qcertonlybackend.lib" )

# Import target "Qt6::QTlsBackendCertOnlyPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QTlsBackendCertOnlyPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QTlsBackendCertOnlyPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/tls/objects-Release/QTlsBackendCertOnlyPlugin_init/QTlsBackendCertOnlyPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QTlsBackendCertOnlyPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QTlsBackendCertOnlyPlugin_init "${_IMPORT_PREFIX}/plugins/tls/objects-Release/QTlsBackendCertOnlyPlugin_init/QTlsBackendCertOnlyPlugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
