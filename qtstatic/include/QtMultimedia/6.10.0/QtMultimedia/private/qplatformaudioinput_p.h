// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QPLATFORMAUDIOINPUT_H
#define QPLATFORMAUDIOINPUT_H

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

#include <private/qtmultimediaglobal_p.h>
#include <qaudiodevice.h>

#include <functional>

QT_BEGIN_NAMESPACE

class QAudioInput;

class Q_MULTIMEDIA_EXPORT QPlatformAudioInput
{
public:
    explicit QPlatformAudioInput(QAudioInput *qq) : q(qq) { }
    virtual ~QPlatformAudioInput() = default;

    virtual void setAudioDevice(const QAudioDevice & /*device*/) { }
    virtual void setMuted(bool /*muted*/) {}
    virtual void setVolume(float /*volume*/) {}

    QAudioInput *q = nullptr;
    QAudioDevice device;
    float volume = 1.;
    bool muted = false;
    std::function<void()> disconnectFunction;
};

QT_END_NAMESPACE


#endif // QPLATFORMAUDIOINPUT_H
