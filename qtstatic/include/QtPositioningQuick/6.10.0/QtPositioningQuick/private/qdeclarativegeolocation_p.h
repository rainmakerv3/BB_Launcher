// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QDECLARATIVEGEOLOCATION_P_H
#define QDECLARATIVEGEOLOCATION_P_H

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
#include <QtCore/QVariantMap>
#include <QtCore/private/qproperty_p.h>
#include <QtPositioning/QGeoLocation>
#include <QtPositioning/QGeoShape>
#include <QtPositioningQuick/private/qdeclarativegeoaddress_p.h>
#include <QtPositioningQuick/private/qpositioningquickglobal_p.h>

QT_BEGIN_NAMESPACE

class Q_POSITIONINGQUICK_EXPORT QDeclarativeGeoLocation : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Location)
    QML_ADDED_IN_VERSION(5, 0)

    Q_PROPERTY(QGeoLocation location READ location WRITE setLocation)
    Q_PROPERTY(QDeclarativeGeoAddress *address READ address WRITE setAddress BINDABLE
                       bindableAddress)
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate WRITE setCoordinate BINDABLE
                       bindableCoordinate)
    Q_PROPERTY(QGeoShape boundingShape READ boundingShape WRITE setBoundingShape BINDABLE
                       bindableBoundingShape REVISION(6, 2))
    Q_PROPERTY(QVariantMap extendedAttributes READ extendedAttributes WRITE setExtendedAttributes
                       BINDABLE bindableExtendedAttributes REVISION(5, 13))

public:
    explicit QDeclarativeGeoLocation(QObject *parent = 0);
    explicit QDeclarativeGeoLocation(const QGeoLocation &src, QObject *parent = 0);
    ~QDeclarativeGeoLocation();

    QGeoLocation location() const;
    void setLocation(const QGeoLocation &src);

    QDeclarativeGeoAddress *address() const;
    void setAddress(QDeclarativeGeoAddress *address);
    QBindable<QDeclarativeGeoAddress *> bindableAddress();

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate coordinate);
    QBindable<QGeoCoordinate> bindableCoordinate();

    QGeoShape boundingShape() const;
    void setBoundingShape(const QGeoShape &boundingShape);
    QBindable<QGeoShape> bindableBoundingShape();

    QVariantMap extendedAttributes() const;
    void setExtendedAttributes(const QVariantMap &attributes);
    QBindable<QVariantMap> bindableExtendedAttributes();

private:
    void setLocationInternal(const QGeoLocation &src);

    Q_OBJECT_COMPAT_PROPERTY_WITH_ARGS(QDeclarativeGeoLocation, QDeclarativeGeoAddress *, m_address,
                                       &QDeclarativeGeoLocation::setAddress, nullptr)
    Q_OBJECT_BINDABLE_PROPERTY(QDeclarativeGeoLocation, QGeoShape, m_boundingShape)
    Q_OBJECT_BINDABLE_PROPERTY(QDeclarativeGeoLocation, QGeoCoordinate, m_coordinate)
    Q_OBJECT_BINDABLE_PROPERTY(QDeclarativeGeoLocation, QVariantMap, m_extendedAttributes)
};

QT_END_NAMESPACE

#endif // QDECLARATIVELOCATION_P_H
