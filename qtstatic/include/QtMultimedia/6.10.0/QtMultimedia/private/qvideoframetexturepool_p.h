// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QVIDEOFRAMETEXTUREPOOL_P_H
#define QVIDEOFRAMETEXTUREPOOL_P_H

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

#include <array>
#include <optional>

QT_BEGIN_NAMESPACE

class QRhi;
class QRhiResourceUpdateBatch;

/**
 * @brief The class QVideoFrameTexturePool stores textures in slots to ensure
 *        they are alive during rhi's rendering rounds.
 *        Depending on the rhi backend, 1, 2, or 3 rounds are needed
 *        to complete the texture presentaton.
 *        The strategy of slots filling is based on QRhi::currentFrameSlot results.
 */
class Q_MULTIMEDIA_EXPORT QVideoFrameTexturePool {
    static constexpr size_t MaxSlotsCount = 4;
public:
    /**
     * @brief The flag indicates whether the textures need update.
     *        Whenever a new current frame is set, the flag is turning into true.
     */
    bool texturesDirty() const { return m_texturesDirty; }

    const QVideoFrame& currentFrame() const { return m_currentFrame; }

    /**
     * @brief The method sets the current frame to be converted into textures.
     *        The flag texturesDirty becomes true after setting a new frame.
     */
    void setCurrentFrame(QVideoFrame frame);

    /**
     * @brief The method updates textures basing on the current frame.
     *        It's recommended to invoke it during rhi's rendering,
     *        in other words, between QRhi::beginFrame and QRhi::endFrame.
     *        The method resets texturesDirty to false.
     *
     * @return the pointer to the updated texture or null if failed
     */
    QVideoFrameTextures* updateTextures(QRhi &rhi, QRhiResourceUpdateBatch &rub);

    /**
     * @brief The method should be invoked after finishing QRhi::endFrame.
     *        It propagates the call to the current texture in order to
     *        free resources that are not needed anymore.
     */
    void onFrameEndInvoked();

    /**
     * @brief The method clears all texture slots and sets the dirty flag if
     *        the current frame is valid.
     */
    void clearTextures();

private:
    QVideoFrame m_currentFrame;
    bool m_texturesDirty = false;
    std::array<QVideoFrameTexturesUPtr, MaxSlotsCount> m_textureSlots;
    std::optional<int> m_currentSlot;
    QVideoFrameTexturesUPtr m_oldTextures;
};

QT_END_NAMESPACE

#endif // QVIDEOFRAMETEXTUREPOOL_P_H
