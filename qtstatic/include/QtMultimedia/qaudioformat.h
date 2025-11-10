// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only


#ifndef QAUDIOFORMAT_H
#define QAUDIOFORMAT_H

#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>

#include <QtMultimedia/qtmultimediaglobal.h>

QT_BEGIN_NAMESPACE

namespace QtPrivate {
template <typename... Args>
constexpr int channelConfig(Args... values) {
    return (0 | ... | (1u << values));
}
}

class QAudioFormat
{
public:
    enum SampleFormat : quint16 {
        Unknown,
        UInt8,
        Int16,
        Int32,
        Float,
        NSampleFormats
    };

    // This matches the speaker positions of a 22.2 audio layout. Stereo, Surround 5.1 and Surround 7.1 are subsets of these
    enum AudioChannelPosition {
        UnknownPosition,
        FrontLeft,
        FrontRight,
        FrontCenter,
        LFE,
        BackLeft,
        BackRight,
        FrontLeftOfCenter,
        FrontRightOfCenter,
        BackCenter,
        SideLeft,
        SideRight,
        TopCenter,
        TopFrontLeft,
        TopFrontCenter,
        TopFrontRight,
        TopBackLeft,
        TopBackCenter,
        TopBackRight,
        LFE2,
        TopSideLeft,
        TopSideRight,
        BottomFrontCenter,
        BottomFrontLeft,
        BottomFrontRight
    };
    static constexpr int NChannelPositions = BottomFrontRight + 1;

    enum ChannelConfig : quint32 {
        ChannelConfigUnknown = 0,
        ChannelConfigMono = QtPrivate::channelConfig(FrontCenter),
        ChannelConfigStereo = QtPrivate::channelConfig(FrontLeft, FrontRight),
        ChannelConfig2Dot1 = QtPrivate::channelConfig(FrontLeft, FrontRight, LFE),
        ChannelConfig3Dot0 = QtPrivate::channelConfig(FrontLeft, FrontRight, FrontCenter),
        ChannelConfig3Dot1 = QtPrivate::channelConfig(FrontLeft, FrontRight, FrontCenter, LFE),
        ChannelConfigSurround5Dot0 = QtPrivate::channelConfig(FrontLeft, FrontRight, FrontCenter, BackLeft, BackRight),
        ChannelConfigSurround5Dot1 = QtPrivate::channelConfig(FrontLeft, FrontRight, FrontCenter, LFE, BackLeft, BackRight),
        ChannelConfigSurround7Dot0 = QtPrivate::channelConfig(FrontLeft, FrontRight, FrontCenter, BackLeft, BackRight, SideLeft, SideRight),
        ChannelConfigSurround7Dot1 = QtPrivate::channelConfig(FrontLeft, FrontRight, FrontCenter, LFE, BackLeft, BackRight, SideLeft, SideRight),
    };

    template <typename... Args>
    static constexpr ChannelConfig channelConfig(Args... channels)
    {
        return ChannelConfig(QtPrivate::channelConfig(channels...));
    }

    constexpr bool isValid() const noexcept
    {
        return m_sampleRate > 0 && m_channelCount > 0 && m_sampleFormat != Unknown;
    }

    constexpr void setSampleRate(int sampleRate) noexcept { m_sampleRate = sampleRate; }
    constexpr int sampleRate() const noexcept { return m_sampleRate; }

    Q_MULTIMEDIA_EXPORT void setChannelConfig(ChannelConfig config) noexcept;
    constexpr ChannelConfig channelConfig() const noexcept { return m_channelConfig; }

    constexpr void setChannelCount(int channelCount) noexcept
    {
        m_channelConfig = ChannelConfigUnknown;
        m_channelCount = short(channelCount);
    }
    constexpr int channelCount() const noexcept { return m_channelCount; }

    Q_MULTIMEDIA_EXPORT int channelOffset(AudioChannelPosition channel) const noexcept;

    constexpr void setSampleFormat(SampleFormat f) noexcept { m_sampleFormat = f; }
    constexpr SampleFormat sampleFormat() const noexcept { return m_sampleFormat; }

    // Helper functions
    Q_MULTIMEDIA_EXPORT qint32 bytesForDuration(qint64 microseconds) const;
    Q_MULTIMEDIA_EXPORT qint64 durationForBytes(qint32 byteCount) const;

    Q_MULTIMEDIA_EXPORT qint32 bytesForFrames(qint32 frameCount) const;
    Q_MULTIMEDIA_EXPORT qint32 framesForBytes(qint32 byteCount) const;

    Q_MULTIMEDIA_EXPORT qint32 framesForDuration(qint64 microseconds) const;
    Q_MULTIMEDIA_EXPORT qint64 durationForFrames(qint32 frameCount) const;

    constexpr int bytesPerFrame() const { return bytesPerSample()*channelCount(); }
    constexpr int bytesPerSample() const noexcept
    {
        switch (m_sampleFormat) {
        case Unknown:
        case NSampleFormats: return 0;
        case UInt8: return 1;
        case Int16: return 2;
        case Int32:
        case Float: return 4;
        }
        return 0;
    }

    Q_MULTIMEDIA_EXPORT float normalizedSampleValue(const void *sample) const;

    friend bool operator==(const QAudioFormat &a, const QAudioFormat &b)
    {
        return a.m_sampleRate == b.m_sampleRate &&
               a.m_channelCount == b.m_channelCount &&
               a.m_sampleFormat == b.m_sampleFormat;
    }
    friend bool operator!=(const QAudioFormat &a, const QAudioFormat &b)
    {
        return !(a == b);
    }

    static Q_MULTIMEDIA_EXPORT ChannelConfig defaultChannelConfigForChannelCount(int channelCount);

private:
    SampleFormat m_sampleFormat = SampleFormat::Unknown;
    short m_channelCount = 0;
    ChannelConfig m_channelConfig = ChannelConfigUnknown;
    int m_sampleRate = 0;
    Q_DECL_UNUSED_MEMBER quint64 reserved = 0;
};

#ifndef QT_NO_DEBUG_STREAM
Q_MULTIMEDIA_EXPORT QDebug operator<<(QDebug, const QAudioFormat &);
Q_MULTIMEDIA_EXPORT QDebug operator<<(QDebug, QAudioFormat::SampleFormat);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QAudioFormat)

#endif  // QAUDIOFORMAT_H
