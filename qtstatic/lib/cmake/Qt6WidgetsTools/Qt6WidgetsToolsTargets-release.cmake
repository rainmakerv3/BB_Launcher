#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::uic" for configuration "Release"
set_property(TARGET Qt6::uic APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::uic PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/uic.exe"
  )

list(APPEND _cmake_import_check_targets Qt6::uic )
list(APPEND _cmake_import_check_files_for_Qt6::uic "${_IMPORT_PREFIX}/bin/uic.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
