// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QRHIVALUEMAPPER_P_H
#define QRHIVALUEMAPPER_P_H

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

#include <QtMultimedia/qtmultimediaglobal.h>

#include <qreadwritelock.h>

#include <memory>
#include <map>

QT_BEGIN_NAMESPACE

class QRhi;

/**
 * @brief The QRhiCallback class implements a thread-safe wrapper around QRhi
 *        cleanup callbacks. For adding a callback to QRhi, create a shared insance
 *        of the class and invoke addToRhi for the specific QRhi. To deactivate
 *        the callback delete the instance.
 */
class Q_MULTIMEDIA_EXPORT QRhiCallback : public std::enable_shared_from_this<QRhiCallback>
{
public:
    class Manager;

    QRhiCallback();
    virtual ~QRhiCallback();

    void registerCallback(QRhi &rhi);

protected:
    virtual void onRhiCleanup(QRhi &rhi) = 0;

private:
    std::shared_ptr<Manager> m_manager;
};

/**
 * @brief The class associates values of the specified type with different QRhi.
 *        One instance of QRhiValueMapper associates one QRhi with one value.
 *        The mapped value is deleted when the matching QRhi is cleaned/deleted,
 *        when QRhiValueMapper::clear is invoked, or the QRhiValueMapper's
 *        instance is deleted.
 *
 *        QRhiValueMapper's API is thread safe, whereas the objects, which pointers you
 *        obtain via QRhiValueMapper::get(), are not. Thus, their thread-safity
 *        has to be managed by the code using the mapper.

 *        Note, that QRhiValueMapper destructs the values under its mutex.
 *        Keep it in mind and aim to avoid callbacks and signals emissions fror
 *        the Value's destructor.
 */
template <typename Value>
class QRhiValueMapper
{
    struct Data : QRhiCallback
    {
        QReadWriteLock lock;
        // In most cases, one or 2 elements will be here, so let's use map
        // instead of unordered_map for better efficiency with few elements.
        std::map<QRhi *, Value> storage;

        void onRhiCleanup(QRhi &rhi) override
        {
            QWriteLocker locker(&lock);
            storage.erase(&rhi);
        }
    };

public:
    ~QRhiValueMapper()
    {
        clear(); // it must be cleared when on the destruction to synchronize with rhi cleanup
    }

    Q_DISABLE_COPY(QRhiValueMapper);

    QRhiValueMapper(QRhiValueMapper&& ) noexcept = default;
    QRhiValueMapper& operator = (QRhiValueMapper&&) noexcept = default;

    QRhiValueMapper() : m_data(std::make_shared<Data>()) { }

    template <typename V>
    std::pair<Value *, bool> tryMap(QRhi &rhi, V &&value)
    {
        QWriteLocker locker(&m_data->lock);

        auto [rhiIt, rhiAdded] = m_data->storage.try_emplace(&rhi, std::forward<V>(value));

        if (rhiAdded)
            m_data->registerCallback(rhi);

        return { &rhiIt->second, rhiAdded };
    }

    Value *get(QRhi *rhi) const
    {
        QReadLocker locker(&m_data->lock);
        auto rhiIt = m_data->storage.find(rhi);
        return rhiIt == m_data->storage.end() ? nullptr : &rhiIt->second;
    }

    //    To be added for thread-safe value read:
    //
    //    template <typename Reader>
    //    void read(QRhi &rhi, Reader&& reader) const {
    //        QReadLocker locker(&m_data->lock);
    //        auto rhiIt = m_data->storage.find(&rhi);
    //        if (rhiIt != m_data->storage.end())
    //            reader(const_cast<const Value&>(rhiIt->second));
    //    }

    void clear()
    {
        if (!m_data)
            return; // the object has been moved
        QWriteLocker locker(&m_data->lock);
        m_data->storage.clear();
    }

    template <typename Predicate>
    QRhi *findRhi(Predicate &&p) const
    {
        QReadLocker locker(&m_data->lock);
        auto &storage = m_data->storage;

        auto it = std::find_if(storage.begin(), storage.end(),
                               [&p](auto &rhiItem) { return p(*rhiItem.first); });
        return it == storage.end() ? nullptr : it->first;
    }

private:
    std::shared_ptr<Data> m_data;
};

QT_END_NAMESPACE

#endif // QRHIVALUEMAPPER_P_H
