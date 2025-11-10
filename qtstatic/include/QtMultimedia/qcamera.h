// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QCAMERA_H
#define QCAMERA_H

#include <QtCore/qstringlist.h>
#include <QtCore/qsize.h>
#include <QtCore/qpoint.h>
#include <QtCore/qrect.h>

#include <QtCore/qobject.h>

#include <QtMultimedia/qcameradevice.h>

QT_BEGIN_NAMESPACE


class QCameraDevice;
class QPlatformMediaCaptureSession;
class QMediaCaptureSession;

class QCameraPrivate;
class Q_MULTIMEDIA_EXPORT QCamera : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    // Qt 7: rename to device
    Q_PROPERTY(QCameraDevice cameraDevice READ cameraDevice WRITE setCameraDevice NOTIFY cameraDeviceChanged)
    Q_PROPERTY(Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)
    Q_PROPERTY(QCameraFormat cameraFormat READ cameraFormat WRITE setCameraFormat NOTIFY cameraFormatChanged)

    Q_PROPERTY(FocusMode focusMode READ focusMode WRITE setFocusMode NOTIFY focusModeChanged)
    Q_PROPERTY(QPointF focusPoint READ focusPoint NOTIFY focusPointChanged)
    Q_PROPERTY(QPointF customFocusPoint READ customFocusPoint WRITE setCustomFocusPoint NOTIFY customFocusPointChanged)
    Q_PROPERTY(float focusDistance READ focusDistance WRITE setFocusDistance NOTIFY focusDistanceChanged)

    Q_PROPERTY(float minimumZoomFactor READ minimumZoomFactor NOTIFY minimumZoomFactorChanged)
    Q_PROPERTY(float maximumZoomFactor READ maximumZoomFactor NOTIFY maximumZoomFactorChanged)
    Q_PROPERTY(float zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(float exposureTime READ exposureTime NOTIFY exposureTimeChanged)
    Q_PROPERTY(float manualExposureTime READ manualExposureTime WRITE setManualExposureTime NOTIFY manualExposureTimeChanged)
    Q_PROPERTY(int isoSensitivity READ isoSensitivity NOTIFY isoSensitivityChanged)
    Q_PROPERTY(int manualIsoSensitivity READ manualIsoSensitivity WRITE setManualIsoSensitivity NOTIFY manualIsoSensitivityChanged)
    Q_PROPERTY(float exposureCompensation READ exposureCompensation WRITE setExposureCompensation NOTIFY exposureCompensationChanged)
    Q_PROPERTY(QCamera::ExposureMode exposureMode READ exposureMode WRITE setExposureMode NOTIFY exposureModeChanged)
    Q_PROPERTY(bool flashReady READ isFlashReady NOTIFY flashReady)
    Q_PROPERTY(QCamera::FlashMode flashMode READ flashMode WRITE setFlashMode NOTIFY flashModeChanged)
    Q_PROPERTY(QCamera::TorchMode torchMode READ torchMode WRITE setTorchMode NOTIFY torchModeChanged)

    Q_PROPERTY(WhiteBalanceMode whiteBalanceMode READ whiteBalanceMode WRITE setWhiteBalanceMode NOTIFY whiteBalanceModeChanged)
    Q_PROPERTY(int colorTemperature READ colorTemperature WRITE setColorTemperature NOTIFY colorTemperatureChanged)
    Q_PROPERTY(Features supportedFeatures READ supportedFeatures NOTIFY supportedFeaturesChanged)

public:
    enum Error
    {
        NoError,
        CameraError
    };
    Q_ENUM(Error)

    enum FocusMode {
        FocusModeAuto,
        FocusModeAutoNear,
        FocusModeAutoFar,
        FocusModeHyperfocal,
        FocusModeInfinity,
        FocusModeManual
    };
    Q_ENUM(FocusMode)

    enum FlashMode {
        FlashOff,
        FlashOn,
        FlashAuto
    };
    Q_ENUM(FlashMode)

    enum TorchMode {
        TorchOff,
        TorchOn,
        TorchAuto
    };
    Q_ENUM(TorchMode)

    enum ExposureMode {
        ExposureAuto,
        ExposureManual,
        ExposurePortrait,
        ExposureNight,
        ExposureSports,
        ExposureSnow,
        ExposureBeach,
        ExposureAction,
        ExposureLandscape,
        ExposureNightPortrait,
        ExposureTheatre,
        ExposureSunset,
        ExposureSteadyPhoto,
        ExposureFireworks,
        ExposureParty,
        ExposureCandlelight,
        ExposureBarcode
    };
    Q_ENUM(ExposureMode)

    enum WhiteBalanceMode {
        WhiteBalanceAuto = 0,
        WhiteBalanceManual = 1,
        WhiteBalanceSunlight = 2,
        WhiteBalanceCloudy = 3,
        WhiteBalanceShade = 4,
        WhiteBalanceTungsten = 5,
        WhiteBalanceFluorescent = 6,
        WhiteBalanceFlash = 7,
        WhiteBalanceSunset = 8
    };
    Q_ENUM(WhiteBalanceMode)

    enum class Feature {
        ColorTemperature = 0x1,
        ExposureCompensation = 0x2,
        IsoSensitivity = 0x4,
        ManualExposureTime = 0x8,
        CustomFocusPoint = 0x10,
        FocusDistance = 0x20
    };
    Q_DECLARE_FLAGS(Features, Feature)
    Q_FLAG(Features)

    explicit QCamera(QObject *parent = nullptr);
    explicit QCamera(const QCameraDevice& cameraDevice, QObject *parent = nullptr);
    explicit QCamera(QCameraDevice::Position position, QObject *parent = nullptr);
    ~QCamera() override;

    bool isAvailable() const;
    bool isActive() const;

    QMediaCaptureSession *captureSession() const;

    QCameraDevice cameraDevice() const;
    void setCameraDevice(const QCameraDevice &cameraDevice);

    QCameraFormat cameraFormat() const;
    void setCameraFormat(const QCameraFormat &format);

    Error error() const;
    QString errorString() const;

    Features supportedFeatures() const;

    FocusMode focusMode() const;
    void setFocusMode(FocusMode mode);
    Q_INVOKABLE bool isFocusModeSupported(FocusMode mode) const;

    QPointF focusPoint() const;

    QPointF customFocusPoint() const;
    void setCustomFocusPoint(const QPointF &point);

    void setFocusDistance(float d);
    float focusDistance() const;

    float minimumZoomFactor() const;
    float maximumZoomFactor() const;
    float zoomFactor() const;
    void setZoomFactor(float factor);

    FlashMode flashMode() const;
    Q_INVOKABLE bool isFlashModeSupported(FlashMode mode) const;
    Q_INVOKABLE bool isFlashReady() const;

    TorchMode torchMode() const;
    Q_INVOKABLE bool isTorchModeSupported(TorchMode mode) const;

    ExposureMode exposureMode() const;
    Q_INVOKABLE bool isExposureModeSupported(ExposureMode mode) const;

    float exposureCompensation() const;

    int isoSensitivity() const;
    int manualIsoSensitivity() const;

    float exposureTime() const;
    float manualExposureTime() const;

    int minimumIsoSensitivity() const;
    int maximumIsoSensitivity() const;

    float minimumExposureTime() const;
    float maximumExposureTime() const;

    WhiteBalanceMode whiteBalanceMode() const;
    Q_INVOKABLE bool isWhiteBalanceModeSupported(WhiteBalanceMode mode) const;

    int colorTemperature() const;

public Q_SLOTS:
    void setActive(bool active);
    void start() { setActive(true); }
    void stop() { setActive(false); }

    void zoomTo(float zoom, float rate);

    void setFlashMode(FlashMode mode);
    void setTorchMode(TorchMode mode);
    void setExposureMode(ExposureMode mode);

    void setExposureCompensation(float ev);

    void setManualIsoSensitivity(int iso);
    void setAutoIsoSensitivity();

    void setManualExposureTime(float seconds);
    void setAutoExposureTime();

    void setWhiteBalanceMode(WhiteBalanceMode mode);
    void setColorTemperature(int colorTemperature);

Q_SIGNALS:
    void activeChanged(bool);
    void errorChanged();
    void errorOccurred(QCamera::Error error, const QString &errorString);
    void cameraDeviceChanged();
    void cameraFormatChanged();
    void supportedFeaturesChanged();

    void focusModeChanged();
    void zoomFactorChanged(float);
    void minimumZoomFactorChanged(float);
    void maximumZoomFactorChanged(float);
    void focusDistanceChanged(float);
    void focusPointChanged();
    void customFocusPointChanged();

    void flashReady(bool);
    void flashModeChanged();
    void torchModeChanged();

    void exposureTimeChanged(float speed);
    void manualExposureTimeChanged(float speed);
    void isoSensitivityChanged(int);
    void manualIsoSensitivityChanged(int);
    void exposureCompensationChanged(float);
    void exposureModeChanged();

    void whiteBalanceModeChanged() QT6_ONLY(const);
    void colorTemperatureChanged() QT6_ONLY(const);
    void brightnessChanged();
    void contrastChanged();
    void saturationChanged();
    void hueChanged();

private:
    class QPlatformCamera *platformCamera();
    void setCaptureSession(QMediaCaptureSession *session);
    friend class QMediaCaptureSession;
    Q_DISABLE_COPY(QCamera)
    Q_DECLARE_PRIVATE(QCamera)
    friend class QCameraDevice;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QCamera::Features)

QT_END_NAMESPACE

#endif  // QCAMERA_H
