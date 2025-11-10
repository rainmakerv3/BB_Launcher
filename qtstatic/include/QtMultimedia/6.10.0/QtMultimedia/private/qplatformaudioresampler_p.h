// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLATFORMAUDIORESAMPLER_P_H
#define QPLATFORMAUDIORESAMPLER_P_H

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
#include <qaudiobuffer.h>

QT_BEGIN_NAMESPACE

class QPlatformAudioResampler
{
public:
    virtual ~QPlatformAudioResampler() = default;

    virtual QAudioBuffer resample(const char *data, size_t size) = 0;
};

QT_END_NAMESPACE

#endif // QPLATFORMAUDIORESAMPLER_P_H
