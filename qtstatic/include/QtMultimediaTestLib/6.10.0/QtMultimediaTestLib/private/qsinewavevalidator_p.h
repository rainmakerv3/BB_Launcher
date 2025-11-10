// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QSINEWAVEVALIDATOR_H
#define QSINEWAVEVALIDATOR_H

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

#include <array>

#include <QtCore/qtconfigmacros.h>

QT_BEGIN_NAMESPACE

// sine wave validator: accumulating peak value and result of passing the signal through a notch
// filter. A sine wave of that frequency should not pass the notch filter, so if the peak exceeds a
// small threshold, it we can detect discontinuities for example.
struct QSineWaveValidator
{
    QSineWaveValidator(const QSineWaveValidator &) = delete;
    QSineWaveValidator(QSineWaveValidator &&) = delete;
    QSineWaveValidator &operator=(const QSineWaveValidator &) = delete;
    QSineWaveValidator &operator=(QSineWaveValidator &&) = delete;

    QSineWaveValidator(float notchFrequency, float sampleRate);

    void feedSample(float sample);

    float peak() const { return accumPeak; }
    float notchPeak() const;

private:
    float a0, a1, a2;
    float b0, b1, b2;

    std::array<float, 3> x{}, y{};
    int framesBeforeAnalysis = 128; // unscientific estimate, larger than the transient response
                                    // time for the IIR filter
    int pendingFramesBeforeAnalysis = framesBeforeAnalysis;

    float accumPeak{};
    float accumNotchPeak{};
};

QT_END_NAMESPACE

#endif // QSINEWAVEVALIDATOR_H
