// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QCAMERA_P_H
#define QCAMERA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qobject_p.h"
#include "qcamera.h"
#include "qcameradevice.h"

QT_BEGIN_NAMESPACE

class QPlatformCamera;
class QPlatformMediaCaptureSession;

class QCameraPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QCamera)
public:
    void init(const QCameraDevice &device);

    QMediaCaptureSession *captureSession = nullptr;
    QPlatformCamera *control = nullptr;

    QCameraDevice cameraDevice;
    QCameraFormat cameraFormat;
};

QT_END_NAMESPACE

#endif // QCAMERA_P_H
