// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKGEOCOORDINATEANIMATION_P_H
#define QQUICKGEOCOORDINATEANIMATION_P_H

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

#include <QtPositioningQuick/private/qpositioningquickglobal_p.h>
#include <QtQuick/private/qquickanimation_p.h>
#include <QtPositioning/qgeocoordinate.h>

QT_BEGIN_NAMESPACE

class QQuickGeoCoordinateAnimationPrivate;

class Q_POSITIONINGQUICK_EXPORT QQuickGeoCoordinateAnimation : public QQuickPropertyAnimation
{
    Q_OBJECT
    QML_NAMED_ELEMENT(CoordinateAnimation)
    QML_ADDED_IN_VERSION(5, 3)
    Q_DECLARE_PRIVATE(QQuickGeoCoordinateAnimation)
    Q_PROPERTY(QGeoCoordinate from READ from WRITE setFrom)
    Q_PROPERTY(QGeoCoordinate to READ to WRITE setTo)
    Q_PROPERTY(Direction direction READ direction WRITE setDirection NOTIFY directionChanged
                       BINDABLE bindableDirection)

public:
    enum Direction {
        Shortest,
        West,
        East
    };
    Q_ENUM(Direction)

    QQuickGeoCoordinateAnimation(QObject *parent=0);
    ~QQuickGeoCoordinateAnimation();

    QGeoCoordinate from() const;
    void setFrom(const QGeoCoordinate &);

    QGeoCoordinate to() const;
    void setTo(const QGeoCoordinate &);

    Direction direction() const;
    void setDirection(Direction direction);
    QBindable<Direction> bindableDirection();

Q_SIGNALS:
    void directionChanged();
};

QVariant Q_POSITIONINGQUICK_EXPORT q_coordinateInterpolator(const QGeoCoordinate &from, const QGeoCoordinate &to, qreal progress);

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickGeoCoordinateAnimation)

#endif // QQUICKCOORDINATEANIMATION_P_H
