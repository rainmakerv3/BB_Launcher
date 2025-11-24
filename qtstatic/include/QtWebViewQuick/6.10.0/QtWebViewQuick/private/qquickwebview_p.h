// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKWEBVIEW_H
#define QQUICKWEBVIEW_H

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
#include <QtWebView/private/qwebview_p.h>
#include <QtQml/qqmlregistration.h>

#include <QtQuick/private/qquickwindowcontainer_p.h>

Q_MOC_INCLUDE(<QtWebViewQuick/private/qquickwebviewloadrequest_p.h>)
Q_MOC_INCLUDE(<QtWebViewQuick/private/qquickwebviewsettings_p.h>)
Q_MOC_INCLUDE(<QtWebView/private/qwebviewloadrequest_p.h>)

QT_BEGIN_NAMESPACE

class QQuickWebViewLoadRequest;
class QWebViewLoadRequestPrivate;
class QQuickWebViewSettings;

class Q_WEBVIEWQUICK_EXPORT QQuickWebView : public QQuickWindowContainer
{
    Q_OBJECT
    Q_PROPERTY(QString httpUserAgent READ httpUserAgent WRITE setHttpUserAgent NOTIFY
                       httpUserAgentChanged FINAL REVISION(1, 14))
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged FINAL)
    Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged FINAL REVISION(1, 1))
    Q_PROPERTY(int loadProgress READ loadProgress NOTIFY loadProgressChanged FINAL)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged FINAL)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY loadingChanged FINAL)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY loadingChanged FINAL)
    Q_PROPERTY(QQuickWebViewSettings *settings READ settings CONSTANT FINAL REVISION(6, 5))
    Q_ENUMS(LoadStatus)
    QML_NAMED_ELEMENT(WebView)
    QML_ADDED_IN_VERSION(1, 0)
    QML_EXTRA_VERSION(2, 0)

public:
    enum LoadStatus { // Changes here needs to be done in QWebView as well
        LoadStartedStatus,
        LoadStoppedStatus,
        LoadSucceededStatus,
        LoadFailedStatus
    };
    QQuickWebView(QQuickItem *parent = nullptr);
    ~QQuickWebView();

    QString httpUserAgent() const ;
    void setHttpUserAgent(const QString &userAgent);
    QUrl url() const;
    QWebView &webView() const { return *m_webView; };
    void setUrl(const QUrl &url);
    int loadProgress() const;
    QString title() const;
    bool canGoBack() const;
    bool isLoading() const;
    bool canGoForward() const;
    QWindow *nativeWindow();
    QQuickWebViewSettings *settings() const;

public Q_SLOTS:
    void goBack();
    void goForward();
    void reload();
    void stop();
    Q_REVISION(1, 1) void loadHtml(const QString &html, const QUrl &baseUrl = QUrl());
    Q_REVISION(1, 1)
    void runJavaScript(const QString &script, const QJSValue &callback = QJSValue());
    Q_REVISION(6, 3) void setCookie(const QString &domain, const QString &name, const QString &value);
    Q_REVISION(6, 3) void deleteCookie(const QString &domain, const QString &name);
    Q_REVISION(6, 3) void deleteAllCookies();

Q_SIGNALS:
    void titleChanged();
    void urlChanged();
    Q_REVISION(1, 1) void loadingChanged(QQuickWebViewLoadRequest *loadRequest);
    void loadProgressChanged();
    Q_REVISION(1, 14) void httpUserAgentChanged();
    Q_REVISION(6, 3) void cookieAdded(const QString &domain, const QString &name);
    Q_REVISION(6, 3) void cookieRemoved(const QString &domain, const QString &name);

protected:
#if defined(Q_OS_WASM)
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
#endif // Q_OS_WASM
    void itemChange(ItemChange change, const ItemChangeData &value) override;
    void runJavaScriptPrivate(const QString &script, int callbackId);

private Q_SLOTS:
    void onRunJavaScriptResult(int id, const QVariant &variant);
    void onLoadingChanged(const QWebViewLoadRequestPrivate &loadRequest);
    void onNativeWindowChanged(QWindow *window);

private:
    friend class QWebEngineWebViewPrivate;
    static QJSValue takeCallback(int id);

    QWebView *m_webView;
    QQuickWebViewSettings *m_settings;
};

QT_END_NAMESPACE

#endif // QQUICKWEBVIEW_H
