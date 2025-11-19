// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBSOCKETSERVER_P_H
#define QWEBSOCKETSERVER_P_H
//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QString>
#include <QtNetwork/QHostAddress>
#include <private/qobject_p.h>
#include "qwebsocketserver.h"
#include "qwebsocket.h"

#ifndef QT_NO_SSL
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslError>
#endif

QT_BEGIN_NAMESPACE

class QTcpServer;
class QTcpSocket;

class QWebSocketServerPrivate : public QObjectPrivate
{
    Q_DISABLE_COPY(QWebSocketServerPrivate)

public:
    Q_DECLARE_PUBLIC(QWebSocketServer)
    enum SslMode
    {
        SecureMode = true,
        NonSecureMode
    };

    explicit QWebSocketServerPrivate(const QString &serverName, SslMode secureMode);
    ~QWebSocketServerPrivate() override;

    void init();
    void close(bool aboutToDestroy = false);
    QString errorString() const;
    bool hasPendingConnections() const;
    bool isListening() const;
    bool listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 0);
    int maxPendingConnections() const;
    int handshakeTimeout() const {
        return m_handshakeTimeout;
    }
    virtual QWebSocket *nextPendingConnection();
    void pauseAccepting();
#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy proxy() const;
    void setProxy(const QNetworkProxy &networkProxy);
#endif
    void resumeAccepting();
    QHostAddress serverAddress() const;
    QWebSocketProtocol::CloseCode serverError() const;
    quint16 serverPort() const;
    void setMaxPendingConnections(int numConnections);
    void setHandshakeTimeout(int msec);
    bool setSocketDescriptor(qintptr socketDescriptor);
    qintptr socketDescriptor() const;

    QList<QWebSocketProtocol::Version> supportedVersions() const;
    void setSupportedSubprotocols(const QStringList &protocols);
    QStringList supportedSubprotocols() const;
    QStringList supportedExtensions() const;

    void setServerName(const QString &serverName);
    QString serverName() const;

    SslMode secureMode() const;

#ifndef QT_NO_SSL
    void setSslConfiguration(const QSslConfiguration &sslConfiguration);
    QSslConfiguration sslConfiguration() const;
#endif

    void setError(QWebSocketProtocol::CloseCode code, const QString &errorString);

    void handleConnection(QTcpSocket *pTcpSocket) const;

private slots:
    void startHandshakeTimeout(QTcpSocket *pTcpSocket);

private:
    QTcpServer *m_pTcpServer;
    QString m_serverName;
    SslMode m_secureMode;
    QStringList m_supportedSubprotocols;
    QQueue<QWebSocket *> m_pendingConnections;
    QWebSocketProtocol::CloseCode m_error;
    QString m_errorString;
    int m_maxPendingConnections;
    int m_handshakeTimeout;

    void addPendingConnection(QWebSocket *pWebSocket);
    void setErrorFromSocketError(QAbstractSocket::SocketError error,
                                 const QString &errorDescription);

    void onNewConnection();
    void onSocketDisconnected();
    void handshakeReceived();
    void finishHandshakeTimeout(QTcpSocket *pTcpSocket);
};

QT_END_NAMESPACE

#endif // QWEBSOCKETSERVER_P_H
