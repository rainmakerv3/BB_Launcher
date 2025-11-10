// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLATFORMAUDIODEVICES_H
#define QPLATFORMAUDIODEVICES_H

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

#include <QtCore/qlist.h>
#include <QtCore/qobject.h>
#include <QtMultimedia/private/qaudiodevice_p.h>
#include <QtMultimedia/private/qcachedvalue_p.h>
#include <QtMultimedia/private/qtmultimediaglobal_p.h>
#include <memory>

QT_BEGIN_NAMESPACE

class QPlatformAudioSource;
class QPlatformAudioSink;
class QAudioFormat;

class Q_MULTIMEDIA_EXPORT QPlatformAudioDevices : public QObject
{
    Q_OBJECT

    QT_DEFINE_TAG_STRUCT(PrivateTag);

public:
    QPlatformAudioDevices();
    ~QPlatformAudioDevices() override;

    static std::unique_ptr<QPlatformAudioDevices> create();

    QList<QAudioDevice> audioInputs() const;
    QList<QAudioDevice> audioOutputs() const;

    virtual QPlatformAudioSource *createAudioSource(const QAudioDevice &, const QAudioFormat &,
                                                    QObject *parent);
    virtual QPlatformAudioSink *createAudioSink(const QAudioDevice &, const QAudioFormat &,
                                                QObject *parent);

    QPlatformAudioSource *audioInputDevice(QAudioFormat, const QAudioDevice &, QObject *parent);
    QPlatformAudioSink *audioOutputDevice(QAudioFormat, const QAudioDevice &, QObject *parent);

    void initVideoDevicesConnection();
    virtual QLatin1String backendName() const { return QLatin1String{ "null" }; }

protected:
    virtual QList<QAudioDevice> findAudioInputs() const { return {}; }
    virtual QList<QAudioDevice> findAudioOutputs() const { return {}; }

    void onAudioInputsChanged();
    void onAudioOutputsChanged();

    void updateAudioInputsCache();
    void updateAudioOutputsCache();

Q_SIGNALS:
    void audioInputsChanged(PrivateTag);
    void audioOutputsChanged(PrivateTag);

private:
    mutable QCachedValue<QList<QAudioDevice>> m_audioInputs;
    mutable QCachedValue<QList<QAudioDevice>> m_audioOutputs;
};

QT_END_NAMESPACE


#endif // QPLATFORMAUDIODEVICES_H
