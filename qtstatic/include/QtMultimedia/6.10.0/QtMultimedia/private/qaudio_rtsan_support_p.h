// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIO_RTSAN_SUPPORT_P_H
#define QAUDIO_RTSAN_SUPPORT_P_H

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

#include <QtCore/qtconfigmacros.h>
#include <QtCore/qtclasshelpermacros.h>

#if defined(__has_feature) && __has_feature(realtime_sanitizer)
#  include <sanitizer/rtsan_interface.h>
#endif

// rtsan
#if defined(__has_cpp_attribute) && __has_cpp_attribute(clang::nonblocking)
#  define QT_MM_NONBLOCKING [[clang::nonblocking]]
#else
#  define QT_MM_NONBLOCKING
#endif

QT_BEGIN_NAMESPACE

namespace QtPrivate {

#if defined(__has_feature) && __has_feature(realtime_sanitizer)
using ScopedRTSanDisabler = __rtsan::ScopedDisabler;
#else
struct ScopedRTSanDisabler
{
    ScopedRTSanDisabler()
    {
        // silence unused/unreferenced local variable warning
        // NB: declaring the struct as [[maybe_unused]] does not work for cl.exe
        (void)this;
    }
    ~ScopedRTSanDisabler() = default;
    Q_DISABLE_COPY_MOVE(ScopedRTSanDisabler)
};
#endif

template <typename Functor>
auto withRTSanDisabled(const Functor &f)
{
#if defined(__has_feature) && __has_feature(realtime_sanitizer)
    __rtsan::ScopedDisabler disabler;
#endif
    return f();
}

} // namespace QtPrivate

QT_END_NAMESPACE

#endif // QAUDIO_RTSAN_SUPPORT_P_H
