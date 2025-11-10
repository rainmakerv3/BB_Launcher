// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QPLATFORMMEDIAINTEGRATION_H
#define QPLATFORMMEDIAINTEGRATION_H

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

#include <QtCore/qstring.h>
#include <QtCore/private/qexpected_p.h>

#include <QtMultimedia/qcapturablewindow.h>
#include <QtMultimedia/qmediarecorder.h>
#include <QtMultimedia/private/qmultimediautils_p.h>
#include <QtMultimedia/private/qtmultimediaglobal_p.h>

#include <memory>
#include <mutex>

QT_BEGIN_NAMESPACE

class QAudioDecoder;
class QAudioFormat;
class QAudioInput;
class QAudioOutput;
class QCamera;
class QCameraDevice;
class QCapturableWindow;
class QImageCapture;
class QMediaDevices;
class QMediaPlayer;
class QMediaRecorder;
class QObject;
class QPlatformAudioDecoder;
class QPlatformAudioDevices;
class QPlatformAudioInput;
class QPlatformAudioOutput;
class QPlatformAudioResampler;
class QPlatformCamera;
class QPlatformCapturableWindows;
class QPlatformImageCapture;
class QPlatformMediaCaptureSession;
class QPlatformMediaFormatInfo;
class QPlatformMediaPlayer;
class QPlatformMediaRecorder;
class QPlatformSurfaceCapture;
class QPlatformVideoDevices;
class QPlatformVideoSink;
class QScreenCapture;
class QVideoFrame;
class QVideoSink;
class QWindowCapture;

class Q_MULTIMEDIA_EXPORT QAbstractPlatformSpecificInterface
{
public:
    virtual ~QAbstractPlatformSpecificInterface() = default;
};

class Q_MULTIMEDIA_EXPORT QPlatformMediaIntegration : public QObject
{
    Q_OBJECT
public:
    static QPlatformMediaIntegration *instance();

    explicit QPlatformMediaIntegration(QLatin1String);
    ~QPlatformMediaIntegration() override;
    const QPlatformMediaFormatInfo *formatInfo();

    virtual q23::expected<QPlatformCamera *, QString> createCamera(QCamera *)
    {
        return q23::unexpected{ notAvailable };
    }
    virtual QPlatformSurfaceCapture *createScreenCapture(QScreenCapture *) { return nullptr; }
    virtual QPlatformSurfaceCapture *createWindowCapture(QWindowCapture *) { return nullptr; }

    virtual q23::expected<QPlatformAudioDecoder *, QString> createAudioDecoder(QAudioDecoder *)
    {
        return q23::unexpected{ notAvailable };
    }
    virtual q23::expected<std::unique_ptr<QPlatformAudioResampler>, QString>
    createAudioResampler(const QAudioFormat & /*inputFormat*/,
                         const QAudioFormat & /*outputFormat*/);
    virtual q23::expected<QPlatformMediaCaptureSession *, QString> createCaptureSession()
    {
        return q23::unexpected{ notAvailable };
    }
    virtual q23::expected<QPlatformMediaPlayer *, QString> createPlayer(QMediaPlayer *)
    {
        return q23::unexpected{ notAvailable };
    }
    virtual q23::expected<QPlatformMediaRecorder *, QString> createRecorder(QMediaRecorder *)
    {
        return q23::unexpected{ notAvailable };
    }
    virtual q23::expected<QPlatformImageCapture *, QString> createImageCapture(QImageCapture *)
    {
        return q23::unexpected{ notAvailable };
    }

    virtual q23::expected<QPlatformAudioInput *, QString> createAudioInput(QAudioInput *);
    virtual q23::expected<QPlatformAudioOutput *, QString> createAudioOutput(QAudioOutput *);

    virtual q23::expected<QPlatformVideoSink *, QString> createVideoSink(QVideoSink *)
    {
        return q23::unexpected{ notAvailable };
    }

    QList<QCapturableWindow> capturableWindowsList();
    bool isCapturableWindowValid(const QCapturableWindowPrivate &);
    [[nodiscard]] q23::expected<QCapturableWindow, QString> capturableWindowFromQWindow(QWindow *);

    QPlatformVideoDevices *videoDevices();

    QPlatformCapturableWindows *capturableWindows();

    QPlatformAudioDevices *audioDevices();

    static QStringList availableBackends();
    QLatin1String name(); // for unit tests

    // Convert a QVideoFrame to the destination format
    virtual QVideoFrame convertVideoFrame(QVideoFrame &, const QVideoFrameFormat &);

    virtual QAbstractPlatformSpecificInterface *platformSpecificInterface() { return nullptr; }

    static QLatin1String audioBackendName();

protected:
    virtual QPlatformMediaFormatInfo *createFormatInfo();

    virtual QPlatformVideoDevices *createVideoDevices() { return nullptr; }

    virtual QPlatformCapturableWindows *createCapturableWindows() { return nullptr; }

    virtual std::unique_ptr<QPlatformAudioDevices> createAudioDevices();

    inline static const QString notAvailable = QStringLiteral("Not available");

private:
    friend class QMockIntegration;
    void resetInstance(); // tests only

private:
    std::unique_ptr<QPlatformVideoDevices> m_videoDevices;
    std::once_flag m_videoDevicesOnceFlag;

    std::unique_ptr<QPlatformCapturableWindows> m_capturableWindows;
    std::once_flag m_capturableWindowsOnceFlag;

    mutable std::unique_ptr<QPlatformMediaFormatInfo> m_formatInfo;
    mutable std::once_flag m_formatInfoOnceFlg;

    std::unique_ptr<QPlatformAudioDevices> m_audioDevices;
    std::once_flag m_audioDevicesOnceFlag;

    const QLatin1String m_backendName;
};

QT_END_NAMESPACE

#endif // QPLATFORMMEDIAINTERFACE_H
