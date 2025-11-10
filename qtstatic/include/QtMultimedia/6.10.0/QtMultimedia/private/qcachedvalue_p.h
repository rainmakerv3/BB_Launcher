// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QCACHEDVALUE_P_H
#define QCACHEDVALUE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QReadWriteLock>

#include <optional>
#include <unordered_map>

QT_BEGIN_NAMESPACE

template <typename T>
class QCachedValue
{
public:
    QCachedValue() = default;

    Q_DISABLE_COPY(QCachedValue)

    template <typename Creator>
    T ensure(Creator &&creator)
    {
        {
            QReadLocker locker(&m_lock);
            if (m_cached)
                return *m_cached;
        }

        {
            QWriteLocker locker(&m_lock);
            if (!m_cached)
                m_cached = creator();
            return *m_cached;
        }
    }

    bool update(T value)
    {
        QWriteLocker locker(&m_lock);
        if (value == m_cached)
            return false;
        auto temp = std::exchange(m_cached, std::move(value));
        locker.unlock();
        return true;
    }

    void reset()
    {
        QWriteLocker locker(&m_lock);
        auto temp = std::exchange(m_cached, std::nullopt);
        locker.unlock();
    }

private:
    QReadWriteLock m_lock;
    std::optional<T> m_cached;
};

template <typename Key, typename Value>
class QCachedValueMap
{
public:
    QCachedValueMap() = default;

    Q_DISABLE_COPY(QCachedValueMap)

    template <typename Creator>
    Value ensure(const Key &key, Creator &&creator)
    {
        {
            QReadLocker locker(&m_lock);
            auto it = m_map.find(key);
            if (it != m_map.end())
                return it->second;
        }

        {
            QWriteLocker locker(&m_lock);
            auto emplaceRes = m_map.try_emplace(key, creator());
            return emplaceRes.first->second;
        }
    }

private:
    QReadWriteLock m_lock;
    std::unordered_map<Key, Value> m_map;
};

QT_END_NAMESPACE

#endif // QCACHEDVALUE_P_H
