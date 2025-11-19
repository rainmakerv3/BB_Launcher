// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QJSONTYPEDRPC_P_H
#define QJSONTYPEDRPC_P_H

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

#include <QtJsonRpc/private/qjsonrpcprotocol_p.h>
#include <QtJsonRpc/private/qjsonrpctransport_p.h>
#include <QtJsonRpc/private/qtypedjson_p.h>
#include <QtCore/qjsondocument.h>
#include <functional>
#include <variant>

QT_BEGIN_NAMESPACE

namespace QJsonRpc {
class TypedRpc;

using IdType = std::variant<int, QByteArray>;

template<typename... T>
QString idToString(const std::variant<T...> &v)
{
    struct ToStr
    {
        QString operator()(const QByteArray &v) { return QString::fromUtf8(v); }

        QString operator()(int v) { return QString::number(v); }

        QString operator()(std::nullptr_t) { return QStringLiteral("null"); }
    } toStr;
    return std::visit(toStr, v);
}

// concurrent usage by multiple threads not supported, the user should take care
class Q_JSONRPC_EXPORT TypedResponse
{
    Q_DISABLE_COPY(TypedResponse)
public:
    enum class Status { Started, SentSuccess, SentError, Invalid };
    TypedResponse() = default;

    TypedResponse(IdType id, TypedRpc *typedRpc,
                  const QJsonRpcProtocol::ResponseHandler &responseHandler,
                  Status status = Status::Started)
        : m_status(status), m_id(id), m_typedRpc(typedRpc), m_responseHandler(responseHandler)
    {
    }
    TypedResponse(TypedResponse &&o)
        : m_status(o.m_status),
          m_id(o.m_id),
          m_typedRpc(o.m_typedRpc),
          m_responseHandler(std::move(o.m_responseHandler))
    {
        o.m_status = Status::Invalid;
    }
    TypedResponse &operator=(TypedResponse &&o) noexcept
    {
        m_status = o.m_status;
        m_id = o.m_id;
        m_typedRpc = o.m_typedRpc;
        m_responseHandler = std::move(o.m_responseHandler);
        o.m_status = Status::Invalid;
        return *this;
    }
    ~TypedResponse()
    {
        if (m_status == Status::Started)
            sendErrorResponse(int(QJsonRpcProtocol::ErrorCode::InternalError),
                              QByteArray("Response destroyed before having sent a response"),
                              nullptr);
    }

    template<typename T>
    void sendSuccessfullResponse(const T &result);
    template<typename T>
    void sendErrorResponse(int code, const QByteArray &message, const T &data);
    void sendErrorResponse(int code, const QByteArray &message);
    template<typename... Params>
    void sendNotification(const QByteArray &method, const Params &...params);

    IdType id() const { return m_id; }
    QString idStr()
    {
        if (const int *iPtr = std::get_if<int>(&m_id))
            return QString::number(*iPtr);
        else if (const QByteArray *bPtr = std::get_if<QByteArray>(&m_id))
            return QString::fromUtf8(*bPtr);
        else
            return QString();
    }
    using OnCloseAction = std::function<void(Status, const IdType &, TypedRpc &)>;
    void addOnCloseAction(const OnCloseAction &act);

private:
    void doOnCloseActions();
    Status m_status = Status::Invalid;
    IdType m_id;
    TypedRpc *m_typedRpc = nullptr;
    QJsonRpcProtocol::ResponseHandler m_responseHandler;
    QList<OnCloseAction> m_onCloseActions;
};

class Q_JSONRPC_EXPORT TypedHandler : public QJsonRpcProtocol::MessageHandler
{
public:
    TypedHandler() = default; // invalid instance
    TypedHandler(const QByteArray &method,
                 const std::function<void(const QJsonRpcProtocol::Request &,
                                          const QJsonRpcProtocol::ResponseHandler &)> &rHandler,
                 const std::function<void(const QJsonRpcProtocol::Notification &)> &nHandler)
        : m_method(method), m_requestHandler(rHandler), m_notificationHandler(nHandler)
    {
    }

