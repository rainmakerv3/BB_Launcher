#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QGeoPositionInfoSourceFactoryNmeaPlugin" for configuration "Release"
set_property(TARGET Qt6::QGeoPositionInfoSourceFactoryNmeaPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QGeoPositionInfoSourceFactoryNmeaPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/position/qtposition_nmea.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QGeoPositionInfoSourceFactoryNmeaPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QGeoPositionInfoSourceFactoryNmeaPlugin "${_IMPORT_PREFIX}/plugins/position/qtposition_nmea.lib" )

# Import target "Qt6::QGeoPositionInfoSourceFactoryNmeaPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QGeoPositionInfoSourceFactoryNmeaPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QGeoPositionInfoSourceFactoryNmeaPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/position/objects-Release/QGeoPositionInfoSourceFactoryNmeaPlugin_init/QGeoPositionInfoSourceFactoryNmeaPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QGeoPositionInfoSourceFactoryNmeaPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QGeoPositionInfoSourceFactoryNmeaPlugin_init "${_IMPORT_PREFIX}/plugins/position/objects-Release/QGeoPositionInfoSourceFactoryNmeaPlugin_init/QGeoPositionInfoSourceFactoryNmeaPlugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
