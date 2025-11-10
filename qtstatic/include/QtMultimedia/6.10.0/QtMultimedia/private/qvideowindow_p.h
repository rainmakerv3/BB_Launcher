// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QVIDEOWINDOW_P_H
#define QVIDEOWINDOW_P_H

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

#include <QWindow>
#include <QOffscreenSurface>
#include <qtextlayout.h>
#include <rhi/qrhi.h>
#include <qvideoframe.h>
#include <private/qplatformvideosink_p.h>
#include <private/qvideotexturehelper_p.h>
#include <private/qvideoframetexturepool_p.h>
#include <qbackingstore.h>

QT_BEGIN_NAMESPACE

class QVideoWindow;

class QVideoWindowPrivate {
public:
    QVideoWindowPrivate(QVideoWindow *q);
    ~QVideoWindowPrivate();
    bool canRender() const { return m_useRhi; }

    QRhi *rhi() const { return m_rhi.get(); }

    void init();
    void render();

    void initRhi();

    void resizeSwapChain();
    void releaseSwapChain();

    void updateTextures(QRhiResourceUpdateBatch *rub);
    void updateSubtitle(QRhiResourceUpdateBatch *rub, const QSize &frameSize);

    void setupGraphicsPipeline(QRhiGraphicsPipeline *pipeline, QRhiShaderResourceBindings *bindings, const QVideoFrameFormat &fmt);

    QVideoWindow *q = nullptr;
    Qt::AspectRatioMode aspectRatioMode = Qt::KeepAspectRatio;

    QBackingStore *backingStore = nullptr;

#if QT_CONFIG(opengl)
    std::unique_ptr<QOffscreenSurface> m_fallbackSurface;
#endif
    std::unique_ptr<QRhi> m_rhi;
    std::unique_ptr<QRhiSwapChain> m_swapChain;
    std::unique_ptr<QRhiRenderPassDescriptor> m_renderPass;

    std::unique_ptr<QRhiBuffer> m_vertexBuf;
    bool m_vertexBufReady = false;
    std::unique_ptr<QRhiBuffer> m_uniformBuf;
    std::unique_ptr<QRhiSampler> m_textureSampler;
    std::unique_ptr<QRhiShaderResourceBindings> m_shaderResourceBindings;
    std::unique_ptr<QRhiGraphicsPipeline> m_graphicsPipeline;

    std::unique_ptr<QRhiTexture> m_subtitleTexture;
    std::unique_ptr<QRhiShaderResourceBindings> m_subtitleResourceBindings;
    std::unique_ptr<QRhiGraphicsPipeline> m_subtitlePipeline;
    std::unique_ptr<QRhiBuffer> m_subtitleUniformBuf;

    std::unique_ptr<QVideoSink> m_sink;
    QRhi::Implementation m_graphicsApi = QRhi::Null;
    QVideoTextureHelper::SubtitleLayout m_subtitleLayout;
    QVideoFrameTexturePool m_texturePool;

    bool initialized = false;
    bool isExposed = false;
    bool m_useRhi = true;
    bool m_hasSwapChain = false;
    bool m_subtitleDirty = false;
    bool m_hasSubtitle = false;
    QVideoFrameFormat format;
};

class Q_MULTIMEDIA_EXPORT QVideoWindow : public QWindow
{
    Q_OBJECT
public:
    explicit QVideoWindow(QScreen *screen = nullptr);
    explicit QVideoWindow(QWindow *parent);
    ~QVideoWindow() override;

    Q_INVOKABLE QVideoSink *videoSink() const;

    Qt::AspectRatioMode aspectRatioMode() const;

public Q_SLOTS:
    void setAspectRatioMode(Qt::AspectRatioMode mode);

Q_SIGNALS:
    void aspectRatioModeChanged(Qt::AspectRatioMode mode);

protected:
    bool event(QEvent *e) override;
    void resizeEvent(QResizeEvent *) override;

private Q_SLOTS:
    void setVideoFrame(const QVideoFrame &frame);

private:
    friend class QVideoWindowPrivate;
    std::unique_ptr<QVideoWindowPrivate> d;
};

QT_END_NAMESPACE

#endif
