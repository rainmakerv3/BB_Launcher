// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef TESTVIDEOSINK_H
#define TESTVIDEOSINK_H

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

#include <qvideosink.h>
#include <qvideoframe.h>
#include <qelapsedtimer.h>
#include <qsignalspy.h>
#include <chrono>

QT_BEGIN_NAMESPACE

/*
    This is a simple video surface which records all presented frames.
*/
class TestVideoSink : public QVideoSink
{
    Q_OBJECT
public:
    explicit TestVideoSink(bool storeFrames = false) : m_storeFrames(storeFrames)
    {
        connect(this, &QVideoSink::videoFrameChanged, this, &TestVideoSink::addVideoFrame);
        connect(this, &QVideoSink::videoFrameChanged, this, &TestVideoSink::videoFrameChangedSync);
    }

    QVideoFrame waitForFrame()
    {
        QSignalSpy spy(this, &TestVideoSink::videoFrameChangedSync);
        return spy.wait() ? spy.at(0).at(0).value<QVideoFrame>() : QVideoFrame{};
    }

    void setStoreFrames(bool storeFrames = true) { m_storeFrames = storeFrames; }

private Q_SLOTS:
    void addVideoFrame(const QVideoFrame &frame)
    {
        m_elapsedTimer.start();

        if (m_storeFrames)
            m_frameList.append(frame);

        if (frame.isValid())
            m_frameTimes.emplace_back(std::chrono::microseconds(frame.startTime()));

        ++m_totalFrames;
    }

signals:
    void videoFrameChangedSync(const QVideoFrame &frame);

public:
    QList<QVideoFrame> m_frameList;
    int m_totalFrames = 0; // used instead of the list when frames are not stored
    QElapsedTimer m_elapsedTimer;
    using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
    std::vector<TimePoint> m_frameTimes;

private:
    bool m_storeFrames;
};

QT_END_NAMESPACE

#endif // TESTVIDEOSINK_H
