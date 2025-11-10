// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QMEDIAPLAYERCONTROL_H
#define QMEDIAPLAYERCONTROL_H

#include <QtMultimedia/qmediaplayer.h>
#include <QtMultimedia/qmediatimerange.h>
#include <QtMultimedia/qaudiodevice.h>
#include <QtMultimedia/qmediametadata.h>

#include <QtCore/private/qglobal_p.h>
#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QMediaStreamsControl;
class QPlatformAudioOutput;

class Q_MULTIMEDIA_EXPORT QPlatformMediaPlayer
{
public:
    virtual ~QPlatformMediaPlayer();
    virtual QMediaPlayer::PlaybackState state() const { return m_state; }
    virtual QMediaPlayer::MediaStatus mediaStatus() const { return m_status; };

    virtual qint64 duration() const = 0;

    virtual qint64 position() const { return m_position; }
    virtual void setPosition(qint64 position) = 0;

    virtual float bufferProgress() const = 0;

    virtual bool isAudioAvailable() const { return m_audioAvailable; }
    virtual bool isVideoAvailable() const { return m_videoAvailable; }

    virtual bool isSeekable() const { return m_seekable; }

    virtual QMediaTimeRange availablePlaybackRanges() const = 0;

    virtual qreal playbackRate() const = 0;
    virtual void setPlaybackRate(qreal rate) = 0;

    virtual QUrl media() const = 0;
    virtual const QIODevice *mediaStream() const = 0;
    virtual void setMedia(const QUrl &media, QIODevice *stream) = 0;

    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

    virtual bool streamPlaybackSupported() const { return false; }

    virtual void setAudioOutput(QPlatformAudioOutput *) {}

    virtual void setAudioBufferOutput(QAudioBufferOutput *) { }

    virtual QMediaMetaData metaData() const { return {}; }

    virtual void setVideoSink(QVideoSink * /*sink*/) = 0;

    virtual bool canPlayQrc() const { return false; }

    // media streams
    enum TrackType : uint8_t { VideoStream, AudioStream, SubtitleStream, NTrackTypes };

    virtual int trackCount(TrackType) { return 0; };
    virtual QMediaMetaData trackMetaData(TrackType /*type*/, int /*streamNumber*/) { return QMediaMetaData(); }
    virtual int activeTrack(TrackType) { return -1; }
    virtual void setActiveTrack(TrackType, int /*streamNumber*/) {}

    void durationChanged(std::chrono::milliseconds ms) { durationChanged(ms.count()); }
    void durationChanged(qint64 duration) { emit player->durationChanged(duration); }
    void positionChanged(std::chrono::milliseconds ms) { positionChanged(ms.count()); }
    void positionChanged(qint64 position) {
        if (m_position == position)
            return;
        m_position = position;
        emit player->positionChanged(position);
    }
    void audioAvailableChanged(bool audioAvailable) {
        if (m_audioAvailable == audioAvailable)
            return;
        m_audioAvailable = audioAvailable;
        emit player->hasAudioChanged(audioAvailable);
    }
    void videoAvailableChanged(bool videoAvailable) {
        if (m_videoAvailable == videoAvailable)
            return;
        m_videoAvailable = videoAvailable;
        emit player->hasVideoChanged(videoAvailable);
    }
    void seekableChanged(bool seekable) {
        if (m_seekable == seekable)
            return;
        m_seekable = seekable;
        emit player->seekableChanged(seekable);
    }
    void playbackRateChanged(qreal rate) { emit player->playbackRateChanged(rate); }
    void bufferProgressChanged(float progress) { emit player->bufferProgressChanged(progress); }
    void metaDataChanged() { emit player->metaDataChanged(); }
    void tracksChanged() { emit player->tracksChanged(); }
    void activeTracksChanged() { emit player->activeTracksChanged(); }

    void stateChanged(QMediaPlayer::PlaybackState newState);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void error(QMediaPlayer::Error, const QString &errorString);

    void resetCurrentLoop() { m_currentLoop = 0; }
    bool doLoop() {
        return isSeekable() && (m_loops < 0 || ++m_currentLoop < m_loops);
    }
    int loops() const { return m_loops; }
    virtual void setLoops(int loops)
    {
        if (m_loops == loops)
            return;
        m_loops = loops;
        Q_EMIT player->loopsChanged();
    }

    using PitchCompensationAvailability = QMediaPlayer::PitchCompensationAvailability;

    virtual PitchCompensationAvailability pitchCompensationAvailability() const;
    virtual void setPitchCompensation(bool enabled);
    virtual bool pitchCompensation() const;
    void pitchCompensationChanged(bool enabled) const;

    QPlaybackOptions playbackOptions() const;

    bool qmediaplayerDestructorCalled = false;

protected:
    explicit QPlatformMediaPlayer(QMediaPlayer *parent = nullptr);

private:
    QMediaPlayer *player = nullptr;
    QMediaPlayer::MediaStatus m_status = QMediaPlayer::NoMedia;
    QMediaPlayer::PlaybackState m_state = QMediaPlayer::StoppedState;
    bool m_seekable = false;
    bool m_videoAvailable = false;
    bool m_audioAvailable = false;
    int m_loops = 1;
    int m_currentLoop = 0;
    qint64 m_position = 0;
};

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator<<(QDebug dbg, QPlatformMediaPlayer::TrackType type)
{
    QDebugStateSaver save(dbg);
    dbg.nospace();

    switch (type) {
    case QPlatformMediaPlayer::TrackType::AudioStream:
        return dbg << "AudioStream";
    case QPlatformMediaPlayer::TrackType::VideoStream:
        return dbg << "VideoStream";
    case QPlatformMediaPlayer::TrackType::SubtitleStream:
        return dbg << "SubtitleStream";
    default:
        Q_UNREACHABLE_RETURN(dbg);
    }
}
#endif

QT_END_NAMESPACE


#endif  // QMEDIAPLAYERCONTROL_H

