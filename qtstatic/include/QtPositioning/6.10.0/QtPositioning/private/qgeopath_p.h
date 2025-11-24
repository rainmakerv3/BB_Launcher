// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOPATH_P_H
#define QGEOPATH_P_H

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

#include <QtPositioning/private/qpositioningglobal_p.h>
#include "qgeoshape_p.h"
#include "qgeocoordinate.h"
#include "qlocationutils_p.h"
#include <QtPositioning/qgeopath.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

inline static void computeBBox(const QList<QGeoCoordinate> &m_path, QList<double> &m_deltaXs,
                               double &m_minX, double &m_maxX, double &m_minLati, double &m_maxLati,
                               QGeoRectangle &m_bbox)
{
    if (m_path.isEmpty()) {
        m_deltaXs.clear();
        m_minX = qInf();
        m_maxX = -qInf();
        m_minLati = qInf();
        m_maxLati = -qInf();
        m_bbox = QGeoRectangle();
        return;
    }

    m_minLati = m_maxLati = m_path.at(0).latitude();
    qsizetype minId = 0;
    qsizetype maxId = 0;
    m_deltaXs.resize(m_path.size());
    m_deltaXs[0] = m_minX = m_maxX = 0.0;

    for (qsizetype i = 1; i < m_path.size(); i++) {
        const QGeoCoordinate &geoFrom = m_path.at(i-1);
        const QGeoCoordinate &geoTo   = m_path.at(i);
        double longiFrom    = geoFrom.longitude();
        double longiTo      = geoTo.longitude();
        double deltaLongi = longiTo - longiFrom;
        if (qAbs(deltaLongi) > 180.0) {
            if (longiTo > 0.0)
                longiTo -= 360.0;
            else
                longiTo += 360.0;
            deltaLongi =  longiTo - longiFrom;
        }
        m_deltaXs[i] = m_deltaXs[i-1] + deltaLongi;
        if (m_deltaXs[i] < m_minX) {
            m_minX = m_deltaXs[i];
            minId = i;
        }
        if (m_deltaXs[i] > m_maxX) {
            m_maxX = m_deltaXs[i];
            maxId = i;
        }
        if (geoTo.latitude() > m_maxLati)
            m_maxLati = geoTo.latitude();
        if (geoTo.latitude() < m_minLati)
            m_minLati = geoTo.latitude();
    }

    m_bbox = QGeoRectangle(QGeoCoordinate(m_maxLati, m_path.at(minId).longitude()),
                           QGeoCoordinate(m_minLati, m_path.at(maxId).longitude()));
}

inline static void updateBBox(const QList<QGeoCoordinate> &m_path, QList<double> &m_deltaXs,
                              double &m_minX, double &m_maxX, double &m_minLati, double &m_maxLati,
                              QGeoRectangle &m_bbox)
{
    if (m_path.isEmpty()) {
        m_deltaXs.clear();
        m_minX = qInf();
        m_maxX = -qInf();
        m_minLati = qInf();
        m_maxLati = -qInf();
        m_bbox = QGeoRectangle();
        return;
    } else if (m_path.size() == 1) { // was 0  now is 1
        m_deltaXs.resize(1);
        m_deltaXs[0] = m_minX = m_maxX = 0.0;
        m_minLati = m_maxLati = m_path.at(0).latitude();
        m_bbox = QGeoRectangle(QGeoCoordinate(m_maxLati, m_path.at(0).longitude()),
                               QGeoCoordinate(m_minLati, m_path.at(0).longitude()));
        return;
    } else if ( m_path.size() != m_deltaXs.size() + 1 ) {  // this case should not happen
        computeBBox(m_path, m_deltaXs, m_minX, m_maxX, m_minLati, m_maxLati, m_bbox); // something went wrong
        return;
    }

    const QGeoCoordinate &geoFrom = m_path.at(m_path.size()-2);
    const QGeoCoordinate &geoTo   = m_path.last();
    double longiFrom    = geoFrom.longitude();
    double longiTo      = geoTo.longitude();
    double deltaLongi = longiTo - longiFrom;
    if (qAbs(deltaLongi) > 180.0) {
        if (longiTo > 0.0)
            longiTo -= 360.0;
        else
            longiTo += 360.0;
        deltaLongi =  longiTo - longiFrom;
    }

    m_deltaXs.push_back(m_deltaXs.last() + deltaLongi);
    double currentMinLongi = m_bbox.topLeft().longitude();
    double currentMaxLongi = m_bbox.bottomRight().longitude();
    if (m_deltaXs.last() < m_minX) {
        m_minX = m_deltaXs.last();
        currentMinLongi = geoTo.longitude();
    }
    if (m_deltaXs.last() > m_maxX) {
        m_maxX = m_deltaXs.last();
        currentMaxLongi = geoTo.longitude();
    }
    if (geoTo.latitude() > m_maxLati)
        m_maxLati = geoTo.latitude();
    if (geoTo.latitude() < m_minLati)
        m_minLati = geoTo.latitude();
    m_bbox = QGeoRectangle(QGeoCoordinate(m_maxLati, currentMinLongi),
                           QGeoCoordinate(m_minLati, currentMaxLongi));
}

