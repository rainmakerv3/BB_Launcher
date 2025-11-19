// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBSOCKET_H
#define QWEBSOCKET_H

#include <QtCore/QUrl>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QNetworkRequest>
#ifndef QT_NO_NETWORKPROXY
#include <QtNetwork/QNetworkProxy>
#endif
#ifndef QT_NO_SSL
#include <QtNetwork/QSslError>
#include <QtNetwork/QSslConfiguration>
#endif
#include "QtWebSockets/qwebsockets_global.h"
#include "QtWebSockets/qwebsocketprotocol.h"

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QAuthenticator;
class QTcpSocket;
class QWebSocketPrivate;
class QMaskGenerator;
class QWebSocketHandshakeOptions;

class Q_WEBSOCKETS_EXPORT QWebSocket : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QWebSocket)
    Q_DECLARE_PRIVATE(QWebSocket)

public:
    explicit QWebSocket(const QString &origin = QString(),
                        QWebSocketProtocol::Version version = QWebSocketProtocol::VersionLatest,
                        QObject *parent = nullptr);
    ~QWebSocket() override;

    void abort();
    QAbstractSocket::SocketError error() const;
    QString errorString() const;
    bool flush();
    bool isValid() const;
    QHostAddress localAddress() const;
    quint16 localPort() const;
    QAbstractSocket::PauseModes pauseMode() const;
    QHostAddress peerAddress() const;
    QString peerName() const;
    quint16 peerPort() const;
#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy proxy() const;
    void setProxy(const QNetworkProxy &networkProxy);
#endif
    void setMaskGenerator(const QMaskGenerator *maskGenerator);
    const QMaskGenerator *maskGenerator() const;
    qint64 readBufferSize() const;
    void setReadBufferSize(qint64 size);

    void resume();
    void setPauseMode(QAbstractSocket::PauseModes pauseMode);

    QAbstractSocket::SocketState state() const;

    QWebSocketProtocol::Version version() const;
    QString resourceName() const;
    QUrl requestUrl() const;
    QNetworkRequest request() const;
    QWebSocketHandshakeOptions handshakeOptions() const;
    QString origin() const;
    QString subprotocol() const;
    QWebSocketProtocol::CloseCode closeCode() const;
    QString closeReason() const;

    qint64 sendTextMessage(const QString &message);
    qint64 sendBinaryMessage(const QByteArray &data);

#ifndef QT_NO_SSL
    void ignoreSslErrors(const QList<QSslError> &errors);
    void continueInterruptedHandshake();

    void setSslConfiguration(const QSslConfiguration &sslConfiguration);
    QSslConfiguration sslConfiguration() const;
#endif

    qint64 bytesToWrite() const;

    void setMaxAllowedIncomingFrameSize(quint64 maxAllowedIncomingFrameSize);
    quint64 maxAllowedIncomingFrameSize() const;
    void setMaxAllowedIncomingMessageSize(quint64 maxAllowedIncomingMessageSize);
    quint64 maxAllowedIncomingMessageSize() const;
    static quint64 maxIncomingMessageSize();
    static quint64 maxIncomingFrameSize();

    void setOutgoingFrameSize(quint64 outgoingFrameSize);
    quint64 outgoingFrameSize() const;
    static quint64 maxOutgoingFrameSize();

public Q_SLOTS:
    void close(QWebSocketProtocol::CloseCode closeCode = QWebSocketProtocol::CloseCodeNormal,
               const QString &reason = QString());

    // ### Qt7: Merge overloads
    void open(const QUrl &url);
    void open(const QNetworkRequest &request);
    void open(const QUrl &url, const QWebSocketHandshakeOptions &options);
    void open(const QNetworkRequest &request, const QWebSocketHandshakeOptions &options);

    void ping(const QByteArray &payload = QByteArray());
#ifndef QT_NO_SSL
    void ignoreSslErrors();
#endif

Q_SIGNALS:
    void aboutToClose();
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState state);
#ifndef QT_NO_NETWORKPROXY
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *pAuthenticator);
#endif
    void authenticationRequired(QAuthenticator *authenticator);
    void readChannelFinished();
    void textFrameReceived(const QString &frame, bool isLastFrame);
    void binaryFrameReceived(const QByteArray &frame, bool isLastFrame);
    void textMessageReceived(const QString &message);
    void binaryMessageReceived(const QByteArray &message);
#if QT_DEPRECATED_SINCE(6, 5)
    QT_DEPRECATED_VERSION_X_6_5("Use errorOccurred instead")
    void error(QAbstractSocket::SocketError error);
#endif
    void errorOccurred(QAbstractSocket::SocketError error);
    void pong(quint64 elapsedTime, const QByteArray &payload);
    void bytesWritten(qint64 bytes);

#ifndef QT_NO_SSL
    void peerVerifyError(const QSslError &error);
    void sslErrors(const QList<QSslError> &errors);
    void preSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator *authenticator);
    void alertSent(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description);
    void alertReceived(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description);
    void handshakeInterruptedOnError(const QSslError &error);
#endif

private:
    QWebSocket(QTcpSocket *pTcpSocket, QWebSocketProtocol::Version version,
               QObject *parent = nullptr);
};

QT_END_NAMESPACE

#endif // QWEBSOCKET_H
