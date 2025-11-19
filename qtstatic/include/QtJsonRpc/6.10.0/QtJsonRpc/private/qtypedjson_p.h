// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QTYPEDJSON_H
#define QTYPEDJSON_H

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

#include <QtCore/QObject>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>
#include <QtCore/QScopeGuard>
#include <QtCore/QSet>
#include <QtCore/QByteArray>
#include <QtCore/QMetaEnum>
#include <QtCore/QLoggingCategory>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtJsonRpc/qtjsonrpcglobal.h>

#include <functional>
#include <memory>
#include <typeinfo>
#include <optional>

QT_BEGIN_NAMESPACE

namespace QTypedJson {
QT_DECLARE_EXPORTED_QT_LOGGING_CATEGORY(jsonRpcLog, Q_JSONRPC_EXPORT);

Q_NAMESPACE

enum class ObjectOption { None = 0, KeepExtraFields = 1, WarnExtra = 2 };
Q_ENUM_NS(ObjectOption)
Q_DECLARE_FLAGS(ObjectOptions, ObjectOption)
Q_DECLARE_OPERATORS_FOR_FLAGS(ObjectOptions)

enum class ParseMode { StopOnError };
Q_ENUM_NS(ParseMode)

enum class ParseStatus { Normal, Failed };
Q_ENUM_NS(ParseStatus)

template<typename... Ts>
using void_t = void;

template<typename T, typename = void>
struct HasTypeName : std::false_type
{
};

template<typename T>
struct HasTypeName<T, void_t<decltype(T::TypeName)>> : std::true_type
{
};

template<typename T, typename = void>
struct HasMetaEnum : std::false_type
{
};

template<typename T>
struct HasMetaEnum<T, void_t<decltype(QMetaEnum::fromType<T>())>> : std::true_type
{
};

template<typename T>
const char *typeName()
{
    if constexpr (HasTypeName<T>::value)
        return T::TypeName;
    else
        return typeid(T).name();
}

template<typename T, typename = void>
struct JsonObjectOptions
{
    static constexpr ObjectOptions value = ObjectOption::None;
};

template<typename T>
struct JsonObjectOptions<T, void_t<decltype(T::jsonObjectOptions)>> : std::true_type
{
    static constexpr ObjectOptions value = T::jsonObjectOptions;
};

template<typename T, typename = void>
struct HasExtraFields : std::false_type
{
};

template<typename T>
struct HasExtraFields<T, void_t<decltype(std::declval<T>().extraFields())>> : std::true_type
{
};

template<typename T, typename = void>
struct SetExtraFields : std::false_type
{
};

template<typename T>
struct SetExtraFields<
        T, void_t<decltype(std::declval<T>().setExtraFields(std::declval<QJsonObject>()))>>
    : std::true_type
{
};

template<typename T, typename = void>
struct IsList : std::false_type
{
};

template<typename T>
struct IsList<T, void_t<typename T::value_type>> : std::true_type
{
};

template<typename T>
struct IsPointer : std::is_pointer<T>
{
};

template<typename T>
struct IsPointer<std::shared_ptr<T>> : std::true_type
{
};

template<typename T>
struct IsPointer<std::unique_ptr<T>> : std::true_type
{
};

template<typename T>
struct IsVariant : std::false_type
{
};

template<typename... Args>
struct IsVariant<std::variant<Args...>> : std::true_type
{
};

template<typename T>
inline QString enumToString(T value)
{
    int iValue = int(value);
    if constexpr (HasMetaEnum<T>::value) {
        QMetaEnum metaEnum = QMetaEnum::fromType<T>();
        for (int i = 0; i < metaEnum.keyCount(); ++i) {
            if (iValue == metaEnum.value(i))
                return QString::fromUtf8(metaEnum.key(i));
        }
    }
    return QString::number(iValue);
}

template<typename T>
inline T enumFromString(const QString &value)
{
    bool ok;
    int v = value.toInt(&ok);
    if (ok)
        return T(v);
    if constexpr (HasMetaEnum<T>::value) {
        QMetaEnum metaEnum = QMetaEnum::fromType<T>();
        for (int i = 0; i < metaEnum.keyCount(); ++i) {
            if (value.compare(QLatin1String(metaEnum.key(i)), Qt::CaseInsensitive) == 0)
                return T { metaEnum.value(i) };
        }
    }
    return T {};
}

template<typename T>
inline QString enumToIntString(T value)
{
    return QString::number(int(value));
}

template<typename T>
inline T enumFromIntString(const QString &value)
{
    bool ok;
    int v = value.toInt(&ok);
    if (ok)
        return T(v);
    return T {};
}

class Q_JSONRPC_EXPORT ValueStack
{
public:
    QJsonValue value;
    QString fieldPath;
    int indexPath = -1;
    int warnLevel = 0;
};

class ObjectStack
{
public:
    const char *type;
    ObjectOptions options;
    QSet<QString> visitedFields;
};

class ReaderPrivate
{
public:
    QList<ValueStack> valuesStack = {};
    QList<ObjectStack> objectsStack = {};
    ObjectOptions baseOptions = {};
    ParseMode parseMode = ParseMode::StopOnError;
    ParseStatus parseStatus = ParseStatus::Normal;
    QStringList errorMessages = {};
};

class Q_JSONRPC_EXPORT Reader
{
public:
    Reader(const QJsonValue &v);
    ~Reader();

