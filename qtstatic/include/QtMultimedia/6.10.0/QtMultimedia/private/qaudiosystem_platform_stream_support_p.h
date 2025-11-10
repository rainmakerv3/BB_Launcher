// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIOSYSTEM_PLATFORM_STREAM_SUPPORT_P_H
#define QAUDIOSYSTEM_PLATFORM_STREAM_SUPPORT_P_H

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

#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtMultimedia/qaudioformat.h>
#include <QtMultimedia/qaudiodevice.h>
#include <QtMultimedia/private/qautoresetevent_p.h>
#include <QtMultimedia/private/qaudio_qiodevice_support_p.h>
#include <QtMultimedia/private/qaudio_rtsan_support_p.h>
#include <QtMultimedia/private/qaudiosystem_p.h>
#include <QtMultimedia/private/qaudiohelpers_p.h>
#include <QtMultimedia/private/qaudioringbuffer_p.h>
#include <QtCore/qscopedvaluerollback.h>
#include <QtCore/qthread.h>

#include <optional>
#include <variant>

QT_BEGIN_NAMESPACE

namespace QtPrivate {
class QIODeviceRingBufferWriterBase;
}

namespace QtMultimediaPrivate {

class QPlatformAudioIOStream
{
    template <typename T>
    using QAudioRingBuffer = QtPrivate::QAudioRingBuffer<T>;

    using Ringbuffer = std::variant<QAudioRingBuffer<float>, QAudioRingBuffer<int32_t>,
                                    QAudioRingBuffer<int16_t>, QAudioRingBuffer<uint8_t>>;

public:
    static qsizetype inferRingbufferFrames(const std::optional<int> &ringbufferSize,
                                           const std::optional<int32_t> &hardwareBufferFrames,
                                           const QAudioFormat &);
    static qsizetype inferRingbufferBytes(const std::optional<int> &ringbufferSize,
                                          const std::optional<int32_t> &hardwareBufferFrames,
                                          const QAudioFormat &);

protected:
    using NativeSampleFormat = QAudioHelperInternal::NativeSampleFormat;
    using QAutoResetEvent = QtPrivate::QAutoResetEvent;

    QPlatformAudioIOStream(QAudioDevice m_audioDevice, QAudioFormat m_format,
                           std::optional<int> ringbufferSize,
                           std::optional<int32_t> hardwareBufferFrames, float volume);
    ~QPlatformAudioIOStream();
    Q_DISABLE_COPY_MOVE(QPlatformAudioIOStream)

    void setVolume(float);
    float volume() const { return m_volume.load(std::memory_order_relaxed); }

    template <typename Functor>
    auto visitRingbuffer(Functor &&f)
    {
        return std::visit(f, m_ringbuffer);
    }

    template <typename Functor>
    auto visitRingbuffer(Functor &&f) const
    {
        return std::visit(f, m_ringbuffer);
    }

    void prepareRingbuffer(std::optional<int> ringbufferSize);
    int ringbufferSizeInBytes();

    // stop requests
    void requestStop();
    bool isStopRequested(std::memory_order memory_order = std::memory_order_relaxed) const
    {
        return m_stopRequested.load(memory_order);
    }

    // members
    const QAudioDevice m_audioDevice;
    const QAudioFormat m_format;
    const std::optional<int32_t> m_hardwareBufferFrames;

private:
    std::atomic<float> m_volume{
        1.f,
    };

    Ringbuffer m_ringbuffer{
        std::in_place_type_t<QAudioRingBuffer<float>>{},
        0,
    };

