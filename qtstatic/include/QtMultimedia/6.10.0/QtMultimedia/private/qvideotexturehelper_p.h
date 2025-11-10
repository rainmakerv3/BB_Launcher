// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QVIDEOTEXTUREHELPER_H
#define QVIDEOTEXTUREHELPER_H

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

#include <qvideoframeformat.h>
#include <rhi/qrhi.h>

#include <QtGui/qtextlayout.h>

QT_BEGIN_NAMESPACE

class QVideoFrame;
class QTextLayout;
class QVideoFrameTextures;
using QVideoFrameTexturesUPtr = std::unique_ptr<QVideoFrameTextures>;
class QVideoFrameTexturesHandles;
using QVideoFrameTexturesHandlesUPtr = std::unique_ptr<QVideoFrameTexturesHandles>;

namespace QVideoTextureHelper
{

struct Q_MULTIMEDIA_EXPORT TextureDescription
{
    static constexpr int maxPlanes = 3;
    struct SizeScale {
        int x;
        int y;
    };
    using BytesRequired = int(*)(int stride, int height);

    enum TextureFormat {
        UnknownFormat,
        Red_8,
        RG_8,
        RGBA_8,
        BGRA_8,
        Red_16,
        RG_16,
    };

    QRhiTexture::Format rhiTextureFormat(int plane, QRhi *rhi) const;

    inline int strideForWidth(int width) const { return (width*strideFactor + 15) & ~15; }
    inline int bytesForSize(QSize s) const { return bytesRequired(strideForWidth(s.width()), s.height()); }
    int widthForPlane(int width, int plane) const
    {
        if (plane > nplanes) return 0;
        return (width + sizeScale[plane].x - 1)/sizeScale[plane].x;
    }
    int heightForPlane(int height, int plane) const
    {
        if (plane > nplanes) return 0;
        return (height + sizeScale[plane].y - 1)/sizeScale[plane].y;
    }

    /**
     * \returns Plane scaling factors taking into account possible workarounds due to QRhi backend
     * capabilities.
     */
    SizeScale rhiSizeScale(int plane, QRhi *rhi) const
    {
        if (!rhi)
            return sizeScale[plane];

        // NOTE: We need to handle sizing difference when packing two-component textures to RGBA8,
        // where width gets cut in half.
        // Another option would be to compare QRhiImplementation::TextureFormatInfo to expected
        // based on TextureDescription::Format.
        if ((textureFormat[plane] == TextureDescription::RG_8
             || textureFormat[plane] == TextureDescription::Red_16)
            && rhiTextureFormat(plane, rhi) == QRhiTexture::RGBA8)
            return { sizeScale[plane].x * 2, sizeScale[plane].y };

        return sizeScale[plane];
    }

    QSize rhiPlaneSize(QSize frameSize, int plane, QRhi *rhi) const
    {
        SizeScale scale = rhiSizeScale(plane, rhi);
        return QSize(frameSize.width() / scale.x, frameSize.height() / scale.y);
    }

    bool hasTextureFormat(TextureFormat format) const
    {
        return std::any_of(textureFormat, textureFormat + nplanes, [format](TextureFormat f) {
            return f == format;
        });
    }

    int nplanes;
    int strideFactor;
    BytesRequired bytesRequired;
    TextureFormat textureFormat[maxPlanes];
    SizeScale sizeScale[maxPlanes];
};

Q_MULTIMEDIA_EXPORT const TextureDescription *textureDescription(QVideoFrameFormat::PixelFormat format);

Q_MULTIMEDIA_EXPORT QString vertexShaderFileName(const QVideoFrameFormat &format);
Q_MULTIMEDIA_EXPORT QString
fragmentShaderFileName(const QVideoFrameFormat &format, QRhi *rhi,
                       QRhiSwapChain::Format surfaceFormat = QRhiSwapChain::SDR);
Q_MULTIMEDIA_EXPORT void updateUniformData(QByteArray *dst, QRhi *rhi,
                                           const QVideoFrameFormat &format,
                                           const QVideoFrame &frame, const QMatrix4x4 &transform,
                                           float opacity, float maxNits = 100);

/**
 * @brief Creates plane textures from texture handles set by the specified rhi.
 *        The result owns the specified handles set; the QRhiTexture(s), exposed by the result
 *        refer to the owned handles set.
 *        If the specified size is empty or pixelFormat is invalid, null is returned.
 */
Q_MULTIMEDIA_EXPORT QVideoFrameTexturesUPtr
createTexturesFromHandles(QVideoFrameTexturesHandlesUPtr handles, QRhi &rhi,
                          QVideoFrameFormat::PixelFormat pixelFormat, QSize size);


/**
 * @brief Creates plane textures from a video frame by the specified rhi.
          If possible, the function modifies 'oldTextures', which is the texture from the pool,
          and returns the specified one or a new one with any taken data.
 */
Q_MULTIMEDIA_EXPORT QVideoFrameTexturesUPtr createTextures(const QVideoFrame &frame, QRhi &rhi,
                                                           QRhiResourceUpdateBatch &rub,
                                                           QVideoFrameTexturesUPtr &oldTextures);

/**
 * @brief Returns a QRhiTexture::Format taking into account rhi capabilities and explicitly excluded
 *        formats using a chain of pre-defined fallback texture formats. If no valid fallback is
 *        determined, will return the format argument.
 */
Q_MULTIMEDIA_EXPORT QRhiTexture::Format
resolvedRhiTextureFormat(QRhiTexture::Format format, QRhi *rhi);

Q_MULTIMEDIA_EXPORT void
setExcludedRhiTextureFormats(QList<QRhiTexture::Format> formats); // for tests only

struct UniformData {
    float transformMatrix[4][4];
    float colorMatrix[4][4];
    float opacity;
    float width;
    float masteringWhite;
    float maxLum;
    int redOrAlphaIndex;
    int planeFormats[TextureDescription::maxPlanes];
};

struct Q_MULTIMEDIA_EXPORT SubtitleLayout
{
    QSize videoSize;
    QRectF bounds;
    QTextLayout layout;

    bool update(const QSize &frameSize, QString text);
    void draw(QPainter *painter, const QPointF &translate) const;
    QImage toImage() const;
};

}

QT_END_NAMESPACE

#endif
