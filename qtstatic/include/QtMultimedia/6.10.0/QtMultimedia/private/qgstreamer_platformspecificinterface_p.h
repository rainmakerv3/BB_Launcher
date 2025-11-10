// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef GSTREAMER_PLATFORMSPECIFICINTERFACE_P_H
#define GSTREAMER_PLATFORMSPECIFICINTERFACE_P_H

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

#include <QtMultimedia/private/qplatformmediaintegration_p.h>

typedef struct _GstPipeline GstPipeline; // NOLINT (bugprone-reserved-identifier)
typedef struct _GstElement GstElement; // NOLINT (bugprone-reserved-identifier)

QT_BEGIN_NAMESPACE

class QAudioDevice;

class Q_MULTIMEDIA_EXPORT QGStreamerPlatformSpecificInterface
    : public QAbstractPlatformSpecificInterface
{
public:
    ~QGStreamerPlatformSpecificInterface() override;

    static QGStreamerPlatformSpecificInterface *instance();

    virtual QAudioDevice makeCustomGStreamerAudioInput(const QByteArray &gstreamerPipeline) = 0;
    virtual QAudioDevice makeCustomGStreamerAudioOutput(const QByteArray &gstreamerPipeline) = 0;
    virtual QCamera *makeCustomGStreamerCamera(const QByteArray &gstreamerPipeline,
                                               QObject *parent) = 0;

    // Note: ownership of GstElement is not transferred
    virtual QCamera *makeCustomGStreamerCamera(GstElement *, QObject *parent) = 0;

    virtual GstPipeline *gstPipeline(QMediaPlayer *) = 0;
    virtual GstPipeline *gstPipeline(QMediaCaptureSession *) = 0;
};

QT_END_NAMESPACE

#endif // GSTREAMER_PLATFORMSPECIFICINTERFACE_P_H
