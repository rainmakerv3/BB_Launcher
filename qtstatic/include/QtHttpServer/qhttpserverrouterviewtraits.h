// Copyright (C) 2020 Mikhail Svetkin <mikhail.svetkin@gmail.com>
// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERROUTERVIEWTRAITS_H
#define QHTTPSERVERROUTERVIEWTRAITS_H

#include <QtHttpServer/qhttpserverviewtraits_impl.h>

QT_BEGIN_NAMESPACE

class QHttpServerRequest;
class QHttpServerResponder;

namespace QtPrivate {

template<typename ViewHandler, bool DisableStaticAssert>
struct RouterViewTraitsHelper : ViewTraits<ViewHandler, DisableStaticAssert> {
    using VTraits = ViewTraits<ViewHandler, DisableStaticAssert>;
    using FunctionTraits = typename VTraits::FTraits;

    template<int I>
    struct ArgumentChecker : FunctionTraits::template Arg<I> {
        using IsRequest = typename VTraits::template Special<I, const QHttpServerRequest &>;
        static_assert(IsRequest::AssertCondition,
                      "ViewHandler arguments error: "
                      "QHttpServerRequest can only be passed as a const reference");

        using IsResponder = typename VTraits::template Special<I, QHttpServerResponder &>;
        static_assert(IsResponder::AssertCondition,
                      "ViewHandler arguments error: "
                      "QHttpServerResponder can only be passed as a reference");

        using IsSpecial = CheckAny<IsRequest, IsResponder>;

        struct IsSimple {
            static constexpr bool Value = !IsSpecial::Value &&
                                           I < FunctionTraits::ArgumentCount &&
                                           FunctionTraits::ArgumentIndexMax != -1;
            static constexpr bool Valid =
                    !IsSpecial::Valid && FunctionTraits::template Arg<I>::CopyConstructible;

            static constexpr bool StaticAssert =
                DisableStaticAssert || !Value || Valid;


            static_assert(StaticAssert,
                          "ViewHandler arguments error: "
                          "Type is not copy constructible");
        };

        using CheckOk = CheckAny<IsSimple, IsSpecial>;

        static constexpr bool Valid = CheckOk::Valid;
        static constexpr bool StaticAssert = CheckOk::StaticAssert;
    };


    struct Arguments {
        template<size_t ... I>
        struct ArgumentsReturn {
            template<int Idx>
            using Arg = ArgumentChecker<Idx>;

            template<int Idx>
            static constexpr QMetaType metaType() noexcept
            {
                using Type = typename FunctionTraits::template Arg<Idx>::CleanType;
                constexpr bool Simple = Arg<Idx>::IsSimple::Valid;

                if constexpr (Simple && std::is_copy_assignable_v<Type>)
                    return QMetaType::fromType<Type>();
                else
                    return QMetaType::fromType<void>();
            }

            static constexpr std::size_t Count = FunctionTraits::ArgumentCount;
            static constexpr std::size_t CapturableCount =
                    (0 + ... + static_cast<std::size_t>(!Arg<I>::IsSpecial::Value));

            static constexpr std::size_t SpecialsCount = Count - CapturableCount;

            static constexpr bool Valid = (Arg<I>::Valid && ...);
            static constexpr bool StaticAssert = (Arg<I>::StaticAssert && ...);

            using Indexes = std::index_sequence<I...>;

            using CapturableIndexes = std::make_index_sequence<CapturableCount>;

            using SpecialIndexes = std::make_index_sequence<SpecialsCount>;

            using Last = Arg<FunctionTraits::ArgumentIndexMax>;
        };

        template<size_t ... I>
        static constexpr ArgumentsReturn<I...> eval(std::index_sequence<I...>) noexcept;
    };

    template<int CaptureOffset>
    struct BindType {
        template<typename ... Args>
        struct FunctionWrapper {
            using Type = std::function<typename FunctionTraits::ReturnType (Args...)>;
        };

        template<int Id>
        using OffsetArg = typename FunctionTraits::template Arg<CaptureOffset + Id>::Type;

        template<size_t ... Idx>
        static constexpr typename FunctionWrapper<OffsetArg<Idx>...>::Type
                eval(std::index_sequence<Idx...>) noexcept;
    };
};


} // namespace QtPrivate

template <typename ViewHandler, bool DisableStaticAssert = false>
struct QHttpServerRouterViewTraits
{
    using Helpers = typename QtPrivate::RouterViewTraitsHelper<ViewHandler, DisableStaticAssert>;
    using ReturnType = typename Helpers::FunctionTraits::ReturnType;
    using Arguments = decltype(Helpers::Arguments::eval(typename Helpers::ArgumentIndexes{}));
    using BindableType = decltype(Helpers::template BindType<Arguments::CapturableCount>::eval(
            typename Arguments::SpecialIndexes{}));
};


QT_END_NAMESPACE

#endif  // QHTTPSERVERROUTERVIEWTRAITS_H
