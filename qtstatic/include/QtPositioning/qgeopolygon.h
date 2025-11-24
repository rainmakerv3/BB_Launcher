// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOPOLYGON_H
#define QGEOPOLYGON_H

#include <QtPositioning/QGeoShape>
#include <QtCore/QVariantList>

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoPolygonPrivate;

class Q_POSITIONING_EXPORT QGeoPolygon : public QGeoShape
{
    Q_GADGET
    Q_PROPERTY(QList<QGeoCoordinate> perimeter READ perimeter WRITE setPerimeter REVISION(5, 12))

public:
    QGeoPolygon();
    QGeoPolygon(const QList<QGeoCoordinate> &path);
    QGeoPolygon(const QGeoPolygon &other);
    QGeoPolygon(const QGeoShape &other);

    ~QGeoPolygon();

    QGeoPolygon &operator=(const QGeoPolygon &other);

    void setPerimeter(const QList<QGeoCoordinate> &path);
    const QList<QGeoCoordinate> &perimeter() const;

    Q_INVOKABLE void addHole(const QVariant &holePath);
                void addHole(const QList<QGeoCoordinate> &holePath);
    Q_INVOKABLE const QVariantList hole(qsizetype index) const;
                const QList<QGeoCoordinate> holePath(qsizetype index) const;
    Q_INVOKABLE void removeHole(qsizetype index);
    Q_INVOKABLE qsizetype holesCount() const;
    Q_INVOKABLE void translate(double degreesLatitude, double degreesLongitude);
    Q_INVOKABLE QGeoPolygon translated(double degreesLatitude, double degreesLongitude) const;
    Q_INVOKABLE double length(qsizetype indexFrom = 0, qsizetype indexTo = -1) const;
    Q_INVOKABLE qsizetype size() const;
    Q_INVOKABLE void addCoordinate(const QGeoCoordinate &coordinate);
    Q_INVOKABLE void insertCoordinate(qsizetype index, const QGeoCoordinate &coordinate);
    Q_INVOKABLE void replaceCoordinate(qsizetype index, const QGeoCoordinate &coordinate);
    Q_INVOKABLE QGeoCoordinate coordinateAt(qsizetype index) const;
    Q_INVOKABLE bool containsCoordinate(const QGeoCoordinate &coordinate) const;
    Q_INVOKABLE void removeCoordinate(const QGeoCoordinate &coordinate);
    Q_INVOKABLE void removeCoordinate(qsizetype index);

    Q_INVOKABLE QString toString() const;

private:
    inline QGeoPolygonPrivate *d_func();
    inline const QGeoPolygonPrivate *d_func() const;

#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &stream, const QGeoPolygon &polygon)
    {
        return stream << static_cast<const QGeoShape &>(polygon);
    }
    friend QDataStream &operator>>(QDataStream &stream, QGeoPolygon &polygon)
    {
        return stream >> static_cast<QGeoShape &>(polygon);
    }
#endif
};

Q_DECLARE_TYPEINFO(QGeoPolygon, Q_RELOCATABLE_TYPE);

QT_END_NAMESPACE

QT_DECL_METATYPE_EXTERN(QGeoPolygon, Q_POSITIONING_EXPORT)

#endif // QGEOPOLYGON_H
