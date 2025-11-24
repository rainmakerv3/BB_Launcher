import QtQuick
import QtWebView

Rectangle {
    id: myRect
    width: 1200
    height: 1200

    WebView {
        id: myView
        anchors.fill: parent // Makes the WebView fill its parent's area
        objectName: "currentWebView" // Needed for C++ findChild
        url: "https://www.qt.io"
    }

    Connections{
        target: helper
        onTextChanged: myView.url = text
    }
}


