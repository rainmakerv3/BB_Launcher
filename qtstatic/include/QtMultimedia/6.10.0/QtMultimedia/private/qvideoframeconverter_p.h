// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QVIDEOFRAMECONVERTER_H
#define QVIDEOFRAMECONVERTER_H

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

#include <qvideoframe.h>

QT_BEGIN_NAMESPACE

struct VideoTransformation;

Q_MULTIMEDIA_EXPORT QImage qImageFromVideoFrame(const QVideoFrame &frame,
                                                const VideoTransformation &transformation,
                                                bool forceCpu = false);

Q_MULTIMEDIA_EXPORT QImage qImageFromVideoFrame(const QVideoFrame &frame, bool forceCpu = false);

/**
 *  @brief Maps the video frame and returns an image having a shared ownership for the video frame
 * and referencing to its mapped data.
 */
Q_MULTIMEDIA_EXPORT QImage videoFramePlaneAsImage(QVideoFrame &frame, int plane,
                                                  QImage::Format targetFromat, QSize targetSize);

QT_END_NAMESPACE

#endif

