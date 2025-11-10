// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIODECODER_H
#define QAUDIODECODER_H

#include <QtCore/qobject.h>
#include <QtMultimedia/qaudiobuffer.h>

QT_BEGIN_NAMESPACE

class QAudioDecoderPrivate;
class Q_MULTIMEDIA_EXPORT QAudioDecoder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool isDecoding READ isDecoding NOTIFY isDecodingChanged)
    Q_PROPERTY(QString error READ errorString)
    Q_PROPERTY(bool bufferAvailable READ bufferAvailable NOTIFY bufferAvailableChanged)

public:
    enum Error
    {
        NoError,
        ResourceError,
        FormatError,
        AccessDeniedError,
        NotSupportedError
    };
    Q_ENUM(Error)

    explicit QAudioDecoder(QObject *parent = nullptr);
    ~QAudioDecoder() override;

    bool isSupported() const;
    bool isDecoding() const;

    QUrl source() const;
    void setSource(const QUrl &fileName);

    QIODevice* sourceDevice() const;
    void setSourceDevice(QIODevice *device);

    QAudioFormat audioFormat() const;
    void setAudioFormat(const QAudioFormat &format);

    Error error() const;
    QString errorString() const;

    QAudioBuffer read() const;
    bool bufferAvailable() const;

    qint64 position() const;
    qint64 duration() const;

public Q_SLOTS:
    void start();
    void stop();

Q_SIGNALS:
    void bufferAvailableChanged(bool);
    void bufferReady();
    void finished();
    void isDecodingChanged(bool);

    void formatChanged(const QAudioFormat &format);

    void error(QAudioDecoder::Error error);

    void sourceChanged();

    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);

private:
    Q_DISABLE_COPY(QAudioDecoder)
    Q_DECLARE_PRIVATE(QAudioDecoder)

    // ### Qt7: remove unused member
    QT6_ONLY(Q_DECL_UNUSED_MEMBER void *unused = nullptr;) // for ABI compatibility
};

QT_END_NAMESPACE

#endif  // QAUDIODECODER_H
