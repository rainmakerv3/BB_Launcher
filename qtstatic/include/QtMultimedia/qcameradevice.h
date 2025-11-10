// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QCAMERAINFO_H
#define QCAMERAINFO_H

#include <QtCore/qshareddata.h>
#include <QtMultimedia/qtvideo.h>
#include <QtMultimedia/qvideoframe.h>

QT_BEGIN_NAMESPACE

class QCameraFormatPrivate;
class Q_MULTIMEDIA_EXPORT QCameraFormat
{
    Q_GADGET
    Q_PROPERTY(QSize resolution READ resolution CONSTANT)
    Q_PROPERTY(QVideoFrameFormat::PixelFormat pixelFormat READ pixelFormat CONSTANT)
    Q_PROPERTY(float minFrameRate READ minFrameRate CONSTANT)
    Q_PROPERTY(float maxFrameRate READ maxFrameRate CONSTANT)
public:
    QCameraFormat() noexcept;
    QCameraFormat(const QCameraFormat &other) noexcept;
    QCameraFormat &operator=(const QCameraFormat &other) noexcept;
    ~QCameraFormat();

    QVideoFrameFormat::PixelFormat pixelFormat() const noexcept;
    QSize resolution() const noexcept;
    float minFrameRate() const noexcept;
    float maxFrameRate() const noexcept;

    bool isNull() const noexcept { return !d; }

    bool operator==(const QCameraFormat &other) const;
    inline bool operator!=(const QCameraFormat &other) const
    { return !operator==(other); }

private:
    friend class QCameraFormatPrivate;
    QCameraFormat(QCameraFormatPrivate *p);
    QExplicitlySharedDataPointer<QCameraFormatPrivate> d;
};

class QCameraDevicePrivate;
class Q_MULTIMEDIA_EXPORT QCameraDevice
{
    Q_GADGET
    Q_PROPERTY(QByteArray id READ id CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(bool isDefault READ isDefault CONSTANT)
    Q_PROPERTY(Position position READ position CONSTANT)
    Q_PROPERTY(QList<QCameraFormat> videoFormats READ videoFormats CONSTANT)
    Q_PROPERTY(QtVideo::Rotation correctionAngle READ correctionAngle CONSTANT)
public:
    QCameraDevice();
    QCameraDevice(const QCameraDevice& other);
    QCameraDevice& operator=(const QCameraDevice& other);
    ~QCameraDevice();

    bool operator==(const QCameraDevice &other) const;
    inline bool operator!=(const QCameraDevice &other) const;

    bool isNull() const;

    QByteArray id() const;
    QString description() const;

    // ### Add here and to QAudioDevice
//    QByteArray groupId() const;
//    QString groupDescription() const;

    bool isDefault() const;

    enum Position
    {
        UnspecifiedPosition,
        BackFace,
        FrontFace
    };
    Q_ENUM(Position)

    Position position() const;

    QList<QSize> photoResolutions() const;
    QList<QCameraFormat> videoFormats() const;

    QtVideo::Rotation correctionAngle() const;
    // ### Add zoom and other camera information

private:
    friend class QCameraDevicePrivate;
    QCameraDevice(QCameraDevicePrivate *p);
    QExplicitlySharedDataPointer<QCameraDevicePrivate> d;
};

bool QCameraDevice::operator!=(const QCameraDevice &other) const { return !operator==(other); }

#ifndef QT_NO_DEBUG_STREAM
Q_MULTIMEDIA_EXPORT QDebug operator<<(QDebug, const QCameraDevice&);
#endif

QT_END_NAMESPACE

#endif // QCAMERAINFO_H
