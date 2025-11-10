// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-3.0-only

#ifndef QAUDIOENGINE_H
#define QAUDIOENGINE_H

#include <QtSpatialAudio/qtspatialaudioglobal.h>
#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QAudioEnginePrivate;
class QAudioDevice;

class Q_SPATIALAUDIO_EXPORT QAudioEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(OutputMode outputMode READ outputMode WRITE setOutputMode NOTIFY outputModeChanged)
    Q_PROPERTY(QAudioDevice outputDevice READ outputDevice WRITE setOutputDevice NOTIFY outputDeviceChanged)
    Q_PROPERTY(float masterVolume READ masterVolume WRITE setMasterVolume NOTIFY masterVolumeChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(float distanceScale READ distanceScale WRITE setDistanceScale NOTIFY distanceScaleChanged)
public:
    QAudioEngine() : QAudioEngine(nullptr) {};
    explicit QAudioEngine(QObject *parent) : QAudioEngine(44100, parent) {}
    explicit QAudioEngine(int sampleRate, QObject *parent = nullptr);
    ~QAudioEngine() override;

    enum OutputMode {
        Surround,
        Stereo,
        Headphone
    };
    Q_ENUM(OutputMode)

    void setOutputMode(OutputMode mode);
    OutputMode outputMode() const;

    int sampleRate() const;

    void setOutputDevice(const QAudioDevice &device);
    QAudioDevice outputDevice() const;

    void setMasterVolume(float volume);
    float masterVolume() const;

    void setPaused(bool paused);
    bool paused() const;

    void setRoomEffectsEnabled(bool enabled);
    bool roomEffectsEnabled() const;

    static constexpr float DistanceScaleCentimeter = 1.f;
    static constexpr float DistanceScaleMeter = 100.f;

    void setDistanceScale(float scale);
    float distanceScale() const;

Q_SIGNALS:
    void outputModeChanged();
    void outputDeviceChanged();
    void masterVolumeChanged();
    void pausedChanged();
    void distanceScaleChanged();

public Q_SLOTS:
    void start();
    void stop();

    void pause() { setPaused(true); }
    void resume() { setPaused(false); }

private:
    friend class QAudioEnginePrivate;
    QAudioEnginePrivate *d;
};

QT_END_NAMESPACE

#endif
