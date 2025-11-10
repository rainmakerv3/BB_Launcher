// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWINDOWSAUDIODEVICES_H
#define QWINDOWSAUDIODEVICES_H

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

#include <private/qplatformaudiodevices_p.h>
#include <QtCore/private/qcomptr_p.h>
#include <private/qcominitializer_p.h>
#include <private/qwindowsmediafoundation_p.h>

#include <qaudiodevice.h>

struct IAudioClient3;
struct IMMDevice;
struct IMMDeviceEnumerator;

QT_BEGIN_NAMESPACE

class QWindowsEngine;
class CMMNotificationClient;

class QWindowsAudioDevices : public QPlatformAudioDevices
{
public:
    QWindowsAudioDevices();
    virtual ~QWindowsAudioDevices();

    QPlatformAudioSource *createAudioSource(const QAudioDevice &, const QAudioFormat &,
                                            QObject *parent) override;
    QPlatformAudioSink *createAudioSink(const QAudioDevice &, const QAudioFormat &,
                                        QObject *parent) override;

    using QPlatformAudioDevices::onAudioInputsChanged;
    using QPlatformAudioDevices::onAudioOutputsChanged;

    QLatin1String backendName() const override { return QLatin1String{ "WASAPI" }; }

protected:
    QList<QAudioDevice> findAudioInputs() const override;
    QList<QAudioDevice> findAudioOutputs() const override;

private:
    QComInitializer m_comInitializer;
    QMFRuntimeInit m_wmfRuntime{ QWindowsMediaFoundation::instance() };
    QList<QAudioDevice> availableDevices(QAudioDevice::Mode mode) const;

    ComPtr<IMMDeviceEnumerator> m_deviceEnumerator;
    ComPtr<CMMNotificationClient> m_notificationClient;

    friend CMMNotificationClient;
};

QT_END_NAMESPACE

#endif
