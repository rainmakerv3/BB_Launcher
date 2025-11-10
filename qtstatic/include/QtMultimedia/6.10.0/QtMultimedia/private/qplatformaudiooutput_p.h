// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QPLATFORMAUDIOOUTPUT_H
#define QPLATFORMAUDIOOUTPUT_H

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

QT_BEGIN_NAMESPACE

class QAudioOutput;

class Q_MULTIMEDIA_EXPORT QPlatformAudioOutput
{
public:
    explicit QPlatformAudioOutput(QAudioOutput *qq) : q(qq) { }
    virtual ~QPlatformAudioOutput() = default;

    virtual void setAudioDevice(const QAudioDevice &/*device*/) {}
    virtual void setMuted(bool /*muted*/) {}
    virtual void setVolume(float /*volume*/) {}

    QAudioOutput *q = nullptr;
    QAudioDevice device;
    float volume = 1.;
    bool muted = false;
    std::function<void()>  disconnectFunction;
};

QT_END_NAMESPACE


#endif // QPLATFORMAUDIOOUTPUT_H
