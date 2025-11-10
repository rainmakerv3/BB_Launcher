// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QAUDIO_QIODEVICE_SUPPORT_P_H
#define QAUDIO_QIODEVICE_SUPPORT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qdebug.h>
#include <QtCore/qglobal.h>
#include <QtCore/qiodevice.h>
#include <QtCore/qmutex.h>
#include <QtCore/qspan.h>

#include <QtMultimedia/private/qaudio_alignment_support_p.h>
#include <QtMultimedia/private/qaudio_qspan_support_p.h>
#include <QtMultimedia/private/qaudioringbuffer_p.h>
#include <QtMultimedia/private/qautoresetevent_p.h>

#include <deque>
#include <mutex>

QT_BEGIN_NAMESPACE

namespace QtPrivate {

class QIODeviceRingBufferWriterBase : public QIODevice
{
public:
    explicit QIODeviceRingBufferWriterBase(QObject *parent = nullptr) : QIODevice(parent)
    {
        setOpenMode(QIODevice::WriteOnly | QIODevice::Unbuffered);

        m_bytesConsumed.callOnActivated([&] {
            qint64 bytes = m_bytesConsumedFromRingbuffer.exchange(0, std::memory_order_relaxed);
            if (bytes > 0)
                emit bytesWritten(bytes);
        });
    }

    void bytesConsumedFromRingbuffer(qint64 bytes)
    {
        m_bytesConsumedFromRingbuffer.fetch_add(bytes, std::memory_order_relaxed);
        m_bytesConsumed.set();
    }

    bool isSequential() const override { return true; }

private:
    QtPrivate::QAutoResetEvent m_bytesConsumed;
    std::atomic<qint64> m_bytesConsumedFromRingbuffer{ 0 };
};

// QIODevice writing to a QAudioRingBuffer
template <typename SampleType>
class QIODeviceRingBufferWriter final : public QIODeviceRingBufferWriterBase
{
public:
    using Ringbuffer = QtPrivate::QAudioRingBuffer<SampleType>;

    explicit QIODeviceRingBufferWriter(Ringbuffer *rb, QObject *parent = nullptr)
        : QIODeviceRingBufferWriterBase(parent), m_ringbuffer(rb)
    {
        Q_ASSERT(rb);
    }

    qint64 readData(char * /*data*/, qint64 /*maxlen*/) override { return -1; }
    qint64 writeData(const char *data, qint64 len) override
    {
        using namespace QtMultimediaPrivate; // take/drop

        // we don't write fractional samples
        int64_t usableLength = alignDown(len, sizeof(SampleType));
        auto readRegion = QSpan<const SampleType>{
            reinterpret_cast<const SampleType *>(data),
            qsizetype(usableLength / sizeof(SampleType)),
        };

        qint64 bytesWritten = m_ringbuffer->write(readRegion) * sizeof(SampleType);
        if (bytesWritten)
            emit readyRead();

        return bytesWritten;
    }

    qint64 bytesToWrite() const override { return m_ringbuffer->free() * sizeof(SampleType); }

private:
    Ringbuffer *const m_ringbuffer;
};

// QIODevice reading from a QAudioRingBuffer
template <typename SampleType>
class QIODeviceRingBufferReader final : public QIODevice
{
public:
    using Ringbuffer = QtPrivate::QAudioRingBuffer<SampleType>;

    explicit QIODeviceRingBufferReader(Ringbuffer *rb, QObject *parent = nullptr)
        : QIODevice(parent), m_ringbuffer(rb)
    {
        Q_ASSERT(rb);
    }

    qint64 readData(char *data, qint64 maxlen) override
    {
        using namespace QtMultimediaPrivate; // drop

        QSpan<std::byte> outputRegion = as_writable_bytes(QSpan{ data, qsizetype(maxlen) });

        qsizetype maxSizeToRead = outputRegion.size_bytes() / sizeof(SampleType);

        int samplesConsumed = m_ringbuffer->consumeSome([&](auto readRegion) {
            QSpan readByteRegion = as_bytes(readRegion);
            std::copy(readByteRegion.begin(), readByteRegion.end(), outputRegion.begin());
            outputRegion = drop(outputRegion, readByteRegion.size());

            return readRegion;
        }, maxSizeToRead);

        return samplesConsumed * sizeof(SampleType);
    }