    TypedHandler(const QByteArray &method,
                 const std::function<void(const QJsonRpcProtocol::Request &,
                                          const QJsonRpcProtocol::ResponseHandler &)> &rHandler)
        : m_method(method), m_requestHandler(rHandler), m_notificationHandler()
    {
    }

    TypedHandler(const QByteArray &method,
                 const std::function<void(const QJsonRpcProtocol::Notification &)> &nHandler)
        : m_method(method), m_requestHandler(), m_notificationHandler(nHandler)
    {
    }

    ~TypedHandler() = default;

    void handleRequest(const QJsonRpcProtocol::Request &request,
                       const QJsonRpcProtocol::ResponseHandler &handler) override
    {
        using namespace Qt::StringLiterals;

        if (m_requestHandler) {
            m_requestHandler(request, handler);
            return;
        }
        QString msg;
        if (m_notificationHandler)
            msg = u"Expected notification with method '%1', not request"_s;
        else
            msg = u"Reached null handler for method '%1'"_s;
        msg = msg.arg(request.method);
        handler(MessageHandler::error(int(QJsonRpcProtocol::ErrorCode::InvalidRequest), msg));
        qCWarning(QTypedJson::jsonRpcLog) << msg;
    }

    QByteArray method() const { return m_method; }

    void handleNotification(const QJsonRpcProtocol::Notification &notification) override
    {
        if (m_notificationHandler) {
            m_notificationHandler(notification);
            return;
        }
        if (m_requestHandler)
            qCWarning(QTypedJson::jsonRpcLog) << "Expected Request but got notification for "
                                              << notification.method << ", ignoring it.";
        else
            qCWarning(QTypedJson::jsonRpcLog)
                    << "Reached null handler for method " << notification.method;
    }

private:
    QByteArray m_method;
    std::function<void(const QJsonRpcProtocol::Request &,
                       const QJsonRpcProtocol::ResponseHandler &)>
            m_requestHandler;
    std::function<void(const QJsonRpcProtocol::Notification &)> m_notificationHandler;
};

class Q_JSONRPC_EXPORT TypedRpc : public QJsonRpcProtocol
{
    Q_DISABLE_COPY_MOVE(TypedRpc)
public:
    TypedRpc() = default;

    template<typename... Params>
    void sendRequestId(const std::variant<int, QByteArray> &id, const QByteArray &method,
                       const QJsonRpcProtocol::Handler<QJsonRpcProtocol::Response> &rHandler,
                       const Params &...params)
    {
        QJsonRpcProtocol::sendRequest(Request { QTypedJson::toJsonValue(id),
                                                QString::fromUtf8(method),
                                                QTypedJson::toJsonValue(params...) },
                                      rHandler);
    }

    template<typename... Params>
    void sendRequest(const QByteArray &method,
                     const QJsonRpcProtocol::Handler<QJsonRpcProtocol::Response> &handler,
                     const Params &...params)
    {
        sendRequestId(++m_lastId, method, handler, params...);
    }

    template<typename... Params>
    void sendNotification(const QByteArray &method, const Params &...params)
    {
        QJsonRpcProtocol::sendNotification(
                Notification { QString::fromUtf8(method), QTypedJson::toJsonValue(params...) });
    }

