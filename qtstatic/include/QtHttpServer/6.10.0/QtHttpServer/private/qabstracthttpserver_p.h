// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QABSTRACTHTTPSERVER_P_H
#define QABSTRACTHTTPSERVER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QHttpServer. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include <QtHttpServer/qabstracthttpserver.h>
#include <QtHttpServer/qthttpserverglobal.h>
#include <QtHttpServer/qhttpserverconfiguration.h>
#include <QtHttpServer/private/qhttpserverrequestfilter_p.h>

#include <private/qobject_p.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qtimer.h>

#include <vector>

#if defined(QT_WEBSOCKETS_LIB)
#include <QtWebSockets/qwebsocketserver.h>
#endif // defined(QT_WEBSOCKETS_LIB)

#if QT_CONFIG(ssl)
#include <QtNetwork/qhttp2configuration.h>
#endif

QT_BEGIN_NAMESPACE

class QHttpServerRequest;

class QAbstractHttpServerPrivate: public QObjectPrivate
{
public:
    Q_DECLARE_PUBLIC(QAbstractHttpServer)

    QAbstractHttpServerPrivate();

#if defined(QT_WEBSOCKETS_LIB)
    QWebSocketServer websocketServer {
        QCoreApplication::applicationName() + QLatin1Char('/') + QCoreApplication::applicationVersion(),
        QWebSocketServer::NonSecureMode
    };
#endif // defined(QT_WEBSOCKETS_LIB)

    void handleNewConnections();
    bool verifyThreadAffinity(const QObject *contextObject) const;

#if QT_CONFIG(localserver)
    void handleNewLocalConnections();
#endif

    void createHttp1Handler(QIODevice *socket);
#if QT_CONFIG(ssl) && QT_CONFIG(http)
    void createHttp2Handler(QIODevice *socket);
#endif
    void restartHeartbeatTimer();

#if defined(QT_WEBSOCKETS_LIB)
    mutable bool handlingWebSocketUpgrade = false;
    struct WebSocketUpgradeVerifier
    {
        QPointer<const QObject> context;
        QtPrivate::SlotObjUniquePtr slotObject;
    };
    std::vector<WebSocketUpgradeVerifier> webSocketUpgradeVerifiers;
#endif // defined(QT_WEBSOCKETS_LIB)
#if QT_CONFIG(ssl)
    QHttp2Configuration h2Configuration;
#endif
    QHttpServerConfiguration configuration;
    QHttpServerRequestFilter requestFilter;
    QTimer heartbeatTimer;
};

QT_END_NAMESPACE

#endif // QABSTRACTHTTPSERVER_P_H
