// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWINDOWSMEDIAFOUNDATION_H
#define QWINDOWSMEDIAFOUNDATION_H

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

#include <private/qtmultimediaglobal_p.h>
#include <QtCore/private/qsystemlibrary_p.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

QT_BEGIN_NAMESPACE

class QWindowsMediaFoundation
{
public:
    static QWindowsMediaFoundation *instance();

    QWindowsMediaFoundation();
    ~QWindowsMediaFoundation();

    bool valid() const;

    decltype(&::MFStartup) mfStartup = nullptr;
    decltype(&::MFShutdown) mfShutdown = nullptr;
    decltype(&::MFCreateMediaType) mfCreateMediaType = nullptr;
    decltype(&::MFCreateMemoryBuffer) mfCreateMemoryBuffer = nullptr;
    decltype(&::MFCreateSample) mfCreateSample = nullptr;
    decltype(&::MFCreateAttributes) mfCreateAttributes = nullptr;
    decltype(&::MFEnumDeviceSources) mfEnumDeviceSources = nullptr;
    decltype(&::MFCreateSourceReaderFromMediaSource) mfCreateSourceReaderFromMediaSource = nullptr;

private:
    QSystemLibrary m_mfplat{ QStringLiteral("Mfplat.dll") };
    QSystemLibrary m_mf{ QStringLiteral("Mf.dll") };
    QSystemLibrary m_mfreadwrite{ QStringLiteral("Mfreadwrite.dll") };
    bool m_valid = false;
};

class QMFRuntimeInit
{
    Q_DISABLE_COPY_MOVE(QMFRuntimeInit)
public:
    QMFRuntimeInit(QWindowsMediaFoundation *wmf);
    ~QMFRuntimeInit();

private:
    QWindowsMediaFoundation *m_wmf;
    HRESULT m_initResult;
};

QT_END_NAMESPACE

#endif // QWINDOWSMEDIAFOUNDATION_H
