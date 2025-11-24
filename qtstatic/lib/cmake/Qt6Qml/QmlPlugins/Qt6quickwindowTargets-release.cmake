#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::quickwindow" for configuration "Release"
set_property(TARGET Qt6::quickwindow APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::quickwindow PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Window/quickwindowplugin.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::quickwindow )
list(APPEND _cmake_import_check_files_for_Qt6::quickwindow "${_IMPORT_PREFIX}/qml/QtQuick/Window/quickwindowplugin.lib" )

# Import target "Qt6::quickwindow_resources_1" for configuration "Release"
set_property(TARGET Qt6::quickwindow_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::quickwindow_resources_1 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Window/objects-Release/quickwindow_resources_1/.qt/rcc/qrc_qmake_QtQuick_Window_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::quickwindow_resources_1 )
list(APPEND _cmake_import_check_files_for_Qt6::quickwindow_resources_1 "${_IMPORT_PREFIX}/qml/QtQuick/Window/objects-Release/quickwindow_resources_1/.qt/rcc/qrc_qmake_QtQuick_Window_init.cpp.obj" )

# Import target "Qt6::quickwindow_init" for configuration "Release"
set_property(TARGET Qt6::quickwindow_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::quickwindow_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Window/objects-Release/quickwindow_init/quickwindow_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::quickwindow_init )
list(APPEND _cmake_import_check_files_for_Qt6::quickwindow_init "${_IMPORT_PREFIX}/qml/QtQuick/Window/objects-Release/quickwindow_init/quickwindow_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
