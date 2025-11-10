// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMULTIMEDIAUTILS_P_H
#define QMULTIMEDIAUTILS_P_H

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

#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtMultimedia/private/qvideotransformation_p.h>
#include <QtCore/qsize.h>
#include <QtCore/qurl.h>
#include <QtGui/rhi/qrhi.h>

QT_BEGIN_NAMESPACE

class QRhiSwapChain;
class QVideoFrame;
class QVideoFrameFormat;

struct Fraction {
    int numerator;
    int denominator;
};

Q_MULTIMEDIA_EXPORT Fraction qRealToFraction(qreal value);

Q_MULTIMEDIA_EXPORT QSize qCalculateFrameSize(QSize resolution, Fraction pixelAspectRatio);

// TODO: after adding pixel aspect ratio to QVideoFrameFormat, the function should
// consider PAR as well as rotation
Q_MULTIMEDIA_EXPORT QSize qRotatedFrameSize(QSize size, int rotation);

inline QSize qRotatedFrameSize(QSize size, QtVideo::Rotation rotation)
{
    return qRotatedFrameSize(size, qToUnderlying(rotation));
}

Q_MULTIMEDIA_EXPORT QSize qRotatedFramePresentationSize(const QVideoFrame &frame);

Q_MULTIMEDIA_EXPORT QUrl qMediaFromUserInput(QUrl fileName);

Q_MULTIMEDIA_EXPORT bool qIsAutoHdrEnabled();

Q_MULTIMEDIA_EXPORT QRhiSwapChain::Format
qGetRequiredSwapChainFormat(const QVideoFrameFormat &format);

Q_MULTIMEDIA_EXPORT bool
qShouldUpdateSwapChainFormat(QRhiSwapChain *swapChain,
                             QRhiSwapChain::Format requiredSwapChainFormat);

Q_MULTIMEDIA_EXPORT VideoTransformation
qNormalizedSurfaceTransformation(const QVideoFrameFormat &format);

Q_MULTIMEDIA_EXPORT VideoTransformation qNormalizedFrameTransformation(
        const QVideoFrame &frame, VideoTransformation videoOutputTransformation = {});

Q_MULTIMEDIA_EXPORT QtVideo::Rotation
qVideoRotationFromDegrees(int clockwiseDegrees);

inline VideoTransformation qNormalizedFrameTransformation(const QVideoFrame &frame,
                                                          int videoOutputRotation)
{
    return qNormalizedFrameTransformation(
            frame, VideoTransformation{ qVideoRotationFromDegrees(videoOutputRotation) });
}

/* The function get mirroring and rotation from the specified QTransform.
 *
 * Matrix translation is not taken into consideration.
 * Matrix negative scaling is interpreted as mirroring.
 * Absolute X and Y scale values are not taken into consideration as
 * QVideoFrame and QVideoFrameFormat don't support scaling transformations.
 *
 * Sheared matrixes are not supported,
 * as shearing can make the transformation ambiguous
 * (the same matrix can be reached by different angle,scaleX,scaleY,shearV,shearH).
 *
 * If the given matrix is invalid, or the scale sign is ambiguous,
 * the function returns an empty optional value.
 */
Q_MULTIMEDIA_EXPORT VideoTransformationOpt qVideoTransformationFromMatrix(const QTransform &matrix);

QT_END_NAMESPACE

#endif // QMULTIMEDIAUTILS_P_H

