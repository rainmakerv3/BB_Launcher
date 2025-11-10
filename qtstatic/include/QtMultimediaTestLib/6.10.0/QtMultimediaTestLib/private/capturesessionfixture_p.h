// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef CAPTURESESSIONFIXTURE_P_H
#define CAPTURESESSIONFIXTURE_P_H

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

#include <private/framegenerator_p.h>
#include <QtMultimedia/qvideoframeinput.h>
#include <QtMultimedia/qaudioinput.h>
#include <QtMultimedia/qmediacapturesession.h>
#include <QtMultimedia/qmediarecorder.h>
#include <QtMultimedia/qaudiobufferinput.h>
#include <QtCore/qtemporaryfile.h>

#include <private/testvideosink_p.h>
#include <QtTest/qsignalspy.h>

QT_BEGIN_NAMESPACE

enum class StreamType { Audio, Video, AudioAndVideo };
enum class AutoStop { EmitEmpty, No };
enum class RunMode { Pull, Push };

struct CaptureSessionFixture
{
    explicit CaptureSessionFixture(StreamType streamType);
    ~CaptureSessionFixture();

    void setVideoSink(QVideoSink *videoSink);
    void start(RunMode mode, AutoStop autoStop);
    bool waitForRecorderStopped(std::chrono::milliseconds duration);
    bool hasAudio() const;
    bool hasVideo() const;

    VideoGenerator m_videoGenerator;
    AudioGenerator m_audioGenerator;
    QVideoFrameInput m_videoInput;
    QAudioBufferInput m_audioInput;
    QMediaCaptureSession m_session;
    QMediaRecorder m_recorder;
    QTemporaryFile m_tempFile;
    QVideoSink *m_videoSink = nullptr;
    StreamType m_streamType = StreamType::Video;

    QSignalSpy readyToSendVideoFrame{ &m_videoInput, &QVideoFrameInput::readyToSendVideoFrame };
    QSignalSpy readyToSendAudioBuffer{ &m_audioInput, &QAudioBufferInput::readyToSendAudioBuffer };
    QSignalSpy recorderStateChanged{ &m_recorder, &QMediaRecorder::recorderStateChanged };
    QSignalSpy mediaFormatChanged{ &m_recorder, &QMediaRecorder::mediaFormatChanged };
};

QT_END_NAMESPACE

#endif
