// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QCLIPPERUTILS_P_H
#define QCLIPPERUTILS_P_H

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

/*
 * This file is intended to be include only in source files of
 * QtPositioning/QtLocation. It is in QtPositioning to enable manipulation
 * of geo polygons
 */

#include <QtPositioning/private/qpositioningglobal_p.h>
#include <QtCore/QtGlobal>
#include <QtCore/QList>
#include <QtPositioning/private/qdoublevector2d_p.h>

QT_BEGIN_NAMESPACE

/*
 * This class provides a wrapper around the clip2tri library, so that
 * we do not need to export any of the internal types. That is needed
 * because after QtLocation and QtPositioning are moved to different
 * repos, we need to use the features of the library in QtLocation without
 * explicitly linking to it.
*/

class QClipperUtilsPrivate;
class Q_POSITIONING_EXPORT QClipperUtils
{
public:
    QClipperUtils();
    QClipperUtils(const QClipperUtils &other);
    ~QClipperUtils();

    // Must be in sync with c2t::clip2tri::Operation
    enum Operation {
        Union,
        Intersection,
        Difference,
        Xor
    };

    // Must be in sync with QtClipperLib::PolyFillType
    enum PolyFillType {
        pftEvenOdd,
        pftNonZero,
        pftPositive,
        pftNegative
    };

    static double clipperScaleFactor();

    static int pointInPolygon(const QDoubleVector2D &point, const QList<QDoubleVector2D> &polygon);

    // wrap some useful non-static methods of c2t::clip2tri
    void clearClipper();
    void addSubjectPath(const QList<QDoubleVector2D> &path, bool closed);
    void addClipPolygon(const QList<QDoubleVector2D> &path);
    QList<QList<QDoubleVector2D>> execute(Operation op, PolyFillType subjFillType = pftNonZero,
                                          PolyFillType clipFillType = pftNonZero);

    // For optimization purposes. Set the polygon once and check for multiple
    // points. Without the need to convert between Qt and clip2tri types
    // every time
    void setPolygon(const QList<QDoubleVector2D> &polygon);
    int pointInPolygon(const QDoubleVector2D &point) const;

private:
    QClipperUtilsPrivate *d_ptr;
};

QT_END_NAMESPACE

#endif // QCLIPPERUTILS_P_H
