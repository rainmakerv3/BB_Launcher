// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSGVIDEONODE_P_H
#define QSGVIDEONODE_P_H

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

#include <QtQuick/qsgnode.h>
#include <private/qtmultimediaquickglobal_p.h>
#include "private/qvideotexturehelper_p.h"
#include "private/qmultimediautils_p.h"

#include <QtMultimedia/qvideoframe.h>
#include <QtMultimedia/qvideoframeformat.h>
#include <QtGui/qopenglfunctions.h>

QT_BEGIN_NAMESPACE

class QSGVideoMaterial;
class QQuickVideoOutput;
class QSGInternalTextNode;

class QVideoFrameTexturePool;
using QVideoFrameTexturePoolPtr = std::shared_ptr<QVideoFrameTexturePool>;

class QSGVideoNode : public QSGGeometryNode
{
public:
    QSGVideoNode(QQuickVideoOutput *parent, const QVideoFrameFormat &videoFormat,
                 QRhi *rhi);
    ~QSGVideoNode() override;

    QVideoFrameFormat::PixelFormat pixelFormat() const { return m_videoFormat.pixelFormat(); }
    void setCurrentFrame(const QVideoFrame &frame);
    void setSurfaceFormat(const QRhiSwapChain::Format surfaceFormat);
    void setHdrInfo(const QRhiSwapChainHdrInfo &hdrInfo);

    void setTexturedRectGeometry(const QRectF &boundingRect, const QRectF &textureRect,
                                 VideoTransformation videoOutputTransformation);

    const QVideoFrameTexturePoolPtr &texturePool() const;

private:
    void updateSubtitle(const QVideoFrame &frame);
    void setSubtitleGeometry();

    QQuickVideoOutput *m_parent = nullptr;
    QRectF m_rect;
    QRectF m_textureRect;
    VideoTransformation m_videoOutputTransformation;
    VideoTransformation m_frameTransformation;

    QVideoFrameFormat m_videoFormat;
    QSGVideoMaterial *m_material = nullptr;

    QVideoTextureHelper::SubtitleLayout m_subtitleLayout;
    QSGInternalTextNode *m_subtitleTextNode = nullptr;
};

QT_END_NAMESPACE

#endif // QSGVIDEONODE_H
