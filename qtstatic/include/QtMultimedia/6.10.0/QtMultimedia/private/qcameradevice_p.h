// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QCAMERAINFO_P_H
#define QCAMERAINFO_P_H

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

#include <QtMultimedia/qcameradevice.h>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class QCameraFormatPrivate : public QSharedData
{
public:
    QVideoFrameFormat::PixelFormat pixelFormat = QVideoFrameFormat::Format_Invalid;
    QSize resolution;
    float minFrameRate = 0;
    float maxFrameRate = 0;
    QVideoFrameFormat::ColorRange colorRange = QVideoFrameFormat::ColorRange_Unknown;

    static QVideoFrameFormat::ColorRange getColorRange(const QCameraFormat &format)
    {
        auto d = handle(format);
        return d ? d->colorRange : QVideoFrameFormat::ColorRange_Unknown;
    }

    static const QCameraFormatPrivate *handle(const QCameraFormat &format)
    {
        return format.d.get();
    }

    QCameraFormat create() { return QCameraFormat(this); }
};

class QCameraDevicePrivate : public QSharedData
{
public:
    QByteArray id;
    QString description;
    bool isDefault = false;
    QCameraDevice::Position position = QCameraDevice::UnspecifiedPosition;
    int orientation = 0;
    QList<QSize> photoResolutions;
    QList<QCameraFormat> videoFormats;

    static const QCameraDevicePrivate *handle(const QCameraDevice &device)
    {
        return device.d.data();
    }

    bool operator==(const QCameraDevicePrivate &other) const
    {
        return id == other.id && description == other.description && isDefault == other.isDefault
                && position == other.position && orientation == other.orientation
                && photoResolutions == other.photoResolutions && videoFormats == other.videoFormats;
    }

    QCameraDevice create() { return QCameraDevice(this); }
};

QT_END_NAMESPACE

#endif // QCAMERAINFO_P_H
