// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKGEOCOORDINATEANIMATION_P_P_H
#define QQUICKGEOCOORDINATEANIMATION_P_P_H

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

#include "qquickgeocoordinateanimation_p.h"
#include <QtQuick/private/qquickanimation_p_p.h>
#include <QtCore/private/qproperty_p.h>

QT_BEGIN_NAMESPACE

class QQuickGeoCoordinateAnimationPrivate : public QQuickPropertyAnimationPrivate
{
    Q_DECLARE_PUBLIC(QQuickGeoCoordinateAnimation)
public:
    void setDirection(QQuickGeoCoordinateAnimation::Direction direction)
    {
        q_func()->setDirection(direction);
    }
    void directionChanged()
    {
        Q_EMIT q_func()->directionChanged();
    }

    Q_OBJECT_COMPAT_PROPERTY_WITH_ARGS(QQuickGeoCoordinateAnimationPrivate,
                                       QQuickGeoCoordinateAnimation::Direction, m_direction,
                                       &QQuickGeoCoordinateAnimationPrivate::setDirection,
                                       &QQuickGeoCoordinateAnimationPrivate::directionChanged,
                                       QQuickGeoCoordinateAnimation::Shortest)
};

QT_END_NAMESPACE

#endif // QQUICKGEOCOORDINATEANIMATION_P_P_H
