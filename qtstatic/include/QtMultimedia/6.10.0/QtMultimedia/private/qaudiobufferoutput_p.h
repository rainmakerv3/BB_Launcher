// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIOBUFFEROUTPUT_P_H
#define QAUDIOBUFFEROUTPUT_P_H

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

#include <QtCore/private/qobject_p.h>
#include "qaudiobuffer.h"
#include "qaudiobufferoutput.h"

QT_BEGIN_NAMESPACE

class QMediaPlayer;

class QAudioBufferOutputPrivate : public QObjectPrivate
{
public:
    explicit QAudioBufferOutputPrivate(const QAudioFormat &format = {}) : format(format) { }

    static QMediaPlayer *exchangeMediaPlayer(QAudioBufferOutput &output, QMediaPlayer *player)
    {
        auto outputPrivate = static_cast<QAudioBufferOutputPrivate *>(output.d_func());
        return std::exchange(outputPrivate->mediaPlayer, player);
    }

    QAudioFormat format;
    QMediaPlayer *mediaPlayer = nullptr;
};

QT_END_NAMESPACE

#endif // QAUDIOBUFFEROUTPUT_P_H
