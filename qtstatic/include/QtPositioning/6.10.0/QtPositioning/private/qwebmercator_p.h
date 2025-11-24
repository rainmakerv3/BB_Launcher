// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QWEBMERCATOR_P_H
#define QWEBMERCATOR_P_H

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

#include <qglobal.h>
#include <QtCore/qvariant.h>
#include "qpositioningglobal_p.h"

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QDoubleVector2D;

class Q_POSITIONING_EXPORT QWebMercator
{
public:
    static QDoubleVector2D coordToMercator(const QGeoCoordinate &coord);
    static QGeoCoordinate mercatorToCoord(const QDoubleVector2D &mercator);
    static QGeoCoordinate coordinateInterpolation(const QGeoCoordinate &from, const QGeoCoordinate &to, qreal progress);

private:
    static double realmod(const double a, const double b);
};

QT_END_NAMESPACE

#endif // QWEBMERCATOR_P_H
