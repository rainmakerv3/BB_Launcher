// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSOUNDEFFECTWITHPLAYER_P_H
#define QSOUNDEFFECTWITHPLAYER_P_H

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

#include <QtMultimedia/qaudiosink.h>
#include <QtMultimedia/qmediadevices.h>
#include <QtMultimedia/private/qaudiosystem_p.h>
#include <QtMultimedia/private/qautoresetevent_p.h>
#include <QtMultimedia/private/qrtaudioengine_p.h>
#include <QtMultimedia/private/qsoundeffect_p.h>

QT_BEGIN_NAMESPACE

namespace QtMultimediaPrivate {

using QtPrivate::QAutoResetEvent;

class QSoundEffectVoice final : public QRtAudioEngineVoice
{
public:
    QSoundEffectVoice(VoiceId voiceId, std::shared_ptr<const QSample> sample, float volume,
                      bool muted, int totalLoopCount);

    VoicePlayResult play(QSpan<float>) noexcept QT_MM_NONBLOCKING override;
    bool isActive() noexcept QT_MM_NONBLOCKING override;
    const QAudioFormat &format() noexcept override { return m_sample->format(); }

    int loopsRemaining() const { return m_loopsRemaining.load(std::memory_order_relaxed); }
    std::shared_ptr<QSoundEffectVoice> clone() const;

    const std::shared_ptr<const QSample> m_sample;
    const int m_totalFrames{
        m_sample->format().framesForBytes(m_sample->data().size()),
    };

    float m_volume{};
    bool m_muted{};

    std::atomic_int m_loopsRemaining;
    int m_currentFrame{};

    QAutoResetEvent m_currentLoopChanged;
};

// Design notes
// * each play() will start a new voice
// * stop() will stop all voices
// * the most recently created voice can be obtained via `activeVoice()`
// * mute/volume will affect all voices
// * loopCount/loopRemaining will be obtained from most recently created voice
class QSoundEffectPrivateWithPlayer final : public QObject, public QSoundEffectPrivate
{
public:
    QSoundEffectPrivateWithPlayer(QSoundEffect *q, QAudioDevice audioDevice);
    Q_DISABLE_COPY_MOVE(QSoundEffectPrivateWithPlayer)
    ~QSoundEffectPrivateWithPlayer() override;

    // QSoundEffectPrivate interface
    bool setAudioDevice(QAudioDevice device) override;
    QAudioDevice audioDevice() const override;
    bool setSource(const QUrl &, QSampleCache &) override;
    QUrl url() const override;
    QSoundEffect::Status status() const override;
    int loopCount() const override;
    bool setLoopCount(int) override;
    int loopsRemaining() const override;
    float volume() const override;
    bool setVolume(float) override;
    bool muted() const override;
    bool setMuted(bool) override;
    void play() override;
    void stop() override;
    bool playing() const override;

private:
    void play(std::shared_ptr<QSoundEffectVoice>);
    void setStatus(QSoundEffect::Status status);
    [[nodiscard]] bool updatePlayer();
    std::optional<VoiceId> activeVoice() const;
    static bool formatIsSupported(const QAudioFormat &);
    void setResolvedAudioDevice(QAudioDevice device);
    void resolveAudioDevice();

    QSoundEffect *q_ptr{};
    QAudioDevice m_audioDevice;
    QAudioDevice m_resolvedAudioDevice;
    std::shared_ptr<QRtAudioEngine> m_player;
    QMetaObject::Connection m_voiceFinishedConnection;
    std::set<std::shared_ptr<QSoundEffectVoice>, QRtAudioEngineVoiceCompare> m_voices;

    void setLoopsRemaining(int);
    int m_loopsRemaining{ 0 };

    QFuture<void> m_sampleLoadFuture;
    QUrl m_url;
    SharedSamplePtr m_sample;
    float m_volume = 1.f;
    bool m_muted = false;
    int m_loopCount = 1;
    bool m_playPending = false;

    QSoundEffect::Status m_status{};

    QMediaDevices m_mediaDevices;
    QAudioDevice m_defaultAudioDevice;
};

} // namespace QtMultimediaPrivate

QT_END_NAMESPACE

#endif // QSOUNDEFFECTWITHPLAYER_P_H
