// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMEDIAPLAYER_P_H
#define QMEDIAPLAYER_P_H

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

#include "qmediaplayer.h"
#include "qmediametadata.h"
#include "qvideosink.h"
#include "qaudiooutput.h"
#include "qaudiobufferoutput.h"
#include "qplaybackoptions.h"
#include <private/qplatformmediaplayer_p.h>
#include <private/qerrorinfo_p.h>

#include "private/qobject_p.h"
#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtCore/qurl.h>
#include <QtCore/qfile.h>
#include <QtCore/qtimer.h>

#include <memory>

QT_BEGIN_NAMESPACE

class QPlatformMediaPlayer;

class QMediaPlayerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QMediaPlayer)

public:
    static QMediaPlayerPrivate *get(QMediaPlayer *session)
    {
        return reinterpret_cast<QMediaPlayerPrivate *>(QObjectPrivate::get(session));
    }

    QMediaPlayerPrivate() = default;
    QPlatformMediaPlayer *control = nullptr;

    QPointer<QAudioBufferOutput> audioBufferOutput;
    QPointer<QAudioOutput> audioOutput;
    QPointer<QVideoSink> videoSink;
    QPointer<QObject> videoOutput;
    QUrl qrcMedia;
    std::unique_ptr<QFile> qrcFile;
    QUrl source;
    QIODevice *stream = nullptr;
    QPlaybackOptions playbackOptions;

    QMediaPlayer::PlaybackState state = QMediaPlayer::StoppedState;
    QErrorInfo<QMediaPlayer::Error> error;

    void setMedia(const QUrl &media, QIODevice *stream = nullptr);

    QList<QMediaMetaData> trackMetaData(QPlatformMediaPlayer::TrackType s) const;

    void setState(QMediaPlayer::PlaybackState state);
    void setStatus(QMediaPlayer::MediaStatus status);
    void setError(QMediaPlayer::Error error, const QString &errorString);

    void setVideoSink(QVideoSink *sink)
    {
        Q_Q(QMediaPlayer);
        if (sink == videoSink)
            return;
        if (videoSink)
            videoSink->setSource(nullptr);
        videoSink = sink;
        if (sink)
            sink->setSource(q);
        if (control)
            control->setVideoSink(sink);
        emit q->videoOutputChanged();
    }
};

QT_END_NAMESPACE

#endif
