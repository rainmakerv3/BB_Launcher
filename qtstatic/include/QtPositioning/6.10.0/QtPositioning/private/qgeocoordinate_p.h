// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOCOORDINATE_P_H
#define QGEOCOORDINATE_P_H

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

#include <QSharedData>
#include "qgeocoordinate.h"
#include "private/qglobal_p.h"

QT_BEGIN_NAMESPACE

class Q_POSITIONING_EXPORT QGeoCoordinatePrivate : public QSharedData
{
public:
    QGeoCoordinatePrivate();
    QGeoCoordinatePrivate(const QGeoCoordinatePrivate &other);
    // The destructor needs to be virtual because we have a derived class
    // QGeoMercatorCoordinatePrivate, which is used by
    // QQuickGeoCoordinateAnimation to create QGeoCoordinate.
    virtual ~QGeoCoordinatePrivate();

    double lat;
    double lng;
    double alt;

    static void atDistanceAndAzimuth(const QGeoCoordinate &coord,
                                     qreal distance, qreal azimuth,
                                     double *lon, double *lat);
    static const QGeoCoordinatePrivate *get(const QGeoCoordinate *c) {
           return c->d.constData();
    }
};

class Q_POSITIONING_EXPORT QGeoMercatorCoordinatePrivate : public QGeoCoordinatePrivate
{
public:
    QGeoMercatorCoordinatePrivate();
    QGeoMercatorCoordinatePrivate(const QGeoMercatorCoordinatePrivate &other);
    ~QGeoMercatorCoordinatePrivate();

    double m_mercatorX;
    double m_mercatorY;
};


QT_END_NAMESPACE

#endif // QGEOCOORDINATE_P_H
