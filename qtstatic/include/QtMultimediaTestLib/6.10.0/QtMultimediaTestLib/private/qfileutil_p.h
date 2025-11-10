// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QFILEUTIL_P_H
#define QFILEUTIL_P_H

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

#include <QtCore/qdir.h>

QT_BEGIN_NAMESPACE

bool copyAllFiles(const QDir &source, const QDir &dest);

QT_END_NAMESPACE

#endif
