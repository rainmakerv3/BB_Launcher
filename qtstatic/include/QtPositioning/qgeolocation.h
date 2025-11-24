// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOLOCATION_H
#define QGEOLOCATION_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QMetaType>
#include <QtPositioning/qpositioningglobal.h>

QT_BEGIN_NAMESPACE

class QGeoAddress;
class QGeoCoordinate;
class QGeoShape;
class QGeoLocationPrivate;
QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoLocationPrivate, Q_POSITIONING_EXPORT)

class Q_POSITIONING_EXPORT QGeoLocation
{
public:
    QGeoLocation();
    QGeoLocation(const QGeoLocation &other);
    QGeoLocation(QGeoLocation &&other) noexcept = default;
    ~QGeoLocation();

    QGeoLocation &operator=(const QGeoLocation &other);
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QGeoLocation)

    void swap(QGeoLocation &other) noexcept { d.swap(other.d); }

    friend bool operator==(const QGeoLocation &lhs, const QGeoLocation &rhs)
    {
        return equals(lhs, rhs);
    }
    friend bool operator!=(const QGeoLocation &lhs, const QGeoLocation &rhs)
    {
        return !equals(lhs, rhs);
    }

    QGeoAddress address() const;
    void setAddress(const QGeoAddress &address);
    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &position);
    QGeoShape boundingShape() const;
    void setBoundingShape(const QGeoShape &shape);
    QVariantMap extendedAttributes() const;
    void setExtendedAttributes(const QVariantMap &data);

    bool isEmpty() const;

private:
    static bool equals(const QGeoLocation &lhs, const QGeoLocation &rhs);
    QSharedDataPointer<QGeoLocationPrivate> d;
};

Q_POSITIONING_EXPORT size_t qHash(const QGeoLocation &location, size_t seed = 0) noexcept;

Q_DECLARE_SHARED(QGeoLocation)

QT_END_NAMESPACE

QT_DECL_METATYPE_EXTERN(QGeoLocation, Q_POSITIONING_EXPORT)

#endif
