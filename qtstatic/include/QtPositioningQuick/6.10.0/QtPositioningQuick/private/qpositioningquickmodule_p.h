// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT_POSITIONINGQUICKMODULE_P_H
#define QT_POSITIONINGQUICKMODULE_P_H

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

#include <QtQml/qqml.h>
#include "qpositioningquickglobal_p.h"

#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoAddress>
#include <QtPositioning/QGeoRectangle>
#include <QtPositioning/QGeoCircle>
#include <QtPositioning/QGeoPath>
#include <QtPositioning/QGeoPolygon>
#include <QtPositioning/QGeoLocation>
#include <QtPositioning/QGeoShape>
#include <QtPositioning/QGeoPositionInfo>
#include <QtPositioning/QGeoSatelliteInfo>
#include <QtPositioning/private/qgeocoordinateobject_p.h>

QT_BEGIN_NAMESPACE

struct QGeoCoordinateForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoCoordinate)
    QML_VALUE_TYPE(geoCoordinate)
    QML_STRUCTURED_VALUE
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoAddressForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoAddress)
    QML_VALUE_TYPE(geoAddress)
    QML_STRUCTURED_VALUE
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoRectangleForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoRectangle)
    QML_VALUE_TYPE(geoRectangle)
    QML_STRUCTURED_VALUE
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoCircleForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoCircle)
    QML_VALUE_TYPE(geoCircle)
    QML_STRUCTURED_VALUE
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoPathForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoPath)
    QML_VALUE_TYPE(geoPath)
    QML_STRUCTURED_VALUE
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoPolygonForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoPolygon)
    QML_VALUE_TYPE(geoPolygon)
    QML_STRUCTURED_VALUE
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoLocationForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoLocation)
    QML_VALUE_TYPE(geoLocation)
    QML_STRUCTURED_VALUE
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoShapeForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoShape)
    QML_VALUE_TYPE(geoShape)
    QML_CONSTRUCTIBLE_VALUE
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoCoordinateObjectForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoCoordinateObject)
    QML_ANONYMOUS
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoPositionInfoForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoPositionInfo)
    QML_VALUE_TYPE(geoPositionInfo)
    QML_STRUCTURED_VALUE
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoSatelliteInfoForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoSatelliteInfo)
    QML_VALUE_TYPE(geoSatelliteInfo)
    QML_ADDED_IN_VERSION(6, 5)
};

// To prevent the same QGeoSatelliteInfo type from being exported into qmltypes
// twice for a value type and the enums. See QTBUG-115361.
class QGeoSatelliteInfoDerived : public QGeoSatelliteInfo
{
    Q_GADGET
};

namespace QGeoSatelliteInfoForeignNamespace
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QGeoSatelliteInfoDerived)
    QML_NAMED_ELEMENT(GeoSatelliteInfo)
    QML_ADDED_IN_VERSION(6, 5)
}

QT_END_NAMESPACE

#endif // QT_POSITIONINGQUICKMODULE_P_H
