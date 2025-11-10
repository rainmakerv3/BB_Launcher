// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMEDIADEVICES_H
#define QMEDIADEVICES_H

#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

class QAudioDevice;
class QCameraDevice;

class Q_MULTIMEDIA_EXPORT QMediaDevices : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QAudioDevice> audioInputs READ audioInputs NOTIFY audioInputsChanged)
    Q_PROPERTY(QList<QAudioDevice> audioOutputs READ audioOutputs NOTIFY audioOutputsChanged)
    Q_PROPERTY(QList<QCameraDevice> videoInputs READ videoInputs NOTIFY videoInputsChanged)
    Q_PROPERTY(QAudioDevice defaultAudioInput READ defaultAudioInput NOTIFY audioInputsChanged)
    Q_PROPERTY(QAudioDevice defaultAudioOutput READ defaultAudioOutput NOTIFY audioOutputsChanged)
    Q_PROPERTY(QCameraDevice defaultVideoInput READ defaultVideoInput NOTIFY videoInputsChanged)

public:
    QMediaDevices(QObject *parent = nullptr);
    ~QMediaDevices() override;

    static QList<QAudioDevice> audioInputs();
    static QList<QAudioDevice> audioOutputs();
    static QList<QCameraDevice> videoInputs();

    static QAudioDevice defaultAudioInput();
    static QAudioDevice defaultAudioOutput();
    static QCameraDevice defaultVideoInput();

Q_SIGNALS:
    void audioInputsChanged();
    void audioOutputsChanged();
    void videoInputsChanged();

protected:
    void connectNotify(const QMetaMethod &signal) override;
};

QT_END_NAMESPACE


#endif  // QABSTRACTMEDIASERVICE_H

