#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QuickTestUtilsPrivate" for configuration "Release"
set_property(TARGET Qt6::QuickTestUtilsPrivate APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QuickTestUtilsPrivate PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Qt6QuickTestUtils.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QuickTestUtilsPrivate )
list(APPEND _cmake_import_check_files_for_Qt6::QuickTestUtilsPrivate "${_IMPORT_PREFIX}/lib/Qt6QuickTestUtils.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
