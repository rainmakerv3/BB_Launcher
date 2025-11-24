// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QDOUBLEVECTOR3D_P_H
#define QDOUBLEVECTOR3D_P_H

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
#include <QVector3D>
#endif

#include "qpositioningglobal_p.h"
#include "qdoublevector2d_p.h"
#include <QtCore/qmetatype.h>

QT_BEGIN_NAMESPACE

class Q_POSITIONING_EXPORT QDoubleVector3D
{
public:
    constexpr inline QDoubleVector3D();
    constexpr inline QDoubleVector3D(double xpos, double ypos, double zpos);
    constexpr inline QDoubleVector3D(const QDoubleVector2D &vector);
    constexpr inline QDoubleVector3D(const QDoubleVector2D &vector, double zpos);

    inline bool isNull() const;

    constexpr inline double x() const;
    constexpr inline double y() const;
    constexpr inline double z() const;

    inline void setX(double x);
    inline void setY(double y);
    inline void setZ(double z);

    inline double get(int i) const;
    inline void set(int i, double value);

    double length() const;
    constexpr inline double lengthSquared() const;

    QDoubleVector3D normalized() const;
    void normalize();

    inline QDoubleVector3D &operator+=(const QDoubleVector3D &vector);
    inline QDoubleVector3D &operator-=(const QDoubleVector3D &vector);
    inline QDoubleVector3D &operator*=(double factor);
    inline QDoubleVector3D &operator*=(const QDoubleVector3D &vector);
    inline QDoubleVector3D &operator/=(double divisor);