    template<typename Req, typename Resp>
    void registerRequestHandler(
            const QByteArray &method,
            const std::function<void(const QByteArray &, const Req &, Resp &&)> &handler)
    {
        if (m_handlers.contains(method) && handler) {
            qCWarning(QTypedJson::jsonRpcLog)
                    << "QJsonRpc double registration for method" << QString::fromUtf8(method);
            Q_ASSERT(false);
            return;
        }
        TypedHandler *h;
        if (handler)
            h = new TypedHandler(
                    method,
                    [handler, method, this](const QJsonRpcProtocol::Request &req,
                                            const QJsonRpcProtocol::ResponseHandler &rH) {
                        std::variant<int, QByteArray> id = req.id.toInt(0);
                        if (req.id.isString())
                            id = req.id.toString().toUtf8();
                        TypedResponse typedResponse(id, this, rH);
                        Req tReq;
                        {
                            QTypedJson::Reader r(req.params);
                            QTypedJson::doWalk(r, tReq);
                            if (!r.errorMessages().isEmpty()) {
                                qCWarning(QTypedJson::jsonRpcLog)
                                        << "Warnings decoding parameters for Request" << method
                                        << idToString(id) << "from" << req.params << ":\n    "
                                        << r.errorMessages().join(u"\n    ");
                                r.clearErrorMessages();
                            }
                        }
                        Resp myResponse(std::move(typedResponse));
                        handler(method, tReq, std::move(myResponse));
                    });
        else
            h = new TypedHandler;
        m_handlers[method] = h;
        setMessageHandler(QString::fromUtf8(method), h);
    }

    template<typename N>
    void registerNotificationHandler(
            const QByteArray &method,
            const std::function<void(const QByteArray &, const N &)> &handler)
    {
        if (m_handlers.contains(method) && handler) {
            qCWarning(QTypedJson::jsonRpcLog)
                    << "QJsonRpc double registration for method" << QString::fromUtf8(method);
            Q_ASSERT(false);
            return;
        }
        TypedHandler *h;
        if (handler)
            h = new TypedHandler(
                    method, [handler, method](const QJsonRpcProtocol::Notification &notif) {
                        N tNotif;
                        {
                            QTypedJson::Reader r(notif.params);
                            QTypedJson::doWalk(r, tNotif);
                            if (!r.errorMessages().isEmpty()) {
                                qCWarning(QTypedJson::jsonRpcLog)
                                        << "Warnings decoding parameters for Notification" << method
                                        << "from" << notif.params << ":\n    "
                                        << r.errorMessages().join(u"\n    ");
                                r.clearErrorMessages();
                            }
                        }
                        handler(method, tNotif);
                    });
        else
            h = new TypedHandler;
        setMessageHandler(QString::fromUtf8(method), h);
        m_handlers[method] = h;
    }

    void sendBatch(
            QJsonRpcProtocol::Batch &&batch,
            const QJsonRpcProtocol::Handler<QJsonRpcProtocol::Response> &handler)
        = delete; // disable batch support
    void installOnCloseAction(const TypedResponse::OnCloseAction &closeAction);
    TypedResponse::OnCloseAction onCloseAction();
    void doOnCloseAction(TypedResponse::Status, const IdType &);

private:
    QAtomicInt m_lastId;
    QHash<QByteArray, TypedHandler *> m_handlers;
    TypedResponse::OnCloseAction m_onCloseAction;
};

template<typename T>
void TypedResponse::sendSuccessfullResponse(const T &result)
{
    if (m_status == Status::Started) {
        m_status = Status::SentSuccess;
        m_responseHandler(QJsonRpcProtocol::Response {
            QTypedJson::toJsonValue(m_id),
            QTypedJson::toJsonValue(result)
        });
        doOnCloseActions();
    } else {
        qCWarning(QTypedJson::jsonRpcLog)
                << "Ignoring response in already answered request" << idStr();
    }
}

template<typename T>
void TypedResponse::sendErrorResponse(int code, const QByteArray &message, const T &data)
{
    if (m_status == Status::Started) {
        m_status = Status::SentError;
        m_responseHandler(QJsonRpcProtocol::Response { QTypedJson::toJsonValue(m_id),
                                                       QTypedJson::toJsonValue(data), code,
                                                       QString::fromUtf8(message) });
        doOnCloseActions();
    } else {
        qCWarning(QTypedJson::jsonRpcLog)
                << "Ignoring error response" << code << QString::fromUtf8(message)
                << "in already answered request" << idStr();
    }
}

template<typename... Params>
void TypedResponse::sendNotification(const QByteArray &method, const Params &...params)
{
    m_typedRpc->sendNotification(method, params...);
}
} // namespace QTypedJson
QT_END_NAMESPACE

#endif // QJSONTYPEDRPC_P_H
