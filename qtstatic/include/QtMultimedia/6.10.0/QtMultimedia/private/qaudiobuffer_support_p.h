// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIOBUFFER_SUPPORT_P_H
#define QAUDIOBUFFER_SUPPORT_P_H

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

#include <QtMultimedia/qaudiobuffer.h>
#include <QtCore/qspan.h>

QT_BEGIN_NAMESPACE

namespace QtPrivate {

enum class Mutability {
    Mutable,
    Immutable,
};

constexpr Mutability QAudioBufferMutable = Mutability::Mutable;
constexpr Mutability QAudioBufferImmutable = Mutability::Immutable;

// LATER: c++23 has std::views::stride(3)
template <typename SampleType>
struct QAudioBufferChannelView
{
    SampleType &operator[](int frame) { return m_buffer[frame * m_numberOfChannels + m_channel]; }

    QSpan<SampleType> m_buffer;
    const int m_channel;
    const int m_numberOfChannels;
};

template <typename SampleType>
void validateBufferFormat(const QAudioBuffer &buffer, int channel)
{
    Q_ASSERT(channel < buffer.format().channelCount());

    if constexpr (std::is_same_v<std::remove_const_t<SampleType>, float>) {
        Q_ASSERT(buffer.format().sampleFormat() == QAudioFormat::SampleFormat::Float);
    } else if constexpr (std::is_same_v<std::remove_const_t<SampleType>, int32_t>) {
        Q_ASSERT(buffer.format().sampleFormat() == QAudioFormat::SampleFormat::Int32);
    } else if constexpr (std::is_same_v<std::remove_const_t<SampleType>, int16_t>) {
        Q_ASSERT(buffer.format().sampleFormat() == QAudioFormat::SampleFormat::Int16);
    } else if constexpr (std::is_same_v<std::remove_const_t<SampleType>, uint8_t>) {
        Q_ASSERT(buffer.format().sampleFormat() == QAudioFormat::SampleFormat::UInt8);
    }
}

template <typename T, bool Predicate>
using add_const_if_t = std::conditional_t<Predicate, std::add_const_t<T>, T>;

template <typename SampleType>
auto makeChannelView(add_const_if_t<QAudioBuffer, std::is_const_v<SampleType>> &buffer, int channel)
{
    validateBufferFormat<SampleType>(buffer, channel);
    return QAudioBufferChannelView<SampleType>{
        QSpan{ buffer.template data<SampleType>(), buffer.sampleCount() },
        channel,
        buffer.format().channelCount(),
    };
}

template <typename SampleType>
struct QAudioBufferDeinterleaveAdaptor
{
    using BufferType = add_const_if_t<QAudioBuffer, std::is_const_v<SampleType>>;

    QAudioBufferChannelView<SampleType> operator[](int channel)
    {
        return makeChannelView<SampleType>(m_buffer, channel);
    }

    QAudioBufferChannelView<const SampleType> operator[](int channel) const
    {
        return makeChannelView<const SampleType>(m_buffer, channel);
    }

    explicit QAudioBufferDeinterleaveAdaptor(BufferType &buffer) : m_buffer(buffer) { }

private:
    BufferType &m_buffer;
    int m_numberOfChannels = m_buffer.format().channelCount();
};

} // namespace QtPrivate

QT_END_NAMESPACE

#endif // QAUDIOBUFFER_SUPPORT_P_H
