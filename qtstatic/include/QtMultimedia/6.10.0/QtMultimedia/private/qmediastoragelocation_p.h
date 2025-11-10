// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMEDIASTORAGELOCATION_H
#define QMEDIASTORAGELOCATION_H

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

#include <qtmultimediaglobal.h>
#include <QStandardPaths>
#include <QDir>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace QMediaStorageLocation
{
Q_MULTIMEDIA_EXPORT QDir defaultDirectory(QStandardPaths::StandardLocation type);
Q_MULTIMEDIA_EXPORT QString generateFileName(const QString &requestedName,
                                             QStandardPaths::StandardLocation type,
                                             const QString &extension);
};

QT_END_NAMESPACE

#endif // QMEDIASTORAGELOCATION_H
