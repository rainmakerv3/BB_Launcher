// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMEDIATIMERANGE_H
#define QMEDIATIMERANGE_H

#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qlist.h>
#include <QtCore/qmetatype.h>

QT_BEGIN_NAMESPACE


class QMediaTimeRangePrivate;

QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QMediaTimeRangePrivate, Q_MULTIMEDIA_EXPORT)

class Q_MULTIMEDIA_EXPORT QMediaTimeRange
{
public:
    struct Interval
    {
        constexpr Interval() noexcept = default;
        explicit constexpr Interval(qint64 start, qint64 end) noexcept
        : s(start), e(end)
        {}

        constexpr qint64 start() const noexcept { return s; }
        constexpr qint64 end() const noexcept { return e; }

        constexpr bool contains(qint64 time) const noexcept
        {
            return isNormal() ? (s <= time && time <= e)
                : (e <= time && time <= s);
        }

        constexpr bool isNormal() const noexcept { return s <= e; }
        constexpr Interval normalized() const
        {
            return s > e ? Interval(e, s) : *this;
        }
        constexpr Interval translated(qint64 offset) const
        {
            return Interval(s + offset, e + offset);
        }

        friend constexpr bool operator==(Interval lhs, Interval rhs) noexcept
        {
            return lhs.start() == rhs.start() && lhs.end() == rhs.end();
        }
        friend constexpr bool operator!=(Interval lhs, Interval rhs) noexcept
        {
            return lhs.start() != rhs.start() || lhs.end() != rhs.end();
        }

    private:
        friend class QMediaTimeRangePrivate;
        qint64 s = 0;
        qint64 e = 0;
    };

    QMediaTimeRange();
    explicit QMediaTimeRange(qint64 start, qint64 end);
    QMediaTimeRange(const Interval&);
    QMediaTimeRange(const QMediaTimeRange &range) noexcept;
    ~QMediaTimeRange();

    QMediaTimeRange &operator=(const QMediaTimeRange&) noexcept;

    QMediaTimeRange(QMediaTimeRange &&other) noexcept = default;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QMediaTimeRange)
    void swap(QMediaTimeRange &other) noexcept
    { d.swap(other.d); }
    void detach();

    QMediaTimeRange &operator=(const Interval&);

    qint64 earliestTime() const;
    qint64 latestTime() const;

    QList<QMediaTimeRange::Interval> intervals() const;
    bool isEmpty() const;
    bool isContinuous() const;

    bool contains(qint64 time) const;

    void addInterval(qint64 start, qint64 end);
    void addInterval(const Interval &interval);
    void addTimeRange(const QMediaTimeRange&);

    void removeInterval(qint64 start, qint64 end);
    void removeInterval(const Interval &interval);
    void removeTimeRange(const QMediaTimeRange&);

    QMediaTimeRange& operator+=(const QMediaTimeRange&);
    QMediaTimeRange& operator+=(const Interval&);
    QMediaTimeRange& operator-=(const QMediaTimeRange&);
    QMediaTimeRange& operator-=(const Interval&);

    void clear();

    friend inline bool operator==(const QMediaTimeRange &lhs, const QMediaTimeRange &rhs)
    { return lhs.intervals() == rhs.intervals(); }
    friend inline bool operator!=(const QMediaTimeRange &lhs, const QMediaTimeRange &rhs)
    { return lhs.intervals() != rhs.intervals(); }

private:
    QExplicitlySharedDataPointer<QMediaTimeRangePrivate> d;
};

#ifndef QT_NO_DEBUG_STREAM
Q_MULTIMEDIA_EXPORT QDebug operator<<(QDebug, const QMediaTimeRange::Interval &);
Q_MULTIMEDIA_EXPORT QDebug operator<<(QDebug, const QMediaTimeRange &);
#endif

inline QMediaTimeRange operator+(const QMediaTimeRange &r1, const QMediaTimeRange &r2)
{ return (QMediaTimeRange(r1) += r2); }
inline QMediaTimeRange operator-(const QMediaTimeRange &r1, const QMediaTimeRange &r2)
{ return (QMediaTimeRange(r1) -= r2); }

Q_DECLARE_SHARED(QMediaTimeRange)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QMediaTimeRange)
Q_DECLARE_METATYPE(QMediaTimeRange::Interval)

#endif  // QMEDIATIMERANGE_H
