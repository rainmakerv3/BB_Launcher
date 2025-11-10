// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIO_PLATFORM_IMPLEMENTATION_SUPPORT_P_H
#define QAUDIO_PLATFORM_IMPLEMENTATION_SUPPORT_P_H

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

#include <QtMultimedia/private/qaudiosystem_p.h>
#include <QtMultimedia/private/qaudiosystem_platform_stream_support_p.h>

QT_BEGIN_NAMESPACE

namespace QtMultimediaPrivate {

///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cpp_concepts
// clang-format off
template <typename T>
concept QPlatformSinkStream = requires(T t, QIODevice *device,
                                       QPlatformAudioIOStream::ShutdownPolicy shutdownPolicy,
                                       AudioSinkCallback callback)
{
    requires std::constructible_from<
        T,
        QAudioDevice,
        const QAudioFormat&,
        std::optional<qsizetype>,
        typename T::SinkType*,
        float,
        std::optional<int32_t>,
        AudioEndpointRole
    >;

    { t.open() } -> std::same_as<bool>;

    { t.start() } -> std::same_as<QIODevice *>;
    { t.start(device) } -> std::same_as<bool>;
    { t.start(std::move(callback)) } -> std::same_as<bool>;

    { t.suspend() } -> std::same_as<void>;
    { t.resume() } -> std::same_as<void>;
    { t.stop(shutdownPolicy) } -> std::same_as<void>;

    { t.setVolume(0.0f) } -> std::same_as<void>;
    { t.bytesFree() } -> std::same_as<quint64>;

    { t.processedDuration() } -> std::same_as<std::chrono::microseconds>;
};

// clang-format on
#  define STREAM_TYPE_ARG QPlatformSinkStream StreamType
#else
#  define STREAM_TYPE_ARG typename StreamType
#endif

template <STREAM_TYPE_ARG, typename DerivedType = void>
class QPlatformAudioSinkImplementation : public QPlatformAudioSink
{
public:
    QPlatformAudioSinkImplementation(QAudioDevice device, const QAudioFormat &format,
                                     QObject *parent);
    ~QPlatformAudioSinkImplementation() override;

    void start(QIODevice *device) override;
    void start(AudioCallback &&) override;
    QIODevice *start() override;

    void stop() override final;
    void reset() override;

    void suspend() override;
    void resume() override;

    qsizetype bytesFree() const override;
    void setBufferSize(qsizetype value) override;
    qsizetype bufferSize() const override;
    void setHardwareBufferFrames(int32_t) override;
    int32_t hardwareBufferFrames() override;
    qint64 processedUSecs() const override;

    void setVolume(float volume) override;
    bool hasCallbackAPI() override;
    void setRole(AudioEndpointRole role) override;

protected:
    friend class QtMultimediaPrivate::QPlatformAudioSinkStream;
    friend StreamType;
    using ShutdownPolicy = QPlatformAudioIOStream::ShutdownPolicy;

    std::optional<int> m_internalBufferSize;
    std::optional<int32_t> m_hardwareBufferFrames;
    AudioEndpointRole m_role = AudioEndpointRole::Other;

    std::shared_ptr<StreamType> m_stream;

