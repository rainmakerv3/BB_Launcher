// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWINDOWSAUDIOUTILS_H
#define QWINDOWSAUDIOUTILS_H

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

#include <QtCore/qstring.h>
#include <QtCore/private/qcomptr_p.h>
#include <QtCore/private/quniquehandle_types_p.h>
#include <QtMultimedia/qaudioformat.h>
#include <QtMultimedia/private/qaudiosystem_p.h>

#include <mmreg.h>

#include <chrono>
#include <optional>

struct IAudioClient;
struct IAudioClient3;
struct IMFMediaType;
struct IMMDevice;
typedef LONGLONG REFERENCE_TIME;

QT_BEGIN_NAMESPACE

class QWindowsMediaFoundation;

namespace QWindowsAudioUtils
{
using QtMultimediaPrivate::AudioEndpointRole;

// REFERENCE_TIME helper
using reference_time = std::chrono::duration<long long, std::ratio<1, 10000000>>;
static_assert(reference_time(1) == std::chrono::nanoseconds(100));

// format utilities
bool formatToWaveFormatExtensible(const QAudioFormat &format, WAVEFORMATEXTENSIBLE &wfx);
std::optional<WAVEFORMATEXTENSIBLE> toWaveFormatExtensible(const QAudioFormat &format);

QAudioFormat waveFormatExToFormat(const WAVEFORMATEX &in);
Q_MULTIMEDIA_EXPORT QAudioFormat mediaTypeToFormat(IMFMediaType *mediaType);
ComPtr<IMFMediaType> formatToMediaType(QWindowsMediaFoundation &, const QAudioFormat &format);
QAudioFormat::ChannelConfig maskToChannelConfig(UINT32 mask, int count);

// IAudioClient helpers
struct AudioClientCreationResult
{
    ComPtr<IAudioClient3> client;
    reference_time periodSize;
    qsizetype audioClientFrames;
};
std::optional<AudioClientCreationResult>
createAudioClient(const ComPtr<IMMDevice> &, const QAudioFormat &,
                  std::optional<qsizetype> hardwareBufferFrames,
                  const QUniqueWin32NullHandle &wasapiEventHandle,
                  std::optional<AudioEndpointRole> = {});

bool audioClientStart(const ComPtr<IAudioClient3> &);
bool audioClientStop(const ComPtr<IAudioClient3> &);
bool audioClientReset(const ComPtr<IAudioClient3> &);
bool audioClientSetRate(const ComPtr<IAudioClient3> &, int rate);
bool audioClientSetRole(const ComPtr<IAudioClient3> &client, AudioEndpointRole role);

std::optional<quint32> getBufferSizeInFrames(const ComPtr<IAudioClient3> &client);
struct AudioClientDevicePeriod
{
    reference_time defaultDuration;
    reference_time minimalDuration;
};
std::optional<AudioClientDevicePeriod> getDevicePeriod(const ComPtr<IAudioClient3> &client);

// wasapi thread helper
void setMCSSForPeriodSize(reference_time);

// error stringification
QString audioClientErrorString(HRESULT);

} // namespace QWindowsAudioUtils

QT_END_NAMESPACE

#endif // QWINDOWSAUDIOUTILS_H