    qint64 writeData(const char * /*data*/, qint64 /*len*/) override { return -1; }
    qint64 bytesAvailable() const override { return m_ringbuffer->used() * sizeof(SampleType); }
    bool isSequential() const override { return true; }

private:
    Ringbuffer *const m_ringbuffer;
};

// QIODevice backed by a std::deque
class QDequeIODevice final : public QIODevice
{
public:
    using Deque = std::deque<char>;

    explicit QDequeIODevice(QObject *parent = nullptr) : QIODevice(parent) { }

    qint64 bytesAvailable() const override { return qint64(m_deque.size()); }

private:
    qint64 readData(char *data, qint64 maxlen) override
    {
        std::lock_guard guard{ m_mutex };

        size_t bytesToRead = std::min<size_t>(m_deque.size(), maxlen);
        std::copy_n(m_deque.begin(), bytesToRead, data);

        m_deque.erase(m_deque.begin(), m_deque.begin() + bytesToRead);
        return qint64(bytesToRead);
    }

    qint64 writeData(const char *data, qint64 len) override
    {
        std::lock_guard guard{ m_mutex };
        m_deque.insert(m_deque.end(), data, data + len);
        return len;
    }

    QMutex m_mutex;
    Deque m_deque;
};

inline qint64 writeToDevice(QIODevice &device, QSpan<const std::byte> data)
{
    return device.write(reinterpret_cast<const char *>(data.data()), data.size());
}

inline qint64 readFromDevice(QIODevice &device, QSpan<std::byte> outputBuffer)
{
    return device.read(reinterpret_cast<char *>(outputBuffer.data()), outputBuffer.size());
}

template <typename SampleType>
qsizetype pullFromQIODeviceToRingbuffer(QIODevice &device, QAudioRingBuffer<SampleType> &ringbuffer)
{
    using namespace QtMultimediaPrivate;

    int totalSamplesWritten = ringbuffer.produceSome([&](QSpan<SampleType> writeRegion) {
        qint64 bytesAvailableInDevice = alignDown(device.bytesAvailable(), sizeof(SampleType));
        if (!bytesAvailableInDevice)
            return QSpan<SampleType>{}; // no data in iodevice

        qint64 samplesAvailableInDevice = bytesAvailableInDevice / sizeof(SampleType);
        writeRegion = take(writeRegion, samplesAvailableInDevice);

        qint64 bytesRead = readFromDevice(device, as_writable_bytes(writeRegion));
        if (bytesRead < 0) {
            qWarning() << "pullFromQIODeviceToRingbuffer cannot read from QIODevice:"
                       << device.errorString();
            return QSpan<SampleType>{};
        }

        return take(writeRegion, bytesRead / sizeof(SampleType));
    });

    return totalSamplesWritten * sizeof(SampleType);
}

template <typename SampleType>
qsizetype pushToQIODeviceFromRingbuffer(QIODevice &device, QAudioRingBuffer<SampleType> &ringbuffer)
{
    using namespace QtMultimediaPrivate;

    int totalSamplesWritten = ringbuffer.consumeSome([&](QSpan<SampleType> region) {
        // we do our best effort and only push full samples to the device
        const quint64 bytesToWrite = [&] {
            const qint64 deviceBytesToWrite = device.bytesToWrite();
            return (deviceBytesToWrite > 0) ? alignDown(deviceBytesToWrite, sizeof(SampleType))
                                            : region.size_bytes();
        }();

        QSpan<const std::byte> bufferByteRegion = take(as_bytes(region), bytesToWrite);
        int bytesWritten = writeToDevice(device, bufferByteRegion);
        if (bytesWritten < 0) {
            qWarning() << "pushToQIODeviceFromRingbuffer cannot push data to QIODevice:"
                       << device.errorString();
            return QSpan<SampleType>{};
        }
        Q_ASSERT(isAligned(bytesWritten, sizeof(SampleType)));
        int samplesWritten = bytesWritten / sizeof(SampleType);
        return take(region, samplesWritten);
    });

    return totalSamplesWritten * sizeof(SampleType);
}

} // namespace QtPrivate

QT_END_NAMESPACE

#endif // QAUDIO_QIODEVICE_SUPPORT_P_H
