// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMEDIAPLAYER_H
#define QMEDIAPLAYER_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtMultimedia/qtaudio.h>

QT_BEGIN_NAMESPACE

class QVideoSink;
class QAudioOutput;
class QAudioDevice;
class QMediaMetaData;
class QMediaTimeRange;
class QAudioBufferOutput;
class QPlaybackOptions;

class QMediaPlayerPrivate;
class Q_MULTIMEDIA_EXPORT QMediaPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(qint64 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(float bufferProgress READ bufferProgress NOTIFY bufferProgressChanged)
    Q_PROPERTY(bool hasAudio READ hasAudio NOTIFY hasAudioChanged)
    Q_PROPERTY(bool hasVideo READ hasVideo NOTIFY hasVideoChanged)
    Q_PROPERTY(bool seekable READ isSeekable NOTIFY seekableChanged)
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(qreal playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(int loops READ loops WRITE setLoops NOTIFY loopsChanged)
    Q_PROPERTY(PlaybackState playbackState READ playbackState NOTIFY playbackStateChanged)
    Q_PROPERTY(MediaStatus mediaStatus READ mediaStatus NOTIFY mediaStatusChanged)
    Q_PROPERTY(QMediaMetaData metaData READ metaData NOTIFY metaDataChanged)
    Q_PROPERTY(Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)
    Q_PROPERTY(QObject *videoOutput READ videoOutput WRITE setVideoOutput NOTIFY videoOutputChanged)
    Q_PROPERTY(QAudioOutput *audioOutput READ audioOutput WRITE setAudioOutput NOTIFY
                       audioOutputChanged)
    Q_REVISION(6, 8)
    Q_PROPERTY(QAudioBufferOutput *audioBufferOutput READ audioBufferOutput WRITE
                       setAudioBufferOutput NOTIFY audioBufferOutputChanged)
    Q_PROPERTY(QList<QMediaMetaData> audioTracks READ audioTracks NOTIFY tracksChanged)
    Q_PROPERTY(QList<QMediaMetaData> videoTracks READ videoTracks NOTIFY tracksChanged)
    Q_PROPERTY(QList<QMediaMetaData> subtitleTracks READ subtitleTracks NOTIFY tracksChanged)

    Q_PROPERTY(int activeAudioTrack READ activeAudioTrack WRITE setActiveAudioTrack NOTIFY
                       activeTracksChanged)
    Q_PROPERTY(int activeVideoTrack READ activeVideoTrack WRITE setActiveVideoTrack NOTIFY
                       activeTracksChanged)
    Q_PROPERTY(int activeSubtitleTrack READ activeSubtitleTrack WRITE setActiveSubtitleTrack NOTIFY
                       activeTracksChanged)

    Q_REVISION(6, 10)
    Q_PROPERTY(PitchCompensationAvailability pitchCompensationAvailability READ
                       pitchCompensationAvailability CONSTANT)

    Q_REVISION(6, 10)
    Q_PROPERTY(bool pitchCompensation READ pitchCompensation WRITE setPitchCompensation NOTIFY
                       pitchCompensationChanged)

    Q_REVISION(6, 10)
    Q_PROPERTY(QPlaybackOptions playbackOptions READ playbackOptions WRITE setPlaybackOptions NOTIFY
                       playbackOptionsChanged RESET resetPlaybackOptions)

public:
    enum PlaybackState
    {
        StoppedState,
        PlayingState,
        PausedState
    };
    Q_ENUM(PlaybackState)

    enum MediaStatus
    {
        NoMedia,
        LoadingMedia,
        LoadedMedia,
        StalledMedia,
        BufferingMedia,
        BufferedMedia,
        EndOfMedia,
        InvalidMedia
    };
    Q_ENUM(MediaStatus)

    enum Error
    {
        NoError,
        ResourceError,
        FormatError,
        NetworkError,
        AccessDeniedError
    };
    Q_ENUM(Error)

    enum Loops
    {
        Infinite = -1,
        Once = 1
    };
    Q_ENUM(Loops)

    enum class PitchCompensationAvailability
    {
        AlwaysOn,
        Available,
        Unavailable,
    };
    Q_ENUM(PitchCompensationAvailability)

    explicit QMediaPlayer(QObject *parent = nullptr);
    ~QMediaPlayer() override;

    QList<QMediaMetaData> audioTracks() const;
    QList<QMediaMetaData> videoTracks() const;
    QList<QMediaMetaData> subtitleTracks() const;

    int activeAudioTrack() const;
    int activeVideoTrack() const;
    int activeSubtitleTrack() const;

    void setActiveAudioTrack(int index);
    void setActiveVideoTrack(int index);
    void setActiveSubtitleTrack(int index);

    void setAudioBufferOutput(QAudioBufferOutput *output);
    QAudioBufferOutput *audioBufferOutput() const;

    void setAudioOutput(QAudioOutput *output);
    QAudioOutput *audioOutput() const;

    void setVideoOutput(QObject *);
    QObject *videoOutput() const;

    void setVideoSink(QVideoSink *sink);
    QVideoSink *videoSink() const;

    QUrl source() const;
    const QIODevice *sourceDevice() const;

    PlaybackState playbackState() const;
    MediaStatus mediaStatus() const;

    qint64 duration() const;
    qint64 position() const;

    bool hasAudio() const;
    bool hasVideo() const;

    float bufferProgress() const;
    QMediaTimeRange bufferedTimeRange() const;

    bool isSeekable() const;
    qreal playbackRate() const;

    bool isPlaying() const;

    int loops() const;
    void setLoops(int loops);

    Error error() const;
    QString errorString() const;

    bool isAvailable() const;
    QMediaMetaData metaData() const;

    PitchCompensationAvailability pitchCompensationAvailability() const;
    bool pitchCompensation() const;

    QPlaybackOptions playbackOptions() const;

public Q_SLOTS:
    void play();
    void pause();
    void stop();

    void setPosition(qint64 position);

    void setPlaybackRate(qreal rate);

    void setSource(const QUrl &source);
    void setSourceDevice(QIODevice *device, const QUrl &sourceUrl = QUrl());

    void setPitchCompensation(bool) const;

    void setPlaybackOptions(const QPlaybackOptions &options);
    void resetPlaybackOptions();

Q_SIGNALS:
    void sourceChanged(const QUrl &media);
    void playbackStateChanged(QMediaPlayer::PlaybackState newState);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);

    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);

    void hasAudioChanged(bool available);
    void hasVideoChanged(bool videoAvailable);

    void bufferProgressChanged(float progress);

    void seekableChanged(bool seekable);
    void playingChanged(bool playing);
    void playbackRateChanged(qreal rate);
    void loopsChanged();

    void metaDataChanged();
    void videoOutputChanged();
    void audioOutputChanged();
    Q_REVISION(6, 8) void audioBufferOutputChanged();

    void tracksChanged();
    void activeTracksChanged();

    void errorChanged();
    void errorOccurred(QMediaPlayer::Error error, const QString &errorString);

    Q_REVISION(6, 10)
    void pitchCompensationChanged(bool);

    Q_REVISION(6, 10)
    void playbackOptionsChanged();

private:
    Q_DISABLE_COPY(QMediaPlayer)
    Q_DECLARE_PRIVATE(QMediaPlayer)
    friend class QPlatformMediaPlayer;
};

QT_END_NAMESPACE

#endif  // QMEDIAPLAYER_H
