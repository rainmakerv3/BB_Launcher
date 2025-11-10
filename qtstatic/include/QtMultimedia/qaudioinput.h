// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIOINPUTDEVICE_H
#define QAUDIOINPUTDEVICE_H

#include <QtCore/qobject.h>
#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtMultimedia/qtaudio.h>

#include <functional>

QT_BEGIN_NAMESPACE

class QAudioDevice;
class QPlatformAudioInput;

class Q_MULTIMEDIA_EXPORT QAudioInput : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAudioDevice device READ device WRITE setDevice NOTIFY deviceChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)

public:
    explicit QAudioInput(QObject *parent = nullptr);
    explicit QAudioInput(const QAudioDevice &deviceInfo, QObject *parent = nullptr);
    ~QAudioInput() override;

    QAudioDevice device() const;
    float volume() const;
    bool isMuted() const;

public Q_SLOTS:
    void setDevice(const QAudioDevice &device);
    void setVolume(float volume);
    void setMuted(bool muted);

Q_SIGNALS:
    void deviceChanged();
    void volumeChanged(float volume);
    void mutedChanged(bool muted);

private:
    QPlatformAudioInput *handle() const { return d; }
    void setDisconnectFunction(std::function<void()> disconnectFunction);
    friend class QMediaCaptureSession;
    Q_DISABLE_COPY(QAudioInput)
    QPlatformAudioInput *d = nullptr;
};

QT_END_NAMESPACE

#endif  // QAUDIOINPUTDEVICE_H
