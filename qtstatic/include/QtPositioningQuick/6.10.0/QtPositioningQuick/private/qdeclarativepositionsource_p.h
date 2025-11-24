// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QDECLARATIVEPOSITIONSOURCE_H
#define QDECLARATIVEPOSITIONSOURCE_H

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
#include <QtPositioningQuick/private/qdeclarativeposition_p.h>
#include <QtCore/QObject>
#include <QtNetwork/QAbstractSocket>
#include <QtQml/QQmlParserStatus>
#include <QtPositioning/qgeopositioninfosource.h>
#include <QtPositioningQuick/private/qdeclarativepluginparameter_p.h>
#include <QtCore/private/qproperty_p.h>

QT_BEGIN_NAMESPACE

class QFile;
class QTcpSocket;

class Q_POSITIONINGQUICK_EXPORT QDeclarativePositionSource : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PositionSource)
    QML_ADDED_IN_VERSION(5, 0)

    Q_PROPERTY(QDeclarativePosition *position READ position NOTIFY positionChanged
               BINDABLE bindablePosition)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged
               BINDABLE bindableActive)
    Q_PROPERTY(bool valid READ isValid NOTIFY validityChanged BINDABLE bindableIsValid)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval
               NOTIFY updateIntervalChanged)
    Q_PROPERTY(PositioningMethods supportedPositioningMethods READ supportedPositioningMethods
               NOTIFY supportedPositioningMethodsChanged
               BINDABLE bindableSupportedPositioningMethods)
    Q_PROPERTY(PositioningMethods preferredPositioningMethods READ preferredPositioningMethods
               WRITE setPreferredPositioningMethods NOTIFY preferredPositioningMethodsChanged)
    Q_PROPERTY(SourceError sourceError READ sourceError NOTIFY sourceErrorChanged
               BINDABLE bindableSourceError)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged BINDABLE bindableName)
    Q_PROPERTY(QQmlListProperty<QDeclarativePluginParameter> parameters READ parameters REVISION(5, 14))

    Q_CLASSINFO("DefaultProperty", "parameters")
    Q_INTERFACES(QQmlParserStatus)

public:
    enum PositioningMethod {
        NoPositioningMethods = QGeoPositionInfoSource::NoPositioningMethods,
        SatellitePositioningMethods = QGeoPositionInfoSource::SatellitePositioningMethods,
        NonSatellitePositioningMethods = QGeoPositionInfoSource::NonSatellitePositioningMethods,
        AllPositioningMethods = QGeoPositionInfoSource::AllPositioningMethods
    };

    Q_ENUM(PositioningMethod)
    Q_DECLARE_FLAGS(PositioningMethods, PositioningMethod)
    Q_FLAG(PositioningMethods)

    enum SourceError {
        AccessError = QGeoPositionInfoSource::AccessError,
        ClosedError = QGeoPositionInfoSource::ClosedError,
        UnknownSourceError = QGeoPositionInfoSource::UnknownSourceError,
        NoError = QGeoPositionInfoSource::NoError,
        UpdateTimeoutError = QGeoPositionInfoSource::UpdateTimeoutError,
    };
    Q_ENUM(SourceError)

    QDeclarativePositionSource();
    ~QDeclarativePositionSource();
    void setUpdateInterval(int updateInterval);
    void setActive(bool active);
    void setPreferredPositioningMethods(PositioningMethods methods);

    QString name() const;
    void setName(const QString &name);

    int updateInterval() const;
    bool isActive() const;
    bool isValid() const;
    QDeclarativePosition *position();
    PositioningMethods supportedPositioningMethods() const;
    PositioningMethods preferredPositioningMethods() const;
    SourceError sourceError() const;
    QGeoPositionInfoSource *positionSource() const;
    QQmlListProperty<QDeclarativePluginParameter> parameters();
    QVariantMap parameterMap() const;

    // Virtuals from QQmlParserStatus
    void classBegin() override { }
    void componentComplete() override;

    Q_REVISION(5, 14) Q_INVOKABLE bool setBackendProperty(const QString &name, const QVariant &value);
    Q_REVISION(5, 14) Q_INVOKABLE QVariant backendProperty(const QString &name) const;

    QBindable<PositioningMethods> bindableSupportedPositioningMethods() const;
    QBindable<SourceError> bindableSourceError() const;
    QBindable<bool> bindableIsValid() const;
    QBindable<QString> bindableName();
    QBindable<QDeclarativePosition *> bindablePosition() const;
    QBindable<bool> bindableActive();

public Q_SLOTS:
    void update(int timeout = 0);
    void start();
    void stop();

Q_SIGNALS:
    void positionChanged();
    void activeChanged();
    void updateIntervalChanged();
    void supportedPositioningMethodsChanged();
    void preferredPositioningMethodsChanged();
    void sourceErrorChanged();
    void nameChanged();
    void validityChanged();

private Q_SLOTS:
    void positionUpdateReceived(const QGeoPositionInfo &update);
    void sourceErrorReceived(const QGeoPositionInfoSource::Error error);
    void onParameterInitialized();
    void notifySupportedPositioningMethodsChanged();

private:
    void setPosition(const QGeoPositionInfo &pi);
    void setSource(QGeoPositionInfoSource *source);
    bool parametersReady();
    void tryAttach(const QString &name, bool useFallback = true);

    static void parameter_append(QQmlListProperty<QDeclarativePluginParameter> *prop, QDeclarativePluginParameter *mapObject);
    static qsizetype parameter_count(QQmlListProperty<QDeclarativePluginParameter> *prop);
    static QDeclarativePluginParameter *parameter_at(QQmlListProperty<QDeclarativePluginParameter> *prop, qsizetype index);
    static void parameter_clear(QQmlListProperty<QDeclarativePluginParameter> *prop);

    bool isValidActualComputation() const;
    PositioningMethods supportedMethodsActualComputation() const;

    void executeStart();

    QGeoPositionInfoSource *m_positionSource = nullptr;
    PositioningMethods m_preferredPositioningMethods = AllPositioningMethods;
    int m_updateInterval = 0;
    QList<QDeclarativePluginParameter *> m_parameters;

    Q_OBJECT_COMPAT_PROPERTY(QDeclarativePositionSource, QString, m_sourceName,
                             &QDeclarativePositionSource::setName,
                             &QDeclarativePositionSource::nameChanged)

    Q_OBJECT_COMPAT_PROPERTY_WITH_ARGS(QDeclarativePositionSource, bool, m_active,
                                       &QDeclarativePositionSource::setActive,
                                       &QDeclarativePositionSource::activeChanged, false)

    Q_OBJECT_BINDABLE_PROPERTY(QDeclarativePositionSource, QDeclarativePosition *, m_position)

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(QDeclarativePositionSource, SourceError, m_sourceError,
                                         NoError)

    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePositionSource, PositioningMethods,
                               m_supportedPositioningMethods,
                               &QDeclarativePositionSource::supportedMethodsActualComputation)

    Q_OBJECT_COMPUTED_PROPERTY(QDeclarativePositionSource, bool, m_isValid,
                               &QDeclarativePositionSource::isValidActualComputation)

    quint8 m_singleUpdate : 1;
    quint8 m_regularUpdates : 1;
    quint8 m_componentComplete : 1;
    quint8 m_parametersInitialized : 1;
    quint8 m_startRequested : 1;
    quint8 m_defaultSourceUsed : 1;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePositionSource)

#endif
