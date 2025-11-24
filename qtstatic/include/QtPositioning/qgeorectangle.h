// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEORECTANGLE_H
#define QGEORECTANGLE_H

#include <QtPositioning/QGeoShape>

QT_BEGIN_NAMESPACE

class QGeoRectanglePrivate;

class Q_POSITIONING_EXPORT QGeoRectangle : public QGeoShape
{
    Q_GADGET
    Q_PROPERTY(QGeoCoordinate bottomLeft READ bottomLeft WRITE setBottomLeft)
    Q_PROPERTY(QGeoCoordinate bottomRight READ bottomRight WRITE setBottomRight)
    Q_PROPERTY(QGeoCoordinate topLeft READ topLeft WRITE setTopLeft)
    Q_PROPERTY(QGeoCoordinate topRight READ topRight WRITE setTopRight)
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter)
    Q_PROPERTY(double height READ height WRITE setHeight)
    Q_PROPERTY(double width READ width WRITE setWidth)

public:
    QGeoRectangle();
    QGeoRectangle(const QGeoCoordinate &center, double degreesWidth, double degreesHeight);
    QGeoRectangle(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight);
    QGeoRectangle(const QList<QGeoCoordinate> &coordinates);
    QGeoRectangle(const QGeoRectangle &other);
    QGeoRectangle(const QGeoShape &other);

    ~QGeoRectangle();

    QGeoRectangle &operator=(const QGeoRectangle &other);

    void setTopLeft(const QGeoCoordinate &topLeft);
    QGeoCoordinate topLeft() const;

    void setTopRight(const QGeoCoordinate &topRight);
    QGeoCoordinate topRight() const;

    void setBottomLeft(const QGeoCoordinate &bottomLeft);
    QGeoCoordinate bottomLeft() const;

    void setBottomRight(const QGeoCoordinate &bottomRight);
    QGeoCoordinate bottomRight() const;

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    void setWidth(double degreesWidth);
    double width() const;

    void setHeight(double degreesHeight);
    double height() const;

    using QGeoShape::contains;
    bool contains(const QGeoRectangle &rectangle) const;
    Q_INVOKABLE bool intersects(const QGeoRectangle &rectangle) const;

    Q_INVOKABLE void translate(double degreesLatitude, double degreesLongitude);
    Q_INVOKABLE QGeoRectangle translated(double degreesLatitude, double degreesLongitude) const;
    Q_INVOKABLE void extendRectangle(const QGeoCoordinate &coordinate);

    Q_INVOKABLE QGeoRectangle united(const QGeoRectangle &rectangle) const;
    QGeoRectangle operator|(const QGeoRectangle &rectangle) const;
    QGeoRectangle &operator|=(const QGeoRectangle &rectangle);

    Q_INVOKABLE QString toString() const;

private:
    inline QGeoRectanglePrivate *d_func();
    inline const QGeoRectanglePrivate *d_func() const;

#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &stream, const QGeoRectangle &rectangle)
    {
        return stream << static_cast<const QGeoShape &>(rectangle);
    }
    friend QDataStream &operator>>(QDataStream &stream, QGeoRectangle &rectangle)
    {
        return stream >> static_cast<QGeoShape &>(rectangle);
    }
#endif
};

Q_DECLARE_TYPEINFO(QGeoRectangle, Q_RELOCATABLE_TYPE);

inline QGeoRectangle QGeoRectangle::operator|(const QGeoRectangle &rectangle) const
{
    return united(rectangle);
}

QT_END_NAMESPACE

QT_DECL_METATYPE_EXTERN(QGeoRectangle, Q_POSITIONING_EXPORT)

#endif
