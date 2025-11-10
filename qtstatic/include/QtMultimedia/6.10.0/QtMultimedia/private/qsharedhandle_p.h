// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSHAREDHANDLE_P_H
#define QSHAREDHANDLE_P_H

#include <QtCore/private/quniquehandle_p.h>
#include <QtCore/qtconfigmacros.h>
#include <QtCore/qcompare.h>

#if __cpp_lib_concepts
#  include <concepts>
#endif

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

QT_BEGIN_NAMESPACE

namespace QtPrivate {

#if __cpp_lib_concepts

// clang-format off

// Define a concept for the traits
template <typename T>
concept QSharedHandleTraitsConcept = requires
{
    typename T::Type;

    { T::invalidValue() } noexcept -> std::same_as<typename T::Type>;
    { T::ref(std::declval<typename T::Type>()) } -> std::same_as<typename T::Type>;
    { T::unref(std::declval<typename T::Type>()) } -> std::same_as<bool>;
};

// clang-format on

#endif

#if __cpp_lib_concepts
template <QSharedHandleTraitsConcept SharedHandleTraits>
#else
template <typename SharedHandleTraits>
#endif
struct UniqueHandleTraitsFromSharedHandleTraits
{
    using Type = typename SharedHandleTraits::Type;

    [[nodiscard]] static Type invalidValue() noexcept(noexcept(SharedHandleTraits::invalidValue()))
    {
        return SharedHandleTraits::invalidValue();
    }

    [[nodiscard]] static bool
    close(Type handle) noexcept(noexcept(SharedHandleTraits::unref(handle)))
    {
        return SharedHandleTraits::unref(handle);
    }
};

#if __cpp_lib_concepts
template <QSharedHandleTraitsConcept HandleTraits>
#else
template <typename HandleTraits>
#endif
struct QSharedHandle : private QUniqueHandle<UniqueHandleTraitsFromSharedHandleTraits<HandleTraits>>
{
private:
    using BaseClass = QUniqueHandle<UniqueHandleTraitsFromSharedHandleTraits<HandleTraits>>;

    static constexpr bool BaseResetIsNoexcept =
            noexcept(std::declval<BaseClass>().reset(std::declval<typename HandleTraits::Type>()));

    static constexpr bool RefIsNoexcept =
            noexcept(HandleTraits::ref(std::declval<typename HandleTraits::Type>()));

    static constexpr bool BaseMoveIsNoexcept =
            noexcept(std::declval<BaseClass>().operator=(std::move(std::declval<BaseClass>())));

public:
    using typename BaseClass::Type;

    enum RefMode : uint8_t
    {
        HasRef,
        NeedsRef,

        // syntactic sugar
        AddRef = NeedsRef,
        NoAddRef = HasRef,
    };

    QSharedHandle() = default;

    explicit QSharedHandle(typename HandleTraits::Type object, RefMode mode)
        : BaseClass{ mode == NeedsRef ? HandleTraits::ref(object) : object }
    {
    }

    QSharedHandle(const QSharedHandle &o)
        : BaseClass{
              HandleTraits::ref(o.get()),
          }
    {
    }

    QSharedHandle(QSharedHandle &&) noexcept = default;

    // NOLINTNEXTLINE: bugprone-unhandled-self-assign
    QSharedHandle &operator=(const QSharedHandle &o) noexcept(RefIsNoexcept && BaseResetIsNoexcept)
    {
        if (BaseClass::get() != o.get())
            BaseClass::reset(HandleTraits::ref(o.get()));
        return *this;
    };

    QSharedHandle &operator=(QSharedHandle &&o) noexcept(BaseMoveIsNoexcept)
    {
        BaseClass::operator=(std::forward<QSharedHandle>(o));
        return *this;
    }

    void reset(typename HandleTraits::Type o,
               RefMode mode) noexcept(RefIsNoexcept && BaseResetIsNoexcept)
    {
        if (mode == NeedsRef)
            BaseClass::reset(HandleTraits::ref(o));
        else
            BaseClass::reset(o);
    }

    void reset() noexcept(BaseResetIsNoexcept) { BaseClass::reset(); }

    Q_DECLARE_STRONGLY_ORDERED(QSharedHandle);

    using BaseClass::get;
    using BaseClass::isValid;
    using BaseClass::operator bool;
    using BaseClass::release;
    using BaseClass::operator&;

    void swap(QSharedHandle &other) noexcept(noexcept(std::declval<BaseClass>().swap(other)))
    {
        BaseClass::swap(other);
    }
};

template <typename Trait>
void swap(QSharedHandle<Trait> &lhs, QSharedHandle<Trait> &rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

} // namespace QtPrivate

QT_END_NAMESPACE

#endif // QSHAREDHANDLE_P_H
