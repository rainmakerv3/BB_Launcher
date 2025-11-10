// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSOUNDEFFECT_P_H
#define QSOUNDEFFECT_P_H

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

#include <QtCore/qobject.h>
#include <QtCore/private/qobject_p.h>
#include <QtCore/qurl.h>
#include <QtCore/qloggingcategory.h>
#include <QtMultimedia/qaudio.h>
#include <QtMultimedia/qsoundeffect.h>
#include <QtMultimedia/qaudiodevice.h>
#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtMultimedia/private/qsamplecache_p.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(qLcSoundEffect)

class QSoundEffectPrivate : public QObjectPrivate
{
public:
    virtual bool setAudioDevice(QAudioDevice device) = 0;
    virtual QAudioDevice audioDevice() const = 0;
    virtual bool setSource(const QUrl &, QSampleCache &) = 0;
    virtual QUrl url() const = 0;

    virtual QSoundEffect::Status status() const = 0;

    virtual int loopCount() const = 0;
    virtual bool setLoopCount(int) = 0;
    virtual int loopsRemaining() const = 0;

    virtual float volume() const = 0;
    virtual bool setVolume(float) = 0;
    virtual bool muted() const = 0;
    virtual bool setMuted(bool) = 0;

    virtual void play() = 0;
    virtual void stop() = 0;
    virtual bool playing() const = 0;

    static Q_MULTIMEDIA_EXPORT QSoundEffectPrivate *get(QSoundEffect *);
};

QT_END_NAMESPACE

#endif // QSOUNDEFFECT_P_H
