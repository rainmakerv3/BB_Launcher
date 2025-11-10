// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QRHITEXTUREFORMATS
#define QRHITEXTUREFORMATS

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

const int RhiTextureFormat_RGBA8 = 1;
const int RhiTextureFormat_BGRA8 = 2;
const int RhiTextureFormat_R8 = 3;
const int RhiTextureFormat_RG8 = 4;
const int RhiTextureFormat_R16 = 5;
const int RhiTextureFormat_RG16 = 6;
const int RhiTextureFormat_RED_OR_ALPHA8 = 7;

#ifdef __cplusplus
#include <QtGui/rhi/qrhi.h>

QT_BEGIN_NAMESPACE

static_assert(QRhiTexture::RGBA8 == RhiTextureFormat_RGBA8, "Incompatible RGBA8 value between c++ and shaders");
static_assert(QRhiTexture::BGRA8 == RhiTextureFormat_BGRA8, "Incompatible BGRA8 value between c++ and shaders");
static_assert(QRhiTexture::R8 == RhiTextureFormat_R8, "Incompatible R8 value between c++ and and shaders");
static_assert(QRhiTexture::RG8 == RhiTextureFormat_RG8, "Incompatible RG8 value between c++ and shaders");
static_assert(QRhiTexture::RG16 == RhiTextureFormat_RG16, "Incompatible RG16 value between c++ and shaders");
static_assert(QRhiTexture::RED_OR_ALPHA8 == RhiTextureFormat_RED_OR_ALPHA8, "Incompatible RED_OR_ALPHA value between c++ and shaders");

QT_END_NAMESPACE

#endif

#endif
