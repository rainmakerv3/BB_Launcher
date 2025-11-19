// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERRESPONDER_H
#define QHTTPSERVERRESPONDER_H

#include <QtHttpServer/qthttpserverglobal.h>
#include <QtNetwork/qhttpheaders.h>

#include <QtCore/qstringfwd.h>
#include <QtCore/qmetatype.h>

#include <memory>
#include <utility>
#include <initializer_list>

QT_BEGIN_NAMESPACE

class QHttpServerStream;
class QHttpServerRequest;
class QHttpServerResponse;

class QHttpServerResponderPrivate;
class QHttpServerResponder final
{
    Q_GADGET_EXPORT(Q_HTTPSERVER_EXPORT)
    Q_DECLARE_PRIVATE(QHttpServerResponder)

    friend class QHttpServerHttp1ProtocolHandler;
    friend class QHttpServerHttp2ProtocolHandler;

public:
    enum class StatusCode {
        // 1xx: Informational
        Continue = 100,
        SwitchingProtocols,
        Processing,

        // 2xx: Success
        Ok = 200,
        Created,
        Accepted,
        NonAuthoritativeInformation,
        NoContent,
        ResetContent,
        PartialContent,
        MultiStatus,
        AlreadyReported,
        IMUsed = 226,

        // 3xx: Redirection
        MultipleChoices = 300,
        MovedPermanently,
        Found,
        SeeOther,
        NotModified,
        UseProxy,
        // 306: not used, was proposed as "Switch Proxy" but never standardized
        TemporaryRedirect = 307,
        PermanentRedirect,

        // 4xx: Client Error
        BadRequest = 400,
        Unauthorized,
        PaymentRequired,
        Forbidden,
        NotFound,
        MethodNotAllowed,
        NotAcceptable,
        ProxyAuthenticationRequired,
        RequestTimeout,
        Conflict,
        Gone,
        LengthRequired,
        PreconditionFailed,
        PayloadTooLarge,
        UriTooLong,
        UnsupportedMediaType,
        RequestRangeNotSatisfiable,
        ExpectationFailed,
        ImATeapot,
        MisdirectedRequest = 421,
        UnprocessableEntity,
        Locked,
        FailedDependency,
        UpgradeRequired = 426,
        PreconditionRequired = 428,
        TooManyRequests,
        RequestHeaderFieldsTooLarge = 431,
        UnavailableForLegalReasons = 451,

        // 5xx: Server Error
        InternalServerError = 500,
        NotImplemented,
        BadGateway,
        ServiceUnavailable,
        GatewayTimeout,
        HttpVersionNotSupported,
        VariantAlsoNegotiates,
        InsufficientStorage,
        LoopDetected,
        NotExtended = 510,
        NetworkAuthenticationRequired,
        NetworkConnectTimeoutError = 599,
    };
    Q_ENUM(StatusCode)

    QHttpServerResponder(QHttpServerResponder &&other) noexcept
        : d_ptr(std::exchange(other.d_ptr, nullptr))
    {
    }
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QHttpServerResponder)

    Q_HTTPSERVER_EXPORT ~QHttpServerResponder();

    void swap(QHttpServerResponder &other) noexcept { qt_ptr_swap(d_ptr, other.d_ptr); }

    Q_HTTPSERVER_EXPORT void write(QIODevice *data, const QHttpHeaders &headers,
                                   StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT void write(QIODevice *data, const QByteArray &mimeType,
                                   StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT void write(const QJsonDocument &document, const QHttpHeaders &headers,
                                   StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT void write(const QJsonDocument &document,
                                   StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT void write(const QByteArray &data, const QHttpHeaders &headers,
                                   StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT void write(const QByteArray &data, const QByteArray &mimeType,
                                   StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT void write(const QHttpHeaders &headers, StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT void write(StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT void sendResponse(const QHttpServerResponse &response);

    Q_HTTPSERVER_EXPORT void writeBeginChunked(const QHttpHeaders &headers,
                                               StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT void writeBeginChunked(const QByteArray &mimeType,
                                               StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT void writeBeginChunked(const QHttpHeaders &headers,
                                               QList<QHttpHeaders::WellKnownHeader> trailerNames,
                                               StatusCode status = StatusCode::Ok);

    Q_HTTPSERVER_EXPORT void writeChunk(const QByteArray &data);

    Q_HTTPSERVER_EXPORT void writeEndChunked(const QByteArray &data, const QHttpHeaders &trailers);

    Q_HTTPSERVER_EXPORT void writeEndChunked(const QByteArray &data);

private:
    Q_HTTPSERVER_EXPORT QHttpServerResponder(QHttpServerStream *stream);
    Q_DISABLE_COPY(QHttpServerResponder)

    QHttpServerResponderPrivate *d_ptr;
};

QT_END_NAMESPACE

#endif // QHTTPSERVERRESPONDER_H
