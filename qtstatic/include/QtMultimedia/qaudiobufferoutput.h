// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIOBUFFEROUTPUT_H
#define QAUDIOBUFFEROUTPUT_H

#include <QtMultimedia/qtmultimediaexports.h>
#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QAudioFormat;
class QAudioBuffer;
class QAudioBufferOutputPrivate;

class Q_MULTIMEDIA_EXPORT QAudioBufferOutput : public QObject
{
    Q_OBJECT
public:
    explicit QAudioBufferOutput(QObject *parent = nullptr);

    explicit QAudioBufferOutput(const QAudioFormat &format, QObject *parent = nullptr);

    ~QAudioBufferOutput() override;

    QAudioFormat format() const;

Q_SIGNALS:
    void audioBufferReceived(const QAudioBuffer &buffer);

private:
    Q_DISABLE_COPY(QAudioBufferOutput)
    Q_DECLARE_PRIVATE(QAudioBufferOutput)
};

QT_END_NAMESPACE

#endif // QAUDIOBUFFEROUTPUT_H
