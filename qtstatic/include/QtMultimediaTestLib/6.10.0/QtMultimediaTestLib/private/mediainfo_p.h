// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef MEDIAINFO_P_H
#define MEDIAINFO_P_H

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

#include <QtTest/QTest>
#include <QtMultimedia/qmediaplayer.h>
#include <QtMultimedia/qmediametadata.h>
#include <QtMultimedia/qaudiooutput.h>
#include <QtMultimedia/qaudiobufferoutput.h>
#include <QtMultimedia/qaudiobuffer.h>
#include <private/testvideosink_p.h>
#include <chrono>

QT_USE_NAMESPACE

using namespace std::chrono;

// Extracts media metadata from a input media file
struct MediaInfo
{
    static std::optional<MediaInfo> create(const QUrl &fileLocation, bool keepFrames = false)
    {
        QMediaPlayer player;
        const QSignalSpy mediaStatusChanged{ &player, &QMediaPlayer::mediaStatusChanged };

        QAudioBufferOutput audioOutput;
        player.setAudioBufferOutput(&audioOutput);

        TestVideoSink sink;
        player.setVideoSink(&sink);

        std::vector<std::array<QColor, 4>> colors;
        std::vector<QVideoFrame> frames;
        QObject::connect(&sink, &TestVideoSink::videoFrameChangedSync, &sink,
                         [&](const QVideoFrame &frame) {
                             if (keepFrames)
                                frames.push_back(frame);
                             if (frame.isValid())
                                 colors.push_back(sampleQuadrants(frame.toImage()));
                         });

        QByteArray audioData;
        QAudioFormat audioFormat;
        QObject::connect(&audioOutput, &QAudioBufferOutput::audioBufferReceived, &audioOutput,
                         [&](const QAudioBuffer &buffer) {
                             if (!buffer.isValid())
                                 return;
                             if (!audioFormat.isValid())
                                 audioFormat = buffer.format();
                             else
                                 QTEST_ASSERT(audioFormat == buffer.format());

                             audioData.append(buffer.data<const char>(), buffer.byteCount());
                         });

        player.setSource(fileLocation);

        // Loop through all frames to be able to count them
        player.setPlaybackRate(50); // let's speed it up
        player.play();

        const bool endReached = QTest::qWaitFor(
                [&] {
                    return mediaStatusChanged.contains(QList<QVariant>{ QMediaPlayer::EndOfMedia })
                            || mediaStatusChanged.contains(
                                    QList<QVariant>{ QMediaPlayer::InvalidMedia });
                },
                10min);

        if (!endReached)
            return {};

        MediaInfo info{};
        info.m_url = fileLocation.toString();
        info.m_frameRate = player.metaData().value(QMediaMetaData::VideoFrameRate).toReal();
        info.m_size = player.metaData().value(QMediaMetaData::Resolution).toSize();

        info.m_duration = milliseconds{ player.duration() };
        info.m_frameCount = sink.m_totalFrames - 1;
        info.m_frameTimes = sink.m_frameTimes;
        info.m_hasVideo = player.hasVideo();
        info.m_hasAudio = player.hasAudio();
        info.m_colors = std::move(colors);
        info.m_frames = std::move(frames);
        info.m_audioBuffer = QAudioBuffer(audioData, audioFormat);
        return info;
    }



    static std::array<QColor, 4> sampleQuadrants(const QImage &image)
    {
        const int width = image.width();
        const int height = image.height();
        return {
            image.pixel(width / 4, height / 4),
            image.pixel(3 * width / 4, height / 4),
            image.pixel(width / 4, 3 * height / 4),
            image.pixel(3 * width / 4, 3 * height / 4),
        };

    }

    QString m_url; // Visualize recorded file name when debugging.
    int m_frameCount = 0;
    qreal m_frameRate = 0.0f;
    QSize m_size;
    milliseconds m_duration;
    bool m_hasVideo = false;
    bool m_hasAudio = false;
    std::vector<QVideoFrame> m_frames; // Decoded video frames (optional)
    std::vector<std::array<QColor, 4>> m_colors; // Colors in upper left, upper right, bottom left, and bottom right
    QAudioBuffer m_audioBuffer;

    std::vector<TestVideoSink::TimePoint> m_frameTimes;
};

#endif
