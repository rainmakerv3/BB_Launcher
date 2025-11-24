// Copyright (C) 2022 The Qt Company
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QTMULTIMEDIAQUICKTYPES_H
#define QTMULTIMEDIAQUICKTYPES_H

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

#include <QtMultimediaQuick/private/qtmultimediaquickglobal_p.h>

#include <QtMultimedia/qaudiodevice.h>
#include <QtMultimedia/qaudioinput.h>
#include <QtMultimedia/qaudiooutput.h>
#include <QtMultimedia/qcamera.h>
#include <QtMultimedia/qcameradevice.h>
#include <QtMultimedia/qcapturablewindow.h>
#include <QtMultimedia/qimagecapture.h>
#include <QtMultimedia/qmediacapturesession.h>
#include <QtMultimedia/qmediadevices.h>
#include <QtMultimedia/qmediaformat.h>
#include <QtMultimedia/qmediametadata.h>
#include <QtMultimedia/qmediarecorder.h>
#include <QtMultimedia/qplaybackoptions.h>
#include <QtMultimedia/qscreencapture.h>
#include <QtMultimedia/qwindowcapture.h>

#include <QtQml/qqmlregistration.h>

QT_BEGIN_NAMESPACE

namespace QtMultimediaPrivate {

struct QMediaCaptureSessionForeign
{
    Q_GADGET
    QML_FOREIGN(QMediaCaptureSession)
    QML_NAMED_ELEMENT(CaptureSession) // ### MediaCaptureSession?
};

struct QCameraForeign
{
    Q_GADGET
    QML_FOREIGN(QCamera)
    QML_NAMED_ELEMENT(Camera)
};

struct QImageCaptureForeign
{
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(QImageCapture)
};

struct QScreenCaptureForeign
{
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(QScreenCapture)
};

struct QMediaRecorderForeign
{
    Q_GADGET
    QML_FOREIGN(QMediaRecorder)
    QML_NAMED_ELEMENT(MediaRecorder)
};

struct QMediaMetaDataForeign
{
    Q_GADGET
    QML_FOREIGN(QMediaMetaData)
    QML_VALUE_TYPE(mediaMetaData)
};

// To prevent the same type from being exported twice into qmltypes
// (for value type and for the enums)
struct QMediaMetaDataDerived : public QMediaMetaData
{
    Q_GADGET
};

namespace QMediaMetaDataNamespaceForeign
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QMediaMetaDataDerived)
    QML_NAMED_ELEMENT(MediaMetaData)
} // namespace QMediaMetaDataNamespaceForeign

struct QMediaDevicesForeign
{
    Q_GADGET
    QML_FOREIGN(QMediaDevices)
    QML_NAMED_ELEMENT(MediaDevices)
};

struct QAudioInputForeign
{
    Q_GADGET
    QML_FOREIGN(QAudioInput)
    QML_NAMED_ELEMENT(AudioInput)
};

struct QAudioOutputForeign
{
    Q_GADGET
    QML_FOREIGN(QAudioOutput)
    QML_NAMED_ELEMENT(AudioOutput)
};

struct QAudioDeviceForeign
{
    Q_GADGET
    QML_FOREIGN(QAudioDevice)
    QML_VALUE_TYPE(audioDevice)
};

// To prevent the same type from being exported twice into qmltypes
// (for value type and for the enums)
struct QAudioDeviceDerived : public QAudioDevice
{
    Q_GADGET
};

namespace QAudioDeviceNamespaceForeign
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QAudioDeviceDerived)
    QML_NAMED_ELEMENT(AudioDevice)
} // namespace QAudioDeviceNamespaceForeign

struct QCameraDeviceForeign
{
    Q_GADGET
    QML_FOREIGN(QCameraDevice)
    QML_VALUE_TYPE(cameraDevice)
};

// To prevent the same type from being exported twice into qmltypes
// (for value type and for the enums)
struct QCameraDeviceDerived : public QCameraDevice
{
    Q_GADGET
};

namespace QCameraDeviceNamespaceForeign
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QCameraDeviceDerived)
    QML_NAMED_ELEMENT(CameraDevice)
} // namespace QCameraDeviceNamespaceForeign

struct QMediaFormatForeign
{
    Q_GADGET
    QML_FOREIGN(QMediaFormat)
    QML_VALUE_TYPE(mediaFormat)
};

// To prevent the same type from being exported twice into qmltypes
// (for value type and for the enums)
struct QMediaFormatDerived : public QMediaFormat
{
    Q_GADGET
};

namespace QMediaFormatNamespaceForeign
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QMediaFormatDerived)
    QML_NAMED_ELEMENT(MediaFormat)
} // namespace QMediaFormatNamespaceForeign

struct QCameraFormatForeign
{
    Q_GADGET
    QML_FOREIGN(QCameraFormat)
    QML_VALUE_TYPE(cameraFormat)
};

struct QCapturableWindowForeign
{
    Q_GADGET
    QML_FOREIGN(QCapturableWindow)
    QML_VALUE_TYPE(capturableWindow)
    QML_CONSTRUCTIBLE_VALUE
};

struct QWindowCaptureForeign
{
    Q_GADGET
    QML_FOREIGN(QWindowCapture)
    QML_NAMED_ELEMENT(WindowCapture)
};

class QPlaybackOptionsDerived : public QPlaybackOptions
{
    Q_PROPERTY(qint64 networkTimeoutMs READ networkTimeoutMs WRITE setNetworkTimeoutMs RESET resetNetworkTimeoutMs FINAL)

    Q_GADGET
    QML_FOREIGN(QPlaybackOptions)
    QML_VALUE_TYPE(playbackOptions)
    QML_EXTENDED(QPlaybackOptionsDerived)
    QML_ADDED_IN_VERSION(6, 10)

public:
    qint64 networkTimeoutMs() const { return networkTimeout().count(); }

    void setNetworkTimeoutMs(qint64 timeout) { setNetworkTimeout(std::chrono::milliseconds(timeout)); }

    void resetNetworkTimeoutMs() { resetNetworkTimeout(); }
};

namespace QPlaybackOptionsNamespaceForeign {
    Q_NAMESPACE
    QML_NAMED_ELEMENT(PlaybackOptions)
    QML_FOREIGN_NAMESPACE(QPlaybackOptions)
    QML_ADDED_IN_VERSION(6, 10)
} // namespace QPlaybackOptionsNamespaceForeign



} // namespace QtMultimediaPrivate

QT_END_NAMESPACE

#endif
