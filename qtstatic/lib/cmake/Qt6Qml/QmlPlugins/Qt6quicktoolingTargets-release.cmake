#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::quicktooling" for configuration "Release"
set_property(TARGET Qt6::quicktooling APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::quicktooling PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/tooling/quicktoolingplugin.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::quicktooling )
list(APPEND _cmake_import_check_files_for_Qt6::quicktooling "${_IMPORT_PREFIX}/qml/QtQuick/tooling/quicktoolingplugin.lib" )

# Import target "Qt6::quicktooling_resources_1" for configuration "Release"
set_property(TARGET Qt6::quicktooling_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::quicktooling_resources_1 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/tooling/objects-Release/quicktooling_resources_1/.qt/rcc/qrc_qmake_QtQuick_tooling_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::quicktooling_resources_1 )
list(APPEND _cmake_import_check_files_for_Qt6::quicktooling_resources_1 "${_IMPORT_PREFIX}/qml/QtQuick/tooling/objects-Release/quicktooling_resources_1/.qt/rcc/qrc_qmake_QtQuick_tooling_init.cpp.obj" )

# Import target "Qt6::quicktooling_resources_2" for configuration "Release"
set_property(TARGET Qt6::quicktooling_resources_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::quicktooling_resources_2 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/tooling/objects-Release/quicktooling_resources_2/.qt/rcc/qrc_quicktooling_raw_qml_0_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::quicktooling_resources_2 )
list(APPEND _cmake_import_check_files_for_Qt6::quicktooling_resources_2 "${_IMPORT_PREFIX}/qml/QtQuick/tooling/objects-Release/quicktooling_resources_2/.qt/rcc/qrc_quicktooling_raw_qml_0_init.cpp.obj" )

# Import target "Qt6::quicktooling_init" for configuration "Release"
set_property(TARGET Qt6::quicktooling_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::quicktooling_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/tooling/objects-Release/quicktooling_init/quicktooling_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::quicktooling_init )
list(APPEND _cmake_import_check_files_for_Qt6::quicktooling_init "${_IMPORT_PREFIX}/qml/QtQuick/tooling/objects-Release/quicktooling_init/quicktooling_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
