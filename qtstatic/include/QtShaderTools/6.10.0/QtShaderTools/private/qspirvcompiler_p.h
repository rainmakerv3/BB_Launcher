// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSPIRVCOMPILER_P_H
#define QSPIRVCOMPILER_P_H

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
#include <rhi/qshader.h>
#include <QtCore/QString>

QT_BEGIN_NAMESPACE

struct QSpirvCompilerPrivate;
class QIODevice;

class Q_SHADERTOOLS_EXPORT QSpirvCompiler
{
public:
    QSpirvCompiler();
    ~QSpirvCompiler();

    enum Flag {
        RewriteToMakeBatchableForSG = 0x01,
        FullDebugInfo = 0x02
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    void setSourceFileName(const QString &fileName);
    void setSourceFileName(const QString &fileName, QShader::Stage stage);
    void setSourceDevice(QIODevice *device, QShader::Stage stage, const QString &fileName = QString());
    void setSourceString(const QByteArray &sourceString, QShader::Stage stage, const QString &fileName = QString());
    void setFlags(Flags flags);
    void setPreamble(const QByteArray &preamble);
    void setSGBatchingVertexInputLocation(int location);

    QByteArray compileToSpirv();
    QString errorMessage() const;

private:
    Q_DISABLE_COPY(QSpirvCompiler)
    QSpirvCompilerPrivate *d = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSpirvCompiler::Flags)

QT_END_NAMESPACE

#endif
