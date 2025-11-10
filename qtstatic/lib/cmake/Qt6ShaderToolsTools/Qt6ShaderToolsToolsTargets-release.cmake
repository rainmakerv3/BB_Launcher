#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::qsb" for configuration "Release"
set_property(TARGET Qt6::qsb APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qsb PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/qsb.exe"
  )

list(APPEND _cmake_import_check_targets Qt6::qsb )
list(APPEND _cmake_import_check_files_for_Qt6::qsb "${_IMPORT_PREFIX}/bin/qsb.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
