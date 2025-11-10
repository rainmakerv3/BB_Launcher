// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QAUDIORINGBUFFER_P_H
#define QAUDIORINGBUFFER_P_H

#include <QtCore/qspan.h>
#include <QtCore/qtclasshelpermacros.h>
#include <QtMultimedia/private/qaudio_qspan_support_p.h>

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <limits>
#include <type_traits>

QT_BEGIN_NAMESPACE

namespace QtPrivate {

// Single-producer, single-consumer wait-free queue
template <typename T>
class QAudioRingBuffer
{
    static constexpr bool isTriviallyDestructible = std::is_trivially_destructible_v<T>;

public:
    using ValueType = T;
    using Region = QSpan<T>;
    using ConstRegion = QSpan<const T>;

    explicit QAudioRingBuffer(int bufferSize) : m_bufferSize(bufferSize)
    {
        if (bufferSize)
            m_buffer = reinterpret_cast<T *>(
                    ::operator new(sizeof(T) * bufferSize, std::align_val_t(alignof(T))));
    }

    Q_DISABLE_COPY_MOVE(QAudioRingBuffer)

    ~QAudioRingBuffer()
    {
        if constexpr (!isTriviallyDestructible) {
            consumeAll([](auto /* elements*/) {
            });
        }

        ::operator delete(reinterpret_cast<void *>(m_buffer), std::align_val_t(alignof(T)));
    }

    int write(ConstRegion region)
    {
        using namespace QtMultimediaPrivate; // drop
        return produceSome([&](Region writeRegion) {
            qsizetype writeSize = std::min(region.size(), writeRegion.size());
            std::uninitialized_copy_n(region.data(), writeSize, writeRegion.data());
            region = drop(region, writeSize);

            return Region{
                writeRegion.data(),
                writeSize,
            };
        });
    }

    bool write(T element)
    {
        return produceOne([&] {
            return std::move(element);
        });
    }

    template <typename Functor>
    bool produceOne(Functor &&producer)
#ifdef __cpp_concepts
            requires
            std::is_invocable_v<Functor> &&std::is_constructible_v<T, std::invoke_result_t<Functor>>
#endif
    {
        Region writeRegion = acquireWriteRegion(1);
        if (writeRegion.isEmpty())
            return false;
        T *writeElement = writeRegion.data();
        new (writeElement) T{ producer() };
        releaseWriteRegion(1);
        return true;
    }

    template <typename Functor>
    int produceSome(Functor &&producer, int elements = std::numeric_limits<int>::max())
#ifdef __cpp_concepts
            requires std::is_invocable_v<Functor, Region>
                    &&std::is_same_v<std::invoke_result_t<Functor, Region>, Region>
#endif
    {
        int elementsRemain = elements;
        int elementsWritten = 0;
        while (elementsRemain) {
            Region writeRegion = acquireWriteRegion(elementsRemain);
            if (writeRegion.isEmpty())
                break;

            Region writtenRegion = producer(writeRegion);
            if (writtenRegion.isEmpty())
                break;

            Q_ASSERT(writtenRegion.data() == writeRegion.data());
            Q_ASSERT(writtenRegion.size() <= writeRegion.size());

            elementsRemain -= writtenRegion.size();
            elementsWritten += writtenRegion.size();
            releaseWriteRegion(writtenRegion.size());
        }
        return elementsWritten;
    }

    template <typename Functor>
    int consumeAll(Functor &&consumer)
    {
        return consumeSome([&](Region region) {
            consumer(region);
            return region;
        });
    }

    template <typename Functor>
    int consume(int elements, Functor &&consumer)
    {
        int remainingElemnts = elements;
        return consumeSome([&](Region region) {
            if (remainingElemnts == 0)
                return Region{};

            Region regionToConsume = QtMultimediaPrivate::take(region, remainingElemnts);
            consumer(regionToConsume);
            remainingElemnts -= regionToConsume.size();
            return regionToConsume;
        });
    }

    // consumer has to return the region it has consumed
    template <typename Functor>
    int consumeSome(Functor &&consumer, int elements = std::numeric_limits<int>::max())
#ifdef __cpp_concepts
            requires std::is_invocable_v<Functor, Region>
                    &&std::is_same_v<std::invoke_result_t<Functor, Region>, Region>
#endif
    {
        int elementsConsumed = 0;

        while (elements > elementsConsumed) {
            Region readRegion = acquireReadRegion(elements - elementsConsumed);
            if (readRegion.isEmpty())
                break;

            Region consumedRegion = consumer(readRegion);
            if (consumedRegion.isEmpty())
                break;
            Q_ASSERT(consumedRegion.data() == readRegion.data());
            Q_ASSERT(consumedRegion.size() <= readRegion.size());

            if constexpr (!isTriviallyDestructible)
                std::destroy(consumedRegion.begin(), consumedRegion.end());

            elementsConsumed += consumedRegion.size();
            releaseReadRegion(consumedRegion.size());
        }

        return elementsConsumed;
    }

    // CAVEAT: beware of the thread safety
    int used() const { return m_bufferUsed.load(std::memory_order_relaxed); }
    int free() const { return m_bufferSize - m_bufferUsed.load(std::memory_order_relaxed); }

    int size() const { return m_bufferSize; }

    void reset()
#ifdef __cpp_concepts
            requires isTriviallyDestructible
#endif
    {
        m_readPos = 0;
        m_writePos = 0;
        m_bufferUsed.store(0, std::memory_order_relaxed);
    }

private:
    Region acquireWriteRegion(int size)
    {
        const int free = m_bufferSize - m_bufferUsed.load(std::memory_order_acquire);

        Region output;
        if (free > 0) {
            const int writeSize = qMin(size, qMin(m_bufferSize - m_writePos, free));
            output = writeSize > 0 ? Region(m_buffer + m_writePos, writeSize) : Region();
        } else {
            output = Region();
        }
        return output;
    }

    void releaseWriteRegion(int elementsRead)
    {
        m_writePos = (m_writePos + elementsRead) % m_bufferSize;
        m_bufferUsed.fetch_add(elementsRead, std::memory_order_release);
    }

    Region acquireReadRegion(int size)
    {
        const int used = m_bufferUsed.load(std::memory_order_acquire);

        if (used > 0) {
            const int readSize = qMin(size, qMin(m_bufferSize - m_readPos, used));
            return readSize > 0 ? Region(m_buffer + m_readPos, readSize) : Region();
        }

        return Region();
    }

    // WARNING: we need to ensure that all elements are destroyed
    void releaseReadRegion(int elementsWritten)
    {
        m_readPos = (m_readPos + elementsWritten) % m_bufferSize;
        m_bufferUsed.fetch_sub(elementsWritten, std::memory_order_release);
    }

    const int m_bufferSize;
    int m_readPos{};
    int m_writePos{};
    T *m_buffer{};
    std::atomic_int m_bufferUsed{};
};

} // namespace QtPrivate

QT_END_NAMESPACE

#endif // QAUDIORINGBUFFER_P_H
