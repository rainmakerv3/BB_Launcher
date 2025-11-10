// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSHADERREWRITER_P_H
#define QSHADERREWRITER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QByteArray>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace QShaderRewriter {
void debugTokenizer(const QByteArray &input);
QByteArray addZAdjustment(const QByteArray &input, int vertexInputLocation);
}

QT_END_NAMESPACE

#endif
