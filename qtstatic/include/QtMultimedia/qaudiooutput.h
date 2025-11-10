// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIOOUTPUTDEVICE_H
#define QAUDIOOUTPUTDEVICE_H

#include <QtCore/qobject.h>
#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtMultimedia/qtaudio.h>

#include <functional>

QT_BEGIN_NAMESPACE

class QAudioDevice;
class QPlatformAudioOutput;

class Q_MULTIMEDIA_EXPORT QAudioOutput : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAudioDevice device READ device WRITE setDevice NOTIFY deviceChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)

public:
    explicit QAudioOutput(QObject *parent = nullptr);
    explicit QAudioOutput(const QAudioDevice &device, QObject *parent = nullptr);
    ~QAudioOutput() override;

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
    QPlatformAudioOutput *handle() const { return d; }
    void setDisconnectFunction(std::function<void()> disconnectFunction);
    friend class QMediaCaptureSession;
    friend class QMediaPlayer;
    Q_DISABLE_COPY(QAudioOutput)
    QPlatformAudioOutput *d = nullptr;
};

QT_END_NAMESPACE

#endif  // QAUDIOOUTPUTDEVICE_H
