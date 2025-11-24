#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QuickControlsTestUtilsPrivateplugin" for configuration "Release"
set_property(TARGET Qt6::QuickControlsTestUtilsPrivateplugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QuickControlsTestUtilsPrivateplugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/qml/Qt/test/controls/quickcontrolstestutilsprivateplugin.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QuickControlsTestUtilsPrivateplugin )
list(APPEND _cmake_import_check_files_for_Qt6::QuickControlsTestUtilsPrivateplugin "${_IMPORT_PREFIX}/qml/Qt/test/controls/quickcontrolstestutilsprivateplugin.lib" )

# Import target "Qt6::QuickControlsTestUtilsPrivateplugin_init" for configuration "Release"
set_property(TARGET Qt6::QuickControlsTestUtilsPrivateplugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QuickControlsTestUtilsPrivateplugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/Qt/test/controls/objects-Release/QuickControlsTestUtilsPrivateplugin_init/QuickControlsTestUtilsPrivateplugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QuickControlsTestUtilsPrivateplugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QuickControlsTestUtilsPrivateplugin_init "${_IMPORT_PREFIX}/qml/Qt/test/controls/objects-Release/QuickControlsTestUtilsPrivateplugin_init/QuickControlsTestUtilsPrivateplugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
