// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKIMAGEPREVIEWPROVIDER_H
#define QQUICKIMAGEPREVIEWPROVIDER_H

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

#include <private/qtmultimediaquickglobal_p.h>
#include <QtQuick/qquickimageprovider.h>

QT_BEGIN_NAMESPACE

class Q_MULTIMEDIAQUICK_EXPORT QQuickImagePreviewProvider : public QQuickImageProvider
{
public:
    QQuickImagePreviewProvider();
    ~QQuickImagePreviewProvider() override;

    QImage requestImage(const QString &id, QSize *size, const QSize& requestedSize) override;
    static void registerPreview(const QString &id, const QImage &preview);
};

QT_END_NAMESPACE

#endif
