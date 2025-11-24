// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QV4STACKLIMITS_P_H
#define QV4STACKLIMITS_P_H

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

#include <private/qtqmlglobal_p.h>

#ifndef Q_STACK_GROWTH_DIRECTION
#  ifdef Q_PROCESSOR_HPPA
#    define Q_STACK_GROWTH_DIRECTION (1)
#  else
#    define Q_STACK_GROWTH_DIRECTION (-1)
#  endif
#endif

QT_BEGIN_NAMESPACE

namespace QV4 {

// Note: This does not return a completely accurate stack pointer.
//       Depending on whether this function is inlined or not, we may get the address of
//       this function's stack frame or the caller's stack frame.
//       Always use a safety margin when determining stack limits.
inline const void *currentStackPointer()
{
    // TODO: How often do we actually need the assembler mess below? Is that worth it?

    void *stackPointer;
#if defined(Q_CC_GNU) || __has_builtin(__builtin_frame_address)
    stackPointer = __builtin_frame_address(0);
#elif defined(Q_CC_MSVC)
    stackPointer = &stackPointer;
#elif defined(Q_PROCESSOR_X86_64)
    __asm__ __volatile__("movq %%rsp, %0" : "=r"(stackPointer) : :);
#elif defined(Q_PROCESSOR_X86)
    __asm__ __volatile__("movl %%esp, %0" : "=r"(stackPointer) : :);
#elif defined(Q_PROCESSOR_ARM_64) && defined(__ILP32__)
    quint64 stackPointerRegister = 0;
    __asm__ __volatile__("mov %0, sp" : "=r"(stackPointerRegister) : :);
    stackPointer = reinterpret_cast<void *>(stackPointerRegister);
#elif defined(Q_PROCESSOR_ARM_64) || defined(Q_PROCESSOR_ARM_32)
    __asm__ __volatile__("mov %0, sp" : "=r"(stackPointer) : :);
#else
    stackPointer = &stackPointer;
#endif
    return stackPointer;
}

struct StackProperties
{
    const void *base = nullptr;
    const void *softLimit = nullptr;
    const void *hardLimit = nullptr;
};

StackProperties stackProperties();

} // namespace QV4

QT_END_NAMESPACE

#endif // QV4STACKLIMITS_P_H
