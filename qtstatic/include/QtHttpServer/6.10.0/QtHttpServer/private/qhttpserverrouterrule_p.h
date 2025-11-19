// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERROUTERRULE_P_H
#define QHTTPSERVERROUTERRULE_P_H

#include <QtHttpServer/qhttpserverrouterrule.h>

#include <QtCore/qregularexpression.h>
#include <QtCore/qstring.h>
#include <QtCore/qpointer.h>

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

class QHttpServerRouterRulePrivate
{
public:
    QString pathPattern;
    QHttpServerRequest::Methods methods;
    QtPrivate::SlotObjUniquePtr routerHandler;
    QPointer<const QObject> context;

    QRegularExpression pathRegexp;
};

QT_END_NAMESPACE

#endif // QHTTPSERVERROUTERRULE_P_H
