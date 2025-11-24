// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOSHAPE_H
#define QGEOSHAPE_H

#include <QtCore/QSharedDataPointer>
#include <QtPositioning/QGeoCoordinate>

QT_BEGIN_NAMESPACE

class QDebug;
class QGeoShapePrivate;
class QGeoRectangle;

class Q_POSITIONING_EXPORT QGeoShape
{
    Q_GADGET
    Q_PROPERTY(ShapeType type READ type)
    Q_PROPERTY(bool isValid READ isValid)
    Q_PROPERTY(bool isEmpty READ isEmpty)
    Q_PROPERTY(QGeoCoordinate center READ center)
    Q_ENUMS(ShapeType)

public:
    QGeoShape();
    Q_INVOKABLE QGeoShape(const QGeoShape &other);
    ~QGeoShape();

    enum ShapeType {
        UnknownType,
        RectangleType,
        CircleType,
        PathType,
        PolygonType
    };

    ShapeType type() const;

    bool isValid() const;
    bool isEmpty() const;
    Q_INVOKABLE bool contains(const QGeoCoordinate &coordinate) const;
    Q_INVOKABLE QGeoRectangle boundingGeoRectangle() const;
    QGeoCoordinate center() const;

    friend bool operator==(const QGeoShape &lhs, const QGeoShape &rhs)
    {
        return equals(lhs, rhs);
    }
    friend bool operator!=(const QGeoShape &lhs, const QGeoShape &rhs)
    {
        return !equals(lhs, rhs);
    }

    QGeoShape &operator=(const QGeoShape &other);

    Q_INVOKABLE QString toString() const;
protected:
    QGeoShape(QGeoShapePrivate *d);

    QSharedDataPointer<QGeoShapePrivate> d_ptr;

private:
    static bool equals(const QGeoShape &lhs, const QGeoShape &rhs);
    inline QGeoShapePrivate *d_func();
    inline const QGeoShapePrivate *d_func() const;
#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug dbg, const QGeoShape &shape)
    {
        return debugStreaming(dbg, shape);
    }
    static QDebug debugStreaming(QDebug dbg, const QGeoShape &shape);
#endif
#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &stream, const QGeoShape &shape)
    {
        return dataStreamOut(stream, shape);
    }
    friend QDataStream &operator>>(QDataStream &stream, QGeoShape &shape)
    {
        return dataStreamIn(stream, shape);
    }
    static QDataStream &dataStreamOut(QDataStream &stream, const QGeoShape &shape);
    static QDataStream &dataStreamIn(QDataStream &stream, QGeoShape &shape);
#endif
    friend Q_POSITIONING_EXPORT size_t qHash(const QGeoShape &key, size_t seed) noexcept;
};

Q_POSITIONING_EXPORT size_t qHash(const QGeoShape &shape, size_t seed = 0) noexcept;

Q_DECLARE_TYPEINFO(QGeoShape, Q_RELOCATABLE_TYPE);

QT_END_NAMESPACE

QT_DECL_METATYPE_EXTERN(QGeoShape, Q_POSITIONING_EXPORT)

#endif
