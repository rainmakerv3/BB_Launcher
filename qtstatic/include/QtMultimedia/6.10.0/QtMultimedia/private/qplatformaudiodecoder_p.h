// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIODECODERCONTROL_H
#define QAUDIODECODERCONTROL_H

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

#include <QtMultimedia/qaudiobuffer.h>
#include <QtMultimedia/qaudiodecoder.h>
#include <QtCore/qurl.h>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class QIODevice;
class Q_MULTIMEDIA_EXPORT QPlatformAudioDecoder : public QObject
{
    Q_OBJECT

public:
    virtual QUrl source() const = 0;
    virtual void setSource(const QUrl &fileName) = 0;

    virtual QIODevice* sourceDevice() const = 0;
    virtual void setSourceDevice(QIODevice *device) = 0;

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual QAudioFormat audioFormat() const = 0;
    virtual void setAudioFormat(const QAudioFormat &format) = 0;

    virtual QAudioBuffer read() = 0;
    virtual bool bufferAvailable() const { return m_bufferAvailable; }

    virtual qint64 position() const { return m_position; }
    virtual qint64 duration() const { return m_duration; }

    void formatChanged(const QAudioFormat &format);

    void sourceChanged();

    void error(int error, const QString &errorString);
    void clearError() { error(QAudioDecoder::NoError, QString()); }

    void bufferReady();
    void bufferAvailableChanged(bool available);
    void setIsDecoding(bool running = true) {
        if (m_isDecoding == running)
            return;
        m_isDecoding = running;
        emit q->isDecodingChanged(m_isDecoding);
    }
    void finished();
    bool isDecoding() const { return m_isDecoding; }

    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);

    QAudioDecoder::Error error() const { return m_error; }
    QString errorString() const { return m_errorString; }

    virtual bool canReadQrc() const { return false; }

    ~QPlatformAudioDecoder() override;

protected:
    explicit QPlatformAudioDecoder(QAudioDecoder *parent);

private:
    QAudioDecoder *q = nullptr;

    qint64 m_duration = -1;
    qint64 m_position = -1;
    QAudioDecoder::Error m_error = QAudioDecoder::NoError;
    QString m_errorString;
    bool m_isDecoding = false;
    bool m_bufferAvailable = false;
};

QT_END_NAMESPACE

#endif  // QAUDIODECODERCONTROL_H
