// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QVIDEOFRAMECONVERSIONHELPER_P_H
#define QVIDEOFRAMECONVERSIONHELPER_P_H

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
#include <private/qsimd_p.h>

QT_BEGIN_NAMESPACE

// Converts to RGB32 or ARGB32_Premultiplied
typedef void (QT_FASTCALL *VideoFrameConvertFunc)(const QVideoFrame &frame, uchar *output);
typedef void(QT_FASTCALL *PixelsCopyFunc)(uint32_t *dst, const uint32_t *src, size_t size, uint32_t mask);

VideoFrameConvertFunc qConverterForFormat(QVideoFrameFormat::PixelFormat format);

void Q_MULTIMEDIA_EXPORT qCopyPixelsWithAlphaMask(uint32_t *dst,
                                                  const uint32_t *src,
                                                  size_t size,
                                                  QVideoFrameFormat::PixelFormat format,
                                                  bool srcAlphaVaries);

void Q_MULTIMEDIA_EXPORT qCopyPixelsWithMask(uint32_t *dst, const uint32_t *src, size_t size, uint32_t mask);

uint32_t Q_MULTIMEDIA_EXPORT qAlphaMask(QVideoFrameFormat::PixelFormat format);

template<int a, int r, int g, int b>
struct ArgbPixel
{
    quint32 data;
    inline quint32 convert() const
    {
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        return (((data >> (8*a)) & 0xff) << 24)
             | (((data >> (8*r)) & 0xff) << 16)
             | (((data >> (8*g)) & 0xff) << 8)
             | ((data >> (8*b)) & 0xff);
#else
        return (((data >> (32-8*a)) & 0xff) << 24)
             | (((data >> (32-8*r)) & 0xff) << 16)
             | (((data >> (32-8*g)) & 0xff) << 8)
             | ((data >> (32-8*b)) & 0xff);
#endif
    }
};

template<int r, int g, int b>
struct RgbPixel
{
    quint32 data;
    inline quint32 convert() const
    {
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        return 0xff000000
                | (((data >> (8*r)) & 0xff) << 16)
                | (((data >> (8*g)) & 0xff) << 8)
                | ((data >> (8*b)) & 0xff);
#else
        return 0xff000000
                | (((data >> (32-8*r)) & 0xff) << 16)
                | (((data >> (32-8*g)) & 0xff) << 8)
                | ((data >> (32-8*b)) & 0xff);
#endif
    }
};

template<typename Y>
struct YPixel
{
    Y data;
    static constexpr uint shift = (sizeof(Y) - 1)*8;
    inline quint32 convert() const
    {
        uint y = (data >> shift) & 0xff;
        return (0xff000000)
               | (y << 16)
               | (y << 8)
               | (y);
    }

};


using ARGB8888 = ArgbPixel<0, 1, 2, 3>;
using ABGR8888 = ArgbPixel<0, 3, 2, 1>;
using RGBA8888 = ArgbPixel<3, 0, 1, 2>;
using BGRA8888 = ArgbPixel<3, 2, 1, 0>;
using XRGB8888 = RgbPixel<1, 2, 3>;
using XBGR8888 = RgbPixel<3, 2, 1>;
using RGBX8888 = RgbPixel<0, 1, 2>;
using BGRX8888 = RgbPixel<2, 1, 0>;

#define FETCH_INFO_PACKED(frame) \
    const uchar *src = frame.bits(0); \
    int stride = frame.bytesPerLine(0); \
    int width = frame.width(); \
    int height = frame.height();

#define FETCH_INFO_BIPLANAR(frame) \
    const uchar *plane1 = frame.bits(0); \
    const uchar *plane2 = frame.bits(1); \
    int plane1Stride = frame.bytesPerLine(0); \
    int plane2Stride = frame.bytesPerLine(1); \
    int width = frame.width(); \
    int height = frame.height();

#define FETCH_INFO_TRIPLANAR(frame) \
    const uchar *plane1 = frame.bits(0); \
    const uchar *plane2 = frame.bits(1); \
    const uchar *plane3 = frame.bits(2); \
    int plane1Stride = frame.bytesPerLine(0); \
    int plane2Stride = frame.bytesPerLine(1); \
    int plane3Stride = frame.bytesPerLine(2); \
    int width = frame.width(); \
    int height = frame.height(); \

#define MERGE_LOOPS(width, height, stride, bpp) \
    if (stride == width * bpp) { \
        width *= height; \
        height = 1; \
        stride = 0; \
    }

#define QT_MEDIA_ALIGN(boundary, ptr, x, length) \
    for (; ((reinterpret_cast<qintptr>(ptr) & (boundary - 1)) != 0) && x < length; ++x)

QT_END_NAMESPACE

#endif // QVIDEOFRAMECONVERSIONHELPER_P_H

