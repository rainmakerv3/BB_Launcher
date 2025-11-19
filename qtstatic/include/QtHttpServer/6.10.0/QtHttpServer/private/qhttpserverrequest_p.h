// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERREQUEST_P_H
#define QHTTPSERVERREQUEST_P_H

#include <QtHttpServer/qhttpserverrequest.h>
#include <QtNetwork/private/qhttpheaderparser_p.h>
#include <QtCore/private/qbytedata_p.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QHttpServer. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

QT_BEGIN_NAMESPACE

class QHttp2Stream;

class QHttpServerRequestPrivate : public QSharedData
{
    friend class QHttpServerParser;

public:
    QHttpServerRequestPrivate(const QHostAddress &remoteAddress, quint16 remotePort,
                              const QHostAddress &localAddress, quint16 localPort);
#if QT_CONFIG(ssl)
    QHttpServerRequestPrivate(const QHostAddress &remoteAddress, quint16 remotePort,
                              const QHostAddress &localAddress, quint16 localPort,
                              const QSslConfiguration &sslConfiguration);
#endif
    QHttpServerRequestPrivate() = default;
    QHttpServerRequestPrivate(const QHttpServerRequestPrivate &other) = default;

    QUrl url;
    QHttpServerRequest::Method method;
    QHttpHeaders headers;

    qint64 contentLength() const;

    QHostAddress remoteAddress;
    quint16 remotePort;
    QHostAddress localAddress;
    quint16 localPort;
    int majorVersion;
    int minorVersion;
#if QT_CONFIG(ssl)
    QSslConfiguration sslConfiguration;
#endif
    QByteArray body;
};

QT_END_NAMESPACE

#endif // QHTTPSERVERREQUEST_P_H
