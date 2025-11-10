// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSPIRVSHADER_P_H
#define QSPIRVSHADER_P_H

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

QT_BEGIN_NAMESPACE

struct QSpirvShaderPrivate;

class Q_SHADERTOOLS_EXPORT QSpirvShader
{
public:
    enum class GlslFlag {
        GlslEs = 0x01,
        FixClipSpace = 0x02,
        EsFragDefaultFloatPrecisionMedium = 0x04
    };
    Q_DECLARE_FLAGS(GlslFlags, GlslFlag)

    enum class MslFlag {
        VertexAsCompute = 0x01,
        WithUInt16Index = 0x02,
        WithUInt32Index = 0x04
    };
    Q_DECLARE_FLAGS(MslFlags, MslFlag)

    enum class RemapFlag {
        StripOnly = 0x01
    };
    Q_DECLARE_FLAGS(RemapFlags, RemapFlag)

    QSpirvShader();
    ~QSpirvShader();

    void setSpirvBinary(const QByteArray &spirv, QShader::Stage stage);

    QShaderDescription shaderDescription() const;

    QByteArray spirvBinary() const;
    QByteArray remappedSpirvBinary(RemapFlags flags = RemapFlags(), QString *errorMessage = nullptr) const;

    struct SeparateToCombinedImageSamplerMapping {
        QByteArray textureName;
        QByteArray samplerName;
        QByteArray combinedSamplerName;
    };

    struct TessellationInfo {
        // The tess.evaluation shader will likely declaer something like layout(triangles, fractional_odd_spacing, ccw) in;
        // For Metal we need to know the tessellation mode (triangles) when generating the translated tess.control shader.
        struct {
            QShaderDescription::TessellationMode mode = QShaderDescription::TrianglesTessellationMode;
        } infoForTesc;
        // The tess.control shader will likely declare something like layout(vertices = 3) out;
        // For Metal we need to know this value (3) when generating the translated tess.eval. shader.
        struct {
            int vertexCount = 3;
        } infoForTese;
    };

    struct MultiViewInfo {
        // layout(num_views = viewCount) in; see GL_OVR_multiview
        int viewCount = 0;
    };

    QByteArray translateToGLSL(int version,
                               GlslFlags flags,
                               QShader::Stage stage,
                               const MultiViewInfo &multiViewInfo,
                               QVector<SeparateToCombinedImageSamplerMapping> *separateToCombinedImageSamplerMappings) const;
    QByteArray translateToHLSL(int version,
                               QShader::NativeResourceBindingMap *nativeBindings) const;
    QByteArray translateToMSL(int version,
                              MslFlags flags,
                              QShader::Stage stage,
                              QShader::NativeResourceBindingMap *nativeBindings,
                              QShader::NativeShaderInfo *shaderInfo,
                              const MultiViewInfo &multiViewInfo,
                              const TessellationInfo &tessInfo) const;
    QString translationErrorMessage() const;

private:
    Q_DISABLE_COPY(QSpirvShader)
    QSpirvShaderPrivate *d = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSpirvShader::GlslFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QSpirvShader::MslFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QSpirvShader::RemapFlags)

QT_END_NAMESPACE

#endif
