#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QuickWidgets" for configuration "Release"
set_property(TARGET Qt6::QuickWidgets APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QuickWidgets PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Qt6QuickWidgets.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QuickWidgets )
list(APPEND _cmake_import_check_files_for_Qt6::QuickWidgets "${_IMPORT_PREFIX}/lib/Qt6QuickWidgets.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
