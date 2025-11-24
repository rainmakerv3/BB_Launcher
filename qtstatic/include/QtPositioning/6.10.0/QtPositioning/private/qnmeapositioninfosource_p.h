// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QNMEAPOSITIONINFOSOURCE_P_H
#define QNMEAPOSITIONINFOSOURCE_P_H

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

#include "qnmeapositioninfosource.h"
#include "qgeopositioninfo.h"

#include <QObject>
#include <QQueue>
#include <QPointer>
#include <QtCore/qtimer.h>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class QBasicTimer;
class QTimerEvent;
class QTimer;

class QNmeaReader;
struct QPendingGeoPositionInfo
{
    QGeoPositionInfo info;
    bool hasFix;
};


class QNmeaPositionInfoSourcePrivate : public QObject
{
    Q_OBJECT
public:
    QNmeaPositionInfoSourcePrivate(QNmeaPositionInfoSource *parent, QNmeaPositionInfoSource::UpdateMode updateMode);
    ~QNmeaPositionInfoSourcePrivate();

    void startUpdates();
    void stopUpdates();
    void requestUpdate(int msec);

    bool parsePosInfoFromNmeaData(QByteArrayView data,
                                  QGeoPositionInfo *posInfo,
                                  bool *hasFix);

    void notifyNewUpdate(QGeoPositionInfo *update, bool fixStatus);

    QNmeaPositionInfoSource::UpdateMode m_updateMode;
    QPointer<QIODevice> m_device;
    QGeoPositionInfo m_lastUpdate;
    bool m_invokedStart;
    QGeoPositionInfoSource::Error m_positionError;
    double m_userEquivalentRangeError;

public Q_SLOTS:
    void readyRead();

protected:
    void timerEvent(QTimerEvent *event) override;

private Q_SLOTS:
    void emitPendingUpdate();
    void sourceDataClosed();
    void updateRequestTimeout();

private:
    bool openSourceDevice();
    bool initialize();
    void prepareSourceDevice();
    void emitUpdated(const QGeoPositionInfo &update);

    QNmeaPositionInfoSource *m_source;
    QNmeaReader *m_nmeaReader;
    QGeoPositionInfo m_pendingUpdate;
    QDate m_currentDate;
    QBasicTimer *m_updateTimer; // the timer used in startUpdates()
    QTimer *m_requestTimer; // the timer used in requestUpdate()
    qreal m_horizontalAccuracy;
    qreal m_verticalAccuracy;
    bool m_noUpdateLastInterval;
    bool m_updateTimeoutSent;
    bool m_connectedReadyRead;
};


class QNmeaReader
{
public:
    explicit QNmeaReader(QNmeaPositionInfoSourcePrivate *sourcePrivate)
            : m_proxy(sourcePrivate) {}
    virtual ~QNmeaReader();

    virtual void readAvailableData() = 0;

protected:
    QNmeaPositionInfoSourcePrivate *m_proxy;
};


class QNmeaRealTimeReader : public QNmeaReader
{
public:
    explicit QNmeaRealTimeReader(QNmeaPositionInfoSourcePrivate *sourcePrivate);
    ~QNmeaRealTimeReader() override;

    void readAvailableData() override;
    void notifyNewUpdate();

    // Data members
    QGeoPositionInfo m_update;
    QDateTime m_lastPushedTS;
    bool m_updateParsed = false;
    bool m_hasFix = false;
    QTimer m_timer;
    int m_pushDelay = -1;
};


class QNmeaSimulatedReader : public QObject, public QNmeaReader
{
    Q_OBJECT
public:
    explicit QNmeaSimulatedReader(QNmeaPositionInfoSourcePrivate *sourcePrivate);
    ~QNmeaSimulatedReader();
    void readAvailableData() override;

protected:
    void timerEvent(QTimerEvent *event) override;

private Q_SLOTS:
    void simulatePendingUpdate();

private:
    bool setFirstDateTime();
    void processNextSentence();

    QQueue<QPendingGeoPositionInfo> m_pendingUpdates;
    QByteArray m_nextLine;
    int m_currTimerId;
    bool m_hasValidDateTime;
};

QT_END_NAMESPACE

#endif
