#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::WebViewQuick" for configuration "Release"
set_property(TARGET Qt6::WebViewQuick APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::WebViewQuick PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Qt6WebViewQuick.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::WebViewQuick )
list(APPEND _cmake_import_check_files_for_Qt6::WebViewQuick "${_IMPORT_PREFIX}/lib/Qt6WebViewQuick.lib" )

# Import target "Qt6::WebViewQuick_resources_1" for configuration "Release"
set_property(TARGET Qt6::WebViewQuick_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::WebViewQuick_resources_1 PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/qml/QtWebView/objects-Release/WebViewQuick_resources_1/.qt/rcc/qrc_qmake_QtWebView_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::WebViewQuick_resources_1 )
list(APPEND _cmake_import_check_files_for_Qt6::WebViewQuick_resources_1 "${_IMPORT_PREFIX}/qml/QtWebView/objects-Release/WebViewQuick_resources_1/.qt/rcc/qrc_qmake_QtWebView_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
