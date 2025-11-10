#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::BundledSpirv_Cross" for configuration "Release"
set_property(TARGET Qt6::BundledSpirv_Cross APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::BundledSpirv_Cross PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Qt6BundledSpirv_Cross.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::BundledSpirv_Cross )
list(APPEND _cmake_import_check_files_for_Qt6::BundledSpirv_Cross "${_IMPORT_PREFIX}/lib/Qt6BundledSpirv_Cross.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
