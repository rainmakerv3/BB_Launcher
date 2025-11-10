// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QCOMREGISTRY_P_H
#define QCOMREGISTRY_P_H

#include <QtMultimedia/qtmultimediaexports.h>
#include <QtCore/qtconfigmacros.h>

QT_BEGIN_NAMESPACE

Q_MULTIMEDIA_EXPORT void ensureComInitializedOnThisThread();

struct QComInitializer
{
    QComInitializer()
    {
        ensureComInitializedOnThisThread();
    }
};


QT_END_NAMESPACE

#endif // QCOMREGISTRY_P_H
