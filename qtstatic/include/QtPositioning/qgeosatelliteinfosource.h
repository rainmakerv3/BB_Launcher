// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QGEOSATELLITEINFOSOURCE_H
#define QGEOSATELLITEINFOSOURCE_H

#include <QtPositioning/QGeoSatelliteInfo>

#include <QtCore/QObject>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class QGeoSatelliteInfoSourcePrivate;
class Q_POSITIONING_EXPORT QGeoSatelliteInfoSource : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval BINDABLE
                       bindableUpdateInterval)
    Q_PROPERTY(int minimumUpdateInterval READ minimumUpdateInterval)

public:
    enum Error {
        AccessError = 0,
        ClosedError = 1,
        NoError = 2,
        UnknownSourceError = -1,
        UpdateTimeoutError = 3,
    };
    Q_ENUM(Error)

    explicit QGeoSatelliteInfoSource(QObject *parent);
    virtual ~QGeoSatelliteInfoSource();

    static QGeoSatelliteInfoSource *createDefaultSource(QObject *parent);
    static QGeoSatelliteInfoSource *createSource(const QString &sourceName, QObject *parent);
    static QGeoSatelliteInfoSource *createDefaultSource(const QVariantMap &parameters, QObject *parent);
    static QGeoSatelliteInfoSource *createSource(const QString &sourceName, const QVariantMap &parameters, QObject *parent);
    static QStringList availableSources();

    QString sourceName() const;

    virtual void setUpdateInterval(int msec);
    int updateInterval() const;
    QBindable<int> bindableUpdateInterval();

    virtual int minimumUpdateInterval() const = 0;
    virtual Error error() const = 0;

    virtual bool setBackendProperty(const QString &name, const QVariant &value);
    virtual QVariant backendProperty(const QString &name) const;

public Q_SLOTS:
    virtual void startUpdates() = 0;
    virtual void stopUpdates() = 0;

    virtual void requestUpdate(int timeout = 0) = 0;

Q_SIGNALS:
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void errorOccurred(QGeoSatelliteInfoSource::Error);

protected:
    explicit QGeoSatelliteInfoSource(QGeoSatelliteInfoSourcePrivate &dd, QObject *parent);

private:
    Q_DISABLE_COPY(QGeoSatelliteInfoSource)
    Q_DECLARE_PRIVATE(QGeoSatelliteInfoSource)
};

QT_END_NAMESPACE

#endif
