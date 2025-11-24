// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOCIRCLE_H
#define QGEOCIRCLE_H

#include <QtPositioning/QGeoRectangle>

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoCirclePrivate;

class Q_POSITIONING_EXPORT QGeoCircle : public QGeoShape
{
    Q_GADGET
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)

public:
    QGeoCircle();
    QGeoCircle(const QGeoCoordinate &center, qreal radius = -1.0);
    QGeoCircle(const QGeoCircle &other);
    QGeoCircle(const QGeoShape &other);

    ~QGeoCircle();

    QGeoCircle &operator=(const QGeoCircle &other);

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    void setRadius(qreal radius);
    qreal radius() const;

    Q_INVOKABLE void translate(double degreesLatitude, double degreesLongitude);
    Q_INVOKABLE QGeoCircle translated(double degreesLatitude, double degreesLongitude) const;
    Q_INVOKABLE void extendCircle(const QGeoCoordinate &coordinate);

    Q_INVOKABLE QString toString() const;

private:
    inline QGeoCirclePrivate *d_func();
    inline const QGeoCirclePrivate *d_func() const;

#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &stream, const QGeoCircle &circle)
    {
        return stream << static_cast<const QGeoShape &>(circle);
    }
    friend QDataStream &operator>>(QDataStream &stream, QGeoCircle &circle)
    {
        return stream >> static_cast<QGeoShape &>(circle);
    }
#endif
};

Q_DECLARE_TYPEINFO(QGeoCircle, Q_RELOCATABLE_TYPE);

QT_END_NAMESPACE

QT_DECL_METATYPE_EXTERN(QGeoCircle, Q_POSITIONING_EXPORT)

#endif

