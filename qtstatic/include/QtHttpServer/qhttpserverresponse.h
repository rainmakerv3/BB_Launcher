// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERRESPONSE_H
#define QHTTPSERVERRESPONSE_H

#include <QtHttpServer/qhttpserverresponder.h>
#include <QtNetwork/qhttpheaders.h>

QT_BEGIN_NAMESPACE

class QJsonObject;

class QHttpServerResponsePrivate;
class QHttpServerResponse final
{
    Q_DECLARE_PRIVATE(QHttpServerResponse)
    Q_DISABLE_COPY(QHttpServerResponse)

    friend class QHttpServerResponder;
public:
    using StatusCode = QHttpServerResponder::StatusCode;

    QHttpServerResponse(QHttpServerResponse &&other) noexcept
        : d_ptr(std::exchange(other.d_ptr, nullptr)) {}
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QHttpServerResponse)
    void swap(QHttpServerResponse &other) noexcept { qt_ptr_swap(d_ptr, other.d_ptr); }

    Q_HTTPSERVER_EXPORT Q_IMPLICIT QHttpServerResponse(StatusCode statusCode);

    Q_HTTPSERVER_EXPORT Q_IMPLICIT QHttpServerResponse(const char *data,
                                                       StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT Q_IMPLICIT QHttpServerResponse(const QString &data,
                                                       StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT Q_IMPLICIT QHttpServerResponse(const QByteArray &data,
                                                       StatusCode status = StatusCode::Ok);
    Q_HTTPSERVER_EXPORT Q_IMPLICIT QHttpServerResponse(QByteArray &&data,
                                                       StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT Q_IMPLICIT QHttpServerResponse(const QJsonObject &data,
                                                       StatusCode status = StatusCode::Ok);
    Q_HTTPSERVER_EXPORT Q_IMPLICIT QHttpServerResponse(const QJsonArray &data,
                                                       StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT Q_IMPLICIT QHttpServerResponse(const QByteArray &mimeType,
                                                       const QByteArray &data,
                                                       StatusCode status = StatusCode::Ok);
    Q_HTTPSERVER_EXPORT Q_IMPLICIT QHttpServerResponse(const QByteArray &mimeType,
                                                       QByteArray &&data,
                                                       StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT ~QHttpServerResponse();
    Q_HTTPSERVER_EXPORT static QHttpServerResponse fromFile(const QString &fileName);

    Q_HTTPSERVER_EXPORT QByteArray data() const;

    Q_HTTPSERVER_EXPORT QByteArray mimeType() const;

    Q_HTTPSERVER_EXPORT StatusCode statusCode() const;

    Q_HTTPSERVER_EXPORT QHttpHeaders headers() const;
    Q_HTTPSERVER_EXPORT void setHeaders(const QHttpHeaders &newHeaders);
    Q_HTTPSERVER_EXPORT void setHeaders(QHttpHeaders &&newHeaders);

private:
    QHttpServerResponsePrivate *d_ptr;
};

QT_END_NAMESPACE

#endif   // QHTTPSERVERRESPONSE_H
