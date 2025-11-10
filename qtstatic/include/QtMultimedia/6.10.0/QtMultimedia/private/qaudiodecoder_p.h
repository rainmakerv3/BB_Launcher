// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIODECODER_P_H
#define QAUDIODECODER_P_H

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

#include <QtCore/private/qobject_p.h>
#include <QtMultimedia/private/qplatformaudiodecoder_p.h>

#include <memory.h>

QT_BEGIN_NAMESPACE

class QAudioDecoder;

class QAudioDecoderPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QAudioDecoder)

public:
    QAudioDecoderPrivate() = default;

    QUrl unresolvedUrl;
    std::unique_ptr<QPlatformAudioDecoder> decoder;
};

QT_END_NAMESPACE

#endif // QAUDIODECODER_P_H
