// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef LOCATIONSINGLETON_H
#define LOCATIONSINGLETON_H

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

#include <QtCore/QObject>
#include <QtCore/qnumeric.h>
#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoShape>
#include <QtPositioning/QGeoRectangle>
#include <QtPositioning/QGeoCircle>
#include <QtPositioning/QGeoPath>
#include <QtPositioning/QGeoPolygon>
#include <QtQml/QJSValue>
#include <QVariant>
#include <QPointF>
#include <QQmlEngine>
#include <QtPositioningQuick/private/qpositioningquickglobal_p.h>

QT_BEGIN_NAMESPACE

class Q_POSITIONINGQUICK_EXPORT LocationSingleton : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(QtPositioning)
    QML_SINGLETON
    QML_ADDED_IN_VERSION(5, 0)

public:
    explicit LocationSingleton(QObject *parent = 0);

    Q_INVOKABLE QGeoCoordinate coordinate() const;
    Q_INVOKABLE QGeoCoordinate coordinate(double latitude, double longitude,
                                          double altitude = qQNaN()) const;

    Q_INVOKABLE QGeoShape shape() const;

    Q_INVOKABLE QGeoRectangle rectangle() const;
    Q_INVOKABLE QGeoRectangle rectangle(const QGeoCoordinate &center,
                                        double width, double height) const;
    Q_INVOKABLE QGeoRectangle rectangle(const QGeoCoordinate &topLeft,
                                        const QGeoCoordinate &bottomRight) const;
    Q_INVOKABLE QGeoRectangle rectangle(const QVariantList &coordinates) const;

    Q_INVOKABLE QGeoCircle circle() const;
    Q_INVOKABLE QGeoCircle circle(const QGeoCoordinate &center, qreal radius = -1.0) const;

    Q_INVOKABLE QGeoPath path() const;
    Q_INVOKABLE QGeoPath path(const QJSValue &value, qreal width = 0.0) const;

    Q_INVOKABLE QGeoPolygon polygon() const;
    Q_INVOKABLE QGeoPolygon polygon(const QVariantList &value) const;
    Q_INVOKABLE QGeoPolygon polygon(const QVariantList &perimeter, const QVariantList &holes) const;

    Q_INVOKABLE QGeoCircle shapeToCircle(const QGeoShape &shape) const;
    Q_INVOKABLE QGeoRectangle shapeToRectangle(const QGeoShape &shape) const;
    Q_INVOKABLE QGeoPath shapeToPath(const QGeoShape &shape) const;
    Q_INVOKABLE QGeoPolygon shapeToPolygon(const QGeoShape &shape) const;

    Q_REVISION(5, 12) Q_INVOKABLE QGeoCoordinate mercatorToCoord(const QPointF &mercator) const;
    Q_REVISION(5, 12) Q_INVOKABLE QPointF coordToMercator(const QGeoCoordinate &coord) const;
};

QT_END_NAMESPACE

#endif // LOCATIONSINGLETON_H
