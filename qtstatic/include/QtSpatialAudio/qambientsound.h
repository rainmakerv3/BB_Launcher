// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-3.0-only
#ifndef QAMBIENTSOUND_H
#define QAMBIENTSOUND_H

#include <QtSpatialAudio/qtspatialaudioglobal.h>
#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtCore/QUrl>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QAudioEngine;
class QAmbientSoundPrivate;

class Q_SPATIALAUDIO_EXPORT QAmbientSound : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(int loops READ loops WRITE setLoops NOTIFY loopsChanged)
    Q_PROPERTY(bool autoPlay READ autoPlay WRITE setAutoPlay NOTIFY autoPlayChanged)

public:
    explicit QAmbientSound(QAudioEngine *engine);
    ~QAmbientSound() override;

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

    void setVolume(float volume);
    float volume() const;

    QAudioEngine *engine() const;

Q_SIGNALS:
    void sourceChanged();
    void loopsChanged();
    void autoPlayChanged();
    void volumeChanged();

public Q_SLOTS:
    void play();
    void pause();
    void stop();

private:
    void setEngine(QAudioEngine *engine);

    Q_DECLARE_PRIVATE(QAmbientSound)

    // ### Qt7: remove unused member
    QT6_ONLY(Q_DECL_UNUSED_MEMBER void *unused = nullptr;) // for ABI compatibility
};

QT_END_NAMESPACE

#endif
