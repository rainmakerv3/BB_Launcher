// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QVIDEOFRAMEINPUT_H
#define QVIDEOFRAMEINPUT_H

#include <QtMultimedia/qtmultimediaexports.h>
#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QPlatformVideoFrameInput;
class QVideoFrameInputPrivate;
class QMediaCaptureSession;
class QVideoFrame;
class QVideoFrameFormat;

class Q_MULTIMEDIA_EXPORT QVideoFrameInput : public QObject
{
    Q_OBJECT
public:
    explicit QVideoFrameInput(QObject *parent = nullptr);

    explicit QVideoFrameInput(const QVideoFrameFormat &format, QObject *parent = nullptr);

    ~QVideoFrameInput() override;

    bool sendVideoFrame(const QVideoFrame &frame);

    QVideoFrameFormat format() const;

    QMediaCaptureSession *captureSession() const;

Q_SIGNALS:
    void readyToSendVideoFrame();

private:
    void setCaptureSession(QMediaCaptureSession *captureSession);

    QPlatformVideoFrameInput *platformVideoFrameInput() const;

    friend class QMediaCaptureSession;
    Q_DISABLE_COPY(QVideoFrameInput)
    Q_DECLARE_PRIVATE(QVideoFrameInput)
};

QT_END_NAMESPACE

#endif // QVIDEOFRAMEINPUT_H
