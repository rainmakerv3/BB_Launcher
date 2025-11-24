// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QGEOPOSITIONINFO_H
#define QGEOPOSITIONINFO_H

#include <QtPositioning/QGeoCoordinate>
#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QMetaType>
#include <QtCore/QDateTime>

QT_BEGIN_NAMESPACE

class QDebug;
class QDataStream;

class QGeoPositionInfo;
Q_POSITIONING_EXPORT size_t qHash(const QGeoPositionInfo &key, size_t seed = 0) noexcept;
namespace QTest
{

Q_POSITIONING_EXPORT char *toString(const QGeoPositionInfo &info);

} // namespace QTest

class QGeoPositionInfoPrivate;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoPositionInfoPrivate, Q_POSITIONING_EXPORT)

class Q_POSITIONING_EXPORT QGeoPositionInfo
{
public:
    enum Attribute {
        Direction,
        GroundSpeed,
        VerticalSpeed,
        MagneticVariation,
        HorizontalAccuracy,
        VerticalAccuracy,
        DirectionAccuracy
    };

    QGeoPositionInfo();
    QGeoPositionInfo(const QGeoCoordinate &coordinate, const QDateTime &updateTime);
    QGeoPositionInfo(const QGeoPositionInfo &other);
    QGeoPositionInfo(QGeoPositionInfo &&other) noexcept = default;
    QGeoPositionInfo(QGeoPositionInfoPrivate &dd);
    ~QGeoPositionInfo();

    QGeoPositionInfo &operator=(const QGeoPositionInfo &other);
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QGeoPositionInfo)

    void swap(QGeoPositionInfo &other) noexcept { d.swap(other.d); }

    friend bool operator==(const QGeoPositionInfo &lhs, const QGeoPositionInfo &rhs)
    {
        return equals(lhs, rhs);
    }
    friend bool operator!=(const QGeoPositionInfo &lhs, const QGeoPositionInfo &rhs)
    {
        return !equals(lhs, rhs);
    }

    bool isValid() const;

    void setTimestamp(const QDateTime &timestamp);
    QDateTime timestamp() const;

    void setCoordinate(const QGeoCoordinate &coordinate);
    QGeoCoordinate coordinate() const;

    void setAttribute(Attribute attribute, qreal value);
    qreal attribute(Attribute attribute) const;
    void removeAttribute(Attribute attribute);
    bool hasAttribute(Attribute attribute) const;

    void detach();

private:
    static bool equals(const QGeoPositionInfo &lhs, const QGeoPositionInfo &rhs);
#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug dbg, const QGeoPositionInfo &info)
    {
        return debugStreaming(dbg, info);
    }
    static QDebug debugStreaming(QDebug dbg, const QGeoPositionInfo &info);
#endif
#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &stream, const QGeoPositionInfo &info)
    {
        return dataStreamOut(stream, info);
    }
    friend QDataStream &operator>>(QDataStream &stream, QGeoPositionInfo &info)
    {
        return dataStreamIn(stream, info);
    }
    static QDataStream &dataStreamOut(QDataStream &stream, const QGeoPositionInfo &info);
    static QDataStream &dataStreamIn(QDataStream &stream, QGeoPositionInfo &info);

    friend QDataStream &operator<<(QDataStream &stream, QGeoPositionInfo::Attribute attr)
    {
        return dataStreamOut(stream, attr);
    }
    friend QDataStream &operator>>(QDataStream &stream, QGeoPositionInfo::Attribute &attr)
    {
        return dataStreamIn(stream, attr);
    }
    static QDataStream &dataStreamOut(QDataStream &stream, QGeoPositionInfo::Attribute attr);
    static QDataStream &dataStreamIn(QDataStream &stream, QGeoPositionInfo::Attribute &attr);
#endif
    QExplicitlySharedDataPointer<QGeoPositionInfoPrivate> d;
    friend class QGeoPositionInfoPrivate;

    friend Q_POSITIONING_EXPORT size_t qHash(const QGeoPositionInfo &key, size_t seed) noexcept;
    friend Q_POSITIONING_EXPORT char *QTest::toString(const QGeoPositionInfo &info);
};

Q_DECLARE_SHARED(QGeoPositionInfo)

QT_END_NAMESPACE

QT_DECL_METATYPE_EXTERN(QGeoPositionInfo, Q_POSITIONING_EXPORT)

#endif // QGEOPOSITIONINFO_H
