// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef Q_PMR_EMULATION_P_H
#define Q_PMR_EMULATION_P_H

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

#include <QtCore/qtconfigmacros.h>

#if defined(__APPLE__)
// minimum requirement: iOS 17.0, macOS 14.0, watchOS 10.0 and tvOS 17.0.
#  include <Availability.h>
#  if !_LIBCPP_AVAILABILITY_HAS_PMR
#    define QT_MM_PMR_EMULATION
#  endif
#endif

#if defined(Q_OS_VXWORKS)
// std::pmr::deque<variant> doesn't seem to compile on VxWorks, so we use our emulation
#  define QT_MM_PMR_EMULATION
#endif

#if !__has_include(<memory_resource>)
#  define QT_MM_PMR_EMULATION
#endif

#ifdef QT_MM_PMR_EMULATION

#  include <QtCore/qassert.h>

#  include <cstddef>
#  include <new>
#  include <typeinfo>

#else

#  include <memory_resource>

#endif

QT_BEGIN_NAMESPACE

namespace QtMultimediaPrivate::pmr {

#ifndef QT_MM_PMR_EMULATION

using memory_resource = std::pmr::memory_resource;
template <typename T = std::byte>
using polymorphic_allocator = std::pmr::polymorphic_allocator<T>;

inline auto get_default_resource() noexcept
{
    return std::pmr::get_default_resource();
}

inline auto new_delete_resource() noexcept
{
    return std::pmr::new_delete_resource();
}

#else

class memory_resource;
memory_resource *get_default_resource() noexcept;
memory_resource *new_delete_resource() noexcept;

class memory_resource
{
public:
    memory_resource() = default;
    memory_resource(const memory_resource &) = delete;
    memory_resource &operator=(const memory_resource &) = delete;
    virtual ~memory_resource() noexcept = default;

    [[nodiscard]]
    void *allocate(size_t bytes, size_t alignment = alignof(std::max_align_t))
    {
        return do_allocate(bytes, alignment);
    }

    void deallocate(void *p, size_t bytes, size_t alignment = alignof(std::max_align_t))
    {
        do_deallocate(p, bytes, alignment);
    }

    bool is_equal(const memory_resource &other) const noexcept { return do_is_equal(other); }

protected:
    virtual void *do_allocate(size_t bytes, size_t alignment) = 0;
    virtual void do_deallocate(void *p, size_t bytes, size_t alignment) = 0;
    virtual bool do_is_equal(const memory_resource &other) const noexcept { return this == &other; }

private:
    template <class T>
    friend class polymorphic_allocator;
};

inline bool operator==(const memory_resource &a, const memory_resource &b) noexcept
{
    return &a == &b || a.is_equal(b);
}

inline bool operator!=(const memory_resource &a, const memory_resource &b) noexcept
{
    return !(a == b);
}

template <class T>
class polymorphic_allocator
{
public:
    using value_type = T;

    polymorphic_allocator() noexcept = default;
    polymorphic_allocator(memory_resource *r) : m_resource(r) { Q_ASSERT(r); }
    polymorphic_allocator(const polymorphic_allocator &other) noexcept = default;

    template <class U>
    polymorphic_allocator(const polymorphic_allocator<U> &other) noexcept
        : m_resource(other.resource())
    {
    }

    polymorphic_allocator &operator=(const polymorphic_allocator &other) noexcept = default;

    [[nodiscard]]
    T *allocate(size_t n)
    {
        void *ptr = m_resource->allocate(n * sizeof(T), alignof(T));
        return static_cast<T *>(ptr);
    }

    void deallocate(T *p, size_t n) { m_resource->deallocate(p, n * sizeof(T), alignof(T)); }

    [[nodiscard]]
    memory_resource *resource() const noexcept
    {
        return m_resource;
    }

    [[nodiscard]]
    polymorphic_allocator select_on_container_copy_construction() const
    {
        return polymorphic_allocator();
    }

private:
    memory_resource *m_resource = get_default_resource();
};

template <class T1, class T2>
bool operator==(const polymorphic_allocator<T1> &a, const polymorphic_allocator<T2> &b) noexcept
{
    return *a.resource() == *b.resource();
}

template <class T1, class T2>
bool operator!=(const polymorphic_allocator<T1> &a, const polymorphic_allocator<T2> &b) noexcept
{
    return !(a == b);
}

namespace detail {

class new_delete_resource_impl final : public memory_resource
{
public:
    using memory_resource::memory_resource;

protected:
    void *do_allocate(size_t bytes, size_t alignment) override
    {
        return ::operator new(bytes, static_cast<std::align_val_t>(alignment));
    }

    void do_deallocate(void *p, [[maybe_unused]] size_t bytes, size_t alignment) override
    {
#  ifdef __cpp_sized_deallocation
        ::operator delete(p, bytes, static_cast<std::align_val_t>(alignment));
#  else
        ::operator delete(p, static_cast<std::align_val_t>(alignment));
#  endif
    }

    bool do_is_equal(const memory_resource &other) const noexcept override
    {
        return &other == this || typeid(other) == typeid(new_delete_resource_impl);
    }
};

} // namespace detail

inline memory_resource *new_delete_resource() noexcept
{
    static detail::new_delete_resource_impl instance;
    return &instance;
}

inline memory_resource *get_default_resource() noexcept
{
    return new_delete_resource();
}

#endif

} // namespace QtMultimediaPrivate::pmr

QT_END_NAMESPACE

#ifdef QT_MM_PMR_EMULATION
#  undef QT_MM_PMR_EMULATION
#endif // QT_MM_PMR_EMULATION

#endif // Q_PMR_EMULATION_P_H
