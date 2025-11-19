// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default
#ifndef QWEBSOCKET_P_H
#define QWEBSOCKET_P_H
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

#include <QtCore/QUrl>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#ifndef QT_NO_NETWORKPROXY
#include <QtNetwork/QNetworkProxy>
#endif
#include <QtNetwork/QAuthenticator>
#ifndef QT_NO_SSL
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslError>
#include <QtNetwork/QSslSocket>
#endif
#include <QtCore/QElapsedTimer>
#include <private/qobject_p.h>

#include "qwebsocket.h"
#include "qwebsockethandshakeoptions.h"
#include "qwebsocketprotocol.h"
#include "qwebsocketdataprocessor_p.h"
#include "qdefaultmaskgenerator_p.h"

#ifdef Q_OS_WASM
#    include <emscripten/websocket.h>
#endif

QT_BEGIN_NAMESPACE

class QWebSocketHandshakeRequest;
class QWebSocketHandshakeResponse;
class QTcpSocket;
class QWebSocket;
class QMaskGenerator;

struct QWebSocketConfiguration
{
    Q_DISABLE_COPY(QWebSocketConfiguration)

public:
    QWebSocketConfiguration();

public:
#ifndef QT_NO_SSL
    QSslConfiguration m_sslConfiguration;
    QList<QSslError> m_ignoredSslErrors;
    bool m_ignoreSslErrors;
#endif
#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy m_proxy;
#endif
    QTcpSocket *m_pSocket;
};

class QWebSocketPrivate : public QObjectPrivate
{
    Q_DISABLE_COPY(QWebSocketPrivate)

public:
    Q_DECLARE_PUBLIC(QWebSocket)
    explicit QWebSocketPrivate(const QString &origin,
                               QWebSocketProtocol::Version version);
    ~QWebSocketPrivate() override;

    // both constants are taken from the default settings of Apache
    // see: http://httpd.apache.org/docs/2.2/mod/core.html#limitrequestfieldsize and
    // http://httpd.apache.org/docs/2.2/mod/core.html#limitrequestfields
    static constexpr int MAX_HEADERLINE_LENGTH = 8 * 1024; // maximum length of a http request header line
    static constexpr int MAX_HEADERLINES = 100;            // maximum number of http request header lines

    void init();
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
    void resume();
    void setPauseMode(QAbstractSocket::PauseModes pauseMode);
    void setReadBufferSize(qint64 size);
    QAbstractSocket::SocketState state() const;

    QWebSocketProtocol::Version version() const;
    QString resourceName() const;
    QNetworkRequest request() const;
    QString origin() const;
    QWebSocketHandshakeOptions handshakeOptions() const;
    QString protocol() const;
    QString extension() const;
    QWebSocketProtocol::CloseCode closeCode() const;
    QString closeReason() const;

    qint64 sendTextMessage(const QString &message);
    qint64 sendBinaryMessage(const QByteArray &data);

#ifndef QT_NO_SSL
    void ignoreSslErrors(const QList<QSslError> &errors);
    void ignoreSslErrors();
    void continueInterruptedHandshake();
    void setSslConfiguration(const QSslConfiguration &sslConfiguration);
    QSslConfiguration sslConfiguration() const;
    void _q_updateSslConfiguration();
#endif

    void closeGoingAway();
    void close(QWebSocketProtocol::CloseCode closeCode, QString reason);
    void open(const QNetworkRequest &request, const QWebSocketHandshakeOptions &options, bool mask);
    void ping(const QByteArray &payload);
    void setSocketState(QAbstractSocket::SocketState state);

    void setMaxAllowedIncomingFrameSize(quint64 maxAllowedIncomingFrameSize);
    quint64 maxAllowedIncomingFrameSize() const;
    void setMaxAllowedIncomingMessageSize(quint64 maxAllowedIncomingMessageSize);
    quint64 maxAllowedIncomingMessageSize() const;
    static quint64 maxIncomingMessageSize();
    static quint64 maxIncomingFrameSize();

