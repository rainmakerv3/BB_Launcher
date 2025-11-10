// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIO_SPAN_SUPPORT_P_H
#define QAUDIO_SPAN_SUPPORT_P_H

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

#include <QtCore/qglobal.h>
#include <QtCore/qspan.h>

QT_BEGIN_NAMESPACE

namespace QtMultimediaPrivate {

// some span utils (poor man's ranges-v3)
template <typename U>
inline QSpan<U> drop(QSpan<U> span, qsizetype n) // ranges::drop
{
    if (n < span.size())
        return span.subspan(n);
    else
        return {};
}

template <typename U>
inline QSpan<U> take(QSpan<U> span, qsizetype n) // ranges::take
{
    if (n > span.size())
        return span;
    else
        return span.first(n);
}

} // namespace QtMultimediaPrivate

QT_END_NAMESPACE

#endif // QAUDIO_SPAN_SUPPORT_P_H
