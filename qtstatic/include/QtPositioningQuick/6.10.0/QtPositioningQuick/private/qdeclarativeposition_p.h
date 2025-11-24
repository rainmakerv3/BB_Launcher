// Copyright (C) 2016 Jolla Ltd.
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QDECLARATIVEPOSITION_H
#define QDECLARATIVEPOSITION_H

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

#include <QtPositioningQuick/private/qpositioningquickglobal_p.h>
#include <QtPositioning/QGeoPositionInfo>
#include <QtCore/QObject>
#include <QtCore/QProperty>
#include <QtCore/QDateTime>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class Q_POSITIONINGQUICK_EXPORT QDeclarativePosition : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Position)
    QML_ADDED_IN_VERSION(5, 0)

    Q_PROPERTY(bool latitudeValid READ isLatitudeValid BINDABLE bindableLatitudeValid)
    Q_PROPERTY(bool longitudeValid READ isLongitudeValid BINDABLE bindableLongitudeValid)
    Q_PROPERTY(bool altitudeValid READ isAltitudeValid BINDABLE bindableAltitudeValid)
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate BINDABLE bindableCoordinate)
    Q_PROPERTY(QDateTime timestamp READ timestamp BINDABLE bindableTimestamp)
    Q_PROPERTY(double speed READ speed BINDABLE bindableSpeed)
    Q_PROPERTY(bool speedValid READ isSpeedValid BINDABLE bindableSpeedValid)
    Q_PROPERTY(qreal horizontalAccuracy READ horizontalAccuracy BINDABLE bindableHorizontalAccuracy)
    Q_PROPERTY(qreal verticalAccuracy READ verticalAccuracy BINDABLE binableVerticalAccuracy)
    Q_PROPERTY(bool horizontalAccuracyValid READ isHorizontalAccuracyValid BINDABLE
                       bindableHorizontalAccuracyValid)
    Q_PROPERTY(bool verticalAccuracyValid READ isVerticalAccuracyValid BINDABLE
                       bindableVerticalAccuracyValid)

    Q_PROPERTY(bool directionValid READ isDirectionValid BINDABLE bindableDirectionValid
                       REVISION(5, 1))
    Q_PROPERTY(double direction READ direction BINDABLE bindableDirection REVISION(5, 1))
    Q_PROPERTY(bool verticalSpeedValid READ isVerticalSpeedValid BINDABLE bindableVerticalSpeedValid
                       REVISION(5, 1))
    Q_PROPERTY(double verticalSpeed READ verticalSpeed BINDABLE bindableVerticalSpeed
                       REVISION(5, 1))

    Q_PROPERTY(double magneticVariation READ magneticVariation BINDABLE bindableMagneticVariation
                       REVISION(5, 2))
    Q_PROPERTY(bool magneticVariationValid READ isMagneticVariationValid BINDABLE
                       bindableMagneticVariationValid REVISION(5, 2))

    Q_PROPERTY(double directionAccuracy READ directionAccuracy BINDABLE bindableDirectionAccuracy
               REVISION(6, 3))
    Q_PROPERTY(bool directionAccuracyValid READ isDirectionAccuracyValid BINDABLE
               bindableDirectionAccuracyValid REVISION(6, 3))

