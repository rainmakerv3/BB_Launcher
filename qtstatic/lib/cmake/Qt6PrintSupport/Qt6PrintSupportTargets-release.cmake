#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::PrintSupport" for configuration "Release"
set_property(TARGET Qt6::PrintSupport APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::PrintSupport PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Qt6PrintSupport.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::PrintSupport )
list(APPEND _cmake_import_check_files_for_Qt6::PrintSupport "${_IMPORT_PREFIX}/lib/Qt6PrintSupport.lib" )

# Import target "Qt6::PrintSupport_resources_1" for configuration "Release"
set_property(TARGET Qt6::PrintSupport_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::PrintSupport_resources_1 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/lib/objects-Release/PrintSupport_resources_1/.qt/rcc/qrc_qprintdialog_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::PrintSupport_resources_1 )
list(APPEND _cmake_import_check_files_for_Qt6::PrintSupport_resources_1 "${_IMPORT_PREFIX}/lib/objects-Release/PrintSupport_resources_1/.qt/rcc/qrc_qprintdialog_init.cpp.obj" )

# Import target "Qt6::PrintSupport_resources_2" for configuration "Release"
set_property(TARGET Qt6::PrintSupport_resources_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::PrintSupport_resources_2 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/lib/objects-Release/PrintSupport_resources_2/.qt/rcc/qrc_qprintdialog1_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::PrintSupport_resources_2 )
list(APPEND _cmake_import_check_files_for_Qt6::PrintSupport_resources_2 "${_IMPORT_PREFIX}/lib/objects-Release/PrintSupport_resources_2/.qt/rcc/qrc_qprintdialog1_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
