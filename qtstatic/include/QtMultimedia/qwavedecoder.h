// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:critical reason:data-parser

#ifndef WAVEDECODER_H
#define WAVEDECODER_H

#include <QtCore/qiodevice.h>
#include <QtMultimedia/qaudioformat.h>


QT_BEGIN_NAMESPACE



class Q_MULTIMEDIA_EXPORT QWaveDecoder : public QIODevice
{
    Q_OBJECT

public:
    explicit QWaveDecoder(QIODevice *device, QObject *parent = nullptr);
    explicit QWaveDecoder(QIODevice *device, const QAudioFormat &format,
                        QObject *parent = nullptr);
    ~QWaveDecoder() override;

    QAudioFormat audioFormat() const;
    QIODevice* getDevice();
    int duration() const;
    static qint64 headerLength();

    bool open(QIODevice::OpenMode mode) override;
    void close() override;
    bool seek(qint64 pos) override;
    qint64 pos() const override;
    void setIODevice(QIODevice *device);
    qint64 size() const override;
    bool isSequential() const override;
    qint64 bytesAvailable() const override;

Q_SIGNALS:
    void formatKnown();
    void parsingError();

private Q_SLOTS:
    void handleData();

private:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

    bool writeHeader();
    bool writeDataLength();
    bool enoughDataAvailable();
    bool findChunk(const char *chunkId);
    void discardBytes(qint64 numBytes);
    void parsingFailed();

    enum State {
        InitialState,
        WaitingForFormatState,
        WaitingForDataState
    };

    struct chunk
    {
        char        id[4]; // A four-character code that identifies the representation of the chunk data
                           // padded on the right with blank characters (ASCII 32)
        quint32     size;  // Does not include the size of the id or size fields or the pad byte at the end of payload
    };

    bool peekChunk(chunk* pChunk, bool handleEndianness = true);

    struct RIFFHeader
    {
        chunk       descriptor;
        char        type[4];
    };
    struct WAVEHeader
    {
        chunk       descriptor;
        quint16     audioFormat;
        quint16     numChannels;
        quint32     sampleRate;
        quint32     byteRate;
        quint16     blockAlign;
        quint16     bitsPerSample;
    };

    struct DATAHeader
    {
        chunk       descriptor;
    };

    struct CombinedHeader
    {
        RIFFHeader  riff;
        WAVEHeader  wave;
        DATAHeader  data;
    };
    static const int HeaderLength = sizeof(CombinedHeader);

    bool haveFormat = false;
    bool haveHeader = false;
    qint64 dataSize = 0;
    QIODevice *device = nullptr;
    QAudioFormat format;
    State state = InitialState;
    quint32 junkToSkip = 0;
    bool bigEndian = false;
    bool byteSwap = false;
    int bps = 0;
};

QT_END_NAMESPACE

#endif // WAVEDECODER_H
