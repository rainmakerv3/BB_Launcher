// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QABSTRACTVIDEOSINK_H
#define QABSTRACTVIDEOSINK_H

#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtCore/qobject.h>
#include <QtGui/qwindowdefs.h>

QT_BEGIN_NAMESPACE

class QRectF;
class QVideoFrameFormat;
class QVideoFrame;

class QVideoSinkPrivate;
class QPlatformVideoSink;
class QRhi;

class Q_MULTIMEDIA_EXPORT QVideoSink : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString subtitleText READ subtitleText WRITE setSubtitleText NOTIFY subtitleTextChanged)
    Q_PROPERTY(QSize videoSize READ videoSize NOTIFY videoSizeChanged)
public:
    QVideoSink(QObject *parent = nullptr);
    ~QVideoSink() override;

    QRhi *rhi() const;
    void setRhi(QRhi *rhi);

    QSize videoSize() const;

    QString subtitleText() const;
    void setSubtitleText(const QString &subtitle);

    void setVideoFrame(const QVideoFrame &frame);
    QVideoFrame videoFrame() const;

    QPlatformVideoSink *platformVideoSink() const;
Q_SIGNALS:
    void videoFrameChanged(const QVideoFrame &frame) QT6_ONLY(const);
    void subtitleTextChanged(const QString &subtitleText) QT6_ONLY(const);
    void videoSizeChanged();

private:
    friend class QMediaPlayerPrivate;
    friend class QMediaCaptureSessionPrivate;
    void setSource(QObject *source);

    QVideoSinkPrivate *d = nullptr;
};

QT_END_NAMESPACE

#endif
