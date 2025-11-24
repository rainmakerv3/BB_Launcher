import QtQuick
import QtWebView

Rectangle {
    id: webViewContainer
    width: 1200
    height: 1200

    WebView {
        anchors.fill: parent
        url: "https://www.example.com"
    }
}
