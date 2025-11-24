// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWEBVIEWINTERFACE_H
#define QWEBVIEWINTERFACE_H

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

#include <QtWebView/qwebview_global.h>

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtGui/qimage.h>
#include <QtCore/private/qglobal_p.h>


QT_BEGIN_NAMESPACE

class QJSValue;

class QWebViewInterface
{
public:
    virtual ~QWebViewInterface() {}
    virtual QString httpUserAgent() const = 0;
    virtual void setHttpUserAgent(const QString &httpUserAgent) = 0;
    virtual void setUrl(const QUrl &url) = 0;
    virtual bool canGoBack() const = 0;
    virtual bool canGoForward() const = 0;
    virtual QString title() const = 0;
    virtual int loadProgress() const = 0;
    virtual bool isLoading() const = 0;

    // Q_SLOTS
    virtual void goBack() = 0;
    virtual void goForward() = 0;
    virtual void stop() = 0;
    virtual void reload() = 0;
    virtual void loadHtml(const QString &html, const QUrl &baseUrl = QUrl()) = 0;

    virtual void runJavaScriptPrivate(const QString &script,
                                      int callbackId) = 0;
    virtual void setCookie(const QString &domain, const QString &name, const QString &value) = 0;
    virtual void deleteCookie(const QString &domain, const QString &name) = 0;
    virtual void deleteAllCookies() = 0;

    virtual QWindow *nativeWindow() const = 0;
};

QT_END_NAMESPACE

#endif // QWEBVIEWINTERFACE_H