    QStringList errorMessages();
    void clearErrorMessages();

    // serialization templates

    template<typename T>
    bool startObject(const char *type, ObjectOptions options, quintptr id, T &)
    {
        return this->startObjectF(type, options, id);
    }
    template<typename T>
    void endObject(const char *type, ObjectOptions options, quintptr id, T &obj);

    template<typename T>
    bool startArray(qint32 &size, T &el)
    {
        startArrayF(size);
        using BaseT = std::decay_t<T>;
        if constexpr (std::is_base_of_v<QList<typename BaseT::value_type>, BaseT>) {
            el.resize(size);
        } else {
            assert(false); // currently unsupported
        }
        return true;
    }

    template<typename T>
    bool handleOptional(T &el)
    {
        bool isMissing = currentValue().isUndefined() || currentValue().isNull();
        if (isMissing)
            el.reset();
        else
            el.emplace();
        return bool(el);
    }

    template<typename T>
    bool handlePointer(T &el)
    {
        bool isMissing = currentValue().isUndefined() || currentValue().isNull();
        if (isMissing)
            el = nullptr;
        else
            el = T(new std::decay_t<decltype(*el)>);
        return bool(el);
    }

    template<typename... T>
    void handleVariant(std::variant<T...> &el)
    {
        std::tuple<T...> options;
        int status = 0;
        ReaderPrivate origStatus = *m_p;
        QStringList err;
        auto tryRead = [this, &origStatus, &status, &el, &err](auto &x) {
            if (status == 2)
                return;
            if (status == 1)
                *this->m_p = origStatus;
            else
                status = 1;
            doWalk(*this, x);
            if (m_p->parseStatus == ParseStatus::Normal) {
                status = 2;
                el = x;
                return;
            }
            err.append(QStringLiteral(u"Type %1 failed with errors:")
                               .arg(QLatin1String(typeid(decltype(x)).name())));
            err += m_p->errorMessages;
        };
        std::apply([&tryRead](auto &...x) { (..., tryRead(x)); }, options);
        if (status == 1) {
            m_p->errorMessages.clear();
            m_p->errorMessages.append(QStringLiteral(u"All options of variant failed:"));
            m_p->errorMessages += err;
        }
    }

    template<typename T>
    void handleEnum(T &e)
    {
        if (currentValue().isDouble()) {
            e = T(currentValue().toInt());
        } else {
            e = enumFromString<T>(currentValue().toString());
        }
    }

    template<typename T>
    void endArray(qint32 &size, T &)
    {
        this->endArrayF(size);
    }

