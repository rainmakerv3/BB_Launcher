// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-3.0-only

#ifndef QSPATIALAUDIOSOUNDSOURCE_P_H
#define QSPATIALAUDIOSOUNDSOURCE_P_H

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

#include <QtSpatialAudio/qspatialsound.h>
#include <QtSpatialAudio/private/qambientsound_p.h>
#include <QtSpatialAudio/private/qaudioengine_p.h>
#include <QtCore/qurl.h>
#include <QtCore/qmutex.h>
#include <QtGui/qquaternion.h>
#include <QtGui/qvector3d.h>
#include <QtMultimedia/qaudiobuffer.h>
#include <QtMultimedia/qaudiodevice.h>

QT_BEGIN_NAMESPACE

class QAudioDecoder;
class QAudioEnginePrivate;

class QSpatialSoundPrivate : public QAmbientSoundPrivate
{
    Q_DECLARE_PUBLIC(QSpatialSound)

public:
    QSpatialSoundPrivate() : QAmbientSoundPrivate(1) { }

    static QSpatialSoundPrivate *get(QSpatialSound *soundSource)
    {
        return soundSource ? soundSource->d_func() : nullptr;
    }

    QVector3D pos;
    QQuaternion rotation;
    QSpatialSound::DistanceModel distanceModel = QSpatialSound::DistanceModel::Logarithmic;
    float size = .1f;
    float distanceCutoff = 50.f;
    float manualAttenuation = 0.f;
    float occlusionIntensity = 0.f;
    float directivity = 0.f;
    float directivityOrder = 1.f;
    float nearFieldGain = 0.f;
    float wallDampening = 1.f;
    float wallOcclusion = 0.f;

    void updateDistanceModel();
    void updateRoomEffects();
};

QT_END_NAMESPACE

#endif
