// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSPIRVSHADERREMAP_P_H
#define QSPIRVSHADERREMAP_P_H

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

#include <QtShaderTools/private/qtshadertoolsglobal_p.h>
#include "qspirvshader_p.h"

QT_BEGIN_NAMESPACE

class Q_SHADERTOOLS_EXPORT QSpirvShaderRemapper
{
public:
    QByteArray remap(const QByteArray &ir, QSpirvShader::RemapFlags flags);
    QString errorMessage() const { return remapErrorMsg; }

private:
    void remapErrorHandler(const std::string &s);
    void remapLogHandler(const std::string &s);

    QString remapErrorMsg;
};

QT_END_NAMESPACE

#endif
