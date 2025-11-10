// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIO_ALIGNMENT_SUPPORT_P_H
#define QAUDIO_ALIGNMENT_SUPPORT_P_H

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

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

namespace QtMultimediaPrivate {

template <typename IntType>
inline constexpr bool isPowerOfTwo(IntType arg)
{
    return arg > 0 && (arg & (arg - 1)) == 0;
}

template <typename Type>
constexpr Type alignUp(Type arg, size_t alignment)
{
    if constexpr (std::is_pointer_v<Type>) {
        return Type(alignUp(std::intptr_t(arg), alignment));
    } else {
        Q_ASSERT(isPowerOfTwo(alignment));
        return Type((arg + (Type(alignment) - 1)) & ~Type(alignment - 1));
    }
}

template <typename Type>
constexpr Type alignDown(Type arg, size_t alignment)
{
    if constexpr (std::is_pointer_v<Type>) {
        return Type(alignDown(std::intptr_t(arg), alignment));
    } else {
        Q_ASSERT(isPowerOfTwo(alignment));
        return arg & ~Type(alignment - 1);
    }
}

template <typename IntType>
constexpr bool isAligned(IntType arg, size_t alignment)
{
    if constexpr (std::is_pointer_v<IntType>) {
        return isAligned(std::intptr_t(arg), alignment);
    } else {
        Q_ASSERT(isPowerOfTwo(alignment));
        return (arg & (IntType(alignment) - 1)) == 0;
    }
}

} // namespace QtMultimediaPrivate

QT_END_NAMESPACE

#endif // QAUDIO_ALIGNMENT_SUPPORT_P_H
