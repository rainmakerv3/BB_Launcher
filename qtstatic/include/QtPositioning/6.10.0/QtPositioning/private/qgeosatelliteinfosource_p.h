// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOSATELLITEINFOSOURCE_P_H
#define QGEOSATELLITEINFOSOURCE_P_H

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
#include <QtPositioning/private/qpositioningglobal_p.h>
#include <QString>

QT_BEGIN_NAMESPACE
class QGeoSatelliteInfoSource;
class QGeoSatelliteInfoSourcePrivate : public QObjectPrivate
{
public:
    virtual ~QGeoSatelliteInfoSourcePrivate();
    static QGeoSatelliteInfoSource *createSourceReal(const QCborMap &meta,
                                                     const QVariantMap &parameters,
                                                     QObject *parent);
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(QGeoSatelliteInfoSourcePrivate, int, interval, 0)
    QString providerName;
};

QT_END_NAMESPACE

#endif // QGEOSATELLITEINFOSOURCE_P_H
