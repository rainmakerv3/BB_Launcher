// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLATFORMVIDEOFRAMEINPUT_P_H
#define QPLATFORMVIDEOFRAMEINPUT_P_H

#include "qplatformvideosource_p.h"
#include "qmetaobject.h"
#include "qpointer.h"

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

QT_BEGIN_NAMESPACE

class QMediaInputEncoderInterface;

class Q_MULTIMEDIA_EXPORT QPlatformVideoFrameInput : public QPlatformVideoSource
{
    Q_OBJECT
public:
    QPlatformVideoFrameInput(QVideoFrameFormat format = {}) : m_format(std::move(format)) { }

    void setActive(bool) final { }
    bool isActive() const final { return true; }

    QVideoFrameFormat frameFormat() const final { return m_format; }

    QString errorString() const final { return {}; }

    QMediaInputEncoderInterface *encoderInterface() const { return m_encoderInterface; }
    void setEncoderInterface(QMediaInputEncoderInterface *encoderInterface)
    {
        m_encoderInterface = encoderInterface;
    }

Q_SIGNALS:
    void encoderUpdated();

private:
    QMediaInputEncoderInterface *m_encoderInterface = nullptr;
    QVideoFrameFormat m_format;
};

QT_END_NAMESPACE

#endif // QPLATFORMVIDEOFRAMEINPUT_P_H
