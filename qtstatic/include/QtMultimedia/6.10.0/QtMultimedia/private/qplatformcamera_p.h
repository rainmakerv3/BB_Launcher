// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLATFORMCAMERA_H
#define QPLATFORMCAMERA_H

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

#include "qplatformvideosource_p.h"
#include "private/qerrorinfo_p.h"
#include <QtMultimedia/qcamera.h>

QT_BEGIN_NAMESPACE

class Q_MULTIMEDIA_EXPORT QPlatformCamera : public QPlatformVideoSource
{
    Q_OBJECT

public:
    // If the new camera-device is the same as the old camera-device,
    // do nothing.
    //
    // The implementation needs to handle other camera properties during a successful
    // camera-device-change. This should be done for every property, i.e focusMode,
    // flashMode, focusDistance, zoom etc.
    //
    // The properties should be handled in the following way:
    //  - If property is supported on new device, apply the property to the device immediately.
    //  - If property is supported on new device, but range of valid values has changed,
    //    clamp the property and apply it to the camera-device.
    //  - If property is NOT supported on new device, reset the value to default and do
    //    nothing to the camera-device.
    //
    // TODO: There is currently no rules on the order of how each signal should be triggered.
    // In the future we might want to add a rule that requires the implementation
    // to update all properties at once, then trigger all relevant signals.
    //
    // TODO: There are currently no rules in the public API on how we should handle
    // devices being disconnected or the user passing in an invalid device-id.
    virtual void setCamera(const QCameraDevice &camera) = 0;
    virtual bool setCameraFormat(const QCameraFormat &/*format*/) { return false; }
    QCameraFormat cameraFormat() const { return m_cameraFormat; }

    // FocusModeManual should only be reported as supported if the camera
    // backend is also able to apply the focusDistance setting.
    // This effectively means the backend should also report Feature::FocusDistance
    // as supported if this is the case.
    virtual bool isFocusModeSupported(QCamera::FocusMode mode) const { return mode == QCamera::FocusModeAuto; }

    // If the focusMode is the same as the current, ignore the function call.
    //
    // If the new focusMode is reported as unsupported, send a warning
    // and do nothing.
    //
    // FocusModeAuto should map to continuous autofocus mode in the backend.
    // FocusModeManual should be treated as fixed lens position
    // in the backend.
    //
    // If the new mode is FocusModeManual, apply the focusDistance setting.
    //
    // This function should call QPlatformCamera::focusModeChanged if
    // successful.
    virtual void setFocusMode(QCamera::FocusMode /*mode*/) {}

    virtual void setCustomFocusPoint(const QPointF &/*point*/) {}

    // If the new distance is the same as previous, ignore the function call.
    //
    // If supportedFeatures does not include the FocusDistance flag,
    // send a warning and do nothing.
    //
    // If the incoming value is out of bounds (outside [0,1]),
    // send a warning and do nothing.
    //
    // If focusMode is set to Manual, apply this focusDistance to the camera.
    // If not, accept the value but don't apply it to the camera.
    //
    // The value 0 maps to the distance closest to the camera.
    // The value 1 maps to the distance furthest away from the camera.
    //
    // This function should call QPlatformCamera::focusDistanceChanged()
    // if successful.
    virtual void setFocusDistance(float) {}

    // smaller 0: zoom instantly, rate in power-of-two/sec
    virtual void zoomTo(float /*newZoomFactor*/, float /*rate*/ = -1.) {}

    virtual void setFlashMode(QCamera::FlashMode /*mode*/) {}
    virtual bool isFlashModeSupported(QCamera::FlashMode mode) const { return mode == QCamera::FlashOff; }
    virtual bool isFlashReady() const { return false; }

    virtual void setTorchMode(QCamera::TorchMode /*mode*/) {}
    virtual bool isTorchModeSupported(QCamera::TorchMode mode) const { return mode == QCamera::TorchOff; }

    virtual void setExposureMode(QCamera::ExposureMode) {}
    virtual bool isExposureModeSupported(QCamera::ExposureMode mode) const { return mode == QCamera::ExposureAuto; }
    virtual void setExposureCompensation(float) {}
    virtual int isoSensitivity() const { return 100; }
    virtual void setManualIsoSensitivity(int) {}
    virtual void setManualExposureTime(float) {}
    virtual float exposureTime() const { return -1.; }

    virtual bool isWhiteBalanceModeSupported(QCamera::WhiteBalanceMode mode) const { return mode == QCamera::WhiteBalanceAuto; }
    virtual void setWhiteBalanceMode(QCamera::WhiteBalanceMode /*mode*/) {}
    virtual void setColorTemperature(int /*temperature*/) {}

    QVideoFrameFormat frameFormat() const override;

    // Note: Because FocusModeManual effectively cannot function without
    // being able to set FocusDistance, this feature flag is redundant.
    // Should be considered for deprecation in the future.
    QCamera::Features supportedFeatures() const { return m_supportedFeatures; }

    QCamera::FocusMode focusMode() const { return m_focusMode; }
    QPointF focusPoint() const { return m_customFocusPoint; }

    float minZoomFactor() const { return m_minZoom; }
    float maxZoomFactor() const { return m_maxZoom; }
    float zoomFactor() const { return m_zoomFactor; }
    QPointF customFocusPoint() const { return m_customFocusPoint; }
    float focusDistance() const { return m_focusDistance; }

