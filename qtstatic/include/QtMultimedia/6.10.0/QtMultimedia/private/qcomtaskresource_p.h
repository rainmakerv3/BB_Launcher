// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#ifndef QCOMTASKRESOURCE_P_H
#define QCOMTASKRESOURCE_P_H

#include <QtCore/qassert.h>

#include <objbase.h>
#include <algorithm>
#include <type_traits>
#include <utility>

QT_BEGIN_NAMESPACE

class QEmptyDeleter final
{
public:
    template<typename T>
    void operator()(T /*element*/) const
    {
    }
};

class QComDeleter final
{
public:
    template<typename T>
    void operator()(T element) const
    {
        element->Release();
    }
};

template<typename T>
class QComTaskResourceBase
{
public:
    QComTaskResourceBase(const QComTaskResourceBase<T> &source) = delete;
    QComTaskResourceBase &operator=(const QComTaskResourceBase<T> &right) = delete;

    explicit operator bool() const { return m_resource != nullptr; }

    T *get() const { return m_resource; }

protected:
    QComTaskResourceBase() = default;
    explicit QComTaskResourceBase(T *const resource) : m_resource(resource) { }

    T *release() { return std::exchange(m_resource, nullptr); }

    void reset(T *const resource = nullptr)
    {
        if (m_resource != resource) {
            if (m_resource)
                CoTaskMemFree(m_resource);
            m_resource = resource;
        }
    }

    T *m_resource = nullptr;
};

template<typename T, typename TElementDeleter = QEmptyDeleter>
class QComTaskResource final : public QComTaskResourceBase<T>
{
    using Base = QComTaskResourceBase<T>;

public:
    using Base::QComTaskResourceBase;

    ~QComTaskResource() { reset(); }

    T *operator->() const { return m_resource; }
    T &operator*() const { return *m_resource; }

    T **address()
    {
        Q_ASSERT(m_resource == nullptr);
        return &m_resource;
    }

    using Base::release;
    using Base::reset;

private:
    using Base::m_resource;
};

template<typename T, typename TElementDeleter>
class QComTaskResource<T[], TElementDeleter> final : public QComTaskResourceBase<T>
{
    using Base = QComTaskResourceBase<T>;

public:
    QComTaskResource() = default;
    explicit QComTaskResource(T *const resource, const std::size_t size)
        : Base(resource), m_size(size)
    {
    }

    ~QComTaskResource() { reset(); }

    T &operator[](const std::size_t index) const
    {
        Q_ASSERT(index < m_size);
        return m_resource[index];
    }

    T *release()
    {
        m_size = 0;

        return Base::release();
    }

    void reset() { reset(nullptr, 0); }

    void reset(T *const resource, const std::size_t size)
    {
        if (m_resource != resource) {
            resetElements();

            Base::reset(resource);

            m_size = size;
        }
    }

private:
    void resetElements()
    {
        if constexpr (!std::is_same_v<TElementDeleter, QEmptyDeleter>) {
            std::for_each(m_resource, m_resource + m_size, TElementDeleter());
        }
    }

    std::size_t m_size = 0;

    using Base::m_resource;
};

QT_END_NAMESPACE

#endif
