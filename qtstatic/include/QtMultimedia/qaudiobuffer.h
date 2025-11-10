// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIOBUFFER_H
#define QAUDIOBUFFER_H

#include <QtCore/qshareddata.h>

#include <QtMultimedia/qtmultimediaglobal.h>

#include <QtMultimedia/qtaudio.h>
#include <QtMultimedia/qaudioformat.h>

QT_BEGIN_NAMESPACE

namespace QtPrivate {
template <QAudioFormat::SampleFormat> struct AudioSampleFormatHelper
{
};

template <> struct AudioSampleFormatHelper<QAudioFormat::UInt8>
{
    using value_type = unsigned char;
    static constexpr value_type Default = 128;
};

template <> struct AudioSampleFormatHelper<QAudioFormat::Int16>
{
    using value_type = short;
    static constexpr value_type Default = 0;
};

template <> struct AudioSampleFormatHelper<QAudioFormat::Int32>
{
    using value_type = int;
    static constexpr value_type Default = 0;
};

template <> struct AudioSampleFormatHelper<QAudioFormat::Float>
{
    using value_type = float;
    static constexpr value_type Default = 0.;
};

}

template <QAudioFormat::ChannelConfig config, QAudioFormat::SampleFormat format>
struct QAudioFrame
{
private:
    // popcount in qalgorithms.h is unfortunately not constexpr on MSVC.
    // Use this here as a fallback
    static constexpr int constexprPopcount(quint32 i)
    {
        i = i - ((i >> 1) & 0x55555555);        // add pairs of bits
        i = (i & 0x33333333) + ((i >> 2) & 0x33333333);  // quads
        i = (i + (i >> 4)) & 0x0F0F0F0F;        // groups of 8
        return (i * 0x01010101) >> 24;          // horizontal sum of bytes
    }
    static constexpr int nChannels = constexprPopcount(config);
public:
    using value_type = typename QtPrivate::AudioSampleFormatHelper<format>::value_type;
    value_type channels[nChannels];
    static constexpr int positionToIndex(QAudioFormat::AudioChannelPosition pos)
    {
        if (!(config & (1u << pos)))
            return -1;

        uint maskedChannels = config & ((1u << pos) - 1);
        return qPopulationCount(maskedChannels);
    }


    value_type value(QAudioFormat::AudioChannelPosition pos) const {
        int idx = positionToIndex(pos);
        if (idx < 0)
            return QtPrivate::AudioSampleFormatHelper<format>::Default;
        return channels[idx];
    }
    void setValue(QAudioFormat::AudioChannelPosition pos, value_type val) {
        int idx = positionToIndex(pos);
        if (idx < 0)
            return;
        channels[idx] = val;
    }
    value_type operator[](QAudioFormat::AudioChannelPosition pos) const {
        return value(pos);
    }
    constexpr void clear() {
        for (int i = 0; i < nChannels; ++i)
            channels[i] = QtPrivate::AudioSampleFormatHelper<format>::Default;
    }
};

template <QAudioFormat::SampleFormat Format>
using QAudioFrameMono = QAudioFrame<QAudioFormat::ChannelConfigMono, Format>;

template <QAudioFormat::SampleFormat Format>
using QAudioFrameStereo = QAudioFrame<QAudioFormat::ChannelConfigStereo, Format>;

template <QAudioFormat::SampleFormat Format>
using QAudioFrame2Dot1 = QAudioFrame<QAudioFormat::ChannelConfig2Dot1, Format>;

template <QAudioFormat::SampleFormat Format>
using QAudioFrameSurround5Dot1 = QAudioFrame<QAudioFormat::ChannelConfigSurround5Dot1, Format>;

template <QAudioFormat::SampleFormat Format>
using QAudioFrameSurround7Dot1 = QAudioFrame<QAudioFormat::ChannelConfigSurround7Dot1, Format>;


class QAudioBufferPrivate;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QAudioBufferPrivate, Q_MULTIMEDIA_EXPORT)

class Q_MULTIMEDIA_EXPORT QAudioBuffer
{
public:
    QAudioBuffer() noexcept;
    QAudioBuffer(const QAudioBuffer &other) noexcept;
    QAudioBuffer(const QByteArray &data, const QAudioFormat &format, qint64 startTime = -1);
    QAudioBuffer(int numFrames, const QAudioFormat &format, qint64 startTime = -1); // Initialized to empty
    ~QAudioBuffer();

    QAudioBuffer& operator=(const QAudioBuffer &other);

    QAudioBuffer(QAudioBuffer &&other) noexcept = default;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QAudioBuffer)
    void swap(QAudioBuffer &other) noexcept
    { d.swap(other.d); }

    bool isValid() const noexcept { return d != nullptr; };

    void detach();

    QAudioFormat format() const noexcept;

    qsizetype frameCount() const noexcept;
    qsizetype sampleCount() const noexcept;
    qsizetype byteCount() const noexcept;

    qint64 duration() const noexcept;
    qint64 startTime() const noexcept;

    // Structures for easier access to data
    typedef QAudioFrameMono<QAudioFormat::UInt8> U8M;
    typedef QAudioFrameMono<QAudioFormat::Int16> S16M;
    typedef QAudioFrameMono<QAudioFormat::Int32> S32M;
    typedef QAudioFrameMono<QAudioFormat::Float> F32M;

    typedef QAudioFrameStereo<QAudioFormat::UInt8> U8S;
    typedef QAudioFrameStereo<QAudioFormat::Int16> S16S;
    typedef QAudioFrameStereo<QAudioFormat::Int32> S32S;
    typedef QAudioFrameStereo<QAudioFormat::Float> F32S;

    template <typename T> const T* constData() const {
        return static_cast<const T*>(constData());
    }
    template <typename T> const T* data() const {
        return static_cast<const T*>(data());
    }
    template <typename T> T* data() {
        return static_cast<T*>(data());
    }
private:
    const void* constData() const noexcept;
    const void* data() const noexcept;
    void *data();

    QExplicitlySharedDataPointer<QAudioBufferPrivate> d;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QAudioBuffer)

#endif // QAUDIOBUFFER_H
