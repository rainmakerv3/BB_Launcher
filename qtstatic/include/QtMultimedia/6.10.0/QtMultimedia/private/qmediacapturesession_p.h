// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMEDIACAPTURESESSION_P_H
#define QMEDIACAPTURESESSION_P_H

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

#include <QtMultimedia/qmediacapturesession.h>

#include <QtCore/qpointer.h>
#include <QtCore/private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QMediaCaptureSessionPrivate : public QObjectPrivate
{
public:
    static QMediaCaptureSessionPrivate *get(QMediaCaptureSession *session)
    {
        return reinterpret_cast<QMediaCaptureSessionPrivate *>(QObjectPrivate::get(session));
    }

    Q_DECLARE_PUBLIC(QMediaCaptureSession)

    std::unique_ptr<QPlatformMediaCaptureSession> captureSession;
    QAudioInput *audioInput = nullptr;
    QPointer<QAudioBufferInput> audioBufferInput;
    QAudioOutput *audioOutput = nullptr;
    QPointer<QCamera> camera;
    QPointer<QScreenCapture> screenCapture;
    QPointer<QWindowCapture> windowCapture;
    QPointer<QVideoFrameInput> videoFrameInput;
    QPointer<QImageCapture> imageCapture;
    QPointer<QMediaRecorder> recorder;
    QPointer<QVideoSink> videoSink;
    QPointer<QObject> videoOutput;

    void setVideoSink(QVideoSink *sink);
};

QT_END_NAMESPACE

#endif // QMEDIACAPTURESESSION_P_H