    void setOutgoingFrameSize(quint64 outgoingFrameSize);
    quint64 outgoingFrameSize() const;
    static quint64 maxOutgoingFrameSize();
#ifdef Q_OS_WASM
    void setSocketClosed(const EmscriptenWebSocketCloseEvent *emCloseEvent);
    QString closeCodeToString(QWebSocketProtocol::CloseCode code);
#endif
private:
    QWebSocketPrivate(QTcpSocket *pTcpSocket, QWebSocketProtocol::Version version);
    void setVersion(QWebSocketProtocol::Version version);
    void setResourceName(const QString &resourceName);
    void setRequest(const QNetworkRequest &request, const QWebSocketHandshakeOptions &options = {});
    void setOrigin(const QString &origin);
    void setProtocol(const QString &protocol);
    void setExtension(const QString &extension);
    void enableMasking(bool enable);
    void setErrorString(const QString &errorString);

    QStringList requestedSubProtocols() const;

    void socketDestroyed(QObject *socket);

    void processData();
    void processPing(const QByteArray &data);
    void processPong(const QByteArray &data);
    void processClose(QWebSocketProtocol::CloseCode closeCode, QString closeReason);
    void processHandshake(QTcpSocket *pSocket);
    void processStateChanged(QAbstractSocket::SocketState socketState);

    Q_REQUIRED_RESULT qint64 doWriteFrames(const QByteArray &data, bool isBinary);

    void makeConnections(QTcpSocket *pTcpSocket);
    void releaseConnections(const QTcpSocket *pTcpSocket);

    QByteArray getFrameHeader(QWebSocketProtocol::OpCode opCode, quint64 payloadLength,
                              quint32 maskingKey, bool lastFrame);
    QString calculateAcceptKey(const QByteArray &key) const;
    QString createHandShakeRequest(QString resourceName,
                                   QString host,
                                   QString origin,
                                   QString extensions,
                                   const QStringList &options,
                                   QByteArray key,
                                   const QList<QPair<QString, QString> > &headers);

    Q_REQUIRED_RESULT static QWebSocket *
    upgradeFrom(QTcpSocket *tcpSocket,
                const QWebSocketHandshakeRequest &request,
                const QWebSocketHandshakeResponse &response,
                QObject *parent = nullptr);

    quint32 generateMaskingKey() const;
    QByteArray generateKey() const;
    Q_REQUIRED_RESULT qint64 writeFrames(const QList<QByteArray> &frames);
    Q_REQUIRED_RESULT qint64 writeFrame(const QByteArray &frame);
    void emitErrorOccurred(QAbstractSocket::SocketError error);

    QTcpSocket *m_pSocket;
    QString m_errorString;
    QWebSocketProtocol::Version m_version;
    QUrl m_resource;
    QString m_resourceName;
    QNetworkRequest m_request;
    QWebSocketHandshakeOptions m_options;
    QString m_origin;
    QString m_protocol;
    QString m_extension;
    QAbstractSocket::SocketState m_socketState;
    QAbstractSocket::PauseModes m_pauseMode;
    qint64 m_readBufferSize;

    // For WWW-Authenticate handling
    QAuthenticator m_authenticator;
    qint64 m_bytesToSkipBeforeNewResponse = 0;

    QByteArray m_key;	//identification key used in handshake requests

    bool m_mustMask;	//a server must not mask the frames it sends

    bool m_isClosingHandshakeSent;
    bool m_isClosingHandshakeReceived;

    // For WWW-Authenticate handling
    bool m_needsResendWithCredentials = false;
    bool m_needsReconnect = false;

    QWebSocketProtocol::CloseCode m_closeCode;
    QString m_closeReason;

    QElapsedTimer m_pingTimer;

    QWebSocketDataProcessor *m_dataProcessor = new QWebSocketDataProcessor();
    QWebSocketConfiguration m_configuration;

    QMaskGenerator *m_pMaskGenerator;
    QDefaultMaskGenerator m_defaultMaskGenerator;

    quint64 m_outgoingFrameSize;

    friend class QWebSocketServerPrivate;
#ifdef Q_OS_WASM
    EMSCRIPTEN_WEBSOCKET_T m_socketContext = 0;
    uint16_t m_readyState = 0;
#endif
};

QT_END_NAMESPACE

#endif // QWEBSOCKET_H
