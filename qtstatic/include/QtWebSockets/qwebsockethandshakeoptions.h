// Copyright (C) 2022 Menlo Systems GmbH, author Arno Rehn <a.rehn@menlosystems.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBSOCKETHANDSHAKEOPTIONS_H
#define QWEBSOCKETHANDSHAKEOPTIONS_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QStringList>

#include "QtWebSockets/qwebsockets_global.h"

QT_BEGIN_NAMESPACE

class QWebSocketHandshakeOptionsPrivate;

QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QWebSocketHandshakeOptionsPrivate, Q_WEBSOCKETS_EXPORT)

class Q_WEBSOCKETS_EXPORT QWebSocketHandshakeOptions
{
public:
    QWebSocketHandshakeOptions();
    QWebSocketHandshakeOptions(const QWebSocketHandshakeOptions &other);
    QWebSocketHandshakeOptions(QWebSocketHandshakeOptions &&other) noexcept = default;
    ~QWebSocketHandshakeOptions();

    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QWebSocketHandshakeOptions)
    QWebSocketHandshakeOptions &operator=(const QWebSocketHandshakeOptions &other);

    void swap(QWebSocketHandshakeOptions &other) noexcept { d.swap(other.d); }

    QStringList subprotocols() const;
    void setSubprotocols(const QStringList &protocols);

private:
    bool equals(const QWebSocketHandshakeOptions &other) const;

    friend bool operator==(const QWebSocketHandshakeOptions &lhs,
                           const QWebSocketHandshakeOptions &rhs) { return lhs.equals(rhs); }
    friend bool operator!=(const QWebSocketHandshakeOptions &lhs,
                           const QWebSocketHandshakeOptions &rhs) { return !lhs.equals(rhs); }

    QSharedDataPointer<QWebSocketHandshakeOptionsPrivate> d;
    friend class QWebSocketHandshakeOptionsPrivate;
};

QT_END_NAMESPACE

#endif // QWEBSOCKETHANDSHAKEOPTIONS_H
