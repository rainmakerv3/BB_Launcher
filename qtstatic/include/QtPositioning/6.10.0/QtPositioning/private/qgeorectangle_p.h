// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEORECTANGLE_P_H
#define QGEORECTANGLE_P_H

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

#include "qgeoshape_p.h"
#include "qgeocoordinate.h"

QT_BEGIN_NAMESPACE

class QGeoRectanglePrivate : public QGeoShapePrivate
{
public:
    QGeoRectanglePrivate();
    QGeoRectanglePrivate(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight);
    QGeoRectanglePrivate(const QGeoRectanglePrivate &other);
    ~QGeoRectanglePrivate();

    bool isValid() const override;
    bool isEmpty() const override;
    bool contains(const QGeoCoordinate &coordinate) const override;

    QGeoCoordinate center() const override;

    QGeoRectangle boundingGeoRectangle() const override;

    void extendRectangle(const QGeoCoordinate &coordinate);

    QGeoShapePrivate *clone() const override;

    bool operator==(const QGeoShapePrivate &other) const override;

    size_t hash(size_t seed) const override;

    QGeoCoordinate topLeft;
    QGeoCoordinate bottomRight;
};

QT_END_NAMESPACE

#endif
