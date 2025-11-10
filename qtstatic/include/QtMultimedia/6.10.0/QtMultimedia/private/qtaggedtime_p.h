// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QTAGGEDTIME_P_H
#define QTAGGEDTIME_P_H

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

#include "qcompare.h"

QT_BEGIN_NAMESPACE

template <typename ValueType, typename ThisType, typename AmendedType = ThisType>
class BaseTime
{
public:
    constexpr ValueType get() const noexcept { return m_value; }

    constexpr BaseTime(ValueType value) noexcept : m_value(value) { }
    constexpr BaseTime(const ThisType &other) noexcept : m_value(other.m_value) { }

    constexpr ThisType &operator=(const ThisType &other) noexcept
    {
        m_value = other.m_value;
        return static_cast<ThisType &>(*this);
    }

    friend bool comparesEqual(const ThisType &lhs, const ThisType &rhs) noexcept
    {
        return lhs.m_value == rhs.m_value;
    }

    friend constexpr Qt::strong_ordering compareThreeWay(const ThisType &lhs,
                                                         const ThisType &rhs) noexcept
    {
        return qCompareThreeWay(lhs.m_value, rhs.m_value);
    }

    Q_DECLARE_STRONGLY_ORDERED(ThisType);

    constexpr ThisType operator-() const { return ThisType(-m_value); }

    friend constexpr ThisType operator+(const ThisType &lhs, const AmendedType &rhs) noexcept
    {
        return ThisType(lhs.m_value + rhs.get());
    }

    template <typename T = ThisType, std::enable_if_t<!std::is_same_v<AmendedType, T>> * = nullptr>
    friend constexpr ThisType operator+(const AmendedType &lhs, const ThisType &rhs) noexcept
    {
        return ThisType(lhs.get() + rhs.m_value);
    }

    friend constexpr ThisType operator-(const ThisType &lhs, const AmendedType &rhs) noexcept
    {
        return ThisType(lhs.m_value - rhs.get());
    }

    friend constexpr ThisType &operator+=(ThisType &lhs, const AmendedType &rhs) noexcept
    {
        lhs.m_value += rhs.get();
        return lhs;
    }

    friend constexpr ThisType &operator-=(ThisType &lhs, const AmendedType &rhs) noexcept
    {
        lhs.m_value -= rhs.get();
        return lhs;
    }

private:
    ValueType m_value;
};

template <typename ValueType, typename Tag>
class QTaggedTimePoint;

template <typename ValueType, typename Tag>
class QTaggedDuration : public BaseTime<ValueType, QTaggedDuration<ValueType, Tag>>
{
public:
    using TimePoint = QTaggedTimePoint<ValueType, Tag>;
    using BaseTime<ValueType, QTaggedDuration>::BaseTime;

    constexpr TimePoint asTimePoint() const noexcept { return TimePoint(this->get()); }
};

template <typename ValueType, typename Tag>
class QTaggedTimePoint
    : public BaseTime<ValueType, QTaggedTimePoint<ValueType, Tag>, QTaggedDuration<ValueType, Tag>>
{
public:
    using Duration = QTaggedDuration<ValueType, Tag>;
    using Base = BaseTime<ValueType, QTaggedTimePoint, Duration>;
    using Base::Base;

    friend constexpr Duration operator-(const QTaggedTimePoint &lhs,
                                        const QTaggedTimePoint &rhs) noexcept
    {
        return Duration(lhs.get() - rhs.get());
    }

    constexpr Duration asDuration() const noexcept { return Duration(this->get()); }
};

QT_END_NAMESPACE

#endif // QTAGGEDTIME_P_H
