QT.webview.VERSION = 6.10.0
QT.webview.name = QtWebView
QT.webview.module = Qt6WebView
QT.webview.libs = $$QT_MODULE_LIB_BASE
QT.webview.ldflags = 
QT.webview.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/QtWebView
QT.webview.frameworks = 
QT.webview.bins = $$QT_MODULE_BIN_BASE
QT.webview.plugin_types = webview DEPENDENCIES QtQuick/auto
QT.webview.depends =  core gui
QT.webview.uses = 
QT.webview.module_config = v2 staticlib
QT.webview.DEFINES = QT_WEBVIEW_LIB
QT.webview.enabled_features = webview-webview2-plugin
QT.webview.disabled_features = webview-webengine-plugin webview-android-plugin webview-darwin-plugin webview-winrt-plugin webview-wasm-plugin
QT_CONFIG += webview-webview2-plugin
QT_MODULES += webview

