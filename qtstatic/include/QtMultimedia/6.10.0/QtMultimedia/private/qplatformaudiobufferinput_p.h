// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLATFORMAUDIOBUFFERINPUT_P_H
#define QPLATFORMAUDIOBUFFERINPUT_P_H

#include <QtMultimedia/qaudioformat.h>
#include <QtMultimedia/qaudiobuffer.h>

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

QT_BEGIN_NAMESPACE

class QMediaInputEncoderInterface;

class Q_MULTIMEDIA_EXPORT QAudioBufferSource : public QObject
{
    Q_OBJECT
Q_SIGNALS:
    void newAudioBuffer(const QAudioBuffer &buffer);
};

class Q_MULTIMEDIA_EXPORT QPlatformAudioBufferInput : public QAudioBufferSource
{
    Q_OBJECT
public:
    QPlatformAudioBufferInput(const QAudioFormat &format = {}) : m_format(format) { }

    const QAudioFormat &audioFormat() const { return m_format; }

    QMediaInputEncoderInterface *encoderInterface() const { return m_encoderInterface; }
    void setEncoderInterface(QMediaInputEncoderInterface *encoderInterface)
    {
        m_encoderInterface = encoderInterface;
    }

Q_SIGNALS:
    void encoderUpdated();

private:
    QMediaInputEncoderInterface *m_encoderInterface = nullptr;
    QAudioFormat m_format;
};

QT_END_NAMESPACE

#endif // QPLATFORMAUDIOBUFFERINPUT_P_H
