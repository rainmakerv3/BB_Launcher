// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QABSTRACTWEBVIEW_P_H
#define QABSTRACTWEBVIEW_P_H

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

#include "qwebviewinterface_p.h"

QT_BEGIN_NAMESPACE

class QWebView;
class QWebViewSettings;
class QWebViewLoadRequestPrivate;

class Q_WEBVIEW_EXPORT QAbstractWebViewSettings : public QObject
{
    Q_OBJECT
public:
    virtual bool localStorageEnabled() const = 0;
    virtual bool javaScriptEnabled() const = 0;
    virtual bool localContentCanAccessFileUrls() const = 0;
    virtual bool allowFileAccess() const = 0;

    virtual void setLocalContentCanAccessFileUrls(bool) = 0;
    virtual void setJavaScriptEnabled(bool) = 0;
    virtual void setLocalStorageEnabled(bool) = 0;
    virtual void setAllowFileAccess(bool) = 0;

protected:
    explicit QAbstractWebViewSettings(QObject *p = nullptr) : QObject(p) {}
};

class Q_WEBVIEW_EXPORT QAbstractWebView
        : public QObject
{
    Q_OBJECT

public:
    virtual QAbstractWebViewSettings *getSettings() const = 0;
    virtual QString httpUserAgent() const = 0;
    virtual void setHttpUserAgent(const QString &httpUserAgent) = 0;
    virtual void setUrl(const QUrl &url) = 0;
    virtual bool canGoBack() const = 0;
    virtual bool canGoForward() const = 0;
    virtual QString title() const = 0;
    virtual int loadProgress() const = 0;
    virtual bool isLoading() const = 0;
    virtual void goBack() = 0;
    virtual void goForward() = 0;
    virtual void stop() = 0;
    virtual void reload() = 0;
    virtual void loadHtml(const QString &html, const QUrl &baseUrl) = 0;
    virtual void runJavaScriptPrivate(const QString &script, int callbackId) = 0;
    virtual void setCookie(const QString &domain, const QString &name, const QString &value) = 0;
    virtual void deleteCookie(const QString &domain, const QString &name) = 0;
    virtual void deleteAllCookies() = 0;
    virtual QWindow *nativeWindow() const = 0;
    // NOTE: This is a temporary solution for WASM and should
    // be removed once window containers are supported.
#if defined(Q_OS_WASM) || 1
    virtual void setParentView(QObject *) { }
    virtual void geometryChange(const QRectF &) { }
#endif // Q_OS_WASM

Q_SIGNALS:
    void titleChanged(const QString &title);
    void urlChanged(const QUrl &url);
    void loadingChanged(const QWebViewLoadRequestPrivate &loadRequest);
    void loadProgressChanged(int progress);
    void javaScriptResult(int id, const QVariant &result);
    void httpUserAgentChanged(const QString &httpUserAgent);
    void cookieAdded(const QString &domain, const QString &name);
    void cookieRemoved(const QString &domain, const QString &name);
    void nativeWindowChanged(QWindow *window);

protected:
    explicit QAbstractWebView(QObject *p = nullptr) : QObject(p) { }
};

QT_END_NAMESPACE

#endif // QABSTRACTWEBVIEW_P_H

