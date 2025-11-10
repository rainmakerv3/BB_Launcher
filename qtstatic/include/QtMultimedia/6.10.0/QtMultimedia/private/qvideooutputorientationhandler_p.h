// Copyright (C) 2016 Research In Motion
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QVIDEOOUTPUTORIENTATIONHANDLER_P_H
#define QVIDEOOUTPUTORIENTATIONHANDLER_P_H

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

#include <qtmultimediaglobal.h>

#include <QObject>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class Q_MULTIMEDIA_EXPORT QVideoOutputOrientationHandler : public QObject
{
    Q_OBJECT
public:
    explicit QVideoOutputOrientationHandler(QObject *parent = nullptr);

    int currentOrientation() const;

    static void setIsRecording(bool isRecording) { m_isRecording = isRecording; }
    static bool isRecording() { return m_isRecording; }

Q_SIGNALS:
    void orientationChanged(int angle);

private Q_SLOTS:
    void screenOrientationChanged(Qt::ScreenOrientation orientation);

private:
    int m_currentOrientation;
    static bool m_isRecording;
};

QT_END_NAMESPACE


#endif
