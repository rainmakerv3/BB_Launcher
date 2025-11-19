// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVER_P_H
#define QHTTPSERVER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QHttpServer. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include <private/qabstracthttpserver_p.h>

#include <QtHttpServer/qhttpserver.h>
#include <QtHttpServer/qhttpserverresponse.h>
#include <QtHttpServer/qhttpserverrequest.h>
#include <QtHttpServer/qhttpserverrouter.h>

#include <QtCore/qglobal.h>

#include <vector>

QT_BEGIN_NAMESPACE

class QHttpServerPrivate: public QAbstractHttpServerPrivate
{
    Q_DECLARE_PUBLIC(QHttpServer)

public:
    QHttpServerPrivate(QHttpServer *p);

    QHttpServerRouter router;
    struct AfterRequestHandler
    {
        QPointer<const QObject> context;
        QtPrivate::SlotObjUniquePtr slotObject;
    };
    std::vector<AfterRequestHandler> afterRequestHandlers;
    struct MissingHandler
    {
        QPointer<const QObject> context = nullptr;
        QtPrivate::SlotObjUniquePtr slotObject;
    } missingHandler;

    void callMissingHandler(const QHttpServerRequest &request, QHttpServerResponder &responder);
};

QT_END_NAMESPACE

#endif // QHTTPSERVER_P_H
