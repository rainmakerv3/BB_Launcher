// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKSCREENCAPTURE_H
#define QQUICKSCREENCAPTURE_H

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

#include <QScreenCapture>
#include <qtmultimediaquickexports.h>
#include <private/qglobal_p.h>
#include <private/qquickscreen_p.h>

QT_BEGIN_NAMESPACE

class Q_MULTIMEDIAQUICK_EXPORT QQuickScreenCatpure : public QScreenCapture
{
    Q_OBJECT
    Q_PROPERTY(QQuickScreenInfo *screen READ qmlScreen WRITE qmlSetScreen NOTIFY screenChanged)
    QML_NAMED_ELEMENT(ScreenCapture)

public:
    QQuickScreenCatpure(QObject *parent = nullptr);

    void qmlSetScreen(const QQuickScreenInfo *info);

    QQuickScreenInfo *qmlScreen();

Q_SIGNALS:
    void screenChanged(QQuickScreenInfo *);
};

QT_END_NAMESPACE

#endif
