// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOADDRESS_H
#define QGEOADDRESS_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>
#include <QtPositioning/qpositioningglobal.h>

QT_BEGIN_NAMESPACE

class QString;
class QGeoAddressPrivate;
QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoAddressPrivate, Q_POSITIONING_EXPORT)

class Q_POSITIONING_EXPORT QGeoAddress
{
public:
    QGeoAddress();
    QGeoAddress(const QGeoAddress &other);
    QGeoAddress(QGeoAddress &&other) noexcept = default;
    ~QGeoAddress();

    QGeoAddress &operator=(const QGeoAddress &other);
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QGeoAddress)

    void swap(QGeoAddress &other) noexcept { d.swap(other.d); }

    friend bool operator==(const QGeoAddress &lhs, const QGeoAddress &rhs)
    {
        return equals(lhs, rhs);
    }
    friend bool operator!=(const QGeoAddress &lhs, const QGeoAddress &rhs)
    {
        return !equals(lhs, rhs);
    }

    QString text() const;
    void setText(const QString &text);

    QString country() const;
    void setCountry(const QString &country);

    QString countryCode() const;
    void setCountryCode(const QString &countryCode);

    QString state() const;
    void setState(const QString &state);

    QString county() const;
    void setCounty(const QString &county);

    QString city() const;
    void setCity(const QString &city);

    QString district() const;
    void setDistrict(const QString &district);

    QString postalCode() const;
    void setPostalCode(const QString &postalCode);

    QString street() const;
    void setStreet(const QString &street);

    QString streetNumber() const;
    void setStreetNumber(const QString &streetNumber);

    bool isEmpty() const;
    void clear();

    bool isTextGenerated() const;

private:
    static bool equals(const QGeoAddress &lhs, const QGeoAddress &rhs);
    QSharedDataPointer<QGeoAddressPrivate> d;
};

Q_POSITIONING_EXPORT size_t qHash(const QGeoAddress &address, size_t seed = 0) noexcept;

Q_DECLARE_SHARED(QGeoAddress)

QT_END_NAMESPACE

QT_DECL_METATYPE_EXTERN(QGeoAddress, Q_POSITIONING_EXPORT)

#endif
