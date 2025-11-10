// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QVIDEOTRANSFORMATION_P_H
#define QVIDEOTRANSFORMATION_P_H

#include <qtvideo.h>
#include <optional>

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

QT_BEGIN_NAMESPACE

struct VideoTransformation
{
    QtVideo::Rotation rotation = QtVideo::Rotation::None;
    bool mirroredHorizontallyAfterRotation = false;

    void rotate(QtVideo::Rotation rotation)
    {
        if (rotation != QtVideo::Rotation::None) {
            int angle = qToUnderlying(rotation);
            if (mirroredHorizontallyAfterRotation && angle % 180 != 0)
                angle += 180;

            appendRotation(angle);
        }
    }

    void mirrorHorizontally(bool mirror = true) { mirroredHorizontallyAfterRotation ^= mirror; }

    void mirrorVertically(bool mirror = true)
    {
        if (mirror) {
            mirroredHorizontallyAfterRotation ^= true;
            appendRotation(180);
        }
    }

    int rotationIndex() const { return qToUnderlying(rotation) / 90; }

private:
    void appendRotation(quint32 angle)
    {
        rotation = QtVideo::Rotation((angle + qToUnderlying(rotation)) % 360);
    }
};

using VideoTransformationOpt = std::optional<VideoTransformation>;

inline bool operator==(const VideoTransformation &lhs, const VideoTransformation &rhs)
{
    return lhs.rotation == rhs.rotation
            && lhs.mirroredHorizontallyAfterRotation == rhs.mirroredHorizontallyAfterRotation;
}

inline bool operator!=(const VideoTransformation &lhs, const VideoTransformation &rhs)
{
    return !(lhs == rhs);
}

Q_MULTIMEDIA_EXPORT QDebug operator<<(QDebug dbg, const VideoTransformation &transform);

QT_END_NAMESPACE

#endif // QVIDEOTRANSFORMATION_P_H
