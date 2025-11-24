#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QmlCore" for configuration "Release"
set_property(TARGET Qt6::QmlCore APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QmlCore PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Qt6QmlCore.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QmlCore )
list(APPEND _cmake_import_check_files_for_Qt6::QmlCore "${_IMPORT_PREFIX}/lib/Qt6QmlCore.lib" )

# Import target "Qt6::QmlCore_resources_1" for configuration "Release"
set_property(TARGET Qt6::QmlCore_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QmlCore_resources_1 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtCore/objects-Release/QmlCore_resources_1/.qt/rcc/qrc_qmake_QtCore_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QmlCore_resources_1 )
list(APPEND _cmake_import_check_files_for_Qt6::QmlCore_resources_1 "${_IMPORT_PREFIX}/qml/QtCore/objects-Release/QmlCore_resources_1/.qt/rcc/qrc_qmake_QtCore_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
