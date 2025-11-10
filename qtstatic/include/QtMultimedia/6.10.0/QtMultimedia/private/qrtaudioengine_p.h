// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QRTAUDIOENGINE_P_H
#define QRTAUDIOENGINE_P_H

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

#include <QtCore/qtclasshelpermacros.h>
#include <QtCore/qtimer.h>
#include <QtCore/qmutex.h>

#include <QtMultimedia/qaudiosink.h>
#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtMultimedia/private/qaudio_rtsan_support_p.h>
#include <QtMultimedia/private/qaudioringbuffer_p.h>
#include <QtMultimedia/private/qautoresetevent_p.h>
#include <QtMultimedia/private/q_pmr_emulation_p.h>

#include <cstdint>
#include <deque>
#include <set>
#include <variant>
#include <vector>

QT_BEGIN_NAMESPACE

namespace QtMultimediaPrivate {

///////////////////////////////////////////////////////////////////////////////////////////////////

// ID to uniquely identify a voice
enum class VoiceId : uint64_t
{
};

enum class VoicePlayResult : uint8_t
{
    Playing,
    Finished,
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class Q_MULTIMEDIA_EXPORT QRtAudioEngineVoice
{
public:
    using VoicePlayResult = QtMultimediaPrivate::VoicePlayResult;
    using VoiceId = QtMultimediaPrivate::VoiceId;

    explicit QRtAudioEngineVoice(VoiceId id) : m_voiceId{ id } { }
    Q_DISABLE_COPY_MOVE(QRtAudioEngineVoice)
    virtual ~QRtAudioEngineVoice() = default;

    // once play() returns finished or isActive is false, the QAudioPlaybackEngine will stop the
    // voice
    [[nodiscard]] virtual VoicePlayResult play(QSpan<float>) noexcept QT_MM_NONBLOCKING = 0;
    virtual bool isActive() noexcept QT_MM_NONBLOCKING = 0;

    virtual const QAudioFormat &format() noexcept = 0;

    VoiceId voiceId() const { return m_voiceId; }

private:
    const VoiceId m_voiceId;
};

struct QRtAudioEngineVoiceCompare : std::less<uint64_t>
{
    using std::less<uint64_t>::operator();
    template <typename Lhs, typename Rhs>
    bool operator()(const Lhs &lhs, const Rhs &rhs) const
    {
        return operator()(getId(lhs), getId(rhs));
    }

    static uint64_t getId(VoiceId arg) { return qToUnderlying(arg); }
    static uint64_t getId(const QRtAudioEngineVoice &arg) { return getId(arg.voiceId()); }
    static uint64_t getId(const std::shared_ptr<QRtAudioEngineVoice> &arg) { return getId(*arg); }

    using is_transparent = std::true_type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace Impl {
template <typename T>
struct visitor_arg;

template <typename R, typename Arg>
struct visitor_arg<R(Arg)>
{
    using type = Arg;
};

template <typename R, typename Arg>
struct visitor_arg<R (*)(Arg)>
{
    using type = Arg;
};

template <typename F>
struct visitor_arg : visitor_arg<decltype(&F::operator())>
{
};

template <typename C, typename R, typename Arg>
struct visitor_arg<R (C::*)(Arg) const>
{
    using type = Arg;
};

template <typename C, typename R, typename Arg>
struct visitor_arg<R (C::*)(Arg)>
{
    using type = Arg;
};

} // namespace Impl

template <typename F>
using visitor_arg_t = typename Impl::visitor_arg<F>::type;

///////////////////////////////////////////////////////////////////////////////////////////////////

// playback engine for QRtAudioEngineVoice instances
// keeps a QAudioSink alive, but in a suspended state if no voices are playing
class Q_MULTIMEDIA_EXPORT QRtAudioEngine final : public QObject
{
public:
    using RtVoiceVisitor = std::function<void(QRtAudioEngineVoice &)>;
    using SharedVoice = std::shared_ptr<QRtAudioEngineVoice>;

    Q_OBJECT

    // commands (app->rt)
    struct PlayCommand
    {
        SharedVoice voice;
    };

    struct StopCommand
    {
        const VoiceId voiceId;
    };

    // visitors are sent back to the non-rt thread, so that they are destroyed in a safe context
    struct VisitCommand
    {
        const VoiceId voiceId;
        RtVoiceVisitor callback;
    };

    // "trivial" visitors are not sent back to the non-rt thread
    struct VisitCommandTrivial
    {
        const VoiceId voiceId;
        RtVoiceVisitor callback;
    };

    using RtCommand = std::variant<PlayCommand, StopCommand, VisitCommand, VisitCommandTrivial>;

    // notifications (rt->app)
    struct StopNotification
    {
        SharedVoice voice;
    };

    struct VisitReply
    {
        RtVoiceVisitor callback;
    };