    constexpr static inline double dotProduct(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
    { return v1.xp * v2.xp + v1.yp * v2.yp + v1.zp * v2.zp; }

    constexpr static inline QDoubleVector3D crossProduct(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
    { return QDoubleVector3D(v1.yp * v2.zp - v1.zp * v2.yp,
                    v1.zp * v2.xp - v1.xp * v2.zp,
                    v1.xp * v2.yp - v1.yp * v2.xp); }

    static QDoubleVector3D normal(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    static QDoubleVector3D normal
        (const QDoubleVector3D &v1, const QDoubleVector3D &v2, const QDoubleVector3D &v3);

    double distanceToPlane(const QDoubleVector3D &plane, const QDoubleVector3D &normal) const;
    double distanceToPlane(const QDoubleVector3D &plane1, const QDoubleVector3D &plane2, const QDoubleVector3D &plane3) const;
    double distanceToLine(const QDoubleVector3D &point, const QDoubleVector3D &direction) const;

    friend constexpr inline bool operator==(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    friend constexpr inline bool operator!=(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    friend constexpr inline const QDoubleVector3D operator+(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    friend constexpr inline const QDoubleVector3D operator-(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    friend constexpr inline const QDoubleVector3D operator*(double factor, const QDoubleVector3D &vector);
    friend constexpr inline const QDoubleVector3D operator*(const QDoubleVector3D &vector, double factor);
    friend constexpr inline const QDoubleVector3D operator*(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    friend constexpr inline const QDoubleVector3D operator-(const QDoubleVector3D &vector);
    friend constexpr inline const QDoubleVector3D operator/(const QDoubleVector3D &vector, double divisor);

    friend constexpr inline bool qFuzzyCompare(const QDoubleVector3D &v1, const QDoubleVector3D &v2);

    constexpr inline QDoubleVector2D toVector2D() const;

private:
    double xp, yp, zp;

    friend class QDoubleVector2D;
};

Q_DECLARE_TYPEINFO(QDoubleVector3D, Q_RELOCATABLE_TYPE);

constexpr inline QDoubleVector3D::QDoubleVector3D() : xp(0.0), yp(0.0), zp(0.0) {}

constexpr inline QDoubleVector3D::QDoubleVector3D(double xpos, double ypos, double zpos) : xp(xpos), yp(ypos), zp(zpos) {}

constexpr inline QDoubleVector3D::QDoubleVector3D(const QDoubleVector2D &v)
    : xp(v.xp), yp(v.yp), zp(0.0) {}

constexpr inline QDoubleVector3D::QDoubleVector3D(const QDoubleVector2D &v, double zpos)
    : xp(v.xp), yp(v.yp), zp(zpos) {}

inline bool QDoubleVector3D::isNull() const
{
    return qIsNull(xp) && qIsNull(yp) && qIsNull(zp);
}

constexpr inline double QDoubleVector3D::x() const { return xp; }
constexpr inline double QDoubleVector3D::y() const { return yp; }
constexpr inline double QDoubleVector3D::z() const { return zp; }

constexpr inline double QDoubleVector3D::lengthSquared() const
{ return xp * xp + yp * yp + zp * zp; }


inline void QDoubleVector3D::setX(double aX) { xp = aX; }
inline void QDoubleVector3D::setY(double aY) { yp = aY; }
inline void QDoubleVector3D::setZ(double aZ) { zp = aZ; }

inline double QDoubleVector3D::get(int i) const
{
    switch (i) {
    case 0:
        return xp;
    case 1:
        return yp;
    case 2:
        return zp;
    default:
        return 0.0;
    }
}

inline void QDoubleVector3D::set(int i, double value)
{
    switch (i) {
    case 0:
        xp = value;
        break;
    case 1:
        yp = value;
        break;
    case 2:
        zp = value;
        break;
    default:
        break;
    }
}

inline QDoubleVector3D &QDoubleVector3D::operator+=(const QDoubleVector3D &vector)
{
    xp += vector.xp;
    yp += vector.yp;
    zp += vector.zp;
    return *this;
}

inline QDoubleVector3D &QDoubleVector3D::operator-=(const QDoubleVector3D &vector)
{
    xp -= vector.xp;
    yp -= vector.yp;
    zp -= vector.zp;
    return *this;
}

inline QDoubleVector3D &QDoubleVector3D::operator*=(double factor)
{
    xp *= factor;
    yp *= factor;
    zp *= factor;
    return *this;
}

inline QDoubleVector3D &QDoubleVector3D::operator*=(const QDoubleVector3D &vector)
{
    xp *= vector.xp;
    yp *= vector.yp;
    zp *= vector.zp;
    return *this;
}

inline QDoubleVector3D &QDoubleVector3D::operator/=(double divisor)
{
    xp /= divisor;
    yp /= divisor;
    zp /= divisor;
    return *this;
}

constexpr inline bool operator==(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
{
    return v1.xp == v2.xp && v1.yp == v2.yp && v1.zp == v2.zp;
}

constexpr inline bool operator!=(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
{
    return v1.xp != v2.xp || v1.yp != v2.yp || v1.zp != v2.zp;
}

constexpr inline const QDoubleVector3D operator+(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
{
    return QDoubleVector3D(v1.xp + v2.xp, v1.yp + v2.yp, v1.zp + v2.zp);
}

constexpr inline const QDoubleVector3D operator-(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
{
    return QDoubleVector3D(v1.xp - v2.xp, v1.yp - v2.yp, v1.zp - v2.zp);
}

constexpr inline const QDoubleVector3D operator*(double factor, const QDoubleVector3D &vector)
{
    return QDoubleVector3D(vector.xp * factor, vector.yp * factor, vector.zp * factor);
}

constexpr inline const QDoubleVector3D operator*(const QDoubleVector3D &vector, double factor)
{
    return QDoubleVector3D(vector.xp * factor, vector.yp * factor, vector.zp * factor);
}

constexpr inline const QDoubleVector3D operator*(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
{
    return QDoubleVector3D(v1.xp * v2.xp, v1.yp * v2.yp, v1.zp * v2.zp);
}

constexpr inline const QDoubleVector3D operator-(const QDoubleVector3D &vector)
{
    return QDoubleVector3D(-vector.xp, -vector.yp, -vector.zp);
}

constexpr inline const QDoubleVector3D operator/(const QDoubleVector3D &vector, double divisor)
{
    return QDoubleVector3D(vector.xp / divisor, vector.yp / divisor, vector.zp / divisor);
}

constexpr inline bool qFuzzyCompare(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
{
    return qFuzzyCompare(v1.xp, v2.xp) &&
           qFuzzyCompare(v1.yp, v2.yp) &&
           qFuzzyCompare(v1.zp, v2.zp);
}

constexpr inline QDoubleVector2D QDoubleVector3D::toVector2D() const
{
    return QDoubleVector2D(xp, yp);
}


#ifndef QT_NO_DEBUG_STREAM
Q_POSITIONING_EXPORT QDebug operator<<(QDebug dbg, const QDoubleVector3D &vector);
#endif

#ifndef QT_NO_DATASTREAM
Q_POSITIONING_EXPORT QDataStream &operator<<(QDataStream &, const QDoubleVector3D &);
Q_POSITIONING_EXPORT QDataStream &operator>>(QDataStream &, QDoubleVector3D &);
#endif

QT_END_NAMESPACE

#endif
