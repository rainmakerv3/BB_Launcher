// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWINDOWSMULTIMEDIATUTILS_P_H
#define QWINDOWSMULTIMEDIATUTILS_P_H

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

#include <private/qtmultimediaglobal_p.h>
#include <private/qplatformmediaformatinfo_p.h>
#include <qvideoframeformat.h>
#include <guiddef.h>
#include <qstring.h>

QT_BEGIN_NAMESPACE

namespace QWindowsMultimediaUtils {

    Q_MULTIMEDIA_EXPORT QVideoFrameFormat::PixelFormat pixelFormatFromMediaSubtype(const GUID &subtype);

    Q_MULTIMEDIA_EXPORT GUID videoFormatForCodec(QMediaFormat::VideoCodec codec);

    Q_MULTIMEDIA_EXPORT QMediaFormat::VideoCodec codecForVideoFormat(GUID format);

    Q_MULTIMEDIA_EXPORT GUID audioFormatForCodec(QMediaFormat::AudioCodec codec);

    Q_MULTIMEDIA_EXPORT QMediaFormat::AudioCodec codecForAudioFormat(GUID format);

    Q_MULTIMEDIA_EXPORT GUID containerForVideoFileFormat(QMediaFormat::FileFormat format);

    Q_MULTIMEDIA_EXPORT GUID containerForAudioFileFormat(QMediaFormat::FileFormat format);
}

QT_END_NAMESPACE

#endif
