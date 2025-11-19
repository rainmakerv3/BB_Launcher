// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QJSONRPCPROTOCOL_P_H
#define QJSONRPCPROTOCOL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtJsonRpc/qtjsonrpcglobal.h>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qjsondocument.h>

#include <memory>

QT_BEGIN_NAMESPACE

class QLaguageServer;
class QJsonRpcTransport;
class QJsonRpcProtocolPrivate;
class Q_JSONRPC_EXPORT QJsonRpcProtocol
{
    Q_DISABLE_COPY(QJsonRpcProtocol)
public:
    QJsonRpcProtocol();
    QJsonRpcProtocol(QJsonRpcProtocol &&) noexcept;
    QJsonRpcProtocol &operator=(QJsonRpcProtocol &&) noexcept;

    ~QJsonRpcProtocol();

    enum class ErrorCode {
        ParseError = -32700,
        InvalidRequest = -32600,
        MethodNotFound = -32601,
        InvalidParams = -32602,
        InternalError = -32603,
    };

    struct Request
    {
        QJsonValue id = QJsonValue::Undefined;
        QString method;
        QJsonValue params = QJsonValue::Undefined;
    };

    struct Response
    {
        // ID is disregarded on responses generated from MessageHandlers.
        // You cannot reply to a request you didn't handle. The original request ID is used instead.
        QJsonValue id = QJsonValue::Undefined;

        // In case of !errorCode.isDouble(), data is the "data" member of the error object.
        // Otherwise it's the "result" member of the base response.
        QJsonValue data = QJsonValue::Undefined;

        // Error codes from and including -32768 to -32000 are reserved for pre-defined errors.
        // Don't use them for your own protocol. We cannot enforce this here.
        QJsonValue errorCode = QJsonValue::Undefined;

        QString errorMessage = QString();
    };

    template<typename T>
    using Handler = std::function<void(const T &)>;

    struct Notification
    {
        QString method;
        QJsonValue params = QJsonValue::Undefined;
    };

    class Q_JSONRPC_EXPORT MessageHandler
    {
        Q_DISABLE_COPY_MOVE(MessageHandler)
    public:
        using ResponseHandler = std::function<void(const Response &)>;

        MessageHandler();
        virtual ~MessageHandler();
        virtual void handleRequest(const Request &request, const ResponseHandler &handler);
        virtual void handleNotification(const Notification &notification);

        static Response error(ErrorCode code);
        static Response error(int code, const QString &message,
                              const QJsonValue &data = QJsonValue::Undefined);

    protected:
        static Response result(const QJsonValue &result);
    };

    class BatchPrivate;
    class Q_JSONRPC_EXPORT Batch
    {
        Q_DISABLE_COPY(Batch)
    public:
        Batch();
        ~Batch();

        Batch(Batch &&) noexcept;
        Batch &operator=(Batch &&) noexcept;

        void addNotification(const Notification &notification);
        void addRequest(const Request &request);

    private:
        friend class QJsonRpcProtocol;
        std::unique_ptr<BatchPrivate> d;
    };

    void setMessageHandler(const QString &method, MessageHandler *handler);
    void setDefaultMessageHandler(MessageHandler *handler);
    MessageHandler *messageHandler(const QString &method) const;
    MessageHandler *defaultMessageHandler() const;

    void sendRequest(const Request &request, const QJsonRpcProtocol::Handler<Response> &handler);
    void sendNotification(const Notification &notification);
    void sendBatch(Batch &&batch, const QJsonRpcProtocol::Handler<Response> &handler);

    void setTransport(QJsonRpcTransport *transport);

    // For id:null responses
    using ResponseHandler = std::function<void(const Response &)>;
    void setProtocolErrorHandler(const ResponseHandler &handler);
    ResponseHandler protocolErrorHandler() const;

    // For responses with unknown IDs
    void setInvalidResponseHandler(const ResponseHandler &handler);
    ResponseHandler invalidResponseHandler() const;

    enum class Processing { Continue, Stop };
    using MessagePreprocessor =
            std::function<Processing(const QJsonDocument &, const QJsonParseError &,
                                     const QJsonRpcProtocol::Handler<Response> &handler)>;
    MessagePreprocessor messagePreprocessor() const;
    void installMessagePreprocessor(const MessagePreprocessor &preHandler);

private:
    std::unique_ptr<QJsonRpcProtocolPrivate> d;
};

QT_END_NAMESPACE

#endif // QJSONRPCPROTOCOL_P_H
