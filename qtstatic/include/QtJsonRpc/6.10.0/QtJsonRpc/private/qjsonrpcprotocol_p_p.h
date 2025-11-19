// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QJSONRPCPROTOCOL_P_P_H
#define QJSONRPCPROTOCOL_P_P_H

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

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>

#include <unordered_map>
#include <memory>

QT_BEGIN_NAMESPACE

template<typename T>
struct QHasher
{
    using argument_type = T;
    using result_type = size_t;
    result_type operator()(const argument_type &value) const { return qHash(value); }
};

class QJsonRpcProtocolPrivate
{
public:
    template<typename K, typename V>
    using Map = std::unordered_map<K, V, QHasher<K>>;

    using ResponseHandler = QJsonRpcProtocol::ResponseHandler;
    using MessageHandler = QJsonRpcProtocol::MessageHandler;
    using OwnedMessageHandler = std::unique_ptr<MessageHandler>;
    using MessageHandlerMap = Map<QString, OwnedMessageHandler>;
    using ResponseMap = Map<QJsonValue, QJsonRpcProtocol::Handler<QJsonRpcProtocol::Response>>;

    void processMessage(const QJsonDocument &message, const QJsonParseError &error);
    void processError(const QString &error);

    template<typename JSON>
    void sendMessage(const JSON &value)
    {
        m_transport->sendMessage(QJsonDocument(value));
    }

    void processRequest(const QJsonObject &object);
    void processResponse(const QJsonObject &object);
    void processNotification(const QJsonObject &object);

    MessageHandler *messageHandler(const QString &method) const
    {
        auto it = m_messageHandlers.find(method);
        return it != m_messageHandlers.end() ? it->second.get() : m_defaultHandler.get();
    }

    void setMessageHandler(const QString &method, OwnedMessageHandler handler)
    {
        m_messageHandlers[method] = std::move(handler);
    }

    MessageHandler *defaultMessageHandler() const { return m_defaultHandler.get(); }
    void setDefaultMessageHandler(OwnedMessageHandler handler)
    {
        m_defaultHandler = std::move(handler);
    }

    bool addPendingRequest(const QJsonValue &id,
                           QJsonRpcProtocol::Handler<QJsonRpcProtocol::Response> handler)
    {
        auto it = m_pendingRequests.find(id);
        if (it == m_pendingRequests.end()) {
            m_pendingRequests.insert(std::make_pair(id, std::move(handler)));
            return true;
        }
        return false;
    }

    void setTransport(QJsonRpcTransport *newTransport);
    QJsonRpcTransport *transport() const { return m_transport; }

    ResponseHandler invalidResponseHandler() const { return m_invalidResponseHandler; }
    void setInvalidResponseHandler(ResponseHandler handler)
    {
        m_invalidResponseHandler = std::move(handler);
    }

    ResponseHandler protocolErrorHandler() const { return m_protocolErrorHandler; }
    void setProtocolErrorHandler(ResponseHandler handler)
    {
        m_protocolErrorHandler = std::move(handler);
    }

    QJsonRpcProtocol::MessagePreprocessor messagePreprocessor() const
    {
        return m_messagePreprocessor;
    }
    void installMessagePreprocessor(QJsonRpcProtocol::MessagePreprocessor preHandler)
    {
        m_messagePreprocessor = std::move(preHandler);
    }

private:
    ResponseMap m_pendingRequests;
    MessageHandlerMap m_messageHandlers;

    OwnedMessageHandler m_defaultHandler;

    QJsonRpcTransport *m_transport = nullptr;

    ResponseHandler m_protocolErrorHandler;
    ResponseHandler m_invalidResponseHandler;
    QJsonRpcProtocol::MessagePreprocessor m_messagePreprocessor;
};

class QJsonRpcProtocol::BatchPrivate
{
public:
    struct Item
    {
        QJsonValue id = QJsonValue::Undefined;
        QString method;
        QJsonValue params = QJsonValue::Undefined;
    };

    std::vector<Item> m_items;
};

QT_END_NAMESPACE

#endif // QJSONRPCPROTOCOL_P_P_H
