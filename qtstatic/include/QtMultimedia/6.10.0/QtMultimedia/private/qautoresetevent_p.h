// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#ifndef QAUTORESETEVENT_P_H
#define QAUTORESETEVENT_P_H

#include <QtCore/qglobal.h>


// TODO: kqueue based implementation
#ifdef Q_OS_LINUX
#  include "qautoresetevent_linux_p.h"

QT_BEGIN_NAMESPACE
namespace QtPrivate {
using QAutoResetEvent = QAutoResetEventEventFD;
} // namespace QtPrivate
QT_END_NAMESPACE

#elif defined(Q_OS_WIN)
#  include "qautoresetevent_win32_p.h"

QT_BEGIN_NAMESPACE
namespace QtPrivate {
using QAutoResetEvent = QAutoResetEventWin32;
} // namespace QtPrivate
QT_END_NAMESPACE

#elif defined(Q_OS_APPLE)
#  include "qautoresetevent_kqueue_p.h"

QT_BEGIN_NAMESPACE
namespace QtPrivate {
using QAutoResetEvent = QAutoResetEventKQueue;
} // namespace QtPrivate
QT_END_NAMESPACE

#else
#  include "qautoresetevent_pipe_p.h"

QT_BEGIN_NAMESPACE
namespace QtPrivate {
using QAutoResetEvent = QAutoResetEventPipe;
} // namespace QtPrivate
QT_END_NAMESPACE

#endif

#endif // QAUTORESETEVENT_P_H
