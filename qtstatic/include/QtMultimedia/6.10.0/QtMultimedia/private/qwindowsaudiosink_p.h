// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#ifndef QWINDOWSAUDIOSINK_H
#define QWINDOWSAUDIOSINK_H

#include <QtCore/qthread.h>
#include <QtCore/qtclasshelpermacros.h>
#include <QtCore/private/qcomptr_p.h>
#include <QtMultimedia/private/qaudiosystem_p.h>
#include <QtMultimedia/private/qaudiosystem_platform_stream_support_p.h>
#include <QtMultimedia/private/qaudio_platform_implementation_support_p.h>
#include <QtMultimedia/private/qwindowsaudioutils_p.h>

#include <atomic>
#include <memory>
#include <memory_resource>

struct IAudioRenderClient;

QT_BEGIN_NAMESPACE

class QWindowsResampler;

namespace QtWASAPI {

class QWindowsAudioSink;
using namespace QtMultimediaPrivate;

///////////////////////////////////////////////////////////////////////////////////////////////////

struct QWASAPIAudioSinkStream final : std::enable_shared_from_this<QWASAPIAudioSinkStream>,
                                      QPlatformAudioSinkStream
{
    using SampleFormat = QAudioFormat::SampleFormat;
    using SinkType = QWindowsAudioSink;

    enum class StreamType : uint8_t {
        Ringbuffer,
        Callback,
    };

    QWASAPIAudioSinkStream(QAudioDevice, const QAudioFormat &,
                           std::optional<qsizetype> ringbufferSize, QWindowsAudioSink *parent,
                           float volume, std::optional<int32_t> hardwareBufferSize,
                           AudioEndpointRole);
    Q_DISABLE_COPY_MOVE(QWASAPIAudioSinkStream)
    ~QWASAPIAudioSinkStream() = default;

    bool open();

    using QPlatformAudioSinkStream::bytesFree;
    using QPlatformAudioSinkStream::processedDuration;
    using QPlatformAudioSinkStream::ringbufferSizeInBytes;
    using QPlatformAudioSinkStream::setVolume;

    bool start(QIODevice *);
    QIODevice *start();
    bool start(AudioCallback);

    void suspend();
    void resume();
    void stop(ShutdownPolicy);

    void updateStreamIdle(bool) override;

private:
    bool openAudioClient(ComPtr<IMMDevice>, AudioEndpointRole);
    bool startAudioClient(StreamType);

    template <typename Functor>
    bool visitAudioClientBuffer(Functor &&f);

    void fillInitialHostBuffer();
    void runProcessRingbufferLoop();
    void runProcessCallbackLoop();
    bool processRingbuffer() noexcept QT_MM_NONBLOCKING;
    bool processCallback() noexcept QT_MM_NONBLOCKING;

    void handleAudioClientError();

    ComPtr<IAudioClient3> m_audioClient;
    ComPtr<IAudioRenderClient> m_renderClient;

    QWindowsAudioUtils::reference_time m_periodSize;
    qsizetype m_audioClientFrames;

    std::atomic_bool m_suspended{};
    std::atomic<ShutdownPolicy> m_shutdownPolicy{ ShutdownPolicy::DiscardRingbuffer };
    QAutoResetEvent m_ringbufferDrained;

    const AudioEndpointRole m_role;

    const QUniqueWin32NullHandle m_wasapiHandle;
    std::unique_ptr<QThread> m_workerThread;

    AudioCallback m_audioCallback;

    QWindowsAudioSink *m_parent;

    QAudioFormat m_hostFormat;
    std::unique_ptr<char[]> m_preallocatedBuffer;
    std::unique_ptr<std::pmr::memory_resource> m_memoryResource;
    std::unique_ptr<QWindowsResampler> m_resampler;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class QWindowsAudioSink final
    : public QPlatformAudioSinkImplementation<QWASAPIAudioSinkStream, QWindowsAudioSink>
{
    using BaseClass = QPlatformAudioSinkImplementation<QWASAPIAudioSinkStream, QWindowsAudioSink>;

public:
    QWindowsAudioSink(QAudioDevice, const QAudioFormat &, QObject *parent);
};

} // namespace QtWASAPI

QT_END_NAMESPACE

#endif // QWINDOWSAUDIOSINK_H
