// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOPOSITIONINFOSOURCE_P_H
#define QGEOPOSITIONINFOSOURCE_P_H

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

#include <QtCore/private/qobject_p.h>
#include <QtCore/private/qproperty_p.h>
#include <QtPositioning/private/qpositioningglobal_p.h>
#include "qgeopositioninfosource.h"
#include "qgeopositioninfosourcefactory.h"
#include <QCborMap>
#include <QString>
#include <QMultiHash>
#include <QList>

QT_BEGIN_NAMESPACE

class QGeoPositionInfoSourcePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGeoPositionInfoSource)
public:
    virtual ~QGeoPositionInfoSourcePrivate();

    static QGeoPositionInfoSourceFactory *loadFactory(const QCborMap &meta);
    static QGeoPositionInfoSource *createSourceReal(const QCborMap &meta,
                                                    const QVariantMap &parameters,
                                                    QObject *parent);

    void setPositioningMethods(QGeoPositionInfoSource::PositioningMethods methods)
    {
        q_func()->setPreferredPositioningMethods(methods);
    }

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(QGeoPositionInfoSourcePrivate, int, interval, 0)
    Q_OBJECT_COMPAT_PROPERTY_WITH_ARGS(QGeoPositionInfoSourcePrivate,
                                       QGeoPositionInfoSource::PositioningMethods, methods,
                                       &QGeoPositionInfoSourcePrivate::setPositioningMethods,
                                       QGeoPositionInfoSource::NoPositioningMethods)
    QString sourceName;

    static QMultiHash<QString, QCborMap> plugins(bool reload = false);
    static void loadPluginMetadata(QMultiHash<QString, QCborMap> &list);
    static QList<QCborMap> pluginsSorted();
};

QT_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCE_P_H
