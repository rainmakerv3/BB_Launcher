// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef OSDETECTION_P_H
#define OSDETECTION_P_H

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

#include <QtCore/qsystemdetection.h>

QT_BEGIN_NAMESPACE

constexpr bool isLinux =
#ifdef Q_OS_LINUX
        true;
#else
        false;
#endif

constexpr bool isMacOS =
#ifdef Q_OS_MACOS
        true;
#else
        false;
#endif

constexpr bool isWindows =
#ifdef Q_OS_WINDOWS
        true;
#else
        false;
#endif

constexpr bool isAndroid =
#ifdef Q_OS_ANDROID
        true;
#else
        false;
#endif

constexpr bool isArm =
#ifdef Q_PROCESSOR_ARM
        true;
#else
        false;
#endif


QT_END_NAMESPACE

#endif
