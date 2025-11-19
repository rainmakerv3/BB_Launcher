// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QJSONRPCTRANSPORT_H
#define QJSONRPCTRANSPORT_H

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
#include <QtCore/qjsondocument.h>
#include <functional>

QT_BEGIN_NAMESPACE

class Q_JSONRPC_EXPORT QJsonRpcTransport
{
    Q_DISABLE_COPY_MOVE(QJsonRpcTransport)
public:
    enum DiagnosticLevel { Warning, Error };

    using MessageHandler = std::function<void(const QJsonDocument &, const QJsonParseError &)>;
    using DataHandler = std::function<void(const QByteArray &)>;
    using DiagnosticHandler = std::function<void(DiagnosticLevel, const QString &)>;

    QJsonRpcTransport() = default;
    virtual ~QJsonRpcTransport() = default;

    // Parse data and call messageHandler for any messages found in it.
    virtual void receiveData(const QByteArray &data) = 0;

    // serialize the message and call dataHandler for the resulting data.
    // Needs to be guarded by a mutex if called  by different threads
    virtual void sendMessage(const QJsonDocument &packet) = 0;

    void setMessageHandler(const MessageHandler &handler) { m_messageHandler = handler; }
    MessageHandler messageHandler() const { return m_messageHandler; }

    void setDataHandler(const DataHandler &handler) { m_dataHandler = handler; }
    DataHandler dataHandler() const { return m_dataHandler; }

    void setDiagnosticHandler(const DiagnosticHandler &handler) { m_diagnosticHandler = handler; }
    DiagnosticHandler diagnosticHandler() const { return m_diagnosticHandler; }

private:
    MessageHandler m_messageHandler;
    DataHandler m_dataHandler;
    DiagnosticHandler m_diagnosticHandler;
};

QT_END_NAMESPACE

#endif // QJSONRPCTRANSPORT_H