    // stop requests
    std::atomic<bool> m_stopRequested = false;

public:
    enum class ShutdownPolicy : uint8_t
    {
        DrainRingbuffer,
        DiscardRingbuffer,
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class QPlatformAudioSinkStream : protected QPlatformAudioIOStream
{
public:
    using QPlatformAudioIOStream::ShutdownPolicy;
    using AudioCallback = QPlatformAudioSink::AudioCallback;

protected:
    QPlatformAudioSinkStream(QAudioDevice, const QAudioFormat &, std::optional<int> ringbufferSize,
                             std::optional<int32_t> hardwareBufferFrames, float volume);
    ~QPlatformAudioSinkStream();
    Q_DISABLE_COPY_MOVE(QPlatformAudioSinkStream)

    uint64_t process(QSpan<std::byte> hostBuffer, qsizetype totalNumberOfFrames,
                     std::optional<NativeSampleFormat> = {}) noexcept QT_MM_NONBLOCKING;

    // ringbuffer / stats
    quint64 bytesFree() const;
    std::chrono::microseconds processedDuration() const;

    // downstream delegates
    virtual void updateStreamIdle(bool) = 0;

    // iodevice
    QIODevice *createRingbufferWriterDevice();
    void setQIODevice(QIODevice *device);
    void createQIODeviceConnections(QIODevice *device);
    void disconnectQIODeviceConnections();
    void pullFromQIODevice();

    // LATER: do we want to relax notifying the app thread?
    static constexpr int notificationThresholdBytes = 0;

    // idle detection
    void setIdleState(bool);
    bool isIdle(std::memory_order order = std::memory_order_relaxed) const
    {
        return m_streamIsIdle.load(order);
    }
    void stopIdleDetection();

    template <typename Functor>
    auto connectIdleHandler(Functor &&f)
    {
        return m_streamIdleDetectionNotifier.callOnActivated(std::forward<Functor>(f));
    }

    template <typename ParentType>
    void handleIOError(ParentType *parent)
    {
        if (parent) {
            Q_ASSERT(thread()->isCurrentThread());
            parent->setError(QAudio::IOError);
            parent->updateStreamState(QtAudio::State::StoppedState);

            parent->m_stream = {};
        }
    }

    QThread *thread() const;

    template <typename Functor>
    void invokeOnAppThread(Functor &&f)
    {
        // note: this is not a QObject, so we use the first QObject member of the stream as context
        QMetaObject::invokeMethod(&m_streamIdleDetectionNotifier, std::forward<Functor>(f));
    }

private:
    // qiodevice
    QIODevice *m_device = nullptr;

    // idle detection
    std::atomic<bool> m_streamIsIdle = false;
    QAutoResetEvent m_streamIdleDetectionNotifier;
    QMetaObject::Connection m_streamIdleDetectionConnection;

    // ringbuffer events
    QAutoResetEvent m_ringbufferHasSpace;
    QMetaObject::Connection m_ringbufferHasSpaceConnection;
    QMetaObject::Connection m_iodeviceHasNewDataConnection;

    std::unique_ptr<QtPrivate::QIODeviceRingBufferWriterBase> m_ringbufferWriterDevice;

    // stats
    std::atomic_int64_t m_totalFrameCount{};
    std::atomic_int64_t m_processedFrameCount{};

    void convertToNative(QSpan<const std::byte> internal, QSpan<std::byte> native, float volume,
                         NativeSampleFormat) noexcept QT_MM_NONBLOCKING;

    // pullFromQIODeviceToRingbuffer is not reentrant. however we might end up in situations where a
    // QIODevice emits readReady from within QIODevice::readData. We protect against this using a
    // reentrancy guard and queue invocations if we detect a reentrant call
    template <typename Functor>
    void withPullIODeviceReentrancyGuard(Functor f)
    {
        if (!m_pullIODeviceReentrancyGuard) {
            QScopedValueRollback<bool> guard{
                m_pullIODeviceReentrancyGuard,
                true,
            };
            f();
        } else {
            QMetaObject::invokeMethod(&m_streamIdleDetectionNotifier,
                                      [this, f = std::move(f)]() mutable {
                withPullIODeviceReentrancyGuard(std::move(f));
            }, Qt::QueuedConnection);
        }
    }
    bool m_pullIODeviceReentrancyGuard = false;

    void pullFromQIODeviceImpl();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class QPlatformAudioSourceStream : protected QPlatformAudioIOStream
{
public:
    using QPlatformAudioIOStream::ShutdownPolicy;
    using AudioCallback = QPlatformAudioSource::AudioCallback;

protected:
    QPlatformAudioSourceStream(QAudioDevice, const QAudioFormat &,
                               std::optional<int> ringbufferSize,
                               std::optional<int32_t> hardwareBufferFrames, float volume);
    ~QPlatformAudioSourceStream();

    Q_DISABLE_COPY_MOVE(QPlatformAudioSourceStream)

    uint64_t process(QSpan<const std::byte> hostBuffer, qsizetype numberOfFrames,
                     std::optional<NativeSampleFormat> = {}) noexcept QT_MM_NONBLOCKING;

    // ringbuffer / stats
    qsizetype bytesReady() const;
    std::chrono::microseconds processedDuration() const;

    // iodevice
    void setQIODevice(QIODevice *device);
    void createQIODeviceConnections(QIODevice *device);
    void disconnectQIODeviceConnections();
    QIODevice *createRingbufferReaderDevice();
    void pushToIODevice();
    bool deviceIsRingbufferReader() const;
    void finalizeQIODevice(ShutdownPolicy);
    void emptyRingbuffer();

    // downstream delegates
    virtual void updateStreamIdle(bool) = 0;

    template <typename ParentType>
    void handleIOError(ParentType *parent)
    {
        if (parent) {
            Q_ASSERT(thread()->isCurrentThread());
            parent->setError(QAudio::IOError);
            parent->updateStreamState(QtAudio::State::StoppedState);

            if (deviceIsRingbufferReader())
                // we own the qiodevice, so let's keep it alive to allow users to drain the
                // ringbuffer
                parent->m_retiredStream = std::move(parent->m_stream);
            else
                parent->m_stream = {};
        }
    }

    QThread *thread() const;

    template <typename Functor>
    void invokeOnAppThread(Functor &&f)
    {
        // note: this is not a QObject, so we use the first QObject member of the stream as context
        QMetaObject::invokeMethod(&m_ringbufferHasData, std::forward<Functor>(f));
    }

private:
    // qiodevice
    QIODevice *m_device = nullptr;
    std::unique_ptr<QIODevice> m_ringbufferReaderDevice;

    // ringbuffer events
    QAutoResetEvent m_ringbufferHasData;
    QAutoResetEvent m_ringbufferIsFull;

    QMetaObject::Connection m_ringbufferHasDataConnection;
    QMetaObject::Connection m_ringbufferIsFullConnection;

    // stats
    std::atomic_uint64_t m_totalNumberOfFramesPushedToRingbuffer{};

    void convertFromNative(QSpan<const std::byte> native, QSpan<std::byte> internal, float volume,
                           NativeSampleFormat) noexcept QT_MM_NONBLOCKING;
};

} // namespace QtMultimediaPrivate

QT_END_NAMESPACE

#endif // QAUDIOSYSTEM_PLATFORM_STREAM_SUPPORT_P_H
