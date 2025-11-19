// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default
#ifndef QWEBSOCKETCORSAUTHENTICATOR_H
#define QWEBSOCKETCORSAUTHENTICATOR_H

#include "QtWebSockets/qwebsockets_global.h"
#include <memory>

QT_BEGIN_NAMESPACE

class QWebSocketCorsAuthenticatorPrivate;

class Q_WEBSOCKETS_EXPORT QWebSocketCorsAuthenticator
{
    Q_DECLARE_PRIVATE(QWebSocketCorsAuthenticator)

public:
    explicit QWebSocketCorsAuthenticator(const QString &origin);
    ~QWebSocketCorsAuthenticator();
    explicit QWebSocketCorsAuthenticator(const QWebSocketCorsAuthenticator &other);

    QWebSocketCorsAuthenticator(QWebSocketCorsAuthenticator &&other) noexcept;
    QWebSocketCorsAuthenticator &operator =(QWebSocketCorsAuthenticator &&other) noexcept;

    void swap(QWebSocketCorsAuthenticator &other) noexcept;

    QWebSocketCorsAuthenticator &operator =(const QWebSocketCorsAuthenticator &other);

    QString origin() const;

    void setAllowed(bool allowed);
    bool allowed() const;

private:
    std::unique_ptr<QWebSocketCorsAuthenticatorPrivate> d_ptr;
};

QT_END_NAMESPACE

#endif // QWEBSOCKETCORSAUTHENTICATOR_H
