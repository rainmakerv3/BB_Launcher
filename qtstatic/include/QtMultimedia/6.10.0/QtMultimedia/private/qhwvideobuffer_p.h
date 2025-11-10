// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QHWVIDEOBUFFER_P_H
#define QHWVIDEOBUFFER_P_H

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

#include "qabstractvideobuffer.h"
#include "qvideoframe.h"

#include <QtGui/qmatrix4x4.h>

QT_BEGIN_NAMESPACE

class QRhi;
class QRhiTexture;
class QVideoFrame;

/**
 * @brief QVideoFrameTexturesHandles is the base class, providing texture handles for frame planes.
 *        Instances of the class may own textures, share ownership, or refer to inner hw textures
 *        of QVideoFrame. Referencing to inner frame's textures without shared ownership is
 *        not recommended, we strive to get around it; if texture are referencing,
 *        the source frame must be kept in QVideoFrameTextures's instance
 *        (see QVideoFrameTextures::setSourceFrame).
 */
class Q_MULTIMEDIA_EXPORT QVideoFrameTexturesHandles
{
public:
    virtual ~QVideoFrameTexturesHandles();

    virtual quint64 textureHandle(QRhi &, int /*plane*/) { return 0; };
};
using QVideoFrameTexturesHandlesUPtr = std::unique_ptr<QVideoFrameTexturesHandles>;

/**
 * @brief QVideoFrameTextures is the base class representing an abstraction layer
 *        between QVideoFrame's texture(s) and rhi's plane textures.
 *        QVideoFrameTextures must own the inner rhi textures or
 *        share ownership. QVideoFrameTextures instances are propagated to
 *        QVideoFrameTexturesPool, where their lifetime is managed
 *        according to results of QRhi::currentFrameSlot upon rendering.
 *
 */
class Q_MULTIMEDIA_EXPORT QVideoFrameTextures
{
public:
    virtual ~QVideoFrameTextures();
    virtual QRhiTexture *texture(uint plane) const = 0;

    /**
     * @brief The virtual method should be invoked after QRhi::endFrame to unmap and free
     *        internal resources that are not needed anymore.
     */
    virtual void onFrameEndInvoked() { }

    virtual QVideoFrameTexturesHandlesUPtr takeHandles() { return nullptr; }

    /**
     * @brief Sets the source frame. It is a temporary solution to delegate
     *        frame's shared ownership to the instance.
     *        Ideally, the creators of QVideoFrameTextures's or QVideoFrameTexturesHandles's
     *        instances should manage ownership.
     */
    void setSourceFrame(QVideoFrame sourceFrame) { m_sourceFrame = std::move(sourceFrame); }

private:
    QVideoFrame m_sourceFrame;
};
using QVideoFrameTexturesUPtr = std::unique_ptr<QVideoFrameTextures>;

class Q_MULTIMEDIA_EXPORT QHwVideoBuffer : public QAbstractVideoBuffer,
                                           public QVideoFrameTexturesHandles
{
public:
    QHwVideoBuffer(QVideoFrame::HandleType type, QRhi *rhi = nullptr);

    ~QHwVideoBuffer() override;

    QVideoFrame::HandleType handleType() const { return m_type; }
    virtual QRhi *rhi() const { return m_rhi; }

    QVideoFrameFormat format() const override { return {}; }

    virtual QMatrix4x4 externalTextureMatrix() const { return {}; }

    virtual QVideoFrameTexturesUPtr mapTextures(QRhi &, QVideoFrameTexturesUPtr& /*oldTextures*/) { return nullptr; };

    virtual void initTextureConverter(QRhi &) { }

protected:
    QVideoFrame::HandleType m_type;
    QRhi *m_rhi = nullptr;
};

QT_END_NAMESPACE

#endif // QHWVIDEOBUFFER_P_H
