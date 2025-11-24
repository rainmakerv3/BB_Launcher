// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QGEOPOSITIONINFO_P_H
#define QGEOPOSITIONINFO_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtPositioning/private/qpositioningglobal_p.h>
#include "qgeopositioninfo.h"
#include <QHash>
#include <QDateTime>
#include <QtPositioning/qgeocoordinate.h>

QT_BEGIN_NAMESPACE

class QGeoPositionInfoPrivate : public QSharedData
{
public:
    QGeoPositionInfoPrivate();
    QGeoPositionInfoPrivate(const QGeoPositionInfoPrivate &other);
    virtual ~QGeoPositionInfoPrivate();
    bool operator==(const QGeoPositionInfoPrivate &other) const;

    QDateTime timestamp;
    QGeoCoordinate coord;
    QHash<QGeoPositionInfo::Attribute, qreal> doubleAttribs;

    static QGeoPositionInfoPrivate *get(const QGeoPositionInfo &info);
};

QT_END_NAMESPACE

#endif // QGEOPOSITIONINFO_P_H
