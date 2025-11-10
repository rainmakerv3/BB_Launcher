// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMEDIAFRAMEINPUT_P_H
#define QMEDIAFRAMEINPUT_P_H

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

#include <QtMultimedia/qmediacapturesession.h>
#include <QtCore/private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QMediaFrameInputPrivate : public QObjectPrivate
{
public:
    void setCaptureSession(QMediaCaptureSession *session);

    QMediaCaptureSession *captureSession() const { return m_captureSession; }

protected:
    template <typename Sender>
    bool sendMediaFrame(Sender &&sender)
    {
        if (!m_canSendMediaFrame)
            return false;

        sender();
        postponeCheckReadyToSend();
        return true;
    }

    template <typename Sender, typename Signal>
    void addUpdateSignal(Sender sender, Signal signal)
    {
        connect(sender, signal, this, &QMediaFrameInputPrivate::updateCanSendMediaFrame);
    }

    template <typename Sender, typename Signal>
    void removeUpdateSignal(Sender sender, Signal signal)
    {
        disconnect(sender, signal, this, &QMediaFrameInputPrivate::updateCanSendMediaFrame);
    }

    void updateCanSendMediaFrame();

private:
    void postponeCheckReadyToSend();

    virtual bool checkIfCanSendMediaFrame() const = 0;

    virtual void emitReadyToSendMediaFrame() = 0;

    virtual void updateCaptureSessionConnections(QMediaCaptureSession *prevSession,
                                                 QMediaCaptureSession *currentSession) = 0;

private:
    QMediaCaptureSession *m_captureSession = nullptr;
    bool m_canSendMediaFrame = false;
    bool m_postponeReadyToSendCheckRun = false;
};

QT_END_NAMESPACE

#endif // QMEDIAFRAMEINPUT_P_H
