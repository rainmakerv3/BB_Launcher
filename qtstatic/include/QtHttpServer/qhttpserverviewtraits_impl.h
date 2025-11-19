// Copyright (C) 2020 Mikhail Svetkin <mikhail.svetkin@gmail.com>
// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERVIEWTRAITS_IMPL_H
#define QHTTPSERVERVIEWTRAITS_IMPL_H

#include <QtCore/qglobal.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qobjectdefs.h>

#include <tuple>
#include <type_traits>

QT_BEGIN_NAMESPACE

namespace QtPrivate {

template<typename ReturnT, typename... Args>
struct FunctionTraitsHelper
{
    static constexpr const int ArgumentCount = sizeof ... (Args);
    static constexpr const int ArgumentIndexMax = ArgumentCount - 1;
    using ReturnType = ReturnT;

    template <size_t I>
    struct Arg {
        using Type = typename std::tuple_element<I, std::tuple<Args...>>::type;

        using CleanType = q20::remove_cvref_t<Type>;

        static constexpr bool CopyConstructible = std::is_copy_constructible_v<CleanType>;
    };
};

template<typename T>
struct FunctionTraitsImpl;

template<typename T>
struct FunctionTraitsImpl : public FunctionTraitsImpl<decltype(&T::operator())>
{
};

template<typename ReturnT, typename... Args>
struct FunctionTraitsImpl<ReturnT (*)(Args...)> : public FunctionTraitsHelper<ReturnT, Args...>
{
};

template<typename ReturnT, typename ClassT, typename... Args>
struct FunctionTraitsImpl<ReturnT (ClassT::*)(Args...)>
    : public FunctionTraitsHelper<ReturnT, Args...>
{
};

template<typename ReturnT, typename ClassT, typename... Args>
struct FunctionTraitsImpl<ReturnT (ClassT::*)(Args...) const>
    : public FunctionTraitsHelper<ReturnT, Args...>
{
};

template<typename T>
using FunctionTraits = FunctionTraitsImpl<std::decay_t<T>>;

template<typename ... T>
struct CheckAny {
    static constexpr bool Value = (T::Value || ...);
    static constexpr bool Valid = (T::Valid || ...);
    static constexpr bool StaticAssert = (T::StaticAssert || ...);
};

template<typename ViewHandler, bool DisableStaticAssert>
struct ViewTraits {
    using FTraits = FunctionTraits<ViewHandler>;
    using ArgumentIndexes = typename std::make_index_sequence<FTraits::ArgumentCount>;

    template<int I, typename Special>
    struct SpecialHelper {
        using Arg = typename FTraits::template Arg<I>;
        using CleanSpecialT = q20::remove_cvref_t<Special>;

        static constexpr bool TypeMatched = std::is_same<typename Arg::CleanType, CleanSpecialT>::value;
        static constexpr bool TypeCVRefMatched = std::is_same<typename Arg::Type, Special>::value;

        static constexpr bool ValidPosition =
            (I == FTraits::ArgumentIndexMax ||
             I == FTraits::ArgumentIndexMax - 1);
        static constexpr bool ValidAll = TypeCVRefMatched && ValidPosition;

        static constexpr bool AssertCondition =
            DisableStaticAssert || !TypeMatched || TypeCVRefMatched;

        static constexpr bool AssertConditionOrder =
            DisableStaticAssert || !TypeMatched || ValidPosition;

        static constexpr bool StaticAssert = AssertCondition && AssertConditionOrder;

        static_assert(AssertConditionOrder,
                      "ViewHandler arguments error: "
                      "QHttpServerRequest or QHttpServerResponder"
                      " can only be the last argument");
    };

    template<int I, typename T>
    struct Special {
        using Helper = SpecialHelper<I, T>;
        static constexpr bool Value = Helper::TypeMatched;
        static constexpr bool Valid = Helper::ValidAll;
        static constexpr bool StaticAssert = Helper::StaticAssert;
        static constexpr bool AssertCondition = Helper::AssertCondition;
    };
};

} // namespace QtPrivate

QT_END_NAMESPACE

#endif  // QHTTPSERVERVIEWTRAITS_IMPL_H
