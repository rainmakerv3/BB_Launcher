#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QuickControls2Universal" for configuration "Release"
set_property(TARGET Qt6::QuickControls2Universal APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QuickControls2Universal PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Qt6QuickControls2Universal.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QuickControls2Universal )
list(APPEND _cmake_import_check_files_for_Qt6::QuickControls2Universal "${_IMPORT_PREFIX}/lib/Qt6QuickControls2Universal.lib" )

# Import target "Qt6::QuickControls2Universal_resources_1" for configuration "Release"
set_property(TARGET Qt6::QuickControls2Universal_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QuickControls2Universal_resources_1 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Universal/objects-Release/QuickControls2Universal_resources_1/.qt/rcc/qrc_qmake_QtQuick_Controls_Universal_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QuickControls2Universal_resources_1 )
list(APPEND _cmake_import_check_files_for_Qt6::QuickControls2Universal_resources_1 "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Universal/objects-Release/QuickControls2Universal_resources_1/.qt/rcc/qrc_qmake_QtQuick_Controls_Universal_init.cpp.obj" )

# Import target "Qt6::QuickControls2Universal_resources_2" for configuration "Release"
set_property(TARGET Qt6::QuickControls2Universal_resources_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QuickControls2Universal_resources_2 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Universal/objects-Release/QuickControls2Universal_resources_2/.qt/rcc/qrc_QuickControls2Universal_raw_qml_0_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QuickControls2Universal_resources_2 )
list(APPEND _cmake_import_check_files_for_Qt6::QuickControls2Universal_resources_2 "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Universal/objects-Release/QuickControls2Universal_resources_2/.qt/rcc/qrc_QuickControls2Universal_raw_qml_0_init.cpp.obj" )

# Import target "Qt6::QuickControls2Universal_resources_3" for configuration "Release"
set_property(TARGET Qt6::QuickControls2Universal_resources_3 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QuickControls2Universal_resources_3 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/lib/objects-Release/QuickControls2Universal_resources_3/.qt/rcc/qrc_qtquickcontrols2universalstyle_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QuickControls2Universal_resources_3 )
list(APPEND _cmake_import_check_files_for_Qt6::QuickControls2Universal_resources_3 "${_IMPORT_PREFIX}/lib/objects-Release/QuickControls2Universal_resources_3/.qt/rcc/qrc_qtquickcontrols2universalstyle_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
