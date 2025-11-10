// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSOUNDEFFECTSYNCHRONOUS_P_H
#define QSOUNDEFFECTSYNCHRONOUS_P_H

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

#include <QtCore/qiodevice.h>
#include <QtMultimedia/qaudiobuffer.h>
#include <QtMultimedia/qaudiosink.h>
#include <QtMultimedia/private/qsoundeffect_p.h>

QT_BEGIN_NAMESPACE

class Q_MULTIMEDIA_EXPORT QSoundEffectPrivateSynchronous : public QIODevice,
                                                           public QSoundEffectPrivate
{
    struct AudioSinkDeleter
    {
        void operator()(QAudioSink *sink) const;
    };

public:
    QSoundEffectPrivateSynchronous(QSoundEffect *q, const QAudioDevice &audioDevice);
    ~QSoundEffectPrivateSynchronous() override;

    qint64 readData(char *data, qint64 len) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 size() const override;
    qint64 bytesAvailable() const override;
    bool isSequential() const override;
    bool atEnd() const override;

    void setLoopsRemaining(int loopsRemaining);
    void setStatus(QSoundEffect::Status status);
    void setPlaying(bool playing);
    bool updateAudioOutput();

    void decoderError();
    void sampleReady(SharedSamplePtr);

    int loopCount() const override;
    bool setLoopCount(int loopCount) override;

    int loopsRemaining() const override;
    float volume() const override;
    bool setVolume(float volume) override;

    bool muted() const override;
    bool setMuted(bool muted) override;

    void play() override;

    void stop() override;
    bool playing() const override;

    bool setAudioDevice(QAudioDevice device) override;

    bool setSource(const QUrl &url, QSampleCache &sampleCache) override;

    QSoundEffect::Status status() const override;
    QUrl url() const override;
    QAudioDevice audioDevice() const override;

private:
    void stateChanged(QAudio::State);

    QSoundEffect *q_ptr;
    QAudioDevice m_audioDevice;

    int m_loopCount = 1;
    int m_runningCount = 0;
    bool m_playing = false;
    std::unique_ptr<QAudioSink, AudioSinkDeleter> m_audioSink;
    SharedSamplePtr m_sample;
    QAudioBuffer m_audioBuffer;
    bool m_muted = false;
    float m_volume = 1.0;
    qint64 m_offset = 0;

    QFuture<void> m_sampleLoadFuture;
    QUrl m_url;
    QSoundEffect::Status m_status = QSoundEffect::Null;
    bool m_sampleReady = false;
};

QT_END_NAMESPACE

#endif // QSOUNDEFFECTSYNCHRONOUS_P_H
