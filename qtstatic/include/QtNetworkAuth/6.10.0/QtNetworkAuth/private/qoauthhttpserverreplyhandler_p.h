// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the Network Access API.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QOAUTHHTTPSERVERREPLYHANDLER_P_H
#define QOAUTHHTTPSERVERREPLYHANDLER_P_H

#include <QtNetworkAuth/qoauthglobal.h>
#include <QtNetworkAuth/qoauthhttpserverreplyhandler.h>

#include <private/qobject_p.h>

#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/qtcpserver.h>

#include <utility>

QT_BEGIN_NAMESPACE

class QOAuthHttpServerReplyHandlerPrivate
{
    Q_DECLARE_PUBLIC(QOAuthHttpServerReplyHandler)

public:
    explicit QOAuthHttpServerReplyHandlerPrivate(QOAuthHttpServerReplyHandler *p);
    ~QOAuthHttpServerReplyHandlerPrivate();

    QString callback() const;
    QString callbackHost() const;

    QTcpServer *httpServer = nullptr;
    QString text;
    QString path;
    QHostAddress callbackAddress;
    QString callbackHostname;
    quint16 callbackPort = 0;

private:
    void _q_clientConnected();
    void _q_readData(QTcpSocket *socket);
    void _q_answerClient(QTcpSocket *socket, const QUrl &url);
    void initializeLocalServer();
    bool listen(const QHostAddress &address, quint16 port);

    struct QHttpRequest {
        quint16 port = 0;

        bool readMethod(QTcpSocket *socket);
        bool readUrl(QTcpSocket *socket);
        bool readStatus(QTcpSocket *socket);
        bool readHeader(QTcpSocket *socket);

        enum class State {
            ReadingMethod,
            ReadingUrl,
            ReadingStatus,
            ReadingHeader,
            ReadingBody,
            AllDone
        } state = State::ReadingMethod;
        QByteArray fragment;

        enum class Method {
            Unknown,
            Head,
            Get,
            Put,
            Post,
            Delete,
        } method = Method::Unknown;
        QUrl url;
        std::pair<quint8, quint8> version;
        QMap<QByteArray, QByteArray> headers;
    };

    QMap<QTcpSocket *, QHttpRequest> clients;

    QOAuthHttpServerReplyHandler *q_ptr;
};

QT_END_NAMESPACE

#endif // QOAUTHHTTPSERVERREPLYHANDLER_P_H