// Lazy by default. Eager, within the module, used only in MapItems/MapObjectsQSG
class Q_POSITIONING_EXPORT QGeoPathPrivate : public QGeoShapePrivate
{
public:
    QGeoPathPrivate();
    QGeoPathPrivate(const QList<QGeoCoordinate> &path, const qreal width = 0.0);
    ~QGeoPathPrivate();

// QGeoShape API
    virtual QGeoShapePrivate *clone() const override;
    virtual bool isValid() const override;
    virtual bool isEmpty() const override;
    virtual QGeoCoordinate center() const override;
    virtual bool operator==(const QGeoShapePrivate &other) const override;
    virtual bool contains(const QGeoCoordinate &coordinate) const override;
    virtual QGeoRectangle boundingGeoRectangle() const override;
    size_t hash(size_t seed) const override;

// QGeoPathPrivate API
    virtual const QList<QGeoCoordinate> &path() const;
    virtual bool lineContains(const QGeoCoordinate &coordinate) const;
    virtual qreal width() const;
    virtual double length(qsizetype indexFrom, qsizetype indexTo) const;
    virtual qsizetype size() const;
    virtual QGeoCoordinate coordinateAt(qsizetype index) const;
    virtual bool containsCoordinate(const QGeoCoordinate &coordinate) const;

    virtual void setWidth(const qreal &width);
    virtual void translate(double degreesLatitude, double degreesLongitude);
    virtual void setPath(const QList<QGeoCoordinate> &path);
    virtual void clearPath();
    virtual void addCoordinate(const QGeoCoordinate &coordinate);
    virtual void insertCoordinate(qsizetype index, const QGeoCoordinate &coordinate);
    virtual void replaceCoordinate(qsizetype index, const QGeoCoordinate &coordinate);
    virtual void removeCoordinate(const QGeoCoordinate &coordinate);
    virtual void removeCoordinate(qsizetype index);
    virtual void computeBoundingBox();
    virtual void markDirty();

// data members
    QList<QGeoCoordinate> m_path;
    qreal m_width = 0;
    QGeoRectangle m_bbox; // cached
    double m_leftBoundWrapped; // cached
    bool m_bboxDirty = false;
};

class Q_POSITIONING_EXPORT QGeoPathPrivateEager : public QGeoPathPrivate
{
public:
    QGeoPathPrivateEager();
    QGeoPathPrivateEager(const QList<QGeoCoordinate> &path, const qreal width = 0.0);
    ~QGeoPathPrivateEager();

// QGeoShapePrivate API
    virtual QGeoShapePrivate *clone() const override;
    virtual void translate(double degreesLatitude, double degreesLongitude) override;

// QGeoShapePrivate API
    virtual void markDirty() override;
    virtual void addCoordinate(const QGeoCoordinate &coordinate) override;
    virtual void computeBoundingBox() override;

// *Eager API
    void updateBoundingBox();

// data members
    QList<double> m_deltaXs; // longitude deltas from m_path[0]
    double m_minX = 0;              // minimum value inside deltaXs
    double m_maxX = 0;              // maximum value inside deltaXs
    double m_minLati = 0;           // minimum latitude. paths do not wrap around through the poles
    double m_maxLati = 0;           // minimum latitude. paths do not wrap around through the poles
};

// This is a mean of creating a QGeoPathPrivateEager and injecting it into QGeoPaths via operator=
class Q_POSITIONING_EXPORT QGeoPathEager : public QGeoPath
{
    Q_GADGET
public:

    QGeoPathEager();
    QGeoPathEager(const QList<QGeoCoordinate> &path, const qreal &width = 0.0);
    QGeoPathEager(const QGeoPath &other);
    QGeoPathEager(const QGeoShape &other);
    ~QGeoPathEager();
};

QT_END_NAMESPACE

#endif // QGEOPATH_P_H
