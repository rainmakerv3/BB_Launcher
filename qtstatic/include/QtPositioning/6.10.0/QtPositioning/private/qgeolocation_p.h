// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOLOCATION_P_H
#define QGEOLOCATION_P_H

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

#include <QtCore/QSharedData>
#include <QtPositioning/QGeoAddress>
#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoShape>
#include <QVariantMap>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class QGeoLocationPrivate : public QSharedData
{
public:
    QGeoLocationPrivate();
    QGeoLocationPrivate(const QGeoLocationPrivate &other);

    ~QGeoLocationPrivate();

    bool operator==(const QGeoLocationPrivate &other) const;

    bool isEmpty() const;

    QGeoAddress address;
    QGeoCoordinate coordinate;
    QGeoShape viewport;
    QVariantMap extendedAttributes;
};

QT_END_NAMESPACE

#endif
