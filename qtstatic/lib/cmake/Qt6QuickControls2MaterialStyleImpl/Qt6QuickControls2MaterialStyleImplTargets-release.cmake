#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QuickControls2MaterialStyleImpl" for configuration "Release"
set_property(TARGET Qt6::QuickControls2MaterialStyleImpl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QuickControls2MaterialStyleImpl PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Qt6QuickControls2MaterialStyleImpl.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QuickControls2MaterialStyleImpl )
list(APPEND _cmake_import_check_files_for_Qt6::QuickControls2MaterialStyleImpl "${_IMPORT_PREFIX}/lib/Qt6QuickControls2MaterialStyleImpl.lib" )

# Import target "Qt6::QuickControls2MaterialStyleImpl_resources_1" for configuration "Release"
set_property(TARGET Qt6::QuickControls2MaterialStyleImpl_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QuickControls2MaterialStyleImpl_resources_1 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Material/impl/objects-Release/QuickControls2MaterialStyleImpl_resources_1/.qt/rcc/qrc_qmake_QtQuick_Controls_Material_impl_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QuickControls2MaterialStyleImpl_resources_1 )
list(APPEND _cmake_import_check_files_for_Qt6::QuickControls2MaterialStyleImpl_resources_1 "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Material/impl/objects-Release/QuickControls2MaterialStyleImpl_resources_1/.qt/rcc/qrc_qmake_QtQuick_Controls_Material_impl_init.cpp.obj" )

# Import target "Qt6::QuickControls2MaterialStyleImpl_resources_2" for configuration "Release"
set_property(TARGET Qt6::QuickControls2MaterialStyleImpl_resources_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QuickControls2MaterialStyleImpl_resources_2 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Material/impl/objects-Release/QuickControls2MaterialStyleImpl_resources_2/.qt/rcc/qrc_QuickControls2MaterialStyleImpl_raw_qml_0_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QuickControls2MaterialStyleImpl_resources_2 )
list(APPEND _cmake_import_check_files_for_Qt6::QuickControls2MaterialStyleImpl_resources_2 "${_IMPORT_PREFIX}/qml/QtQuick/Controls/Material/impl/objects-Release/QuickControls2MaterialStyleImpl_resources_2/.qt/rcc/qrc_QuickControls2MaterialStyleImpl_raw_qml_0_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
