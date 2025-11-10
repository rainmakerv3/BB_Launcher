// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLATFORMVIDEOSINK_H
#define QPLATFORMVIDEOSINK_H

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

#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qrect.h>
#include <QtCore/qsize.h>
#include <QtCore/qmutex.h>
#include <QtGui/qwindowdefs.h>
#include <qvideosink.h>
#include <qvideoframe.h>
#include <qdebug.h>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

// Required for QDoc workaround
class QString;

class Q_MULTIMEDIA_EXPORT QPlatformVideoSink : public QObject
{
    Q_OBJECT

public:
    ~QPlatformVideoSink() override;

    virtual void setRhi(QRhi * /*rhi*/) {}

    virtual void setWinId(WId) {}
    virtual void setDisplayRect(const QRect &) {};
    virtual void setFullScreen(bool) {}
    virtual void setAspectRatioMode(Qt::AspectRatioMode) {}

    QSize nativeSize() const;

    virtual void setBrightness(float /*brightness*/) {}
    virtual void setContrast(float /*contrast*/) {}
    virtual void setHue(float /*hue*/) {}
    virtual void setSaturation(float /*saturation*/) {}

    QVideoSink *videoSink() { return m_sink; }

    void setNativeSize(QSize s);

    void setVideoFrame(const QVideoFrame &frame);

    QVideoFrame currentVideoFrame() const;

    void setSubtitleText(const QString &subtitleText);

    QString subtitleText() const;

protected:
    explicit QPlatformVideoSink(QVideoSink *parent);

    virtual void onVideoFrameChanged(const QVideoFrame &) { }

Q_SIGNALS:
    void rhiChanged();

private:
    QVideoSink *const m_sink = nullptr;
    mutable QMutex m_mutex; // Protects all members below
    QSize m_nativeSize;
    QString m_subtitleText;
    QVideoFrame m_currentVideoFrame;
};

QT_END_NAMESPACE


#endif
