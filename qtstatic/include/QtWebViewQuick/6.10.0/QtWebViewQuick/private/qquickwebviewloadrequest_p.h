// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKWEBVIEWREQUEST_H
#define QQUICKWEBVIEWREQUEST_H

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
#include <QtWebViewQuick/private/qquickwebview_p.h>
#include <QtQml/qqmlregistration.h>

QT_BEGIN_NAMESPACE

class QWebViewLoadRequestPrivate;

class Q_WEBVIEWQUICK_EXPORT QQuickWebViewLoadRequest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url)
    Q_PROPERTY(QQuickWebView::LoadStatus status READ status)
    Q_PROPERTY(QString errorString READ errorString)
    QML_NAMED_ELEMENT(WebViewLoadRequest)
    QML_ADDED_IN_VERSION(1, 1)
    QML_EXTRA_VERSION(2, 0)
    QML_UNCREATABLE("")

public:
    ~QQuickWebViewLoadRequest();

    QUrl url() const;
    QQuickWebView::LoadStatus status() const;
    QString errorString() const;

private:
    friend class QQuickWebView;
    explicit QQuickWebViewLoadRequest(const QWebViewLoadRequestPrivate &d);
    Q_DECLARE_PRIVATE(QWebViewLoadRequest)
    QScopedPointer<QWebViewLoadRequestPrivate> d_ptr;
};

QT_END_NAMESPACE

#endif // QQUICKWEBVIEWREQUEST_H