    QCamera::FlashMode flashMode() const { return m_flashMode; }
    QCamera::TorchMode torchMode() const { return m_torchMode; }

    QCamera::ExposureMode exposureMode() const { return m_exposureMode; }
    float exposureCompensation() const { return m_exposureCompensation; }
    int manualIsoSensitivity() const { return m_iso; }
    int minIso() const { return m_minIso; }
    int maxIso() const { return m_maxIso; }
    float manualExposureTime() const { return m_exposureTime; }
    float minExposureTime() const { return m_minExposureTime; }
    float maxExposureTime() const { return m_maxExposureTime; }
    QCamera::WhiteBalanceMode whiteBalanceMode() const { return m_whiteBalance; }
    int colorTemperature() const { return m_colorTemperature; }

    void supportedFeaturesChanged(QCamera::Features);
    void minimumZoomFactorChanged(float factor);
    void maximumZoomFactorChanged(float);
    void focusModeChanged(QCamera::FocusMode mode);
    void customFocusPointChanged(const QPointF &point);
    void focusDistanceChanged(float d);
    void zoomFactorChanged(float zoom);
    void flashReadyChanged(bool);
    void flashModeChanged(QCamera::FlashMode mode);
    void torchModeChanged(QCamera::TorchMode mode);
    void exposureModeChanged(QCamera::ExposureMode mode);
    void exposureCompensationChanged(float compensation);
    void exposureCompensationRangeChanged(float min, float max);
    void isoSensitivityChanged(int iso);
    void minIsoChanged(int iso) { m_minIso = iso; }
    void maxIsoChanged(int iso) { m_maxIso = iso; }
    void exposureTimeChanged(float speed);
    void minExposureTimeChanged(float secs) { m_minExposureTime = secs; }
    void maxExposureTimeChanged(float secs) { m_maxExposureTime = secs; }
    void whiteBalanceModeChanged(QCamera::WhiteBalanceMode mode);
    void colorTemperatureChanged(int temperature);

    static int colorTemperatureForWhiteBalance(QCamera::WhiteBalanceMode mode);

    QCamera::Error error() const { return m_error.code(); }
    QString errorString() const final { return m_error.description(); }

    void updateError(QCamera::Error error, const QString &errorString);

Q_SIGNALS:
    void errorOccurred(QCamera::Error error, const QString &errorString);

protected:
    explicit QPlatformCamera(QCamera *parent);

    virtual int cameraPixelFormatScore(QVideoFrameFormat::PixelFormat /*format*/,
                                       QVideoFrameFormat::ColorRange /*colorRange*/) const
    {
        return 0;
    }

    QCameraFormat findBestCameraFormat(const QCameraDevice &camera) const;
    QCameraFormat m_cameraFormat;
    QVideoFrameFormat::PixelFormat m_framePixelFormat = QVideoFrameFormat::Format_Invalid;

    // Helper functions to allow backends to reset properties to default values.
    // i.e using focusModeChanged(defaultFocusMode());
    static constexpr int defaultColorTemperature() { return 0; }
    static constexpr QPointF defaultCustomFocusPoint() { return { -1, -1 }; }
    static constexpr float defaultExposureCompensation() { return 0.f; }
    static constexpr QCamera::ExposureMode defaultExposureMode() { return QCamera::ExposureAuto; }
    static constexpr float defaultExposureTime() { return -1.f; }
    static constexpr QCamera::FlashMode defaultFlashMode() { return QCamera::FlashOff; }
    static constexpr bool defaultFlashReady() { return false; }
    static constexpr float defaultFocusDistance() { return 1.f; }
    static constexpr QCamera::FocusMode defaultFocusMode() { return QCamera::FocusModeAuto; }
    static constexpr int defaultIso() { return -1; }
    static constexpr QCamera::TorchMode defaultTorchMode() { return QCamera::TorchOff; }
    static constexpr QCamera::WhiteBalanceMode defaultWhiteBalanceMode() { return QCamera::WhiteBalanceAuto; }
    static constexpr float defaultZoomFactor() { return 1.f; }

private:
    QCamera *m_camera = nullptr;
    QCamera::Features m_supportedFeatures = {};
    QCamera::FocusMode m_focusMode = defaultFocusMode();
    float m_minZoom = 1.;
    float m_maxZoom = 1.;
    float m_zoomFactor = defaultZoomFactor();
    float m_focusDistance = defaultFocusDistance();
    QPointF m_customFocusPoint = defaultCustomFocusPoint();
    bool m_flashReady = defaultFlashReady();
    QCamera::FlashMode m_flashMode = defaultFlashMode();
    QCamera::TorchMode m_torchMode = defaultTorchMode();
    QCamera::ExposureMode m_exposureMode = defaultExposureMode();
    float m_exposureCompensation = defaultExposureCompensation();
    float m_minExposureCompensation = 0.;
    float m_maxExposureCompensation = 0.;
    int m_iso = defaultIso();
    int m_minIso = -1;
    int m_maxIso = -1;
    float m_exposureTime = defaultExposureTime();
    float m_minExposureTime = -1.;
    float m_maxExposureTime = -1.;
    QCamera::WhiteBalanceMode m_whiteBalance = defaultWhiteBalanceMode();
    int m_colorTemperature = defaultColorTemperature();
    QErrorInfo<QCamera::Error> m_error;
};

QT_END_NAMESPACE


#endif  // QPLATFORMCAMERA_H

