// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWINDOWSVIDEODEVICES_H
#define QWINDOWSVIDEODEVICES_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtMultimedia/private/qplatformvideodevices_p.h>
#include <QtMultimedia/private/qcominitializer_p.h>
#include <QtMultimedia/private/qwindowsmediafoundation_p.h>

QT_BEGIN_NAMESPACE

LRESULT QT_WIN_CALLBACK deviceNotificationWndProc(HWND, UINT, WPARAM, LPARAM);

class QWindowsVideoDevices : public QPlatformVideoDevices
{
public:
    Q_MULTIMEDIA_EXPORT QWindowsVideoDevices(QPlatformMediaIntegration *integration);
    Q_MULTIMEDIA_EXPORT ~QWindowsVideoDevices();

    using QPlatformVideoDevices::onVideoInputsChanged;
protected:
    QList<QCameraDevice> findVideoInputs() const override;

private:
    QComInitializer m_comInitializer;
    HWND m_videoDeviceMsgWindow = nullptr;
    HDEVNOTIFY m_videoDeviceNotification = nullptr;

    QWindowsMediaFoundation *m_wmf{ QWindowsMediaFoundation::instance() };

    friend LRESULT QT_WIN_CALLBACK deviceNotificationWndProc(HWND, UINT, WPARAM, LPARAM);
};


QT_END_NAMESPACE

#endif
