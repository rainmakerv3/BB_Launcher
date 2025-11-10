// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLATFORMCAPTURABLEWINDOWS_P_H
#define QPLATFORMCAPTURABLEWINDOWS_P_H

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

#include "private/qtmultimediaglobal_p.h"
#include "qcapturablewindow.h"

#include <QtCore/qlist.h>
#include <QtCore/private/qexpected_p.h>

QT_BEGIN_NAMESPACE

class QWindow;
class QCapturableWindow;
class QCapturableWindowPrivate;

class QPlatformCapturableWindows
{
public:
    QPlatformCapturableWindows() = default;

    virtual ~QPlatformCapturableWindows() = default;

    virtual QList<QCapturableWindow> windows() const { return {}; }

    virtual bool isWindowValid(const QCapturableWindowPrivate &) const { return false; }

    // QPlatformMediaIntegration::capturableWindowFromQWindow does
    // basic QWindow validity checks.
    [[nodiscard]] virtual q23::expected<QCapturableWindow, QString> fromQWindow(QWindow *) const
    {
        return q23::unexpected{ QStringLiteral("Unimplemented") };
    }

    Q_DISABLE_COPY(QPlatformCapturableWindows);
};

QT_END_NAMESPACE

#endif // QPLATFORMCAPTURABLEWINDOWS_P_H
