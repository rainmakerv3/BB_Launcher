// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIOBUFFERINPUT_H
#define QAUDIOBUFFERINPUT_H

#include <QtMultimedia/qtmultimediaexports.h>
#include <QtMultimedia/qaudiobuffer.h>
#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QPlatformAudioBufferInput;
class QAudioBufferInputPrivate;
class QMediaCaptureSession;

class Q_MULTIMEDIA_EXPORT QAudioBufferInput : public QObject
{
    Q_OBJECT
public:
    explicit QAudioBufferInput(QObject *parent = nullptr);

    explicit QAudioBufferInput(const QAudioFormat &format, QObject *parent = nullptr);

    ~QAudioBufferInput() override;

    bool sendAudioBuffer(const QAudioBuffer &audioBuffer);

    QAudioFormat format() const;

    QMediaCaptureSession *captureSession() const;

Q_SIGNALS:
    void readyToSendAudioBuffer();

private:
    void setCaptureSession(QMediaCaptureSession *captureSession);

    QPlatformAudioBufferInput *platformAudioBufferInput() const;

    friend class QMediaCaptureSession;
    Q_DISABLE_COPY(QAudioBufferInput)
    Q_DECLARE_PRIVATE(QAudioBufferInput)
};

QT_END_NAMESPACE

#endif // QAUDIOBUFFERINPUT_H