    using Notification = std::variant<StopNotification, VisitReply>;

public:
    // we keep a pool of engines with one engine per device/format
    static std::shared_ptr<QRtAudioEngine> getEngineFor(const QAudioDevice &, const QAudioFormat &);

    QRtAudioEngine(const QAudioDevice &, const QAudioFormat &);
    Q_DISABLE_COPY_MOVE(QRtAudioEngine)
    ~QRtAudioEngine() override;

    // play/stop/visitVoiceRT are thread-safe
    void play(SharedVoice);
    void stop(const SharedVoice &);
    void stop(VoiceId);

    template <typename Visitor>
    void visitVoiceRt(VoiceId id, Visitor visitor)
    {
        using visitorArg = visitor_arg_t<Visitor>;
        static_assert(std::is_reference_v<visitorArg>);

        // we need to prevent that the visitor function is going to be destroyed on the real-time
        // thread, unless:
        // * it can be trivially destroyed
        // * it is sufficiently small to fit into the small-buffer-optimization of std::function.
        //   we don't know what the value is, so we are conservative and assume only the size of a
        //   pointer (we could relax it with something like std::inplace_function)
        constexpr size_t smallBufferOptimizationEstimate = 2 * sizeof(void *);
        constexpr bool visitorIsTrivial = std::is_trivially_destructible_v<std::decay_t<Visitor>>
                && sizeof(Visitor) <= smallBufferOptimizationEstimate;

        auto wrapped = [visitor = std::move(visitor)](QRtAudioEngineVoice &voice) {
            visitor(static_cast<visitorArg>(voice));
        };
        visitVoiceRt(id, RtVoiceVisitor{ wrapped }, visitorIsTrivial);
    }

    template <typename Visitor>
    void visitVoiceRt(const SharedVoice &voice, Visitor visitor)
    {
        visitVoiceRt(voice->voiceId(), std::move(visitor));
    }

    static VoiceId allocateVoiceId();

    std::unique_ptr<pmr::memory_resource> &rtMemoryResource() { return m_rtMemoryPool; }

    // testing
    QAudioSink &audioSink() { return m_sink; }
    const auto &voices() const { return m_voices; }

Q_SIGNALS:
    void voiceFinished(VoiceId);

private:
    void visitVoiceRt(VoiceId, RtVoiceVisitor, bool visitorIsTrivial);

    void audioCallback(QSpan<float>) noexcept QT_MM_NONBLOCKING;
    void cleanupRetiredVoices() noexcept QT_MM_NONBLOCKING;

    void runRtCommands() noexcept QT_MM_NONBLOCKING;
    void runRtCommand(PlayCommand) noexcept QT_MM_NONBLOCKING;
    void runRtCommand(StopCommand) noexcept QT_MM_NONBLOCKING;
    void runRtCommand(VisitCommand) noexcept QT_MM_NONBLOCKING;
    void runRtCommand(VisitCommandTrivial) noexcept QT_MM_NONBLOCKING;

    void runNonRtNotifications();
    void runNonRtNotification(StopNotification);
    void runNonRtNotification(VisitReply);

    QAudioSink m_sink;

    QMutex m_mutex;

    // Application side
    std::set<SharedVoice, QRtAudioEngineVoiceCompare> m_voices;

    // Rt memory
    // Note: when the memory pool is exhausted, we fall back to the system allocator. Not great for
    // real-time uses, but a simple fallback strategy
    static constexpr size_t poolSize = 128 * 1024; // 128kb
    std::unique_ptr<pmr::memory_resource> m_rtMemoryPool;

    // Voice registry on the real-time thread:
    // invariant: every voice in m_rtVoiceRegistry is also in m_voices
    using VoiceRegistry = std::set<SharedVoice, QRtAudioEngineVoiceCompare,
                                   pmr::polymorphic_allocator<SharedVoice>>;
    VoiceRegistry m_rtVoiceRegistry{
        m_rtMemoryPool.get(),
    };

    // rt/nrt communication
    static constexpr size_t commandBuffersSize = 2048;
    QtPrivate::QAudioRingBuffer<RtCommand> m_appToRt{ commandBuffersSize };
    QtPrivate::QAudioRingBuffer<Notification> m_rtToApp{ commandBuffersSize };
    std::deque<RtCommand> m_appToRtOverflowBuffer;
    std::deque<Notification, pmr::polymorphic_allocator<Notification>> m_rtToAppOverflowBuffer{
        m_rtMemoryPool.get(),
    };
    void sendAppToRtCommand(RtCommand cmd);
    bool sendRtToAppNotification(Notification cmd);
    void sendPendingRtCommands();
    bool sendPendingAppNotifications();
    QTimer m_pendingCommandsTimer;

    QtPrivate::QAutoResetEvent m_notificationEvent;
    std::vector<VoiceId> m_finishedVoices;
};

} // namespace QtMultimediaPrivate

QT_END_NAMESPACE

#endif // QRTAUDIOENGINE_P_H
