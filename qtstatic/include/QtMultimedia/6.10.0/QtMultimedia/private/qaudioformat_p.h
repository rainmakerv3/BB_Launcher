// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIOFORMAT_P_H
#define QAUDIOFORMAT_P_H

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

#include <array>

QT_BEGIN_NAMESPACE

namespace QtMultimediaPrivate {

inline constexpr std::array allSupportedSampleRates{
    8'000,  11'025, 12'000, 16'000, 22'050,  24'000,  32'000,  44'100,
    48'000, 64'000, 88'200, 96'000, 128'000, 176'400, 192'000,
};

inline constexpr std::array allSupportedSampleFormats{
    QAudioFormat::UInt8,
    QAudioFormat::Int16,
    QAudioFormat::Int32,
    QAudioFormat::Float,
};

template <typename T>
int findClosestSamplingRate(int rate, QSpan<const T> supportedRates)
{
    Q_ASSERT(!supportedRates.empty());

    auto exactMatchIt = std::find(supportedRates.begin(), supportedRates.end(), T(rate));
    if (exactMatchIt != supportedRates.end())
        return int(*exactMatchIt);

    // find supported rate, which is closest to the default pipewire sampling rate (using a
    // logarithmic scaling)
    auto ratioToRate = [&](int arg) {
        return arg > rate ? float(arg) / rate : rate / float(arg);
    };

    return *std::min_element(supportedRates.begin(), supportedRates.end(), [&](auto lhs, auto rhs) {
        return ratioToRate(lhs) < ratioToRate(rhs);
    });
}

} // namespace QtMultimediaPrivate

QT_END_NAMESPACE

#endif  // QAUDIOFORMAT_P_H