    using ConcreteSinkType = std::conditional_t<std::is_same_v<DerivedType, void>,
                                                QPlatformAudioSinkImplementation, DerivedType>;
};

template <STREAM_TYPE_ARG, typename DerivedType>
QPlatformAudioSinkImplementation<StreamType, DerivedType>::QPlatformAudioSinkImplementation(
        QAudioDevice device, const QAudioFormat &format, QObject *parent)
    : QPlatformAudioSink(std::move(device), format, parent)
{
}

template <STREAM_TYPE_ARG, typename DerivedType>
QPlatformAudioSinkImplementation<StreamType, DerivedType>::~QPlatformAudioSinkImplementation()
{
    stop();
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSinkImplementation<StreamType, DerivedType>::start(QIODevice *device)
{
    if (!device) {
        setError(QAudio::IOError);
        return;
    }

    if (m_stream) {
        qWarning("QAudioSink::start() called while already started");
        return;
    }

    m_stream = std::make_shared<StreamType>(m_audioDevice, m_format, m_internalBufferSize,
                                            static_cast<ConcreteSinkType *>(this), volume(),
                                            m_hardwareBufferFrames, m_role);

    if (!m_stream->open()) {
        setError(QAudio::OpenError);
        m_stream = {};
        return;
    }

    m_stream->start(device);
    updateStreamState(QAudio::ActiveState);
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSinkImplementation<StreamType, DerivedType>::start(AudioCallback &&audioCallback)
{
    using namespace QtMultimediaPrivate;
    if (!validateAudioCallback(audioCallback, m_format)) {
        setError(QAudio::OpenError);
        return;
    }

    if (m_stream) {
        qWarning("QAudioSink::start() called while already started");
        return;
    }

    m_stream = std::make_shared<StreamType>(m_audioDevice, m_format, m_internalBufferSize,
                                            static_cast<ConcreteSinkType *>(this), volume(),
                                            m_hardwareBufferFrames, m_role);

    if (!m_stream->open()) {
        setError(QAudio::OpenError);
        m_stream = {};
        return;
    }

    bool started = m_stream->start(std::move(audioCallback));
    if (!started) {
        setError(QAudio::OpenError);
        m_stream = {};
        return;
    }

    updateStreamState(QAudio::ActiveState);
}

template <STREAM_TYPE_ARG, typename DerivedType>
QIODevice *QPlatformAudioSinkImplementation<StreamType, DerivedType>::start()
{
    if (m_stream) {
        qWarning("QAudioSink::start() called while already started");
        return nullptr;
    }

    m_stream = std::make_shared<StreamType>(m_audioDevice, m_format, m_internalBufferSize,
                                            static_cast<ConcreteSinkType *>(this), volume(),
                                            m_hardwareBufferFrames, m_role);

    if (!m_stream->open()) {
        setError(QAudio::OpenError);
        m_stream = {};
        return nullptr;
    }

    QIODevice *device = m_stream->start();
    QObject::connect(device, &QIODevice::readyRead, this, [this] {
        updateStreamIdle(false);
    });
    updateStreamIdle(true);
    updateStreamState(QAudio::ActiveState);
    return device;
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSinkImplementation<StreamType, DerivedType>::stop()
{
    if (m_stream) {
        m_stream->stop(ShutdownPolicy::DrainRingbuffer);
        m_stream = {};
        updateStreamState(QAudio::StoppedState);
    }
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSinkImplementation<StreamType, DerivedType>::reset()
{
    if (m_stream) {
        m_stream->stop(ShutdownPolicy::DiscardRingbuffer);
        m_stream = {};
        updateStreamState(QAudio::StoppedState);
    }
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSinkImplementation<StreamType, DerivedType>::suspend()
{
    if (m_stream) {
        m_stream->suspend();
        updateStreamState(QAudio::SuspendedState);
    }
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSinkImplementation<StreamType, DerivedType>::resume()
{
    if (m_stream) {
        updateStreamState(QAudio::ActiveState);
        m_stream->resume();
    }
}

template <STREAM_TYPE_ARG, typename DerivedType>
qsizetype QPlatformAudioSinkImplementation<StreamType, DerivedType>::bytesFree() const
{
    if (m_stream)
        return m_stream->bytesFree();
    return 0;
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSinkImplementation<StreamType, DerivedType>::setBufferSize(qsizetype value)
{
    m_internalBufferSize = value;
}

template <STREAM_TYPE_ARG, typename DerivedType>
qsizetype QPlatformAudioSinkImplementation<StreamType, DerivedType>::bufferSize() const
{
    if (m_stream)
        return m_stream->ringbufferSizeInBytes();

    return QPlatformAudioIOStream::inferRingbufferBytes(m_internalBufferSize,
                                                        m_hardwareBufferFrames, m_format);
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSinkImplementation<StreamType, DerivedType>::setHardwareBufferFrames(int32_t arg)
{
    if (arg > 0)
        m_hardwareBufferFrames = arg;
    else
        m_hardwareBufferFrames = std::nullopt;
}

template <STREAM_TYPE_ARG, typename DerivedType>
int32_t QPlatformAudioSinkImplementation<StreamType, DerivedType>::hardwareBufferFrames()
{
    return m_hardwareBufferFrames.value_or(-1);
}

template <STREAM_TYPE_ARG, typename DerivedType>
qint64 QPlatformAudioSinkImplementation<StreamType, DerivedType>::processedUSecs() const
{
    if (m_stream)
        return m_stream->processedDuration().count();

    return 0;
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSinkImplementation<StreamType, DerivedType>::setVolume(float volume)
{
    QPlatformAudioEndpointBase::setVolume(volume);
    if (m_stream)
        m_stream->setVolume(volume);
}

template <STREAM_TYPE_ARG, typename DerivedType>
bool QPlatformAudioSinkImplementation<StreamType, DerivedType>::hasCallbackAPI()
{
    return true;
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSinkImplementation<StreamType, DerivedType>::setRole(AudioEndpointRole role)
{
    m_role = role;
}

#undef STREAM_TYPE_ARG

///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cpp_concepts
// clang-format off
template <typename T>
concept QPlatformSourceStream = requires(T t, QIODevice *device,
                                         QPlatformAudioIOStream::ShutdownPolicy shutdownPolicy)
{
    requires std::constructible_from<
        T,
        QAudioDevice,
        const QAudioFormat&,
        std::optional<qsizetype>,
        typename T::SourceType*,
        float,
        std::optional<int32_t>
    >;

    { t.open() } -> std::same_as<bool>;

    { t.start() } -> std::same_as<QIODevice *>;
    { t.start(device) } -> std::same_as<bool>;

    { t.suspend() } -> std::same_as<void>;
    { t.resume() } -> std::same_as<void>;
    { t.stop(shutdownPolicy) } -> std::same_as<void>;

    { t.setVolume(0.0f) } -> std::same_as<void>;
    { t.bytesReady() } -> std::same_as<qsizetype>;
    { t.deviceIsRingbufferReader() } -> std::same_as<bool>;

    { t.processedDuration() } -> std::same_as<std::chrono::microseconds>;
};

// clang-format on
#  define STREAM_TYPE_ARG QPlatformSourceStream StreamType
#else
#  define STREAM_TYPE_ARG typename StreamType
#endif

template <STREAM_TYPE_ARG, typename DerivedType = void>
class QPlatformAudioSourceImplementation : public QPlatformAudioSource
{
public:
    QPlatformAudioSourceImplementation(QAudioDevice device, const QAudioFormat &format,
                                       QObject *parent);
    ~QPlatformAudioSourceImplementation() override;

    void start(QIODevice *device) override;
    QIODevice *start() override;

    void stop() override final;
    void reset() override;

    void suspend() override;
    void resume() override;

    qsizetype bytesReady() const override;
    void setBufferSize(qsizetype value) override;
    qsizetype bufferSize() const override;
    void setHardwareBufferFrames(int32_t) override;
    int32_t hardwareBufferFrames() override;
    qint64 processedUSecs() const override;

    void setVolume(float volume) override;

protected:
    friend class QtMultimediaPrivate::QPlatformAudioSourceStream;
    friend StreamType;
    using ShutdownPolicy = QPlatformAudioIOStream::ShutdownPolicy;

    std::optional<int> m_internalBufferSize;
    std::optional<int32_t> m_hardwareBufferFrames;

    std::shared_ptr<StreamType> m_stream;
    std::shared_ptr<StreamType> m_retiredStream;

    using ConcreteSourceType = std::conditional_t<std::is_same_v<DerivedType, void>,
                                                  QPlatformAudioSourceImplementation, DerivedType>;
};

template <STREAM_TYPE_ARG, typename DerivedType>
QPlatformAudioSourceImplementation<StreamType, DerivedType>::QPlatformAudioSourceImplementation(
        QAudioDevice device, const QAudioFormat &format, QObject *parent)
    : QPlatformAudioSource(std::move(device), format, parent)
{
}

template <STREAM_TYPE_ARG, typename DerivedType>
QPlatformAudioSourceImplementation<StreamType, DerivedType>::~QPlatformAudioSourceImplementation()
{
    stop();
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSourceImplementation<StreamType, DerivedType>::start(QIODevice *device)
{
    if (!device) {
        setError(QAudio::IOError);
        return;
    }

    if (m_stream) {
        qWarning("QAudioSource::start() called while already started");
        return;
    }

    m_stream = std::make_shared<StreamType>(m_audioDevice, m_format, m_internalBufferSize,
                                            static_cast<ConcreteSourceType *>(this), volume(),
                                            m_hardwareBufferFrames);

    if (!m_stream->open()) {
        setError(QAudio::OpenError);
        m_stream = {};
        return;
    }

    m_stream->start(device);
    updateStreamState(QAudio::ActiveState);
}

template <STREAM_TYPE_ARG, typename DerivedType>
QIODevice *QPlatformAudioSourceImplementation<StreamType, DerivedType>::start()
{
    if (m_stream) {
        qWarning("QAudioSource::start() called while already started");
        return nullptr;
    }

    m_stream = std::make_shared<StreamType>(m_audioDevice, m_format, m_internalBufferSize,
                                            static_cast<ConcreteSourceType *>(this), volume(),
                                            m_hardwareBufferFrames);

    if (!m_stream->open()) {
        setError(QAudio::OpenError);
        m_stream = {};
        return nullptr;
    }

    QIODevice *device = m_stream->start();
    QObject::connect(device, &QIODevice::readyRead, this, [this] {
        updateStreamIdle(false);
    });
    updateStreamIdle(true, EmitStateSignal::False);
    updateStreamState(QAudio::ActiveState);
    return device;
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSourceImplementation<StreamType, DerivedType>::stop()
{
    if (!m_stream)
        return;

    if (m_stream->deviceIsRingbufferReader())
        // we own the qiodevice, so let's keep it alive to allow users to drain the ringbuffer
        m_retiredStream = m_stream;

    m_stream->stop(ShutdownPolicy::DrainRingbuffer);
    m_stream = {};
    updateStreamState(QAudio::StoppedState);
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSourceImplementation<StreamType, DerivedType>::reset()
{
    m_retiredStream = {};

    if (!m_stream)
        return;

    m_stream->stop(ShutdownPolicy::DiscardRingbuffer);
    m_stream = {};
    updateStreamState(QAudio::StoppedState);
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSourceImplementation<StreamType, DerivedType>::suspend()
{
    if (m_stream) {
        m_stream->suspend();
        updateStreamState(QAudio::SuspendedState);
    }
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSourceImplementation<StreamType, DerivedType>::resume()
{
    if (m_stream) {
        updateStreamState(QAudio::ActiveState);
        m_stream->resume();
    }
}

template <STREAM_TYPE_ARG, typename DerivedType>
qsizetype QPlatformAudioSourceImplementation<StreamType, DerivedType>::bytesReady() const
{
    return m_stream ? m_stream->bytesReady() : 0;
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSourceImplementation<StreamType, DerivedType>::setBufferSize(qsizetype value)
{
    m_internalBufferSize = value;
}

template <STREAM_TYPE_ARG, typename DerivedType>
qsizetype QPlatformAudioSourceImplementation<StreamType, DerivedType>::bufferSize() const
{
    if (m_stream)
        return m_stream->ringbufferSizeInBytes();

    return QPlatformAudioIOStream::inferRingbufferBytes(m_internalBufferSize,
                                                        m_hardwareBufferFrames, m_format);
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSourceImplementation<StreamType, DerivedType>::setHardwareBufferFrames(
        int32_t arg)
{
    if (arg > 0)
        m_hardwareBufferFrames = arg;
    else
        m_hardwareBufferFrames = std::nullopt;
}

template <STREAM_TYPE_ARG, typename DerivedType>
int32_t QPlatformAudioSourceImplementation<StreamType, DerivedType>::hardwareBufferFrames()
{
    return m_hardwareBufferFrames.value_or(-1);
}

template <STREAM_TYPE_ARG, typename DerivedType>
qint64 QPlatformAudioSourceImplementation<StreamType, DerivedType>::processedUSecs() const
{
    return m_stream ? m_stream->processedDuration().count() : 0;
}

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSourceImplementation<StreamType, DerivedType>::setVolume(float volume)
{
    QPlatformAudioEndpointBase::setVolume(volume);
    if (m_stream)
        m_stream->setVolume(volume);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <STREAM_TYPE_ARG, typename DerivedType = void>
class QPlatformAudioSourceImplementationWithCallback
    : public QPlatformAudioSourceImplementation<StreamType, DerivedType>
{
protected:
    using BaseClass = QPlatformAudioSourceImplementation<StreamType, DerivedType>;
    using BaseClass::BaseClass;
    using BaseClass::start;

    QT_WARNING_PUSH
    QT_WARNING_DISABLE_CLANG("-Woverloaded-virtual")
    void start(QPlatformAudioSource::AudioCallback &&) override;
    QT_WARNING_POP
    bool hasCallbackAPI() override { return true; }
};

template <STREAM_TYPE_ARG, typename DerivedType>
void QPlatformAudioSourceImplementationWithCallback<StreamType, DerivedType>::start(
        QPlatformAudioSource::AudioCallback &&audioCallback)
{
    using namespace QtMultimediaPrivate;
    if (!validateAudioCallback(audioCallback, BaseClass::m_format)) {
        BaseClass::setError(QAudio::OpenError);
        return;
    }

    BaseClass::m_stream = std::make_shared<StreamType>(
            BaseClass::m_audioDevice, BaseClass::m_format, BaseClass::m_internalBufferSize,
            static_cast<typename BaseClass::ConcreteSourceType *>(this), BaseClass::volume(),
            BaseClass::m_hardwareBufferFrames);

    if (!BaseClass::m_stream->open()) {
        BaseClass::setError(QAudio::OpenError);
        BaseClass::m_stream = {};
        return;
    }

    bool started = BaseClass::m_stream->start(std::move(audioCallback));
    if (!started) {
        BaseClass::setError(QAudio::OpenError);
        BaseClass::m_stream = {};
        return;
    }

    BaseClass::updateStreamState(QAudio::ActiveState);
}

#undef STREAM_TYPE_ARG

} // namespace QtMultimediaPrivate

QT_END_NAMESPACE

#endif // QAUDIO_PLATFORM_IMPLEMENTATION_SUPPORT_P_H
