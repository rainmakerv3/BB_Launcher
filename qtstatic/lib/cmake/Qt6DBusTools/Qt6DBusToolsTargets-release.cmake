#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::qdbuscpp2xml" for configuration "Release"
set_property(TARGET Qt6::qdbuscpp2xml APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qdbuscpp2xml PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/qdbuscpp2xml.exe"
  )

list(APPEND _cmake_import_check_targets Qt6::qdbuscpp2xml )
list(APPEND _cmake_import_check_files_for_Qt6::qdbuscpp2xml "${_IMPORT_PREFIX}/bin/qdbuscpp2xml.exe" )

# Import target "Qt6::qdbusxml2cpp" for configuration "Release"
set_property(TARGET Qt6::qdbusxml2cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::qdbusxml2cpp PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/qdbusxml2cpp.exe"
  )

list(APPEND _cmake_import_check_targets Qt6::qdbusxml2cpp )
list(APPEND _cmake_import_check_files_for_Qt6::qdbusxml2cpp "${_IMPORT_PREFIX}/bin/qdbusxml2cpp.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
