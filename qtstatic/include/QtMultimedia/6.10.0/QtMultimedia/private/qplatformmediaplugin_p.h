// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#ifndef QPLATFORMMEDIAPLUGIN_P_H
#define QPLATFORMMEDIAPLUGIN_P_H

#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtCore/qplugin.h>
#include <QtCore/qfactoryinterface.h>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class QPlatformMediaIntegration;

#define QPlatformMediaPlugin_iid "org.qt-project.Qt.QPlatformMediaPlugin"

class Q_MULTIMEDIA_EXPORT QPlatformMediaPlugin : public QObject
{
    Q_OBJECT
public:
    explicit QPlatformMediaPlugin(QObject *parent = nullptr);
    ~QPlatformMediaPlugin() override;

    virtual QPlatformMediaIntegration *create(const QString &key) = 0;

};

QT_END_NAMESPACE

#endif // QPLATFORMMEDIAPLUGIN_P_H
