// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-3.0-only
#ifndef QAUDIOROOM_P_H
#define QAUDIOROOM_P_H

//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtSpatialAudio/qaudioroom.h>
#include <QtSpatialAudio/private/qtspatialaudioglobal_p.h>
#include <QtSpatialAudio/private/qaudioengine_p.h>
#include <QtCore/private/qobject_p.h>

#include <resonance_audio.h>
#include "platforms/common/room_properties.h"

QT_BEGIN_NAMESPACE

class QAudioRoomPrivate : public QObjectPrivate
{
public:
    static QAudioRoomPrivate *get(QAudioRoom *r) { return r->d_func(); }

    QAudioEngine *engine = nullptr;
    vraudio::RoomProperties roomProperties;
    bool dirty = true;

    vraudio::ReverbProperties reverb;
    vraudio::ReflectionProperties reflections;

    float m_wallOcclusion[6] = { -1.f, -1.f, -1.f, -1.f, -1.f, -1.f };
    float m_wallDampening[6] = { -1.f, -1.f, -1.f, -1.f, -1.f, -1.f };

    float wallOcclusion(QAudioRoom::Wall wall) const;
    float wallDampening(QAudioRoom::Wall wall) const;

    void update();
};

QT_END_NAMESPACE

#endif
