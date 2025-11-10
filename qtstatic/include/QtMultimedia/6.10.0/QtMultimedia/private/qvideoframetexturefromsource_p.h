// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QVIDEOFRAMETEXTUREFROMSOURCE_P_H
#define QVIDEOFRAMETEXTUREFROMSOURCE_P_H

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

#include "private/qhwvideobuffer_p.h"
#include "qvideotexturehelper_p.h"

#include <rhi/qrhi.h>

QT_BEGIN_NAMESPACE

namespace QVideoTextureHelper {

using RhiTextureArray = std::array<std::unique_ptr<QRhiTexture>, TextureDescription::maxPlanes>;

class QVideoFrameTexturesFromRhiTextureArray : public QVideoFrameTextures
{
public:
    QVideoFrameTexturesFromRhiTextureArray(RhiTextureArray &&rhiTextures = {});

    ~QVideoFrameTexturesFromRhiTextureArray() override;

    QRhiTexture *texture(uint plane) const override;

    RhiTextureArray &textureArray() { return m_rhiTextures; }

private:
    RhiTextureArray m_rhiTextures;
};

class QVideoFrameTexturesFromMemory : public QVideoFrameTexturesFromRhiTextureArray
{
public:
    using QVideoFrameTexturesFromRhiTextureArray::QVideoFrameTexturesFromRhiTextureArray;

    void setMappedFrame(QVideoFrame mappedFrame);

    ~QVideoFrameTexturesFromMemory() override;

    void onFrameEndInvoked() override;

private:
    QVideoFrame m_mappedFrame;
};

class QVideoFrameTexturesFromHandlesSet : public QVideoFrameTexturesFromRhiTextureArray
{
public:
    QVideoFrameTexturesFromHandlesSet(RhiTextureArray &&rhiTextures,
                                      QVideoFrameTexturesHandlesUPtr handles);

    ~QVideoFrameTexturesFromHandlesSet() override;

    QVideoFrameTexturesHandlesUPtr takeHandles() override { return std::move(m_textureHandles); }

private:
    QVideoFrameTexturesHandlesUPtr m_textureHandles;
};

} // namespace QVideoTextureHelper

QT_END_NAMESPACE

#endif // QVIDEOFRAMETEXTUREFROMSOURCE_P_H
