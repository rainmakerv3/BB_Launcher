// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QCAMERAIMAGECAPTURECONTROL_H
#define QCAMERAIMAGECAPTURECONTROL_H

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

#include <QtMultimedia/qimagecapture.h>
#include <QtMultimedia/qmediametadata.h>
#include <QtMultimedia/qimagecapture.h>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class QImage;
class QPlatformMediaCaptureSession;

class QImageEncoderSettingsPrivate;
class Q_MULTIMEDIA_EXPORT QImageEncoderSettings
{
    QImageCapture::FileFormat m_format = QImageCapture::UnspecifiedFormat;
    QSize m_resolution;
    QImageCapture::Quality m_quality = QImageCapture::NormalQuality;

public:
    bool operator==(const QImageEncoderSettings &other) {
        return m_format == other.m_format &&
               m_resolution == other.m_resolution &&
               m_quality == other.m_quality;
    }
    bool operator!=(const QImageEncoderSettings &other) { return !operator==(other); }

    QImageCapture::FileFormat format() const { return m_format; }
    void setFormat(QImageCapture::FileFormat f) { m_format = f; }

    QSize resolution() const { return m_resolution; }
    void setResolution(const QSize &s) { m_resolution = s; }
    void setResolution(int width, int height) { m_resolution = QSize(width, height); }

    QImageCapture::Quality quality() const { return m_quality; }
    void setQuality(QImageCapture::Quality quality) { m_quality = quality; }
};

class Q_MULTIMEDIA_EXPORT QPlatformImageCapture : public QObject
{
    Q_OBJECT

public:
    virtual bool isReadyForCapture() const = 0;

    virtual int capture(const QString &fileName) = 0;
    virtual int captureToBuffer() = 0;

    virtual QImageEncoderSettings imageSettings() const = 0;
    virtual void setImageSettings(const QImageEncoderSettings &settings) = 0;

    virtual void setMetaData(const QMediaMetaData &m) { m_metaData = m; }
    QMediaMetaData metaData() const { return m_metaData; }

    QImageCapture *imageCapture() { return m_imageCapture; }

    static QString msgCameraNotReady();
    static QString msgImageCaptureNotSet();

Q_SIGNALS:
    void readyForCaptureChanged(bool ready);

    void imageExposed(int requestId);
    void imageCaptured(int requestId, const QImage &preview);
    void imageMetadataAvailable(int id, const QMediaMetaData &);
    void imageAvailable(int requestId, const QVideoFrame &buffer);
    void imageSaved(int requestId, const QString &fileName);

    void error(int id, int error, const QString &errorString);

protected:
    explicit QPlatformImageCapture(QImageCapture *parent = nullptr);
private:
    QImageCapture *m_imageCapture = nullptr;
    QMediaMetaData m_metaData;
};

QT_END_NAMESPACE


#endif  // QCAMERAIMAGECAPTURECONTROL_H

