// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-3.0-only
#ifndef QAUDIOLISTENER_H
#define QAUDIOLISTENER_H

#include <QtSpatialAudio/qtspatialaudioglobal.h>
#include <QtMultimedia/qaudioformat.h>
#include <QtCore/qobject.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qquaternion.h>

QT_BEGIN_NAMESPACE

class QAudioEngine;
class QAudioListenerPrivate;

class Q_SPATIALAUDIO_EXPORT QAudioListener : public QObject
{
public:
    explicit QAudioListener(QAudioEngine *engine);
    ~QAudioListener() override;

    void setPosition(QVector3D pos);
    QVector3D position() const;
    void setRotation(const QQuaternion &q);
    QQuaternion rotation() const;

    QAudioEngine *engine() const;

private:
    void setEngine(QAudioEngine *engine);
    Q_DECLARE_PRIVATE(QAudioListener)

    // ### Qt7: remove unused member
    QT6_ONLY(Q_DECL_UNUSED_MEMBER void *unused = nullptr;) // for ABI compatibility
};

QT_END_NAMESPACE

#endif // QAUDIOLISTENER_H