    //  serialization callbacks
    void handleBasic(bool &);
    void handleBasic(QByteArray &);
    void handleBasic(int &);
    void handleBasic(double &);
    void handleNullType();
    void handleJson(QJsonValue &v);
    void handleJson(QJsonObject &v);
    void handleJson(QJsonArray &v);
    bool startField(const QString &fieldName);
    bool startField(const char *fieldName);
    void endField(const QString &fieldName);
    void endField(const char *fieldName);
    bool startElement(qint32 index);
    void endElement(qint32 index);
    bool startTuple(qint32 size);
    void endTuple(qint32 size);

private:
    void warnExtra(const QJsonObject &e);
    void warnMissing(QStringView s);
    void warnNonNull();
    void warnInvalidSize(qint32 size, qint32 expectedSize);
    void warn(const QString &msg);
    QJsonObject getExtraFields() const;
    bool startObjectF(const char *type, ObjectOptions options, quintptr id);
    void endObjectF(const char *type, ObjectOptions options, quintptr id);
    void startArrayF(qint32 &size);
    void endArrayF(qint32 &size);
    bool hasElement();
    QString currentPath() const;
    const QJsonValue &currentValue() const { return m_p->valuesStack.last().value; }
    ReaderPrivate *m_p;
};

template<typename T, typename = void>
struct HasWalk : std::false_type
{
};

template<typename T>
struct HasWalk<T, void_t<decltype(T {}.walk(std::declval<Reader>))>> : std::true_type
{
};

template<typename W, typename C, typename T>
void field(W &w, const C &fieldName, T &el)
{
    if (w.startField(fieldName)) {
        auto guard = qScopeGuard([&w, &fieldName]() { w.endField(fieldName); });
        doWalk(w, el);
    }
}

template<typename W, typename T>
inline void doWalk(W &w, T &el)
{
    using BaseT = std::decay_t<T>;
    if constexpr (
            std::is_same_v<
                    BaseT,
                    int> || std::is_same_v<BaseT, double> || std::is_same_v<BaseT, bool> || std::is_same_v<BaseT, QByteArray>) {
        w.handleBasic(el);
    } else if constexpr (HasWalk<BaseT>::value) {
        const char *type = typeName<BaseT>();
        ObjectOptions options = JsonObjectOptions<BaseT>::value;
        quintptr id = quintptr(&el);
        if (w.startObject(type, options, id, el)) {
            el.walk(w);
            w.endObject(type, options, id, el);
        }
    } else if constexpr (
            std::is_same_v<
                    BaseT,
                    QJsonValue> || std::is_same_v<BaseT, QJsonObject> || std::is_same_v<BaseT, QJsonArray>) {
        w.handleJson(el);
    } else if constexpr (std::is_enum_v<BaseT>) {
        w.handleEnum(el);
    } else if constexpr (std::is_same_v<std::nullptr_t, BaseT>) {
        w.handleNullType();
    } else if constexpr (IsPointer<BaseT>::value) {
        if (w.handlePointer(el) && el)
            doWalk(w, *el);
    } else if constexpr (IsVariant<BaseT>::value) {
        w.handleVariant(el);
    } else if constexpr (IsList<BaseT>::value) {
        if constexpr (std::is_same_v<std::optional<typename BaseT::value_type>, BaseT>) {
            if (w.handleOptional(el) && el)
                doWalk(w, *el);
        } else {
            // broken gcc warns about optional being uninitialized, QTBUG-128574
            QT_WARNING_PUSH
            QT_WARNING_DISABLE_GCC("-Wmaybe-uninitialized")
            int size = el.size();
            QT_WARNING_POP
            if (!w.startArray(size, el))
                return;
            int i = 0;
            for (auto &subEl : el) {
                if (!w.startElement(i))
                    break;
                doWalk(w, subEl);
                w.endElement(i);
                ++i;
            }
            w.endArray(size, el);
        }
    } else {
        qWarning() << "Unhandled type" << typeid(T).name();
        assert(false);
    }
}

template<typename T>
inline void Reader::endObject(const char *type, ObjectOptions options, quintptr id, T &obj)
{
    using BaseT = std::decay_t<T>;
    QJsonObject extra;
    if (SetExtraFields<BaseT>::value
        || (options & (ObjectOption::KeepExtraFields | ObjectOption::WarnExtra)))
        extra = this->getExtraFields();
    this->endObjectF(type, options, id);
    if constexpr (SetExtraFields<BaseT>::value)
        obj.setExtraFields(extra);
    else if (extra.constBegin() != extra.constEnd())
        warnExtra(extra);
}

class Q_JSONRPC_EXPORT JsonBuilder
{
public:
    JsonBuilder() = default;

