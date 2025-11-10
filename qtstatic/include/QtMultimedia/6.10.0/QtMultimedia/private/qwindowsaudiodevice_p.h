// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QWINDOWSAUDIODEVICE_H
#define QWINDOWSAUDIODEVICE_H

#include <QtCore/qbytearray.h>
#include <QtCore/qstring.h>
#include <QtCore/private/qcomptr_p.h>

#include <QtMultimedia/qaudiodevice.h>
#include <QtMultimedia/private/qaudioformat_p.h>
#include <QtMultimedia/private/qaudiosystem_p.h>
#include <QtMultimedia/private/qaudiodevice_p.h>

struct IMMDevice;

QT_BEGIN_NAMESPACE

class QWindowsAudioDevice : public QAudioDevicePrivate
{
public:
    QWindowsAudioDevice(QByteArray deviceId, ComPtr<IMMDevice> immdev, QString description,
                        QAudioDevice::Mode mode);
    ~QWindowsAudioDevice();

    ComPtr<IMMDevice> open() const;

    std::pair<int, int> m_probedChannelCountRange{ 1, 2 }; // fallback: mono/stereo
    std::pair<int, int> m_probedSampleRateRange{
        QtMultimediaPrivate::allSupportedSampleRates.front(),
        QtMultimediaPrivate::allSupportedSampleRates.back(),
    }; // fallback: full range
};

QT_END_NAMESPACE

#endif // QWINDOWSAUDIODEVICE_H
