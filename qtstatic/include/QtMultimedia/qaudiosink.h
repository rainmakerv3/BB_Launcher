// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only


#ifndef QAUDIOOUTPUT_H
#define QAUDIOOUTPUT_H

#include <QtCore/qiodevice.h>

#include <QtMultimedia/qtmultimediaglobal.h>

#include <QtMultimedia/qtaudio.h>
#include <QtMultimedia/qaudioformat.h>
#include <QtMultimedia/qaudiodevice.h>


QT_BEGIN_NAMESPACE



class QPlatformAudioSink;

class Q_MULTIMEDIA_EXPORT QAudioSink : public QObject
{
    Q_OBJECT

public:
    explicit QAudioSink(const QAudioFormat &format = QAudioFormat(), QObject *parent = nullptr);
    explicit QAudioSink(const QAudioDevice &audioDeviceInfo, const QAudioFormat &format = QAudioFormat(), QObject *parent = nullptr);
    ~QAudioSink() override;

    bool isNull() const { return !d; }

    QAudioFormat format() const;

    void start(QIODevice *device);
    QIODevice* start();

    void stop();
    void reset();
    void suspend();
    void resume();

    void setBufferSize(qsizetype bytes);
    qsizetype bufferSize() const;

    void setBufferFrameCount(qsizetype framesCount);
    qsizetype bufferFrameCount() const;

    qsizetype bytesFree() const;
    qsizetype framesFree() const;

    qint64 processedUSecs() const;
    qint64 elapsedUSecs() const;

    QtAudio::Error error() const;
    QtAudio::State state() const;

    void setVolume(qreal);
    qreal volume() const;

Q_SIGNALS:
#if defined(Q_QDOC)
    void stateChanged(QtAudio::State state);
#else
    // use QAudio here to keep string-based connections working
    void stateChanged(QAudio::State state);
#endif

private:
    Q_DISABLE_COPY(QAudioSink)

    friend class QPlatformAudioSink;
    QPlatformAudioSink *d;
};

QT_END_NAMESPACE

#endif // QAUDIOOUTPUT_H