    // public api
    QJsonValue popLastValue();

    // serialization templates
    template<typename T>
    bool handleOptional(T &el)
    {
        if (el)
            return true;
        this->handleMissingOptional();
        return false;
    }

    template<typename T>
    bool handlePointer(T &el)
    {
        return bool(el);
    }

    template<typename T>
    bool startObject(const char *type, ObjectOptions options, quintptr id, T &)
    {
        return this->startObjectF(type, options, id);
    }

    template<typename T>
    void endObject(const char *type, ObjectOptions options, quintptr id, T &)
    {
        this->endObjectF(type, options, id);
    }

    template<typename T>
    bool startArray(qint32 &size, T &el)
    {
        using BaseT = std::decay_t<T>;
        if constexpr (std::is_base_of_v<QList<typename BaseT::value_type>, BaseT>) {
            size = el.size();
        } else {
            assert(false); // currently unsupported
        }
        return startArrayF(size);
    }

    template<typename T>
    void endArray(qint32 &size, T &)
    {
        this->endArrayF(size);
    }

    template<typename T>
    void handleVariant(T &el)
    {
        std::visit([this](auto &v) { doWalk(*this, v); }, el);
    }

    template<typename T>
    void handleEnum(T &el)
    {
        QString eVal = enumToString(el);
        bool ok;
        int value = eVal.toInt(&ok);
        if (ok)
            this->handleBasic(value);
        else
            this->handleBasic(eVal.toUtf8());
    }

    // serialization callbacks
    void handleBasic(const bool &v);
    void handleBasic(const QByteArray &v);
    void handleBasic(const int &v);
    void handleBasic(const double &v);
    void handleNullType();
    void handleJson(QJsonValue &v);
    void handleJson(QJsonObject &v);
    void handleJson(QJsonArray &v);
    bool startField(const QString &fieldName);
    bool startField(const char *fieldName);
    void endField(const QString &);
    void endField(const char *);
    bool startElement(qint32 index);
    void endElement(qint32);
    bool startTuple(qint32 size);
    void endTuple(qint32 size);

private:
    void handleMissingOptional();
    bool startObjectF(const char *, ObjectOptions, quintptr);
    void endObjectF(const char *, ObjectOptions, quintptr);
    bool startArrayF(qint32 &);
    void endArrayF(qint32 &);

    QList<qsizetype> m_fieldLevel;
    QList<qsizetype> m_arrayLevel;
    QList<std::variant<QJsonObject, QJsonArray, QJsonValue>> m_values;
};

template<typename... Params>
QJsonValue toJsonValue(Params ...params)
{
    if constexpr (sizeof...(Params) == 0)
        return QJsonValue(QJsonValue::Type::Undefined);

    JsonBuilder b;
    if constexpr (sizeof...(Params) == 1) {
        doWalk(b, params...);
    } else if (b.startTuple(sizeof...(Params))) {
        qint32 i = 0;
        bool skipRest = false;
        std::apply([&i, &b, &skipRest](auto &el) {
            if (skipRest || !b.startElement(i)) {
                skipRest = true;
            } else {
                    doWalk(b, el);
                    b.endElement(i++);
                }
            }, params...);
        b.endTuple(sizeof...(Params));
    }
    return b.popLastValue();
}

} // namespace QTypedJson
QT_END_NAMESPACE
#endif // QTYPEDJSON_H
