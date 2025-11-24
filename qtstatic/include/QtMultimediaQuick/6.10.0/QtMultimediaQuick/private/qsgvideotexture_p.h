// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSGVIDEOTEXTURE_H
#define QSGVIDEOTEXTURE_H

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

#include <QtQuick/qsgtexture.h>
#include <QtGui/qimage.h>
#include <QtGui/rhi/qrhi.h>
#include <QtMultimediaQuick/private/qtmultimediaquickglobal_p.h>

#include <memory>

QT_BEGIN_NAMESPACE

class QSGVideoTexturePrivate;
class Q_MULTIMEDIAQUICK_EXPORT QSGVideoTexture : public QSGTexture
{
    Q_DECLARE_PRIVATE(QSGVideoTexture)
public:
    QSGVideoTexture();
    ~QSGVideoTexture() override;

    qint64 comparisonKey() const override;
    QRhiTexture *rhiTexture() const override;
    QSize textureSize() const override;
    bool hasAlphaChannel() const override;
    bool hasMipmaps() const override;
    void setRhiTexture(QRhiTexture *texture);
    void setData(QRhiTexture::Format f, const QSize &s, const uchar *data, int bytes);

protected:
    std::unique_ptr<QSGVideoTexturePrivate> d_ptr;
};

QT_END_NAMESPACE

#endif // QSGVIDEOTEXTURE_H
