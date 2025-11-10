// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIO_H
#define QAUDIO_H

#if 0
#pragma qt_class(QAudio)
#endif

#include <QtMultimedia/qtmultimediaglobal.h>

QT_BEGIN_NAMESPACE

// ### Qt7: merge the QAudio namespace into QtAudio
#if defined(Q_QDOC)
namespace QtAudio
#else
namespace QAudio
#endif
{
enum Error
{
    NoError,
    OpenError,
    IOError,
    UnderrunError,
    FatalError,
};
enum State
{
    ActiveState,
    SuspendedState,
    StoppedState,
    IdleState,
};

enum VolumeScale
{
    LinearVolumeScale,
    CubicVolumeScale,
    LogarithmicVolumeScale,
    DecibelVolumeScale,
};

} // namespace QtAudio


namespace QtAudio {

#if !defined(Q_QDOC)
using Error = QAudio::Error;
using State = QAudio::State;
using VolumeScale = QAudio::VolumeScale;

inline constexpr auto NoError = QAudio::NoError;
inline constexpr auto OpenError = QAudio::OpenError;
inline constexpr auto IOError = QAudio::IOError;
inline constexpr auto UnderrunError = QAudio::UnderrunError;
inline constexpr auto FatalError = QAudio::FatalError;
inline constexpr auto ActiveState = QAudio::ActiveState;
inline constexpr auto SuspendedState = QAudio::SuspendedState;
inline constexpr auto StoppedState = QAudio::StoppedState;
inline constexpr auto IdleState = QAudio::IdleState;
inline constexpr auto LinearVolumeScale = QAudio::LinearVolumeScale;
inline constexpr auto CubicVolumeScale = QAudio::CubicVolumeScale;
inline constexpr auto LogarithmicVolumeScale = QAudio::LogarithmicVolumeScale;
inline constexpr auto DecibelVolumeScale = QAudio::DecibelVolumeScale;
#endif

Q_MULTIMEDIA_EXPORT float convertVolume(float volume, VolumeScale from, VolumeScale to);

} // namespace QtAudio


#if !defined(Q_QDOC)
namespace QAudio
{
#if QT_CORE_REMOVED_SINCE(6, 10)
Q_MULTIMEDIA_EXPORT float convertVolume(float volume, VolumeScale from, VolumeScale to);
#elif !defined(QT_MULTIMEDIA_BUILD_REMOVED_API)
using QtAudio::convertVolume;
#endif
} // namespace QAudio
#endif

#ifndef QT_NO_DEBUG_STREAM
Q_MULTIMEDIA_EXPORT QDebug operator<<(QDebug dbg, QtAudio::Error error);
Q_MULTIMEDIA_EXPORT QDebug operator<<(QDebug dbg, QtAudio::State state);
Q_MULTIMEDIA_EXPORT QDebug operator<<(QDebug dbg, QtAudio::VolumeScale role);
#endif

QT_END_NAMESPACE

#endif // QAUDIO_H
