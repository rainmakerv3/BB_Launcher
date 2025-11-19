// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERWEBSOCKETUPGRADERESPONSE_H
#define QHTTPSERVERWEBSOCKETUPGRADERESPONSE_H

#include <QtHttpServer/qthttpserverglobal.h>

#include <QtCore/qbytearray.h>

#include <algorithm>

QT_BEGIN_NAMESPACE

class QHttpServerWebSocketUpgradeResponsePrivate;
class QHttpServerWebSocketUpgradeResponse final
{

public:
    Q_HTTPSERVER_EXPORT
    QHttpServerWebSocketUpgradeResponse(const QHttpServerWebSocketUpgradeResponse &other);
    QHttpServerWebSocketUpgradeResponse(QHttpServerWebSocketUpgradeResponse &&other) noexcept
        : responseType(std::move(other.responseType))
        , errorStatus(std::move(other.errorStatus))
        , errorMessage(std::move(other.errorMessage))
        , reserved(std::exchange(other.reserved, nullptr))
    {
    }

    Q_HTTPSERVER_EXPORT ~QHttpServerWebSocketUpgradeResponse();
    Q_HTTPSERVER_EXPORT QHttpServerWebSocketUpgradeResponse &
    operator=(const QHttpServerWebSocketUpgradeResponse &other);
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QHttpServerWebSocketUpgradeResponse)

    void swap(QHttpServerWebSocketUpgradeResponse &other) noexcept
    {
        std::swap(responseType, other.responseType);
        std::swap(errorStatus, other.errorStatus);
        errorMessage.swap(other.errorMessage);
        std::swap(reserved, other.reserved);
    }

    enum class ResponseType {
        Accept,
        Deny,
        PassToNext,
    };

    [[nodiscard]] ResponseType type() const { return responseType; };
    [[nodiscard]] int denyStatus() const { return errorStatus; }
    [[nodiscard]] const QByteArray &denyMessage() const & { return errorMessage; }
    [[nodiscard]] QByteArray denyMessage() && { return std::move(errorMessage); }

    Q_HTTPSERVER_EXPORT static QHttpServerWebSocketUpgradeResponse accept();
    Q_HTTPSERVER_EXPORT static QHttpServerWebSocketUpgradeResponse deny();
    Q_HTTPSERVER_EXPORT static QHttpServerWebSocketUpgradeResponse deny(int status,
                                                                        QByteArray message);
    Q_HTTPSERVER_EXPORT static QHttpServerWebSocketUpgradeResponse passToNext();

private:
    QHttpServerWebSocketUpgradeResponse() = delete;
    Q_IMPLICIT QHttpServerWebSocketUpgradeResponse(ResponseType type);
    QHttpServerWebSocketUpgradeResponse(ResponseType type, int status, QByteArray message);

    ResponseType responseType;
    int errorStatus = 403;
    QByteArray errorMessage;
    void *reserved = nullptr;
};

Q_DECLARE_SHARED(QHttpServerWebSocketUpgradeResponse)

QT_END_NAMESPACE

#endif // QHTTPSERVERWEBSOCKETUPGRADERESPONSE_H
