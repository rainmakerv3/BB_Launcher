#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::qmlwebsockets" for configuration "Release"
set_property(TARGET Qt6::qmlwebsockets APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qmlwebsockets PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/qml/QtWebSockets/qmlwebsocketsplugin.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::qmlwebsockets )
list(APPEND _cmake_import_check_files_for_Qt6::qmlwebsockets "${_IMPORT_PREFIX}/qml/QtWebSockets/qmlwebsocketsplugin.lib" )

# Import target "Qt6::qmlwebsockets_resources_1" for configuration "Release"
set_property(TARGET Qt6::qmlwebsockets_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qmlwebsockets_resources_1 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtWebSockets/objects-Release/qmlwebsockets_resources_1/.qt/rcc/qrc_qmake_QtWebSockets_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::qmlwebsockets_resources_1 )
list(APPEND _cmake_import_check_files_for_Qt6::qmlwebsockets_resources_1 "${_IMPORT_PREFIX}/qml/QtWebSockets/objects-Release/qmlwebsockets_resources_1/.qt/rcc/qrc_qmake_QtWebSockets_init.cpp.obj" )

# Import target "Qt6::qmlwebsockets_init" for configuration "Release"
set_property(TARGET Qt6::qmlwebsockets_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qmlwebsockets_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtWebSockets/objects-Release/qmlwebsockets_init/qmlwebsockets_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::qmlwebsockets_init )
list(APPEND _cmake_import_check_files_for_Qt6::qmlwebsockets_init "${_IMPORT_PREFIX}/qml/QtWebSockets/objects-Release/qmlwebsockets_init/qmlwebsockets_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
