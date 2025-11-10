// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QCAMERAIMAGECAPTURE_H
#define QCAMERAIMAGECAPTURE_H

#include <QtCore/qobject.h>
#include <QtMultimedia/qvideoframe.h>

QT_BEGIN_NAMESPACE

class QSize;
class QMediaMetaData;
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE

class QImageEncoderSettings;
class QCamera;
class QMediaCaptureSession;

class QImageCapturePrivate;
class Q_MULTIMEDIA_EXPORT QImageCapture : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool readyForCapture READ isReadyForCapture NOTIFY readyForCaptureChanged)
    Q_PROPERTY(QMediaMetaData metaData READ metaData WRITE setMetaData NOTIFY metaDataChanged)
    Q_PROPERTY(Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)
    Q_PROPERTY(FileFormat fileFormat READ fileFormat WRITE setFileFormat NOTIFY fileFormatChanged)
    Q_PROPERTY(Quality quality READ quality WRITE setQuality NOTIFY qualityChanged)
    Q_PROPERTY(QList<FileFormat> supportedFormats READ supportedFormats CONSTANT REVISION(6, 10))
public:
    enum Error
    {
        NoError,
        NotReadyError,
        ResourceError,
        OutOfSpaceError,
        NotSupportedFeatureError,
        FormatError
    };
    Q_ENUM(Error)

    enum Quality
    {
        VeryLowQuality,
        LowQuality,
        NormalQuality,
        HighQuality,
        VeryHighQuality
    };
    Q_ENUM(Quality)

    enum FileFormat {
        UnspecifiedFormat,
        JPEG,
        PNG,
        WebP,
        Tiff,
        LastFileFormat = Tiff
    };
    Q_ENUM(FileFormat)

    explicit QImageCapture(QObject *parent = nullptr);
    ~QImageCapture() override;

    bool isAvailable() const;

    QMediaCaptureSession *captureSession() const;

    Error error() const;
    QString errorString() const;

    bool isReadyForCapture() const;

    FileFormat fileFormat() const;
    void setFileFormat(FileFormat format);

    static QList<FileFormat> supportedFormats();
    static QString fileFormatName(FileFormat c);
    static QString fileFormatDescription(FileFormat c);

    QSize resolution() const;
    void setResolution(const QSize &);
    void setResolution(int width, int height);

    Quality quality() const;
    void setQuality(Quality quality);

    QMediaMetaData metaData() const;
    void setMetaData(const QMediaMetaData &metaData);
    void addMetaData(const QMediaMetaData &metaData);

public Q_SLOTS:
    int captureToFile(const QString &location = QString());
    int capture();

Q_SIGNALS:
    void errorChanged();
    void errorOccurred(int id, QImageCapture::Error error, const QString &errorString);

    void readyForCaptureChanged(bool ready);
    void metaDataChanged();

    void fileFormatChanged();
    void qualityChanged();
    void resolutionChanged();

    void imageExposed(int id);
    void imageCaptured(int id, const QImage &preview);
    void imageMetadataAvailable(int id, const QMediaMetaData &metaData);
    void imageAvailable(int id, const QVideoFrame &frame);
    void imageSaved(int id, const QString &fileName);

private:
    // This is here to flag an incompatibilities with Qt 5
    QImageCapture(QCamera *) = delete;

    friend class QMediaCaptureSession;
    class QPlatformImageCapture *platformImageCapture();
    void setCaptureSession(QMediaCaptureSession *session);
    QImageCapturePrivate *d_ptr;
    Q_DISABLE_COPY(QImageCapture)
    Q_DECLARE_PRIVATE(QImageCapture)
    Q_PRIVATE_SLOT(d_func(), void _q_error(int, int, const QString &))
};

QT_END_NAMESPACE

#endif

