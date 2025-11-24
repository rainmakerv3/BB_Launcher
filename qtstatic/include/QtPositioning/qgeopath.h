// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOPATH_H
#define QGEOPATH_H

#include <QtPositioning/QGeoShape>
#include <QtCore/QVariantList>

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoPathPrivate;

class Q_POSITIONING_EXPORT QGeoPath : public QGeoShape
{
    Q_GADGET
    Q_PROPERTY(QVariantList path READ variantPath WRITE setVariantPath)
    Q_PROPERTY(qreal width READ width WRITE setWidth)

public:
    QGeoPath();
    QGeoPath(const QList<QGeoCoordinate> &path, const qreal &width = 0.0);
    QGeoPath(const QGeoPath &other);
    QGeoPath(const QGeoShape &other);

    ~QGeoPath();

    QGeoPath &operator=(const QGeoPath &other);

    void setPath(const QList<QGeoCoordinate> &path);
    const QList<QGeoCoordinate> &path() const;
    void clearPath();
    void setVariantPath(const QVariantList &path);
    QVariantList variantPath() const;

    void setWidth(const qreal &width);
    qreal width() const;

    Q_INVOKABLE void translate(double degreesLatitude, double degreesLongitude);
    Q_INVOKABLE QGeoPath translated(double degreesLatitude, double degreesLongitude) const;
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
    inline QGeoPathPrivate *d_func();
    inline const QGeoPathPrivate *d_func() const;

#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &stream, const QGeoPath &path)
    {
        return stream << static_cast<const QGeoShape &>(path);
    }
    friend QDataStream &operator>>(QDataStream &stream, QGeoPath &path)
    {
        return stream >> static_cast<QGeoShape &>(path);
    }
#endif
};

Q_DECLARE_TYPEINFO(QGeoPath, Q_RELOCATABLE_TYPE);

QT_END_NAMESPACE

QT_DECL_METATYPE_EXTERN(QGeoPath, Q_POSITIONING_EXPORT)

#endif // QGEOPATH_H
