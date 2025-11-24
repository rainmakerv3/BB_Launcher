// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QV4REFERENCEOBJECT_P_H
#define QV4REFERENCEOBJECT_P_H

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

#include <private/qv4object_p.h>
#include <private/qv4stackframe_p.h>
#include <private/qqmlnotifier_p.h>
#include <private/qv4qobjectwrapper_p.h>

QT_BEGIN_NAMESPACE

namespace QV4 {
namespace Heap {

struct ReferenceObject;
struct ReferenceObjectEndpoint : QQmlNotifierEndpoint {
    ReferenceObjectEndpoint(ReferenceObject* reference)
        : QQmlNotifierEndpoint(QQmlDirtyReferenceObject),
          reference(reference)
    {}

    ReferenceObject* reference;
};

#define ReferenceObjectMembers(class, Member) \
    Member(class, Pointer, Object *, m_object)

DECLARE_HEAP_OBJECT(ReferenceObject, Object) {
    DECLARE_MARKOBJECTS(ReferenceObject);

    enum Flag : quint8 {
        NoFlag           = 0,
        CanWriteBack     = 1 << 0,
        IsVariant        = 1 << 1,
        EnforcesLocation = 1 << 2,
        IsDirty = 1 << 3,
    };
    Q_DECLARE_FLAGS(Flags, Flag);

    void init(Object *object, int property, Flags flags)
    {
        auto connectToNotifySignal = [this](QObject* obj, int property, QQmlEngine* engine) {
            Q_ASSERT(obj);
            Q_ASSERT(engine);

            Q_ASSERT(!referenceEndpoint);
            Q_ASSERT(!bindableNotifier);

            referenceEndpoint = new ReferenceObjectEndpoint(this);
            referenceEndpoint->connect(
                obj,
                // Connect and signal emission work on "signal
                // indexe"s. Those are different from "method
                // indexes".
                // The public MetaObject interface can, generally,
                // give us the "method index" of the notify
                // signal.
                // Quite unintuitively, this is true for
                // "notifySignalIndex".
                // As the "method index" and the "signal index"
                // can be different, connecting the "method index"
                // of the notify signal can incur in issues when
                // the signal is being emitted and checking for
                // connected endpoints.
                // For example, we might be connected to the
                // "method index" of the notify signal for the
                // property and end up checking for the
                // subscribers of a different signal when the
                // notify signal is emitted, due to the different
                // meaning of the same index.
                // Thus we pass by the private interface to ensure
                // that we are connecting based on the "signal
                // index" instead.
                QMetaObjectPrivate::signalIndex(obj->metaObject()->property(property).notifySignal()),
                engine);
            // When the object that is being referenced is destroyed, we
            // need to ensure that one additional read is performed to
            // invalidate the data we hold.
            // As the object might be destroyed in a way that doesn't
            // trigger the notify signal for the relevant property, we react
            // directly to the destruction itself.
            // We use a plain connection instead of a QQmlNotifierEndpoint
            // based connection as, currently, declarative-side signals are
            // always discarded during destruction (see
            // QQmlData::signalEmitted).
            // In theory it should be possible to relax that condition for
            // the destroy signal specifically, which should allow a more
            // optimized way of connecting.
            // Nonetheless this seems to be the only place where we have
            // this kind of need, and thus go for the simpler solution,
            // which can be changed later if the need arise.
            new(onDelete) QMetaObject::Connection(QObject::connect(obj, &QObject::destroyed, [this](){ setDirty(true); }));
        };

        auto connectToBindable = [this](QObject* obj, int property, QQmlEngine* engine) {
            Q_ASSERT(obj);
            Q_ASSERT(engine);

            Q_ASSERT(!referenceEndpoint);
            Q_ASSERT(!bindableNotifier);

            bindableNotifier = new QPropertyNotifier(obj->metaObject()->property(property).bindable(obj).addNotifier([this](){ setDirty(true); }));
            new(onDelete) QMetaObject::Connection(QObject::connect(obj, &QObject::destroyed, [this](){ setDirty(true); }));
        };

        setObject(object);
        m_property = property;
        m_flags = flags;

        while (object &&
               object->internalClass->vtable->type != Managed::Type_V4QObjectWrapper &&
               object->internalClass->vtable->type != Managed::Type_QMLTypeWrapper)
        {
            if (!(object->internalClass->vtable->type == Managed::Type_V4ReferenceObject) &&
                !(object->internalClass->vtable->type == Managed::Type_V4Sequence) &&
                !(object->internalClass->vtable->type == Managed::Type_DateObject) &&
                !(object->internalClass->vtable->type == Managed::Type_QMLValueTypeWrapper))
            {
                break;
            }

            property = static_cast<QV4::Heap::ReferenceObject*>(object)->property();
            object = static_cast<QV4::Heap::ReferenceObject*>(object)->object();
        }

        if (object && object->internalClass->vtable->type == Managed::Type_V4QObjectWrapper)
        {
            auto wrapper = static_cast<QV4::Heap::QObjectWrapper*>(object);
            QObject* obj = wrapper->object();

            if (obj->metaObject()->property(property).isBindable() && internalClass->engine->qmlEngine())
                connectToBindable(obj, property, internalClass->engine->qmlEngine());
            else if (obj->metaObject()->property(property).hasNotifySignal() && internalClass->engine->qmlEngine())
                connectToNotifySignal(obj, property, internalClass->engine->qmlEngine());
        }

        if (object && object->internalClass->vtable->type == Managed::Type_QMLTypeWrapper) {
            auto wrapper = static_cast<QV4::Heap::QQmlTypeWrapper*>(object);

            Scope scope(internalClass->engine);
            Scoped<QV4::QQmlTypeWrapper> scopedWrapper(scope, wrapper);
            QObject* obj = scopedWrapper->object();

            if (obj->metaObject()->property(property).isBindable() && internalClass->engine->qmlEngine())
                connectToBindable(obj, property, internalClass->engine->qmlEngine());
            else if (obj->metaObject()->property(property).hasNotifySignal() && internalClass->engine->qmlEngine())
                connectToNotifySignal(obj, property, internalClass->engine->qmlEngine());
        }

        // If we could not connect to anything we don't have a way to
        // dirty on-demand and thus should be in an always dirty state
        // to ensure that reads go through.
        if (!isConnected())
            setDirty(true);

        Object::init();
    }

    Flags flags() const { return Flags(m_flags); }

    Object *object() const { return m_object.get(); }
    void setObject(Object *object) { m_object.set(internalClass->engine, object); }

    int property() const { return m_property; }

    bool canWriteBack() const { return hasFlag(CanWriteBack); }
    bool isVariant() const { return hasFlag(IsVariant); }
    bool enforcesLocation() const { return hasFlag(EnforcesLocation); }

    void setLocation(const Function *function, quint16 statement)
    {
        m_function = function;
        m_statementIndex = statement;
    }

    const Function *function() const { return m_function; }
    quint16 statementIndex() const { return m_statementIndex; }

    bool isAttachedToProperty() const
    {
        if (enforcesLocation()) {
            if (CppStackFrame *frame = internalClass->engine->currentStackFrame) {
                if (frame->v4Function != function() || frame->statementNumber() != statementIndex())
                    return false;
            } else {
                return false;
            }
        }

        return true;
    }

    bool isReference() const { return m_object; }

    bool isDirty() const { return hasFlag(IsDirty); }
    void setDirty(bool dirty) { setFlag(IsDirty, dirty); }
    bool isConnected() {
        return (referenceEndpoint && referenceEndpoint->isConnected()) || bindableNotifier;
    }

    void destroy() {
        // If we allocated any connection then we must have connected
        // to the destroyed signal too, and we should clean it up.
        if (referenceEndpoint || bindableNotifier) {
            QObject::disconnect(*reinterpret_cast<QMetaObject::Connection*>(&onDelete));
            std::destroy_at(reinterpret_cast<QMetaObject::Connection*>(&onDelete));
        }

        if (referenceEndpoint)
            delete referenceEndpoint;

        if (bindableNotifier)
            delete bindableNotifier;
    }

    private:

        bool hasFlag(Flag flag) const
        {
            return m_flags & quint8(flag);
        }

        void setFlag(Flag flag, bool set)
        {
            m_flags = set ? (m_flags | quint8(flag)) : (m_flags & ~quint8(flag));
        }

        const Function *m_function;
        int m_property;
        quint16 m_statementIndex;
        quint8 m_flags;
        ReferenceObjectEndpoint* referenceEndpoint;
        QPropertyNotifier* bindableNotifier;
        // We need to store an handle if we connect to the destroyed
        // signal so that we can disconnect from it. To avoid yet
        // another allocation, considering that
        // QMetaObject::Connection is not trivial, we store it in
        // block memory.
        alignas(alignof(QMetaObject::Connection))
        std::byte onDelete[sizeof(QMetaObject::Connection)];
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(ReferenceObject::Flags)

    } // namespace Heap


