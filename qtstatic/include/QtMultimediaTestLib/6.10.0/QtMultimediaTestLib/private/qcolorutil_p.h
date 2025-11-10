// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QCOLORUTIL_P_H
#define QCOLORUTIL_P_H

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

QT_BEGIN_NAMESPACE

class QColor;

// Considers two colors equal if their YUV components are
// pointing in the same direction and have similar luma (Y)
bool fuzzyCompare(const QColor &lhs, const QColor &rhs, float tol = 1e-2f);

QT_END_NAMESPACE

#endif
