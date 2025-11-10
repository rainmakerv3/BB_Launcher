// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef MULTIMEDIA_DEBUG_SUPPORT_P_H
#define MULTIMEDIA_DEBUG_SUPPORT_P_H

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

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

template <typename Type>
QString qAsQString(Type e)
{
    QString result;
    QDebug stream(&result);
    stream.nospace();
    stream << e;
    return result;
}

QT_END_NAMESPACE

#endif // MULTIMEDIA_DEBUG_SUPPORT_P_H
