// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOCOORDINATEOBJECT_P_H
#define QGEOCOORDINATEOBJECT_P_H

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
#include <QObject>
#include <QProperty>
#include <QGeoCoordinate>
#include <QVariantMap>

QT_BEGIN_NAMESPACE

class Q_POSITIONING_EXPORT QGeoCoordinateObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate WRITE setCoordinate NOTIFY
                       coordinateChanged BINDABLE bindableCoordinate)

public:
    QGeoCoordinateObject(QObject *parent = 0);
    QGeoCoordinateObject(const QGeoCoordinate &c, QObject *parent = 0);
    virtual ~QGeoCoordinateObject();

    bool operator==(const QGeoCoordinate &other) const;
    bool operator==(const QGeoCoordinateObject &other) const;
    inline bool operator!=(const QGeoCoordinate &other) const {
        return !operator==(other);
    }
    inline bool operator!=(const QGeoCoordinateObject &other) const {
        return !operator==(other);
    }

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &c);
    QBindable<QGeoCoordinate> bindableCoordinate();

Q_SIGNALS:
    void coordinateChanged();

protected:
    Q_OBJECT_BINDABLE_PROPERTY(QGeoCoordinateObject, QGeoCoordinate, m_coordinate,
                               &QGeoCoordinateObject::coordinateChanged)
};

QT_END_NAMESPACE

QT_DECL_METATYPE_EXTERN_TAGGED(QGeoCoordinateObject*, QGeoCoordinateObject_ptr,
                               Q_POSITIONING_EXPORT)

#endif // QGEOCOORDINATEOBJECT_P_H
