// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-3.0-only

#ifndef QAMBIENTSOUND_P_H
#define QAMBIENTSOUND_P_H

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

#include <QtSpatialAudio/qambientsound.h>
#include <QtSpatialAudio/private/qtspatialaudioglobal_p.h>
#include <QtCore/qmutex.h>
#include <QtCore/qurl.h>
#include <QtCore/qfile.h>
#include <QtCore/private/qobject_p.h>
#include <QtMultimedia/qaudiodecoder.h>
#include <QtMultimedia/qaudiobuffer.h>

QT_BEGIN_NAMESPACE

class QAudioEngine;

class QAmbientSoundPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QAmbientSound)

public:
    explicit QAmbientSoundPrivate(int nchannels = 2) : nchannels(nchannels) { }

    template <typename T>
    static QAmbientSoundPrivate *get(T *soundSource)
    {
        return soundSource ? soundSource->d_func() : nullptr;
    }

    QUrl url;
    float volume = 1.;
    int nchannels = 2;
    std::unique_ptr<QAudioDecoder> decoder;
    std::unique_ptr<QFile> sourceDeviceFile;
    QAudioEngine *engine = nullptr;

    QMutex mutex;
    int currentBuffer = 0;
    int bufPos = 0;
    int m_currentLoop = 0;
    QList<QAudioBuffer> buffers;
    int sourceId = -1; // kInvalidSourceId

    QAtomicInteger<bool> m_autoPlay = true;
    QAtomicInteger<bool> m_playing = false;
    QAtomicInt m_loops = 1;
    bool m_loading = false;

    void play() {
        m_playing = true;
    }
    void pause() {
        m_playing = false;
    }
    void stop() {
        QMutexLocker locker(&mutex);
        m_playing = false;
        currentBuffer = 0;
        bufPos = 0;
        m_currentLoop = 0;
    }

    void load();
    void getBuffer(float *buf, int frames, int channels);
};

QT_END_NAMESPACE

#endif // QAMBIENTSOUND_P_H
