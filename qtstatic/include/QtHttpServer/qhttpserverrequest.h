// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERREQUEST_H
#define QHTTPSERVERREQUEST_H

#include <QtHttpServer/qthttpserverglobal.h>

#include <QtCore/qglobal.h>
#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>
#include <QtNetwork/qhostaddress.h>
#if QT_CONFIG(ssl)
#include <QtNetwork/qsslconfiguration.h>
#endif

#include <memory>

QT_BEGIN_NAMESPACE

class QRegularExpression;
class QString;
class QHttpHeaders;
class QHttpServerParser;

class QHttpServerRequestPrivate;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR(QHttpServerRequestPrivate)

class QHttpServerRequest final
{
    friend class QHttpServerResponse;
    friend class QHttpServerParser;
    friend class QHttpServerStream;
    friend class QHttpServerHttp1ProtocolHandler;
    friend class QHttpServerHttp2ProtocolHandler;

    Q_GADGET_EXPORT(Q_HTTPSERVER_EXPORT)

public:
    Q_HTTPSERVER_EXPORT QHttpServerRequest();
    Q_HTTPSERVER_EXPORT QHttpServerRequest(const QHttpServerRequest &other);
    Q_HTTPSERVER_EXPORT QHttpServerRequest &operator=(const QHttpServerRequest &other);
    QHttpServerRequest(QHttpServerRequest &&other) noexcept = default;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QHttpServerRequest)
    Q_HTTPSERVER_EXPORT ~QHttpServerRequest();
    void swap(QHttpServerRequest &other) noexcept { d.swap(other.d); }

    enum class Method
    {
        Unknown = 0x0000,
        Get     = 0x0001,
        Put     = 0x0002,
        Delete  = 0x0004,
        Post    = 0x0008,
        Head    = 0x0010,
        Options = 0x0020,
        Patch   = 0x0040,
        Connect = 0x0080,
        Trace   = 0x0100,

        AnyKnown = Get | Put | Delete | Post | Head | Options | Patch | Connect | Trace,
    };
    Q_ENUM(Method)
    Q_DECLARE_FLAGS(Methods, Method)
    Q_FLAG(Methods)

    Q_HTTPSERVER_EXPORT QByteArray value(const QByteArray &key) const;
    Q_HTTPSERVER_EXPORT QUrl url() const;
    Q_HTTPSERVER_EXPORT QUrlQuery query() const;
    Q_HTTPSERVER_EXPORT Method method() const;
    Q_HTTPSERVER_EXPORT const QHttpHeaders &headers() const &;
    Q_HTTPSERVER_EXPORT QHttpHeaders headers() &&;
    Q_HTTPSERVER_EXPORT QByteArray body() const;
    Q_HTTPSERVER_EXPORT QHostAddress remoteAddress() const;
    Q_HTTPSERVER_EXPORT quint16 remotePort() const;
    Q_HTTPSERVER_EXPORT QHostAddress localAddress() const;
    Q_HTTPSERVER_EXPORT quint16 localPort() const;
#if QT_CONFIG(ssl)
    Q_HTTPSERVER_EXPORT QSslConfiguration sslConfiguration() const;
#endif

private:
#if !defined(QT_NO_DEBUG_STREAM)
    friend Q_HTTPSERVER_EXPORT QDebug operator<<(QDebug debug, const QHttpServerRequest &request);
#endif

    Q_HTTPSERVER_EXPORT static QHttpServerRequest create(const QHttpServerParser &parser);
#if QT_CONFIG(ssl)
    Q_HTTPSERVER_EXPORT static QHttpServerRequest create(const QHttpServerParser &parser,
                                                         const QSslConfiguration &configuration);
#endif

    Q_HTTPSERVER_EXPORT explicit QHttpServerRequest(const QHostAddress &remoteAddress,
                                                    quint16 remotePort,
                                                    const QHostAddress &localAddress,
                                                    quint16 localPort);
#if QT_CONFIG(ssl)
    Q_HTTPSERVER_EXPORT explicit QHttpServerRequest(const QHostAddress &remoteAddress,
                                                    quint16 remotePort,
                                                    const QHostAddress &localAddress,
                                                    quint16 localPort,
                                                    const QSslConfiguration &sslConfiguration);
#endif

    QExplicitlySharedDataPointer<QHttpServerRequestPrivate> d;
};

Q_DECLARE_SHARED(QHttpServerRequest)

Q_DECLARE_OPERATORS_FOR_FLAGS(QHttpServerRequest::Methods)

QT_END_NAMESPACE

#endif // QHTTPSERVERREQUEST_H
