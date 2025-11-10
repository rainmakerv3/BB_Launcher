// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLATFORMVIDEOSOURCE_P_H
#define QPLATFORMVIDEOSOURCE_P_H

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

#include "qvideoframeformat.h"

#include <QtCore/qobject.h>
#include <QtCore/qnativeinterface.h>
#include <QtCore/private/qglobal_p.h>

#include <optional>

QT_BEGIN_NAMESPACE

class QVideoFrame;
class QPlatformMediaCaptureSession;

class Q_MULTIMEDIA_EXPORT QPlatformVideoSource : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    virtual void setActive(bool active) = 0;
    virtual bool isActive() const = 0;

    virtual QVideoFrameFormat frameFormat() const = 0;

    virtual std::optional<int> ffmpegHWPixelFormat() const;

    virtual void setCaptureSession(QPlatformMediaCaptureSession *) { }

    virtual QString errorString() const = 0;

    bool hasError() const { return !errorString().isEmpty(); }

Q_SIGNALS:
    void newVideoFrame(const QVideoFrame &);
    void activeChanged(bool);
    void errorChanged();
};

QT_END_NAMESPACE

#endif // QPLATFORMVIDEOSOURCE_P_H
