// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOCOORDINATE_H
#define QGEOCOORDINATE_H

#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QDebug>
#include <QtPositioning/qpositioningglobal.h>

QT_BEGIN_NAMESPACE

class QDebug;
class QDataStream;

class QGeoCoordinatePrivate;
QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoCoordinatePrivate, Q_POSITIONING_EXPORT)

class Q_POSITIONING_EXPORT QGeoCoordinate
{
    Q_GADGET
    Q_ENUMS(CoordinateFormat)

    Q_PROPERTY(double latitude READ latitude WRITE setLatitude)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude)
    Q_PROPERTY(double altitude READ altitude WRITE setAltitude)
    Q_PROPERTY(bool isValid READ isValid)

public:

    enum CoordinateType {
        InvalidCoordinate,
        Coordinate2D,
        Coordinate3D
    };

    enum CoordinateFormat {
        Degrees,
        DegreesWithHemisphere,
        DegreesMinutes,
        DegreesMinutesWithHemisphere,
        DegreesMinutesSeconds,
        DegreesMinutesSecondsWithHemisphere
    };

    QGeoCoordinate();
    QGeoCoordinate(double latitude, double longitude);
    QGeoCoordinate(double latitude, double longitude, double altitude);
    QGeoCoordinate(const QGeoCoordinate &other);
    QGeoCoordinate(QGeoCoordinate &&other) noexcept = default;
    ~QGeoCoordinate();

    QGeoCoordinate &operator=(const QGeoCoordinate &other);
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QGeoCoordinate)

    void swap(QGeoCoordinate &other) noexcept { d.swap(other.d); }

    friend bool operator==(const QGeoCoordinate &lhs, const QGeoCoordinate &rhs)
    {
        return equals(lhs, rhs);
    }
    friend bool operator!=(const QGeoCoordinate &lhs, const QGeoCoordinate &rhs)
    {
        return !equals(lhs, rhs);
    }

    bool isValid() const;
    CoordinateType type() const;

    void setLatitude(double latitude);
    double latitude() const;

    void setLongitude(double longitude);
    double longitude() const;

    void setAltitude(double altitude);
    double altitude() const;

    Q_INVOKABLE qreal distanceTo(const QGeoCoordinate &other) const;
    Q_INVOKABLE qreal azimuthTo(const QGeoCoordinate &other) const;

    Q_INVOKABLE QGeoCoordinate atDistanceAndAzimuth(qreal distance, qreal azimuth, qreal distanceUp = 0.0) const;

    Q_INVOKABLE QString toString(CoordinateFormat format = DegreesMinutesSecondsWithHemisphere) const;

private:
    static bool equals(const QGeoCoordinate &lhs, const QGeoCoordinate &rhs);
    QGeoCoordinate(QGeoCoordinatePrivate &dd);
    QSharedDataPointer<QGeoCoordinatePrivate> d;
    friend class QGeoCoordinatePrivate;
    friend class QQuickGeoCoordinateAnimation;
#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug dbg, const QGeoCoordinate &coord)
    {
        return debugStreaming(dbg, coord);
    }
    static QDebug debugStreaming(QDebug dbg, const QGeoCoordinate &coord);
#endif
#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &stream, const QGeoCoordinate &coordinate)
    {
        return dataStreamOut(stream, coordinate);
    }
    friend QDataStream &operator>>(QDataStream &stream, QGeoCoordinate &coordinate)
    {
        return dataStreamIn(stream, coordinate);
    }
    static QDataStream &dataStreamOut(QDataStream &stream, const QGeoCoordinate &coordinate);
    static QDataStream &dataStreamIn(QDataStream &stream, QGeoCoordinate &coordinate);
#endif
};

Q_DECLARE_SHARED(QGeoCoordinate)


Q_POSITIONING_EXPORT size_t qHash(const QGeoCoordinate &coordinate, size_t seed = 0);


QT_END_NAMESPACE

QT_DECL_METATYPE_EXTERN(QGeoCoordinate, Q_POSITIONING_EXPORT)

#endif
