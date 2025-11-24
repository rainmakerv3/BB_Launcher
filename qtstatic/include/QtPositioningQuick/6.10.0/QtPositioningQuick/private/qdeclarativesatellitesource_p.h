// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QDECLARATIVESATELLITESOURCE_H
#define QDECLARATIVESATELLITESOURCE_H

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

#include <QtCore/private/qproperty_p.h>
#include <QtCore/QObject>

#include <QtPositioning/qgeosatelliteinfosource.h>

#include <QtPositioningQuick/private/qdeclarativepluginparameter_p.h>
#include <QtPositioningQuick/private/qpositioningquickglobal_p.h>

#include <QtQml/QQmlParserStatus>

QT_BEGIN_NAMESPACE

class Q_POSITIONINGQUICK_EXPORT QDeclarativeSatelliteSource : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SatelliteSource)
    QML_ADDED_IN_VERSION(6, 5)

    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validityChanged)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval
               NOTIFY updateIntervalChanged)
    Q_PROPERTY(SourceError sourceError READ sourceError NOTIFY sourceErrorChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QQmlListProperty<QDeclarativePluginParameter> parameters READ parameters)
    Q_PROPERTY(QList<QGeoSatelliteInfo> satellitesInUse READ satellitesInUse
               NOTIFY satellitesInUseChanged)
    Q_PROPERTY(QList<QGeoSatelliteInfo> satellitesInView READ satellitesInView
               NOTIFY satellitesInViewChanged)

    Q_CLASSINFO("DefaultProperty", "parameters")
    Q_INTERFACES(QQmlParserStatus)

public:
    enum SourceError {
        AccessError = QGeoSatelliteInfoSource::AccessError,
        ClosedError = QGeoSatelliteInfoSource::ClosedError,
        NoError = QGeoSatelliteInfoSource::NoError,
        UnknownSourceError = QGeoSatelliteInfoSource::UnknownSourceError,
        UpdateTimeoutError = QGeoSatelliteInfoSource::UpdateTimeoutError,
    };
    Q_ENUM(SourceError)

    QDeclarativeSatelliteSource();
    ~QDeclarativeSatelliteSource();

    bool isActive() const;
    bool isValid() const;
    int updateInterval() const;
    SourceError sourceError() const;
    QString name() const;
    QQmlListProperty<QDeclarativePluginParameter> parameters();
    QList<QGeoSatelliteInfo> satellitesInUse() const;
    QList<QGeoSatelliteInfo> satellitesInView() const;

    void setUpdateInterval(int updateInterval);
    void setActive(bool active);
    void setName(const QString &name);

    // virtuals from QQmlParserStatus
    void classBegin() override { }
    void componentComplete() override;

    Q_INVOKABLE bool setBackendProperty(const QString &name, const QVariant &value);
    Q_INVOKABLE QVariant backendProperty(const QString &name) const;

public Q_SLOTS:
    void update(int timeout = 0);
    void start();
    void stop();

Q_SIGNALS:
    void activeChanged();
    void validityChanged();
    void updateIntervalChanged();
    void sourceErrorChanged();
    void nameChanged();
    void satellitesInUseChanged();
    void satellitesInViewChanged();

private Q_SLOTS:
    void sourceErrorReceived(const QGeoSatelliteInfoSource::Error error);
    void onParameterInitialized();
    void satellitesInViewUpdateReceived(const QList<QGeoSatelliteInfo> &satellites);
    void satellitesInUseUpdateReceived(const QList<QGeoSatelliteInfo> &satellites);

private:
    QVariantMap parameterMap() const;
    void createSource(const QString &newName);
    void handleSingleUpdateReceived();
    void executeStart();
    void executeSingleUpdate(int timeout);

    using PluginParameterProperty = QQmlListProperty<QDeclarativePluginParameter>;
    static void parameter_append(PluginParameterProperty *prop,
                                 QDeclarativePluginParameter *parameter);
    static qsizetype parameter_count(PluginParameterProperty *prop);
    static QDeclarativePluginParameter *parameter_at(PluginParameterProperty *prop,
                                                     qsizetype index);
    static void parameter_clear(PluginParameterProperty *prop);

    std::unique_ptr<QGeoSatelliteInfoSource> m_source;
    QList<QDeclarativePluginParameter *> m_parameters;

    int m_updateInterval = 0;
    SourceError m_error = SourceError::NoError;
    QString m_name;
    QList<QGeoSatelliteInfo> m_satellitesInView;
    QList<QGeoSatelliteInfo> m_satellitesInUse;

    int m_singleUpdateDesiredTimeout = 0;

    quint8 m_active : 1;
    quint8 m_componentComplete : 1;
    quint8 m_parametersInitialized : 1;
    quint8 m_startRequested : 1;
    quint8 m_defaultSourceUsed : 1;
    quint8 m_regularUpdates : 1;
    quint8 m_singleUpdate : 1;
    quint8 m_singleUpdateRequested : 1;
};

QT_END_NAMESPACE

#endif // QDECLARATIVESATELLITESOURCE_H
