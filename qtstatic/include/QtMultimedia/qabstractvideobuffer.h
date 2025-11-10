// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QABSTRACTVIDEOBUFFER_H
#define QABSTRACTVIDEOBUFFER_H

#include <QtMultimedia/qtmultimediaexports.h>
#include <QtMultimedia/qvideoframe.h>
#include <QtMultimedia/qvideoframeformat.h>
#include <QtMultimedia/qtvideo.h>

QT_BEGIN_NAMESPACE

class Q_MULTIMEDIA_EXPORT QAbstractVideoBuffer
{
public:
    struct MapData
    {
        int planeCount = 0;
        int bytesPerLine[4] = {};
        uchar *data[4] = {};
        int dataSize[4] = {};
    };

    virtual ~QAbstractVideoBuffer();
    virtual MapData map(QVideoFrame::MapMode mode) = 0;
    virtual void unmap();
    virtual QVideoFrameFormat format() const = 0;
};

QT_END_NAMESPACE

#endif
