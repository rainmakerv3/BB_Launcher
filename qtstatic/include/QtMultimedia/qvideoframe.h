// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QVIDEOFRAME_H
#define QVIDEOFRAME_H

#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtMultimedia/qtvideo.h>
#include <QtMultimedia/qvideoframeformat.h>

#include <QtCore/qmetatype.h>
#include <QtCore/qshareddata.h>
#include <QtGui/qimage.h>

QT_BEGIN_NAMESPACE

class QSize;
class QVideoFramePrivate;
class QAbstractVideoBuffer;
class QRhi;
class QRhiResourceUpdateBatch;
class QRhiTexture;

QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QVideoFramePrivate, Q_MULTIMEDIA_EXPORT)

class Q_MULTIMEDIA_EXPORT QVideoFrame
{
    Q_GADGET
public:

    enum HandleType
    {
        NoHandle,
        RhiTextureHandle
    };

    enum MapMode
    {
        NotMapped = 0x00,
        ReadOnly  = 0x01,
        WriteOnly = 0x02,
        ReadWrite = ReadOnly | WriteOnly
    };
    Q_ENUM(MapMode)

#if QT_DEPRECATED_SINCE(6, 7)
    enum RotationAngle
    {
        Rotation0 Q_DECL_ENUMERATOR_DEPRECATED_X("Use QtVideo::Rotation::None instead") = 0,
        Rotation90 Q_DECL_ENUMERATOR_DEPRECATED_X("Use QtVideo::Rotation::Clockwise90 instead") = 90,
        Rotation180 Q_DECL_ENUMERATOR_DEPRECATED_X("Use QtVideo::Rotation::Clockwise180 instead") = 180,
        Rotation270 Q_DECL_ENUMERATOR_DEPRECATED_X("Use QtVideo::Rotation::Clockwise270 instead") = 270
    };
#endif

    QVideoFrame();
    QVideoFrame(const QVideoFrameFormat &format);
    explicit QVideoFrame(const QImage &image);
    explicit QVideoFrame(std::unique_ptr<QAbstractVideoBuffer> videoBuffer);
    QVideoFrame(const QVideoFrame &other);
    ~QVideoFrame();

    QVideoFrame(QVideoFrame &&other) noexcept = default;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QVideoFrame)
    void swap(QVideoFrame &other) noexcept
    { d.swap(other.d); }


    QVideoFrame &operator =(const QVideoFrame &other);
    bool operator==(const QVideoFrame &other) const;
    bool operator!=(const QVideoFrame &other) const;

    bool isValid() const;

    QVideoFrameFormat::PixelFormat pixelFormat() const;

    QVideoFrameFormat surfaceFormat() const;
    QVideoFrame::HandleType handleType() const;

    QSize size() const;
    int width() const;
    int height() const;

    bool isMapped() const;
    bool isReadable() const;
    bool isWritable() const;

    QVideoFrame::MapMode mapMode() const;

    bool map(QVideoFrame::MapMode mode);
    void unmap();

    int bytesPerLine(int plane) const;

    uchar *bits(int plane);
    const uchar *bits(int plane) const;
    int mappedBytes(int plane) const;
    int planeCount() const;

    qint64 startTime() const;
    void setStartTime(qint64 time);

    qint64 endTime() const;
    void setEndTime(qint64 time);

#if QT_DEPRECATED_SINCE(6, 7)
    QT_DEPRECATED_VERSION_X_6_7("Use QVideoFrame::setRotation(QtVideo::Rotation) instead")
    void setRotationAngle(RotationAngle angle) { setRotation(QtVideo::Rotation(angle)); }

    QT_DEPRECATED_VERSION_X_6_7("Use QVideoFrame::rotation() instead")
    RotationAngle rotationAngle() const { return RotationAngle(rotation()); }
#endif

    void setRotation(QtVideo::Rotation angle);
    QtVideo::Rotation rotation() const;

    void setMirrored(bool);
    bool mirrored() const;

    void setStreamFrameRate(qreal rate);
    qreal streamFrameRate() const;

    QImage toImage() const;

    struct PaintOptions {
        QColor backgroundColor = Qt::transparent;
        Qt::AspectRatioMode aspectRatioMode = Qt::KeepAspectRatio;
        enum PaintFlag {
            DontDrawSubtitles = 0x1
        };
        Q_DECLARE_FLAGS(PaintFlags, PaintFlag)
        PaintFlags paintFlags = {};
    };

    QString subtitleText() const;
    void setSubtitleText(const QString &text);

    void paint(QPainter *painter, const QRectF &rect, const PaintOptions &options);

#if QT_DEPRECATED_SINCE(6, 8)
    QT_DEPRECATED_VERSION_X_6_8("The constructor is internal and deprecated")
    QVideoFrame(QAbstractVideoBuffer *buffer, const QVideoFrameFormat &format);

    QT_DEPRECATED_VERSION_X_6_8("The method is internal and deprecated")
    QAbstractVideoBuffer *videoBuffer() const;
#endif
private:
    friend class QVideoFramePrivate;
    QExplicitlySharedDataPointer<QVideoFramePrivate> d;
};

Q_DECLARE_SHARED(QVideoFrame)

#ifndef QT_NO_DEBUG_STREAM
Q_MULTIMEDIA_EXPORT QDebug operator<<(QDebug, const QVideoFrame&);
Q_MULTIMEDIA_EXPORT QDebug operator<<(QDebug, QVideoFrame::HandleType);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QVideoFrame)

#endif

