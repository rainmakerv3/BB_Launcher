// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERROUTER_P_H
#define QHTTPSERVERROUTER_P_H

#include <QtHttpServer/qhttpserverrouter.h>
#include <QtHttpServer/qhttpserverrouterrule.h>

#include <QtCore/qhash.h>
#include <QtCore/qstring.h>

#include <memory>
#include <vector>

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

class QHttpServerRouterPrivate
{
public:
    QHttpServerRouterPrivate(QAbstractHttpServer *server);

    QHash<QMetaType, QString> converters;
    std::vector<std::unique_ptr<QHttpServerRouterRule>> rules;
    QAbstractHttpServer *server;

    bool verifyThreadAffinity(const QObject *contextObject) const;
};

QT_END_NAMESPACE

#endif // QHTTPSERVERROUTER_P_H
