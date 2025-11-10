// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSHADERBAKER_H
#define QSHADERBAKER_H

//
//  W A R N I N G
//  -------------
//
// This file is part of the RHI API, with limited compatibility guarantees.
// Usage of this API may make your code source and binary incompatible with
// future versions of Qt.
//

#include <QtShaderTools/qtshadertoolsglobal.h>
#include <rhi/qshader.h>

QT_BEGIN_NAMESPACE

struct QShaderBakerPrivate;
class QIODevice;

class Q_SHADERTOOLS_EXPORT QShaderBaker
{
public:
    enum class SpirvOption {
        GenerateFullDebugInfo = 0x01,
        StripDebugAndVarInfo = 0x02
    };
    Q_DECLARE_FLAGS(SpirvOptions, SpirvOption)

    enum class GlslOption {
        GlslEsFragDefaultFloatPrecisionMedium = 0x01
    };
    Q_DECLARE_FLAGS(GlslOptions, GlslOption)

    QShaderBaker();
    ~QShaderBaker();

    void setSourceFileName(const QString &fileName);
    void setSourceFileName(const QString &fileName, QShader::Stage stage);

    void setSourceDevice(QIODevice *device, QShader::Stage stage,
                         const QString &fileName = QString());

    void setSourceString(const QByteArray &sourceString, QShader::Stage stage,
                         const QString &fileName = QString());

    typedef QPair<QShader::Source, QShaderVersion> GeneratedShader;
    void setGeneratedShaders(const QList<GeneratedShader> &v);
    void setGeneratedShaderVariants(const QList<QShader::Variant> &v);

    void setPreamble(const QByteArray &preamble);
    void setBatchableVertexShaderExtraInputLocation(int location);
    void setPerTargetCompilation(bool enable);
    void setBreakOnShaderTranslationError(bool enable);
    void setTessellationMode(QShaderDescription::TessellationMode mode);
    void setTessellationOutputVertexCount(int count);
    void setMultiViewCount(int count);

    void setSpirvOptions(SpirvOptions options);
    void setGlslOptions(GlslOptions options);

    QShader bake();

    QString errorMessage() const;

private:
    Q_DISABLE_COPY(QShaderBaker)
    QShaderBakerPrivate *d = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QShaderBaker::SpirvOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(QShaderBaker::GlslOptions)

QT_END_NAMESPACE

#endif
