// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QDOUBLEVECTOR2D_P_H
#define QDOUBLEVECTOR2D_P_H

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

#ifdef QT_BUILD_LOCATION_LIB
#include <QVector2D>
#endif

#include "qpositioningglobal_p.h"
#include <QtCore/qmetatype.h>
#include <QPointF>

QT_BEGIN_NAMESPACE

class QDoubleVector3D;

class Q_POSITIONING_EXPORT QDoubleVector2D
{
public:
    constexpr inline QDoubleVector2D();
    constexpr inline QDoubleVector2D(double xpos, double ypos);
    constexpr explicit inline QDoubleVector2D(const QPointF &p);
    explicit QDoubleVector2D(const QDoubleVector3D &vector);

    constexpr inline double manhattanLength() const;
    inline bool isNull() const;
    inline bool isFinite() const;

    constexpr inline double x() const;
    constexpr inline double y() const;

    inline void setX(double x);
    inline void setY(double y);

    double length() const;
    constexpr inline double lengthSquared() const;

    QDoubleVector2D normalized() const;
    void normalize();

    inline QDoubleVector2D &operator+=(const QDoubleVector2D &vector);
    inline QDoubleVector2D &operator-=(const QDoubleVector2D &vector);
    inline QDoubleVector2D &operator*=(double factor);
    inline QDoubleVector2D &operator*=(const QDoubleVector2D &vector);
    inline QDoubleVector2D &operator/=(double divisor);
    inline QDoubleVector2D &operator/=(const QDoubleVector2D &vector);

    constexpr static inline double dotProduct(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
    { return v1.xp * v2.xp + v1.yp * v2.yp; }


    friend constexpr inline bool operator==(const QDoubleVector2D &v1, const QDoubleVector2D &v2);
    friend constexpr inline bool operator!=(const QDoubleVector2D &v1, const QDoubleVector2D &v2);
    friend constexpr inline const QDoubleVector2D operator+(const QDoubleVector2D &v1, const QDoubleVector2D &v2);
    friend constexpr inline const QDoubleVector2D operator-(const QDoubleVector2D &v1, const QDoubleVector2D &v2);
    friend constexpr inline const QDoubleVector2D operator*(double factor, const QDoubleVector2D &vector);
    friend constexpr inline const QDoubleVector2D operator*(const QDoubleVector2D &vector, double factor);
    friend constexpr inline const QDoubleVector2D operator*(const QDoubleVector2D &v1, const QDoubleVector2D &v2);
    friend constexpr inline const QDoubleVector2D operator-(const QDoubleVector2D &vector);
    friend constexpr inline const QDoubleVector2D operator/(const QDoubleVector2D &vector, double divisor);

    friend constexpr inline bool qFuzzyCompare(const QDoubleVector2D &v1, const QDoubleVector2D &v2);

    QDoubleVector3D toVector3D() const;
    constexpr inline QPointF toPointF() const;

private:
    double xp, yp;

    friend class QDoubleVector3D;
};

Q_DECLARE_TYPEINFO(QDoubleVector2D, Q_RELOCATABLE_TYPE);

constexpr inline QDoubleVector2D::QDoubleVector2D() : xp(0.0), yp(0.0) {}

constexpr inline QDoubleVector2D::QDoubleVector2D(double xpos, double ypos) : xp(xpos), yp(ypos) {}

constexpr inline QDoubleVector2D::QDoubleVector2D(const QPointF &p) : xp(p.x()), yp(p.y()) { }

constexpr inline double QDoubleVector2D::manhattanLength() const
{
    return qAbs(x())+qAbs(y());
}

inline bool QDoubleVector2D::isNull() const
{
    return qIsNull(xp) && qIsNull(yp);
}

inline bool QDoubleVector2D::isFinite() const
{
    return qIsFinite(xp) && qIsFinite(yp);
}

constexpr inline double QDoubleVector2D::x() const { return xp; }
constexpr inline double QDoubleVector2D::y() const { return yp; }

inline void QDoubleVector2D::setX(double aX) { xp = aX; }
inline void QDoubleVector2D::setY(double aY) { yp = aY; }

constexpr inline double QDoubleVector2D::lengthSquared() const
{ return xp * xp + yp * yp; }

inline QDoubleVector2D &QDoubleVector2D::operator+=(const QDoubleVector2D &vector)
{
    xp += vector.xp;
    yp += vector.yp;
    return *this;
}

inline QDoubleVector2D &QDoubleVector2D::operator-=(const QDoubleVector2D &vector)
{
    xp -= vector.xp;
    yp -= vector.yp;
    return *this;
}

inline QDoubleVector2D &QDoubleVector2D::operator*=(double factor)
{
    xp *= factor;
    yp *= factor;
    return *this;
}

inline QDoubleVector2D &QDoubleVector2D::operator*=(const QDoubleVector2D &vector)
{
    xp *= vector.xp;
    yp *= vector.yp;
    return *this;
}

inline QDoubleVector2D &QDoubleVector2D::operator/=(double divisor)
{
    xp /= divisor;
    yp /= divisor;
    return *this;
}

inline QDoubleVector2D &QDoubleVector2D::operator/=(const QDoubleVector2D &vector)
{
    xp /= vector.xp;
    yp /= vector.yp;
    return *this;
}

constexpr inline bool operator==(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
{
    return v1.xp == v2.xp && v1.yp == v2.yp;
}

constexpr inline bool operator!=(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
{
    return v1.xp != v2.xp || v1.yp != v2.yp;
}

constexpr inline const QDoubleVector2D operator+(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
{
    return QDoubleVector2D(v1.xp + v2.xp, v1.yp + v2.yp);
}

constexpr inline const QDoubleVector2D operator-(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
{
    return QDoubleVector2D(v1.xp - v2.xp, v1.yp - v2.yp);
}

constexpr inline const QDoubleVector2D operator*(double factor, const QDoubleVector2D &vector)
{
    return QDoubleVector2D(vector.xp * factor, vector.yp * factor);
}

constexpr inline const QDoubleVector2D operator*(const QDoubleVector2D &vector, double factor)
{
    return QDoubleVector2D(vector.xp * factor, vector.yp * factor);
}

constexpr inline const QDoubleVector2D operator*(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
{
    return QDoubleVector2D(v1.xp * v2.xp, v1.yp * v2.yp);
}

constexpr inline const QDoubleVector2D operator-(const QDoubleVector2D &vector)
{
    return QDoubleVector2D(-vector.xp, -vector.yp);
}

constexpr inline const QDoubleVector2D operator/(const QDoubleVector2D &vector, double divisor)
{
    return QDoubleVector2D(vector.xp / divisor, vector.yp / divisor);
}

constexpr inline bool qFuzzyCompare(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
{
    return qFuzzyCompare(v1.xp, v2.xp) && qFuzzyCompare(v1.yp, v2.yp);
}

constexpr inline QPointF QDoubleVector2D::toPointF() const
{
    return QPointF(qreal(xp), qreal(yp));
}

#ifndef QT_NO_DEBUG_STREAM
Q_POSITIONING_EXPORT QDebug operator<<(QDebug dbg, const QDoubleVector2D &vector);
#endif

#ifndef QT_NO_DATASTREAM
Q_POSITIONING_EXPORT QDataStream &operator<<(QDataStream &, const QDoubleVector2D &);
Q_POSITIONING_EXPORT QDataStream &operator>>(QDataStream &, QDoubleVector2D &);
#endif

QT_END_NAMESPACE

#endif
