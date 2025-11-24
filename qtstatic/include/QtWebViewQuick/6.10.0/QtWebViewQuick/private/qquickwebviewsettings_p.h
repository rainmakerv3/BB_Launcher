// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKWEBVIEWSETTINGS_H
#define QQUICKWEBVIEWSETTINGS_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtWebViewQuick/private/qtwebviewquickglobal_p.h>
#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>
#include <QtWebView/private/qwebview_p.h>
#include <QtCore/qpointer.h>

QT_BEGIN_NAMESPACE

class QWebView;
class QWebViewSettings;

class Q_WEBVIEWQUICK_EXPORT QQuickWebViewSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool localStorageEnabled READ localStorageEnabled WRITE setLocalStorageEnabled NOTIFY localStorageEnabledChanged)
    Q_PROPERTY(bool javaScriptEnabled READ javaScriptEnabled WRITE setJavaScriptEnabled NOTIFY javaScriptEnabledChanged)
    Q_PROPERTY(bool allowFileAccess READ allowFileAccess WRITE setAllowFileAccess NOTIFY allowFileAccessChanged)
    Q_PROPERTY(bool localContentCanAccessFileUrls READ localContentCanAccessFileUrls WRITE setLocalContentCanAccessFileUrls NOTIFY localContentCanAccessFileUrlsChanged)
    QML_NAMED_ELEMENT(WebViewSettings)
    QML_ADDED_IN_VERSION(6, 5)
    QML_UNCREATABLE("")

public:
    ~QQuickWebViewSettings() override;

    bool localStorageEnabled() const;
    bool javaScriptEnabled() const;
    bool localContentCanAccessFileUrls() const;
    bool allowFileAccess() const;

public Q_SLOTS:
    void setLocalStorageEnabled(bool enabled);
    void setJavaScriptEnabled(bool enabled);
    void setAllowFileAccess(bool enabled);
    void setLocalContentCanAccessFileUrls(bool enabled);

Q_SIGNALS:
    void localStorageEnabledChanged();
    void javaScriptEnabledChanged();
    void allowFileAccessChanged();
    void localContentCanAccessFileUrlsChanged();

private:
    friend class QQuickWebView;

    explicit QQuickWebViewSettings(QWebViewSettings *webviewsettings, QObject *p = nullptr);
    QPointer<QWebViewSettings> d;
    bool m_allowFileAccess;
};

QT_END_NAMESPACE

#endif // QQUICKWEBVIEWSETTINGS_H
