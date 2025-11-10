// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMULTIMEDIA_ENUM_TO_STRING_CONVERTER_P_H
#define QMULTIMEDIA_ENUM_TO_STRING_CONVERTER_P_H

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

#include <QtCore/private/qglobal_p.h>
#include <QtCore/qstring.h>

#include <optional>

QT_BEGIN_NAMESPACE

#define QT_MM_CAT(x, y) QT_MM_IMPL_CAT(x, y)
#define QT_MM_IMPL_CAT(x, y) x##y

// clang-format off

#define QT_MM_IMPL_GEN_CASE_MAP_ENUM_TO_STRING(SYMBOL, STRING) \
    case SYMBOL:                                               \
        return QStringLiteral(STRING);

// clang-format on

#define QT_MM_IMPL_GEN_CASES_ENUM_TO_STRING(seq)               \
    QT_MM_CAT(QT_MM_IMPL_GEN_CASES_ENUM_TO_STRING_1 seq, _END) \
    static_assert(true, "force semicolon")
#define QT_MM_IMPL_GEN_CASES_ENUM_TO_STRING_1(x, y) \
    QT_MM_IMPL_GEN_CASE_MAP_ENUM_TO_STRING(x, y) QT_MM_IMPL_GEN_CASES_ENUM_TO_STRING_2
#define QT_MM_IMPL_GEN_CASES_ENUM_TO_STRING_2(x, y) \
    QT_MM_IMPL_GEN_CASE_MAP_ENUM_TO_STRING(x, y) QT_MM_IMPL_GEN_CASES_ENUM_TO_STRING_1
#define QT_MM_IMPL_GEN_CASES_ENUM_TO_STRING_1_END
#define QT_MM_IMPL_GEN_CASES_ENUM_TO_STRING_2_END

namespace QtMultimediaPrivate {

struct EnumName
{
};

template <typename Enum, typename Role = EnumName>
struct StringResolver
{
    static std::optional<QString> toQString(Enum);
};

} // namespace QtMultimediaPrivate

// clang-format off

#define QT_MM_MAKE_STRING_RESOLVER(Enum, EnumName, ...)        \
    template <>                                                \
    struct QtMultimediaPrivate::StringResolver<Enum, EnumName> \
    {                                                          \
        static std::optional<QString> toQString(Enum arg)      \
        {                                                      \
            switch (arg) {                                     \
            QT_MM_IMPL_GEN_CASES_ENUM_TO_STRING(__VA_ARGS__);  \
            default:                                           \
                return std::nullopt;                           \
            }                                                  \
        }                                                      \
    };                                                         \
    static_assert(true, "force semicolon")

// clang-format on

#define QT_MM_DEFINE_QDEBUG_ENUM(EnumType)                                     \
    QDebug operator<<(QDebug dbg, EnumType arg)                                \
    {                                                                          \
        std::optional<QString> resolved =                                      \
                QtMultimediaPrivate::StringResolver<EnumType>::toQString(arg); \
        if (resolved)                                                          \
            dbg << *resolved;                                                  \
        else                                                                   \
            dbg << "Unknown Enum value";                                       \
        return dbg;                                                            \
    }

QT_END_NAMESPACE

#endif // QMULTIMEDIA_ENUM_TO_STRING_CONVERTER_P_H
