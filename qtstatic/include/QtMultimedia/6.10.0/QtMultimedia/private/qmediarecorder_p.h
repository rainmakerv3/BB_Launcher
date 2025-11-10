// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMediaRecorder_P_H
#define QMediaRecorder_P_H

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

#include "qmediarecorder.h"
#include "qcamera.h"
#include <QtCore/qurl.h>
#include <QtCore/qpointer.h>
#include "private/qplatformmediarecorder_p.h"

QT_BEGIN_NAMESPACE

class QPlatformMediaRecorder;
class QTimer;

class Q_MULTIMEDIA_EXPORT QMediaRecorderPrivate
{
    Q_DECLARE_PUBLIC(QMediaRecorder)

public:
    QMediaRecorderPrivate();

    static QString msgFailedStartRecording();

    QMediaCaptureSession *captureSession = nullptr;
    QPlatformMediaRecorder *control = nullptr;
    QString initErrorMessage;
    bool autoStop = false;

    bool settingsChanged = false;

    QMediaEncoderSettings encoderSettings;

    QMediaRecorder *q_ptr = nullptr;
};

#undef Q_DECLARE_NON_CONST_PUBLIC

QT_END_NAMESPACE

#endif

