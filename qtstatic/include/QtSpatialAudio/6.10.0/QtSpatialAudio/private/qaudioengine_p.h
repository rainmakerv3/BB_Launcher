// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-3.0-only

#ifndef QAUDIOENGINE_P_H
#define QAUDIOENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtSpatialAudio/qaudioengine.h>
#include <QtSpatialAudio/private/qtspatialaudioglobal_p.h>
#include <QtMultimedia/qaudiodevice.h>
#include <QtCore/qthread.h>
#include <QtCore/qtclasshelpermacros.h>
#include <QtCore/qmutex.h>

namespace vraudio {
class ResonanceAudio;
} // namespace vraudio

QT_BEGIN_NAMESPACE

class QSpatialSound;
class QAmbientSound;
class QAudioOutputStream;
class QAudioRoom;
class QAudioListener;
class QAudioEngine;

class QAudioEnginePrivate
{
public:
    static QAudioEnginePrivate *get(QAudioEngine *engine) { return engine ? engine->d : nullptr; }

    static constexpr int bufferSize = 128;

    explicit QAudioEnginePrivate(QAudioEngine *);
    Q_DISABLE_COPY_MOVE(QAudioEnginePrivate)
    ~QAudioEnginePrivate();

    std::unique_ptr<vraudio::ResonanceAudio> resonanceAudio;
    int sampleRate = 44100;
    float masterVolume = 1.;
    QAudioEngine::OutputMode outputMode = QAudioEngine::Surround;
    bool roomEffectsEnabled = true;

    void start();
    void stop();
    void setPaused(bool paused);
    void setOutputDevice(const QAudioDevice &device);
    void setOutputMode(QAudioEngine::OutputMode);

    // Resonance Audio uses meters internally, while Qt Quick 3D and our API uses cm by
    // default. To make things independent from the scale setting, we store all distances in
    // meters internally and convert in the setters and getters.
    float distanceScale = 0.01f;

    QMutex mutex;
    QAudioDevice device;
    QAtomicInteger<bool> paused = false;

    QThread audioThread;
    std::unique_ptr<QAudioOutputStream> outputStream;

    QAudioListener *listener = nullptr;
    QList<QSpatialSound *> sources;
    QList<QAmbientSound *> stereoSources;
    QList<QAudioRoom *> rooms;
    mutable bool listenerPositionDirty = true;
    QAudioRoom *currentRoom = nullptr;

    void addSpatialSound(QSpatialSound *sound);
    void removeSpatialSound(QSpatialSound *sound);
    void addStereoSound(QAmbientSound *sound);
    void removeStereoSound(QAmbientSound *sound);

    void addRoom(QAudioRoom *room);
    void removeRoom(QAudioRoom *room);
    void updateRooms();

    QVector3D listenerPosition() const;
    QAudioEngine *q;
};

QT_END_NAMESPACE

#endif
