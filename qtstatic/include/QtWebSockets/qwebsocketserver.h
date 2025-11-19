// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBSOCKETSERVER_H
#define QWEBSOCKETSERVER_H

#include "QtWebSockets/qwebsockets_global.h"
#include "QtWebSockets/qwebsocketprotocol.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtNetwork/QHostAddress>

#ifndef QT_NO_SSL
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslError>
#endif

#if __has_include(<chrono>)
#include <chrono>
#endif

QT_BEGIN_NAMESPACE

class QTcpSocket;
class QWebSocketServerPrivate;
class QWebSocket;
class QWebSocketCorsAuthenticator;

class Q_WEBSOCKETS_EXPORT QWebSocketServer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QWebSocketServer)
    Q_DECLARE_PRIVATE(QWebSocketServer)

public:
    enum SslMode {
#ifndef QT_NO_SSL
        SecureMode = 0,
#endif
        NonSecureMode = 1
    };
    Q_ENUM(SslMode)

    explicit QWebSocketServer(const QString &serverName, SslMode secureMode,
                              QObject *parent = nullptr);
    ~QWebSocketServer() override;

    bool listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 0);
    void close();

    bool isListening() const;

    void setMaxPendingConnections(int numConnections);
    int maxPendingConnections() const;

#if __has_include(<chrono>) || defined(Q_QDOC)
    void setHandshakeTimeout(std::chrono::milliseconds msec)
    {
        setHandshakeTimeout(int(msec.count()));
    }
    std::chrono::milliseconds handshakeTimeout() const
    {
        return std::chrono::milliseconds(handshakeTimeoutMS());
    }
#endif
    void setHandshakeTimeout(int msec);
    int handshakeTimeoutMS() const;

    quint16 serverPort() const;
    QHostAddress serverAddress() const;
    QUrl serverUrl() const;

    SslMode secureMode() const;

    bool setSocketDescriptor(qintptr socketDescriptor);
    qintptr socketDescriptor() const;
#if QT_DEPRECATED_SINCE(6, 2)
    QT_DEPRECATED_VERSION_X_6_2("Use setSocketDescriptor instead")
    bool setNativeDescriptor(qintptr descriptor) { return setSocketDescriptor(descriptor); }
    QT_DEPRECATED_VERSION_X_6_2("Use socketDescriptor instead")
    qintptr nativeDescriptor() const { return socketDescriptor(); }
#endif

    bool hasPendingConnections() const;
    virtual QWebSocket *nextPendingConnection();

    QWebSocketProtocol::CloseCode error() const;
    QString errorString() const;

    void pauseAccepting();
    void resumeAccepting();

    void setServerName(const QString &serverName);
    QString serverName() const;

    void setSupportedSubprotocols(const QStringList &protocols);
    QStringList supportedSubprotocols() const;

#ifndef QT_NO_NETWORKPROXY
    void setProxy(const QNetworkProxy &networkProxy);
    QNetworkProxy proxy() const;
#endif
#ifndef QT_NO_SSL
    void setSslConfiguration(const QSslConfiguration &sslConfiguration);
    QSslConfiguration sslConfiguration() const;
#endif

    QList<QWebSocketProtocol::Version> supportedVersions() const;

    void handleConnection(QTcpSocket *socket) const;

Q_SIGNALS:
    void acceptError(QAbstractSocket::SocketError socketError);
    void serverError(QWebSocketProtocol::CloseCode closeCode);
    //TODO: should use a delegate iso of a synchronous signal
    //see also QTBUG-16251
    void originAuthenticationRequired(QWebSocketCorsAuthenticator *pAuthenticator);
    void newConnection();
#ifndef QT_NO_SSL
    void peerVerifyError(const QSslError &error);
    void sslErrors(const QList<QSslError> &errors);
    void preSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator *authenticator);
    void alertSent(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description);
    void alertReceived(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description);
    void handshakeInterruptedOnError(const QSslError &error);
#endif
    void closed();
};

QT_END_NAMESPACE

#endif // QWEBSOCKETSERVER_H
