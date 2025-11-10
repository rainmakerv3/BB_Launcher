#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::qhelpgenerator" for configuration "Release"
set_property(TARGET Qt6::qhelpgenerator APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qhelpgenerator PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/qhelpgenerator.exe"
  )

list(APPEND _cmake_import_check_targets Qt6::qhelpgenerator )
list(APPEND _cmake_import_check_files_for_Qt6::qhelpgenerator "${_IMPORT_PREFIX}/bin/qhelpgenerator.exe" )

# Import target "Qt6::qtattributionsscanner" for configuration "Release"
set_property(TARGET Qt6::qtattributionsscanner APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qtattributionsscanner PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/qtattributionsscanner.exe"
  )

list(APPEND _cmake_import_check_targets Qt6::qtattributionsscanner )
list(APPEND _cmake_import_check_files_for_Qt6::qtattributionsscanner "${_IMPORT_PREFIX}/bin/qtattributionsscanner.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
