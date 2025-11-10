// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QWFM_SUPPORT_P_H
#define QWFM_SUPPORT_P_H

#include <QtMultimedia/qtmultimediaexports.h>
#include <QtCore/qtconfigmacros.h>
#include <QtCore/qspan.h>
#include <QtCore/qbytearray.h>
#include <QtCore/private/qexpected_p.h>
#include <QtCore/private/qcomobject_p.h>
#include <QtCore/private/qcomptr_p.h>

#include <mfobjects.h>

#include <memory_resource>

QT_BEGIN_NAMESPACE

namespace QWMF {

///////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Functor>
using IMFBufferReaderReturnType = std::invoke_result_t<Functor, QSpan<BYTE>, QSpan<BYTE>>;

template <typename Functor>
[[nodiscard]]
auto withLockedBuffer(IMFMediaBuffer *buffer, Functor &&f)
        -> q23::expected<IMFBufferReaderReturnType<Functor>, HRESULT>
{
    if (!buffer)
        return q23::unexpected{ E_POINTER };

    BYTE *data = nullptr;
    DWORD maxLength = 0;
    DWORD currentLength = 0;

    HRESULT hr = buffer->Lock(&data, &maxLength, &currentLength);
    if (FAILED(hr))
        return q23::unexpected{ hr };

    auto unlockGuard = qScopeGuard([buffer]() {
        buffer->Unlock();
    });

    if constexpr (std::is_void_v<IMFBufferReaderReturnType<Functor>>) {
        f(QSpan{ data, qsizetype(currentLength) }, QSpan{ data, qsizetype(maxLength) });
        return {};
    } else
        return f(QSpan{ data, qsizetype(currentLength) }, QSpan{ data, qsizetype(maxLength) });
}

template <typename Functor>
[[nodiscard]]
auto withLockedBuffer(const ComPtr<IMFMediaBuffer> &buffer, Functor &&f)
        -> q23::expected<IMFBufferReaderReturnType<Functor>, HRESULT>
{
    return withLockedBuffer(buffer.Get(), std::forward<Functor>(f));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

class QByteArrayMFMediaBuffer final : public QComObject<IMFMediaBuffer>
{
public:
    static HRESULT CreateInstance(QByteArray data, IMFMediaBuffer **ppBuffer,
                                  bool isReadOnly = false);
    static HRESULT CreateInstance(qsizetype capacity, IMFMediaBuffer **ppBuffer);

    // --- IMFMediaBuffer Methods ---
    STDMETHODIMP Lock(BYTE **ppbBuffer, DWORD *pcbMaxLength, DWORD *pcbCurrentLength) override;
    STDMETHODIMP Unlock() override;
    STDMETHODIMP GetCurrentLength(DWORD *pcbCurrentLength) override;
    STDMETHODIMP SetCurrentLength(DWORD cbCurrentLength) override;
    STDMETHODIMP GetMaxLength(DWORD *pcbMaxLength) override;

    QByteArray takeByteArray();

private:
    // Protected constructor to enforce creation via CreateInstance factory method.
    explicit QByteArrayMFMediaBuffer(QByteArray &&data, bool isReadOnly);
    ~QByteArrayMFMediaBuffer() = default;

    DWORD GetMaxLengthInternal() const;

    // Member variables
    std::atomic_flag m_isLocked = ATOMIC_FLAG_INIT;
    DWORD m_currentLength{ 0 };
    QByteArray m_byteArray;
    const bool m_isReadOnly{};

public:
    QByteArrayMFMediaBuffer(const QByteArrayMFMediaBuffer &) = delete;
    QByteArrayMFMediaBuffer &operator=(const QByteArrayMFMediaBuffer &) = delete;
    QByteArrayMFMediaBuffer(QByteArrayMFMediaBuffer &&) = delete;
    QByteArrayMFMediaBuffer &operator=(QByteArrayMFMediaBuffer &&) = delete;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class QPmrMediaBuffer final : public QComObject<IMFMediaBuffer>
{
public:
    static HRESULT CreateInstance(QSpan<const std::byte>, std::pmr::memory_resource *,
                                  IMFMediaBuffer **ppBuffer);
    static HRESULT CreateInstance(qsizetype capacity, std::pmr::memory_resource *,
                                  IMFMediaBuffer **ppBuffer);

    // --- IUnknown Methods ---
    // Note: we cannot access the reference count from QComObject (yet), we override all reference
    // count methods
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;

    // --- IMFMediaBuffer Methods ---
    STDMETHODIMP Lock(BYTE **ppbBuffer, DWORD *pcbMaxLength, DWORD *pcbCurrentLength) override;
    STDMETHODIMP Unlock() override;
    STDMETHODIMP GetCurrentLength(DWORD *pcbCurrentLength) override;
    STDMETHODIMP SetCurrentLength(DWORD cbCurrentLength) override;
    STDMETHODIMP GetMaxLength(DWORD *pcbMaxLength) override;

private:
    // Protected constructor to enforce creation via CreateInstance factory method.
    QPmrMediaBuffer(QSpan<const std::byte>, std::pmr::memory_resource *);
    QPmrMediaBuffer(qsizetype capacity, std::pmr::memory_resource *);
    ~QPmrMediaBuffer();

    // Member variables
    std::atomic_flag m_isLocked = ATOMIC_FLAG_INIT;
    std::pmr::memory_resource *m_memoryResource;

    DWORD m_currentLength{ 0 };
    DWORD m_maxLength;
    std::byte *const m_buffer;

    std::atomic<LONG> m_referenceCount = 1;

public:
    Q_DISABLE_COPY_MOVE(QPmrMediaBuffer)
};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace QWMF

QT_END_NAMESPACE

#endif // QWFM_SUPPORT_P_H
