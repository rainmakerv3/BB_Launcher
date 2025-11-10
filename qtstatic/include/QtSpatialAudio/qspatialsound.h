// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-3.0-only
#ifndef QSPATIALSOUND_H
#define QSPATIALSOUND_H

#include <QtSpatialAudio/qtspatialaudioglobal.h>
#include <QtCore/QObject>
#include <QtGui/qvector3d.h>
#include <QtGui/qquaternion.h>

QT_BEGIN_NAMESPACE

class QAudioEngine;
class QAmbientSoundPrivate;

class QSpatialSoundPrivate;
class Q_SPATIALAUDIO_EXPORT QSpatialSound : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(DistanceModel distanceModel READ distanceModel WRITE setDistanceModel NOTIFY distanceModelChanged)
    Q_PROPERTY(float size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(float distanceCutoff READ distanceCutoff WRITE setDistanceCutoff NOTIFY distanceCutoffChanged)
    Q_PROPERTY(float manualAttenuation READ manualAttenuation WRITE setManualAttenuation NOTIFY manualAttenuationChanged)
    Q_PROPERTY(float occlusionIntensity READ occlusionIntensity WRITE setOcclusionIntensity NOTIFY occlusionIntensityChanged)
    Q_PROPERTY(float directivity READ directivity WRITE setDirectivity NOTIFY directivityChanged)
    Q_PROPERTY(float directivityOrder READ directivityOrder WRITE setDirectivityOrder NOTIFY directivityOrderChanged)
    Q_PROPERTY(float nearFieldGain READ nearFieldGain WRITE setNearFieldGain NOTIFY nearFieldGainChanged)
    Q_PROPERTY(int loops READ loops WRITE setLoops NOTIFY loopsChanged)
    Q_PROPERTY(bool autoPlay READ autoPlay WRITE setAutoPlay NOTIFY autoPlayChanged)

public:
    explicit QSpatialSound(QAudioEngine *engine);
    ~QSpatialSound() override;

    void setSource(const QUrl &url);
    QUrl source() const;

    enum Loops
    {
        Infinite = -1,
        Once = 1
    };
    Q_ENUM(Loops)

    int loops() const;
    void setLoops(int loops);

    bool autoPlay() const;
    void setAutoPlay(bool autoPlay);

    void setPosition(QVector3D pos);
    QVector3D position() const;

    void setRotation(const QQuaternion &q);
    QQuaternion rotation() const;

    void setVolume(float volume);
    float volume() const;

    enum class DistanceModel {
        Logarithmic,
        Linear,
        ManualAttenuation
    };
    Q_ENUM(DistanceModel);

    void setDistanceModel(DistanceModel model);
    DistanceModel distanceModel() const;

    void setSize(float size);
    float size() const;

    void setDistanceCutoff(float cutoff);
    float distanceCutoff() const;

    void setManualAttenuation(float attenuation);
    float manualAttenuation() const;

    void setOcclusionIntensity(float occlusion);
    float occlusionIntensity() const;

    void setDirectivity(float alpha);
    float directivity() const;

    void setDirectivityOrder(float alpha);
    float directivityOrder() const;

    void setNearFieldGain(float gain);
    float nearFieldGain() const;

    QAudioEngine *engine() const;

Q_SIGNALS:
    void sourceChanged();
    void loopsChanged();
    void autoPlayChanged();
    void positionChanged();
    void rotationChanged();
    void volumeChanged();
    void distanceModelChanged();
    void sizeChanged();
    void distanceCutoffChanged();
    void manualAttenuationChanged();
    void occlusionIntensityChanged();
    void directivityChanged();
    void directivityOrderChanged();
    void nearFieldGainChanged();

public Q_SLOTS:
    void play();
    void pause();
    void stop();

private:
    void setEngine(QAudioEngine *engine);
    Q_DECLARE_PRIVATE(QSpatialSound)
    friend class QAmbientSoundPrivate;

    // ### Qt7: remove unused member
    QT6_ONLY(void *unused = nullptr;) // for ABI compatibility
};

QT_END_NAMESPACE

#endif
