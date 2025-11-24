// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QGEOSATELLITEINFO_H
#define QGEOSATELLITEINFO_H

#include <QtPositioning/qpositioningglobal.h>
#include <QtCore/QSharedData>
#include <QtCore/QMetaType>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

class QDebug;
class QDataStream;

class QGeoSatelliteInfo;
Q_POSITIONING_EXPORT size_t qHash(const QGeoSatelliteInfo &key, size_t seed = 0) noexcept;
namespace QTest
{

Q_POSITIONING_EXPORT char *toString(const QGeoSatelliteInfo &info);

} // namespace QTest

class QGeoSatelliteInfoPrivate;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoSatelliteInfoPrivate, Q_POSITIONING_EXPORT)

class Q_POSITIONING_EXPORT QGeoSatelliteInfo
{
    Q_GADGET
    Q_PROPERTY(SatelliteSystem satelliteSystem READ satelliteSystem FINAL)
    Q_PROPERTY(int satelliteIdentifier READ satelliteIdentifier FINAL)
    Q_PROPERTY(qreal signalStrength READ signalStrength FINAL)

public:
    enum Attribute {
        Elevation,
        Azimuth
    };
    Q_ENUM(Attribute)

    enum SatelliteSystem {
        Undefined = 0x00,
        GPS = 0x01,
        GLONASS = 0x02,
        GALILEO = 0x03,
        BEIDOU = 0x04,
        QZSS = 0x05,
        Multiple = 0xFF,
        CustomType = 0x100
    };
    Q_ENUM(SatelliteSystem)

    QGeoSatelliteInfo();
    QGeoSatelliteInfo(const QGeoSatelliteInfo &other);
    QGeoSatelliteInfo(QGeoSatelliteInfoPrivate &dd);
    QGeoSatelliteInfo(QGeoSatelliteInfo &&other) noexcept = default;
    ~QGeoSatelliteInfo();

    QGeoSatelliteInfo &operator=(const QGeoSatelliteInfo &other);
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QGeoSatelliteInfo)

    void swap(QGeoSatelliteInfo &other) noexcept { d.swap(other.d); }

    friend bool operator==(const QGeoSatelliteInfo &lhs, const QGeoSatelliteInfo &rhs)
    {
        return equals(lhs, rhs);
    }
    friend bool operator!=(const QGeoSatelliteInfo &lhs, const QGeoSatelliteInfo &rhs)
    {
        return !equals(lhs, rhs);
    }

    void setSatelliteSystem(SatelliteSystem system);
    SatelliteSystem satelliteSystem() const;

    void setSatelliteIdentifier(int satId);
    int satelliteIdentifier() const;

    void setSignalStrength(int signalStrength);
    int signalStrength() const;

    void setAttribute(Attribute attribute, qreal value);
    Q_INVOKABLE qreal attribute(Attribute attribute) const;
    void removeAttribute(Attribute attribute);

    Q_INVOKABLE bool hasAttribute(Attribute attribute) const;

    void detach();

private:
    static bool equals(const QGeoSatelliteInfo &lhs, const QGeoSatelliteInfo &rhs);
#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug dbg, const QGeoSatelliteInfo &info)
    {
        return debugStreaming(dbg, info);
    }
    static QDebug debugStreaming(QDebug dbg, const QGeoSatelliteInfo &info);
#endif
#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &stream, const QGeoSatelliteInfo &info)
    {
        return dataStreamOut(stream, info);
    }
    friend QDataStream &operator>>(QDataStream &stream, QGeoSatelliteInfo &info)
    {
        return dataStreamIn(stream, info);
    }
    static QDataStream &dataStreamOut(QDataStream &stream, const QGeoSatelliteInfo &info);
    static QDataStream &dataStreamIn(QDataStream &stream, QGeoSatelliteInfo &info);
#endif
    QExplicitlySharedDataPointer<QGeoSatelliteInfoPrivate> d;
    friend class QGeoSatelliteInfoPrivate;

    friend Q_POSITIONING_EXPORT size_t qHash(const QGeoSatelliteInfo &key, size_t seed) noexcept;
    friend Q_POSITIONING_EXPORT char *QTest::toString(const QGeoSatelliteInfo &info);
};

Q_DECLARE_SHARED(QGeoSatelliteInfo)

QT_END_NAMESPACE

QT_DECL_METATYPE_EXTERN(QGeoSatelliteInfo, Q_POSITIONING_EXPORT)

#endif
