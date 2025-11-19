// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERRESPONSE_P_H
#define QHTTPSERVERRESPONSE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QHttpServerResponse. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include <private/qabstracthttpserver_p.h>

#include <QtHttpServer/qhttpserverresponse.h>
#include <QtNetwork/qhttpheaders.h>

#include <functional>
#include <unordered_map>

QT_BEGIN_NAMESPACE

class QHttpServerResponsePrivate
{
public:
    QHttpServerResponsePrivate(QByteArray &&d, const QHttpServerResponse::StatusCode sc);
    QHttpServerResponsePrivate(const QHttpServerResponse::StatusCode sc);

    QByteArray data;
    QHttpServerResponse::StatusCode statusCode;
    QHttpHeaders headers;
};

QT_END_NAMESPACE

#endif // QHTTPSERVERRESPONSE_P_H
