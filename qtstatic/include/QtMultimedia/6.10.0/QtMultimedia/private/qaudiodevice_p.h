// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only


#ifndef QAUDIODEVICEINFO_P_H
#define QAUDIODEVICEINFO_P_H

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

#include <QtMultimedia/qaudiodevice.h>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

// Implementations should not include volatile members, such as values  that can change between
// connection sessions. For example, CoreAudio AudioDeviceId on macOS.
class Q_MULTIMEDIA_EXPORT QAudioDevicePrivate : public QSharedData
{
public:
    QAudioDevicePrivate(QByteArray id, QAudioDevice::Mode m, QString description)
        : id(std::move(id)), mode(m), description(std::move(description))
    {}
    virtual ~QAudioDevicePrivate();
    const QByteArray id;
    const QAudioDevice::Mode mode = QAudioDevice::Output;
    const QString description;
    bool isDefault = false;

    QAudioFormat preferredFormat;
    int minimumSampleRate = 0;
    int maximumSampleRate = 0;
    int minimumChannelCount = 0;
    int maximumChannelCount = 0;
    QList<QAudioFormat::SampleFormat> supportedSampleFormats;
    QAudioFormat::ChannelConfig channelConfiguration = QAudioFormat::ChannelConfigUnknown;

    static QAudioDevice createQAudioDevice(std::unique_ptr<QAudioDevicePrivate> devicePrivate);

    static const QAudioDevicePrivate *handle(const QAudioDevice &device) { return device.d.get(); }

    template <typename Derived>
    static const Derived *handle(const QAudioDevice &device)
    {
        // Note: RTTI is required for dispatching in the gstreamer backend
        return dynamic_cast<const Derived *>(handle(device));
    }
};

inline const QList<QAudioFormat::SampleFormat> &qAllSupportedSampleFormats()
{
    static const auto singleton = QList<QAudioFormat::SampleFormat>{
        QAudioFormat::UInt8,
        QAudioFormat::Int16,
        QAudioFormat::Int32,
        QAudioFormat::Float,
    };
    return singleton;
}

struct QAudioDevicePrivateAllMembersEqual
{
    bool operator()(const QAudioDevicePrivate &lhs, const QAudioDevicePrivate &rhs)
    {
        auto asTuple = [](const QAudioDevicePrivate &x) {
            return std::tie(x.id, x.mode, x.isDefault, x.preferredFormat, x.description,
                            x.minimumSampleRate, x.maximumSampleRate, x.minimumChannelCount,
                            x.maximumChannelCount, x.supportedSampleFormats,
                            x.channelConfiguration);
        };

        return asTuple(lhs) == asTuple(rhs);
    }
};

QT_END_NAMESPACE

#endif // QAUDIODEVICEINFO_H
