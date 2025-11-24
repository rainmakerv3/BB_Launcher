// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOSHAPE_P_H
#define QGEOSHAPE_P_H

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

#include <QtCore/QSharedData>

#include "qgeorectangle.h"
#include "private/qglobal_p.h"

QT_BEGIN_NAMESPACE

class QGeoShapePrivate : public QSharedData
{
public:
    explicit QGeoShapePrivate(QGeoShape::ShapeType type);
    virtual ~QGeoShapePrivate();

    virtual bool isValid() const = 0;
    virtual bool isEmpty() const = 0;
    virtual bool contains(const QGeoCoordinate &coordinate) const = 0;

    virtual QGeoCoordinate center() const = 0;

    virtual QGeoRectangle boundingGeoRectangle() const = 0;

    virtual QGeoShapePrivate *clone() const = 0;

    virtual bool operator==(const QGeoShapePrivate &other) const;

    virtual size_t hash(size_t seed) const = 0;

    QGeoShape::ShapeType type;
};

// don't use the copy constructor when detaching from a QSharedDataPointer, use virtual clone()
// call instead.
template <>
Q_INLINE_TEMPLATE QGeoShapePrivate *QSharedDataPointer<QGeoShapePrivate>::clone()
{
    return d->clone();
}

QT_END_NAMESPACE

#endif
