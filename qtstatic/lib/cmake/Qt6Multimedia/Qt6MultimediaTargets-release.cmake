#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::Multimedia" for configuration "Release"
set_property(TARGET Qt6::Multimedia APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::Multimedia PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Qt6Multimedia.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::Multimedia )
list(APPEND _cmake_import_check_files_for_Qt6::Multimedia "${_IMPORT_PREFIX}/lib/Qt6Multimedia.lib" )

# Import target "Qt6::Multimedia_resources_1" for configuration "Release"
set_property(TARGET Qt6::Multimedia_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::Multimedia_resources_1 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/lib/objects-Release/Multimedia_resources_1/.qt/rcc/qrc_qtmultimedia_shaders_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::Multimedia_resources_1 )
list(APPEND _cmake_import_check_files_for_Qt6::Multimedia_resources_1 "${_IMPORT_PREFIX}/lib/objects-Release/Multimedia_resources_1/.qt/rcc/qrc_qtmultimedia_shaders_init.cpp.obj" )

# Import target "Qt6::Multimedia_resources_2" for configuration "Release"
set_property(TARGET Qt6::Multimedia_resources_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::Multimedia_resources_2 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/lib/objects-Release/Multimedia_resources_2/.qt/rcc/qrc_qtmultimedia_shaders_linear_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::Multimedia_resources_2 )
list(APPEND _cmake_import_check_files_for_Qt6::Multimedia_resources_2 "${_IMPORT_PREFIX}/lib/objects-Release/Multimedia_resources_2/.qt/rcc/qrc_qtmultimedia_shaders_linear_init.cpp.obj" )

# Import target "Qt6::Multimedia_resources_3" for configuration "Release"
set_property(TARGET Qt6::Multimedia_resources_3 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::Multimedia_resources_3 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/lib/objects-Release/Multimedia_resources_3/.qt/rcc/qrc_qtmultimedia_shaders_gl_macos_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::Multimedia_resources_3 )
list(APPEND _cmake_import_check_files_for_Qt6::Multimedia_resources_3 "${_IMPORT_PREFIX}/lib/objects-Release/Multimedia_resources_3/.qt/rcc/qrc_qtmultimedia_shaders_gl_macos_init.cpp.obj" )

# Import target "Qt6::Multimedia_resources_4" for configuration "Release"
set_property(TARGET Qt6::Multimedia_resources_4 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::Multimedia_resources_4 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/lib/objects-Release/Multimedia_resources_4/.qt/rcc/qrc_qtmultimedia_shaders_gl_macos_linear_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::Multimedia_resources_4 )
list(APPEND _cmake_import_check_files_for_Qt6::Multimedia_resources_4 "${_IMPORT_PREFIX}/lib/objects-Release/Multimedia_resources_4/.qt/rcc/qrc_qtmultimedia_shaders_gl_macos_linear_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
