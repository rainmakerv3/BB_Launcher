// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QVIDEOFRAME_P_H
#define QVIDEOFRAME_P_H

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

#include "qvideoframe.h"
#include "qhwvideobuffer_p.h"
#include "private/qvideotransformation_p.h"

#include <qmutex.h>

QT_BEGIN_NAMESPACE

class QVideoFramePrivate : public QSharedData
{
public:
    QVideoFramePrivate() = default;

    ~QVideoFramePrivate()
    {
        if (videoBuffer && mapMode != QVideoFrame::NotMapped)
            videoBuffer->unmap();
    }

    template <typename Buffer>
    static QVideoFrame createFrame(std::unique_ptr<Buffer> buffer, QVideoFrameFormat format)
    {
        QVideoFrame result;
        result.d.reset(new QVideoFramePrivate(std::move(format), std::move(buffer)));
        return result;
    }

    template <typename Buffer = QAbstractVideoBuffer>
    QVideoFramePrivate(QVideoFrameFormat format, std::unique_ptr<Buffer> buffer = nullptr)
        : format{ std::move(format) }, videoBuffer{ std::move(buffer) }
    {
        if constexpr (std::is_base_of_v<QHwVideoBuffer, Buffer>)
            hwVideoBuffer = static_cast<QHwVideoBuffer *>(videoBuffer.get());
        else if constexpr (std::is_same_v<QAbstractVideoBuffer, Buffer>)
            hwVideoBuffer = dynamic_cast<QHwVideoBuffer *>(videoBuffer.get());
        // else hwVideoBuffer == nullptr
    }

    static QVideoFramePrivate *handle(QVideoFrame &frame) { return frame.d.get(); };

    static QHwVideoBuffer *hwBuffer(const QVideoFrame &frame)
    {
        return frame.d ? frame.d->hwVideoBuffer : nullptr;
    };

    static QAbstractVideoBuffer *buffer(const QVideoFrame &frame)
    {
        return frame.d ? frame.d->videoBuffer.get() : nullptr;
    };

    QVideoFrame adoptThisByVideoFrame()
    {
        QVideoFrame frame;
        frame.d = QExplicitlySharedDataPointer(this, QAdoptSharedDataTag{});
        return frame;
    }

    qint64 startTime = -1;
    qint64 endTime = -1;
    QAbstractVideoBuffer::MapData mapData;
    QVideoFrame::MapMode mapMode = QVideoFrame::NotMapped;
    QVideoFrameFormat format;
    std::unique_ptr<QAbstractVideoBuffer> videoBuffer;
    QHwVideoBuffer *hwVideoBuffer = nullptr;
    int mappedCount = 0;
    QMutex mapMutex;
    QString subtitleText;
    QImage image;
    QMutex imageMutex;
    VideoTransformation presentationTransformation;

private:
    Q_DISABLE_COPY(QVideoFramePrivate)
};

QT_END_NAMESPACE

#endif // QVIDEOFRAMEPRIVATE_P_H
