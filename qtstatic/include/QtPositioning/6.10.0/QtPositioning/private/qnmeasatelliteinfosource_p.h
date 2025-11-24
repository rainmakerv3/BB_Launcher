// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QNMESATELLITEINFOSOURCE_P_H
#define QNMESATELLITEINFOSOURCE_P_H

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

#include "qnmeasatelliteinfosource.h"
#include <QtPositioning/qgeosatelliteinfo.h>

#include <QObject>
#include <QQueue>
#include <QPointer>
#include <QMap>
#include <QtCore/qiodevice.h>
#include <QtCore/qtimer.h>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

#define USE_SATELLITE_NMEA_PIMPL 1

struct SatelliteInfo
{
    QList<QGeoSatelliteInfo> satellitesInView;
    QList<QGeoSatelliteInfo> satellitesInUse;
    QList<int> inUseIds; // temp buffer for GSA received before GSV
    bool satellitesInUseReceived = false;
    bool updatingGSV = false;
    bool validInView = false;
    bool validInUse = false;
};

struct QNmeaSatelliteInfoUpdate
{
    QMap<QGeoSatelliteInfo::SatelliteSystem, SatelliteInfo> m_satellites;
    QList<QGeoSatelliteInfo> m_satellitesInViewParsed;
    bool m_validInView = false; // global state for all satellite systems
    bool m_validInUse = false; // global state for all satellite systems
    bool m_fresh = false;
#if USE_SATELLITE_NMEA_PIMPL
    QByteArray gsa;
    QList<QByteArray> gsv;
#endif
    QList<QGeoSatelliteInfo> allSatellitesInUse() const;
    QList<QGeoSatelliteInfo> allSatellitesInView() const;
    void setSatellitesInView(QGeoSatelliteInfo::SatelliteSystem system,
                             const QList<QGeoSatelliteInfo> &inView);
    bool setSatellitesInUse(QGeoSatelliteInfo::SatelliteSystem system, const QList<int> &inUse);
    void consume();
    bool isFresh() const;
    void clear();
    bool isValid() const;
    bool calculateValidInUse() const;
    bool calculateValidInView() const;
};

class QNmeaSatelliteReader;
class QNmeaSatelliteInfoSourcePrivate : public QObject
{
    Q_OBJECT
public:
    QNmeaSatelliteInfoSourcePrivate(QNmeaSatelliteInfoSource *parent, QNmeaSatelliteInfoSource::UpdateMode updateMode);
    ~QNmeaSatelliteInfoSourcePrivate();

    void startUpdates();
    void stopUpdates();
    void requestUpdate(int msec);
    void notifyNewUpdate();
    void processNmeaData(QNmeaSatelliteInfoUpdate &updateInfo);

public slots:
    void readyRead();
    void emitPendingUpdate();
    void sourceDataClosed();
    void updateRequestTimeout();

public:
    QNmeaSatelliteInfoSource *m_source = nullptr;
    QGeoSatelliteInfoSource::Error m_satelliteError = QGeoSatelliteInfoSource::NoError;
    QPointer<QIODevice> m_device;
    QNmeaSatelliteInfoUpdate m_pendingUpdate;
    QNmeaSatelliteInfoUpdate m_lastUpdate;
    bool m_invokedStart = false;
    bool m_noUpdateLastInterval = false;
    bool m_updateTimeoutSent = false;
    bool m_connectedReadyRead = false;
    QBasicTimer *m_updateTimer = nullptr; // the timer used in startUpdates()
    QTimer *m_requestTimer = nullptr; // the timer used in requestUpdate()
    QScopedPointer<QNmeaSatelliteReader> m_nmeaReader;
    QNmeaSatelliteInfoSource::UpdateMode m_updateMode;
    int m_simulationUpdateInterval = 100;

protected:
    bool openSourceDevice();
    bool initialize();
    void prepareSourceDevice();
    bool emitUpdated(QNmeaSatelliteInfoUpdate &update, bool fromRequestUpdate);
    void timerEvent(QTimerEvent *event) override;
};

class QNmeaSatelliteReader
{
public:
    QNmeaSatelliteReader(QNmeaSatelliteInfoSourcePrivate *sourcePrivate);
    virtual ~QNmeaSatelliteReader();

    virtual void readAvailableData() = 0;

protected:
    QNmeaSatelliteInfoSourcePrivate *m_proxy;
};

class QNmeaSatelliteRealTimeReader : public QNmeaSatelliteReader
{
public:
    QNmeaSatelliteRealTimeReader(QNmeaSatelliteInfoSourcePrivate *sourcePrivate);
    void readAvailableData() override;
};

class QNmeaSatelliteSimulationReader : public QNmeaSatelliteReader
{
public:
    QNmeaSatelliteSimulationReader(QNmeaSatelliteInfoSourcePrivate *sourcePrivate);
    void readAvailableData() override;
    void setUpdateInterval(int msec);
    int updateInterval() const;

private:
    QScopedPointer<QTimer> m_timer;
    int m_updateInterval;
};

QT_END_NAMESPACE

#endif
