// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOPOLYGON_P_H
#define QGEOPOLYGON_P_H

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

#include <QtPositioning/private/qgeopath_p.h>
#include <QtPositioning/qgeopolygon.h>
#include <QtPositioning/private/qclipperutils_p.h>

QT_BEGIN_NAMESPACE

class Q_POSITIONING_EXPORT QGeoPolygonPrivate : public QGeoPathPrivate
{
public:
    QGeoPolygonPrivate();
    QGeoPolygonPrivate(const QList<QGeoCoordinate> &path);
    ~QGeoPolygonPrivate();

// QGeoShape API
    virtual QGeoShapePrivate *clone() const override;
    virtual bool isValid() const override;
    virtual bool contains(const QGeoCoordinate &coordinate) const override;
    virtual void translate(double degreesLatitude, double degreesLongitude) override;
    virtual bool operator==(const QGeoShapePrivate &other) const override;
    size_t hash(size_t seed) const override;

// QGeoPath API
    virtual void markDirty() override;

// QGeoPolygonPrivate API
    qsizetype holesCount() const;
    bool polygonContains(const QGeoCoordinate &coordinate) const;
    const QList<QGeoCoordinate> holePath(qsizetype index) const;

    virtual void addHole(const QList<QGeoCoordinate> &holePath);
    virtual void removeHole(qsizetype index);
    virtual void updateClipperPath();

// data members
    bool m_clipperDirty = true;
    QList<QList<QGeoCoordinate>> m_holesList;
    QClipperUtils m_clipperWrapper;
};

class Q_POSITIONING_EXPORT QGeoPolygonPrivateEager : public QGeoPolygonPrivate
{
public:
    QGeoPolygonPrivateEager();
    QGeoPolygonPrivateEager(const QList<QGeoCoordinate> &path);
    ~QGeoPolygonPrivateEager();

// QGeoShape API
    virtual QGeoShapePrivate *clone() const override;
    virtual void translate(double degreesLatitude, double degreesLongitude) override;

// QGeoPath API
    virtual void markDirty() override;
    virtual void addCoordinate(const QGeoCoordinate &coordinate) override;
    virtual void computeBoundingBox() override;

// QGeoPolygonPrivate API

// *Eager API
    void updateBoundingBox();

// data members
    QList<double> m_deltaXs; // longitude deltas from m_path[0]
    double m_minX = 0;              // minimum value inside deltaXs
    double m_maxX = 0;              // maximum value inside deltaXs
    double m_minLati = 0;           // minimum latitude. paths do not wrap around through the poles
    double m_maxLati = 0;           // minimum latitude. paths do not wrap around through the poles
};

// This is a mean of creating a QGeoPolygonPrivateEager and injecting it into QGeoPolygons via operator=
class Q_POSITIONING_EXPORT QGeoPolygonEager : public QGeoPolygon
{
    Q_GADGET
public:

    QGeoPolygonEager();
    QGeoPolygonEager(const QList<QGeoCoordinate> &path);
    QGeoPolygonEager(const QGeoPolygon &other);
    QGeoPolygonEager(const QGeoShape &other);
    ~QGeoPolygonEager();
};

QT_END_NAMESPACE

#endif // QGEOPOLYGON_P_H
