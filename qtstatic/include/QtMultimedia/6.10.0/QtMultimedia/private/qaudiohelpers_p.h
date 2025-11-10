// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIOHELPERS_P_H
#define QAUDIOHELPERS_P_H

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

#include <QtCore/qspan.h>
#include <QtMultimedia/qaudioformat.h>
#include <QtMultimedia/private/qtmultimediaglobal_p.h>
#include <QtMultimedia/private/qaudio_rtsan_support_p.h>

QT_BEGIN_NAMESPACE

namespace QAudioHelperInternal {
Q_MULTIMEDIA_EXPORT void qMultiplySamples(float factor,
                                          const QAudioFormat &format,
                                          const void *src,
                                          void *dest,
                                          int len) noexcept QT_MM_NONBLOCKING;

Q_MULTIMEDIA_EXPORT
void applyVolume(float volume,
                 const QAudioFormat &,
                 QSpan<const std::byte> source,
                 QSpan<std::byte> destination) noexcept QT_MM_NONBLOCKING;

enum class NativeSampleFormat : uint8_t {
    uint8_t,
    int16_t,
    int32_t,
    int24_t_3b, // 3 byte lsb
    int24_t_4b_low, // 4 byte
    float32_t,
};

Q_MULTIMEDIA_EXPORT
void convertSampleFormat(QSpan<const std::byte> source, NativeSampleFormat sourceFormat,
                         QSpan<std::byte> destination,
                         NativeSampleFormat destinationFormat) noexcept QT_MM_NONBLOCKING;

Q_MULTIMEDIA_EXPORT
NativeSampleFormat bestNativeSampleFormat(const QAudioFormat &fmt,
                                          QSpan<const NativeSampleFormat> supportedNativeFormats);
QAudioFormat::SampleFormat bestSampleFormat(NativeSampleFormat);

NativeSampleFormat toNativeSampleFormat(QAudioFormat::SampleFormat);

constexpr size_t bytesPerSample(NativeSampleFormat fmt) noexcept QT_MM_NONBLOCKING
{
    switch (fmt) {
    case NativeSampleFormat::uint8_t:
        return 1;
    case NativeSampleFormat::int16_t:
        return 2;
    case NativeSampleFormat::int24_t_3b:
        return 3;
    case NativeSampleFormat::int24_t_4b_low:
    case NativeSampleFormat::float32_t:
    case NativeSampleFormat::int32_t:
        return 4;
    default:
        Q_UNREACHABLE_RETURN(0);
    }
}

std::optional<float> sanitizeVolume(float volume, float lastVolume);

} // namespace QAudioHelperInternal

Q_MULTIMEDIA_EXPORT
QDebug operator<<(QDebug dbg, QAudioHelperInternal::NativeSampleFormat);

QT_END_NAMESPACE

#endif // QAUDIOHELPERS_P_H
