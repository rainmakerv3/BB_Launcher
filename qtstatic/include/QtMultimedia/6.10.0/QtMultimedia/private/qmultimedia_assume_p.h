// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMULTIMEDIA_ASSUME_P_H
#define QMULTIMEDIA_ASSUME_P_H

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


#if defined(__has_cpp_attribute) && __has_cpp_attribute(assume)
#  define QT_MM_ASSUME(assumption) \
      Q_ASSERT(assumption);        \
      [[assume(assumption)]];      \
      static_assert(true, "force semicolon")
#else
#  define QT_MM_ASSUME(assumption) Q_ASSERT(assumption)
#endif

#endif // QMULTIMEDIA_ASSUME_P_H