    struct ReferenceObject : public Object
    {
        V4_OBJECT2(ReferenceObject, Object)
        Q_MANAGED_TYPE(V4ReferenceObject)
        V4_NEEDS_DESTROY

    public:
        static constexpr const int AllProperties = -1;

        template<typename HeapObject>
        static bool readReference(HeapObject *ref)
        {
            if (!ref->object())
                return false;

            if (!ref->isDirty())
                return true;

            QV4::Scope scope(ref->internalClass->engine);
            QV4::ScopedObject object(scope, ref->object());

            bool wasRead = false;
            if (ref->isVariant()) {
                QVariant variant;
                void *a[] = { &variant };
                wasRead = object->metacall(QMetaObject::ReadProperty, ref->property(), a)
                    && ref->setVariant(variant);
            } else {
                void *a[] = { ref->storagePointer() };
                wasRead = object->metacall(QMetaObject::ReadProperty, ref->property(), a);
            }

            ref->setDirty(!ref->isConnected() || !wasRead);
        return wasRead;
    }

    template<typename HeapObject>
    static bool writeBack(HeapObject *ref, int internalIndex = AllProperties)
    {
        if (!ref->object() || !ref->canWriteBack())
            return false;

        QV4::Scope scope(ref->internalClass->engine);
        QV4::ScopedObject object(scope, ref->object());

        int flags = QQmlPropertyData::HasInternalIndex;
        int status = -1;
        if (ref->isVariant()) {
            QVariant variant = ref->toVariant();
            void *a[] = { &variant, nullptr, &status, &flags, &internalIndex };
            return object->metacall(QMetaObject::WriteProperty, ref->property(), a);
        }

        void *a[] = { ref->storagePointer(), nullptr, &status, &flags, &internalIndex };
        return object->metacall(QMetaObject::WriteProperty, ref->property(), a);
    }

    template<typename HeapObject>
    static HeapObject *detached(HeapObject *ref)
    {
        if (ref->object() && !ref->enforcesLocation() && !readReference(ref))
            return ref; // It's dead. No point in detaching it anymore

        return ref->detached();
    }
};

} // namespace QV4

QT_END_NAMESPACE

#endif // QV4REFERENCEOBJECT_P_H