public:
    explicit QDeclarativePosition(QObject *parent = 0);
    ~QDeclarativePosition();

    bool isLatitudeValid() const;
    bool isLongitudeValid() const;
    bool isAltitudeValid() const;
    QDateTime timestamp() const;
    double speed() const;
    bool isSpeedValid() const;
    QGeoCoordinate coordinate() const;
    bool isHorizontalAccuracyValid() const;
    qreal horizontalAccuracy() const;
    bool isVerticalAccuracyValid() const;
    qreal verticalAccuracy() const;

    bool isDirectionValid() const;
    double direction() const;

    bool isVerticalSpeedValid() const;
    double verticalSpeed() const;

    bool isMagneticVariationValid() const;
    double magneticVariation() const;

    void setPosition(const QGeoPositionInfo &info);
    const QGeoPositionInfo &position() const;

    bool isDirectionAccuracyValid() const;
    double directionAccuracy() const;

    QBindable<bool> bindableLatitudeValid() const;
    QBindable<bool> bindableLongitudeValid() const;
    QBindable<bool> bindableAltitudeValid() const;
    QBindable<QGeoCoordinate> bindableCoordinate() const;
    QBindable<QDateTime> bindableTimestamp() const;
    QBindable<double> bindableSpeed() const;
    QBindable<bool> bindableSpeedValid() const;
    QBindable<qreal> bindableHorizontalAccuracy() const;
    QBindable<qreal> binableVerticalAccuracy() const;
    QBindable<bool> bindableHorizontalAccuracyValid() const;
    QBindable<bool> bindableVerticalAccuracyValid() const;
    QBindable<bool> bindableDirectionValid() const;
    QBindable<double> bindableDirection() const;
    QBindable<bool> bindableVerticalSpeedValid() const;
    QBindable<double> bindableVerticalSpeed() const;
    QBindable<double> bindableMagneticVariation() const;
    QBindable<bool> bindableMagneticVariationValid() const;
    QBindable<double> bindableDirectionAccuracy() const;
    QBindable<bool> bindableDirectionAccuracyValid() const;

private:
    bool isLatitudeValidActualCalculation() const;
    bool isLongitudeValidActualCalculation() const;
    bool isAltitudeValidActualCalculation() const;
    QGeoCoordinate coordinateActualCalculation() const;
    QDateTime timestampActualCalculation() const;
    double speedActualCalculation() const;
    bool isSpeedValidActualCalculation() const;
    qreal horizontalAccuracyActualCalculation() const;
    qreal verticalAccuracyActualCalculation() const;
    bool isHorizontalAccuracyValidActualCalculation() const;
    bool isVerticalAccuracyValidActualCalculation() const;
    bool isDirectionValidActualCalculation() const;
    double directionActualCalculation() const;
    bool isVerticalSpeedValidActualCalculation() const;
    double verticalSpeedActualCalculation() const;
    double magneticVariationActualCalculation() const;
    bool isMagneticVariationValidActualCalculation() const;
    double directionAccuracyActualCalculation() const;
    bool isDirectionAccuracyValidActualCalculation() const;

    QGeoPositionInfo m_info;

    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, bool, m_computedLatitudeValid,
                               &QDeclarativePosition::isLatitudeValidActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, bool, m_computedLongitudeValid,
                               &QDeclarativePosition::isLongitudeValidActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, bool, m_computedAltitudeValid,
                               &QDeclarativePosition::isAltitudeValidActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, QGeoCoordinate, m_computedCoordinate,
                               &QDeclarativePosition::coordinateActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, QDateTime, m_computedTimestamp,
                               &QDeclarativePosition::timestampActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, double, m_computedSpeed,
                               &QDeclarativePosition::speedActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, bool, m_computedSpeedValid,
                               &QDeclarativePosition::isSpeedValidActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, qreal, m_computedHorizontalAccuracy,
                               &QDeclarativePosition::horizontalAccuracyActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, qreal, m_computedVerticalAccuracy,
                               &QDeclarativePosition::verticalAccuracyActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, bool, m_computedHorizontalAccuracyValid,
                               &QDeclarativePosition::isHorizontalAccuracyValidActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, bool, m_computedVerticalAccuracyValid,
                               &QDeclarativePosition::isVerticalAccuracyValidActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, bool, m_computedDirectionValid,
                               &QDeclarativePosition::isDirectionValidActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, double, m_computedDirection,
                               &QDeclarativePosition::directionActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, bool, m_computedVerticalSpeedValid,
                               &QDeclarativePosition::isVerticalSpeedValidActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, double, m_computedVerticalSpeed,
                               &QDeclarativePosition::verticalSpeedActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, double, m_computedMagneticVariation,
                               &QDeclarativePosition::magneticVariationActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, bool, m_computedMagneticVariationValid,
                               &QDeclarativePosition::isMagneticVariationValidActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, double, m_computedDirectionAccuracy,
                               &QDeclarativePosition::directionAccuracyActualCalculation)
    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePosition, bool, m_computedDirectionAccuracyValid,
                               &QDeclarativePosition::isDirectionAccuracyValidActualCalculation)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePosition)

#endif
