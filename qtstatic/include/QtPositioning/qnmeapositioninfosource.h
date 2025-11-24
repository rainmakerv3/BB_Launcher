// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QNMEAPOSITIONINFOSOURCE_H
#define QNMEAPOSITIONINFOSOURCE_H

#include <QtPositioning/QGeoPositionInfoSource>

QT_BEGIN_NAMESPACE

class QIODevice;

class QNmeaPositionInfoSourcePrivate;
class Q_POSITIONING_EXPORT QNmeaPositionInfoSource : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    enum UpdateMode {
        RealTimeMode = 1,
        SimulationMode
    };

    explicit QNmeaPositionInfoSource(UpdateMode updateMode, QObject *parent = nullptr);
    ~QNmeaPositionInfoSource();

    void setUserEquivalentRangeError(double uere);
    double userEquivalentRangeError() const;

    UpdateMode updateMode() const;

    void setDevice(QIODevice *source);
    QIODevice *device() const;

    void setUpdateInterval(int msec) override;

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const override;
    PositioningMethods supportedPositioningMethods() const override;
    int minimumUpdateInterval() const override;
    Error error() const override;


public Q_SLOTS:
    void startUpdates() override;
    void stopUpdates() override;
    void requestUpdate(int timeout = 0) override;

protected:
#if QT_DEPRECATED_SINCE(7, 0)
    QT6_ONLY(virtual)
    bool parsePosInfoFromNmeaData(const char *data,
                                  int size,
                                  QGeoPositionInfo *posInfo,
                                  bool *hasFix);
#endif
    // ### Qt 7: design a return type that gets rid of out-parameters
    QT7_ONLY(virtual)
    bool parsePosInfoFromNmeaData(QByteArrayView data,
                                  QGeoPositionInfo *posInfo,
                                  bool *hasFix);

    void setError(QGeoPositionInfoSource::Error positionError);

private:
    Q_DISABLE_COPY(QNmeaPositionInfoSource)
    friend class QNmeaPositionInfoSourcePrivate;
    QNmeaPositionInfoSourcePrivate *d;
};

QT_END_NAMESPACE

#endif
