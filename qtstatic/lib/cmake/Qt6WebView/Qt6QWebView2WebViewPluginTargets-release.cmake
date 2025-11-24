#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QWebView2WebViewPlugin" for configuration "Release"
set_property(TARGET Qt6::QWebView2WebViewPlugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QWebView2WebViewPlugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/plugins/webview/qtwebview_webview2.lib"
  )

list(APPEND _cmake_import_check_targets Qt6::QWebView2WebViewPlugin )
list(APPEND _cmake_import_check_files_for_Qt6::QWebView2WebViewPlugin "${_IMPORT_PREFIX}/plugins/webview/qtwebview_webview2.lib" )

# Import target "Qt6::QWebView2WebViewPlugin_init" for configuration "Release"
set_property(TARGET Qt6::QWebView2WebViewPlugin_init APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Qt6::QWebView2WebViewPlugin_init PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE ""
  IMPORTED_OBJECTS_RELEASE "${_IMPORT_PREFIX}/plugins/webview/objects-Release/QWebView2WebViewPlugin_init/QWebView2WebViewPlugin_init.cpp.obj"
  )

list(APPEND _cmake_import_check_targets Qt6::QWebView2WebViewPlugin_init )
list(APPEND _cmake_import_check_files_for_Qt6::QWebView2WebViewPlugin_init "${_IMPORT_PREFIX}/plugins/webview/objects-Release/QWebView2WebViewPlugin_init/QWebView2WebViewPlugin_init.cpp.obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
