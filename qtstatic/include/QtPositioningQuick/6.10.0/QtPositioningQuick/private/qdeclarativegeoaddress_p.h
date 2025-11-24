// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QDECLARATIVEGEOADDRESS_P_H
#define QDECLARATIVEGEOADDRESS_P_H

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

#include <QtCore/QObject>
#include <QtPositioning/QGeoAddress>
#include <QtPositioningQuick/private/qpositioningquickglobal_p.h>
#include <QQmlEngine>

QT_BEGIN_NAMESPACE

class Q_POSITIONINGQUICK_EXPORT QDeclarativeGeoAddress : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Address)
    QML_ADDED_IN_VERSION(5, 0)

    Q_PROPERTY(QGeoAddress address READ address WRITE setAddress)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString country READ country WRITE setCountry NOTIFY countryChanged)
    Q_PROPERTY(QString countryCode READ countryCode WRITE setCountryCode NOTIFY countryCodeChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString county READ county WRITE setCounty NOTIFY countyChanged)
    Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString district READ district WRITE setDistrict NOTIFY districtChanged)
    Q_PROPERTY(QString street READ street WRITE setStreet NOTIFY streetChanged)
    Q_PROPERTY(QString streetNumber READ streetNumber WRITE setStreetNumber
               NOTIFY streetNumberChanged REVISION(6, 2))
    Q_PROPERTY(QString postalCode READ postalCode WRITE setPostalCode NOTIFY postalCodeChanged)
    Q_PROPERTY(bool isTextGenerated READ isTextGenerated NOTIFY isTextGeneratedChanged)

public:
    explicit QDeclarativeGeoAddress(QObject *parent = 0);
    QDeclarativeGeoAddress(const QGeoAddress &address, QObject *parent = 0);
    QGeoAddress address() const;
    void setAddress(const QGeoAddress &address);

    QString text() const;
    void setText(const QString &address);

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
    QString street() const;
    void setStreet(const QString &street);
    QString streetNumber() const;
    void setStreetNumber(const QString &streetNumber);
    QString postalCode() const;
    void setPostalCode(const QString &postalCode);
    bool isTextGenerated() const;

Q_SIGNALS:
    void textChanged();
    void countryChanged();
    void countryCodeChanged();
    void stateChanged();
    void countyChanged();
    void cityChanged();
    void districtChanged();
    void streetChanged();
    void streetNumberChanged();
    void postalCodeChanged();
    void isTextGeneratedChanged();

private:
    QGeoAddress m_address;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEGEOADDRESS_P_H
