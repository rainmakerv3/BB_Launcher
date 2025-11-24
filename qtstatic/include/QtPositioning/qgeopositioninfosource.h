// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOPOSITIONINFOSOURCE_H
#define QGEOPOSITIONINFOSOURCE_H

#include <QtPositioning/QGeoPositionInfo>

#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QGeoPositionInfoSourcePrivate;
class Q_POSITIONING_EXPORT QGeoPositionInfoSource : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval BINDABLE
                       bindableUpdateInterval)
    Q_PROPERTY(int minimumUpdateInterval READ minimumUpdateInterval)
    Q_PROPERTY(QString sourceName READ sourceName)
    Q_PROPERTY(PositioningMethods preferredPositioningMethods READ preferredPositioningMethods WRITE
                       setPreferredPositioningMethods BINDABLE bindablePreferredPositioningMethods)

public:
    enum Error {
        AccessError = 0,
        ClosedError = 1,
        UnknownSourceError = 2,
        NoError = 3,
        UpdateTimeoutError = 4
    };
    Q_ENUM(Error)

    enum PositioningMethod {
        NoPositioningMethods = 0x00000000,
        SatellitePositioningMethods = 0x000000ff,
        NonSatellitePositioningMethods = 0xffffff00,
        AllPositioningMethods = 0xffffffff
    };
    Q_DECLARE_FLAGS(PositioningMethods, PositioningMethod)

    explicit QGeoPositionInfoSource(QObject *parent);
    virtual ~QGeoPositionInfoSource();

    virtual void setUpdateInterval(int msec);
    int updateInterval() const;
    QBindable<int> bindableUpdateInterval();

    virtual void setPreferredPositioningMethods(PositioningMethods methods);
    PositioningMethods preferredPositioningMethods() const;
    QBindable<PositioningMethods> bindablePreferredPositioningMethods();

    virtual QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const = 0;

    virtual PositioningMethods supportedPositioningMethods() const = 0;
    virtual int minimumUpdateInterval() const = 0;

    QString sourceName() const;

    virtual bool setBackendProperty(const QString &name, const QVariant &value);
    virtual QVariant backendProperty(const QString &name) const;

    static QGeoPositionInfoSource *createDefaultSource(QObject *parent);
    static QGeoPositionInfoSource *createDefaultSource(const QVariantMap &parameters, QObject *parent);
    static QGeoPositionInfoSource *createSource(const QString &sourceName, QObject *parent);
    static QGeoPositionInfoSource *createSource(const QString &sourceName, const QVariantMap &parameters, QObject *parent);
    static QStringList availableSources();
    virtual Error error() const = 0;

public Q_SLOTS:
    virtual void startUpdates() = 0;
    virtual void stopUpdates() = 0;

    virtual void requestUpdate(int timeout = 0) = 0;

Q_SIGNALS:
    void positionUpdated(const QGeoPositionInfo &update);
    void errorOccurred(QGeoPositionInfoSource::Error);
    void supportedPositioningMethodsChanged();

protected:
    explicit QGeoPositionInfoSource(QGeoPositionInfoSourcePrivate &dd, QObject *parent);

private:
    Q_DISABLE_COPY(QGeoPositionInfoSource)
    Q_DECLARE_PRIVATE(QGeoPositionInfoSource)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoPositionInfoSource::PositioningMethods)

QT_END_NAMESPACE

#endif
