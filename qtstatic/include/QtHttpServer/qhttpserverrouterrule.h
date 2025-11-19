// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERROUTERRULE_H
#define QHTTPSERVERROUTERRULE_H

#include <QtHttpServer/qhttpserverrequest.h>
#include <QtHttpServer/qhttpserverresponder.h>
#include <QtHttpServer/qhttpserverrouterviewtraits.h>

#include <QtCore/qcontainerfwd.h>
#include <QtCore/qregularexpression.h>

#include <initializer_list>
#include <memory>

QT_BEGIN_NAMESPACE

class QString;
class QHttpServerRequest;
class QHttpServerResponder;
class QRegularExpressionMatch;
class QHttpServerRouter;

class QHttpServerRouterRulePrivate;
class Q_HTTPSERVER_EXPORT QHttpServerRouterRule
{
    Q_DECLARE_PRIVATE(QHttpServerRouterRule)
    Q_DISABLE_COPY_MOVE(QHttpServerRouterRule)

private:
    using RouterHandlerPrototype = void (*)(const QRegularExpressionMatch &,
                                            const QHttpServerRequest &, QHttpServerResponder &);

    template <typename T>
    using if_routerhandler_prototype_compatible = typename std::enable_if<
            QtPrivate::AreFunctionsCompatible<RouterHandlerPrototype, T>::value, bool>::type;

    QHttpServerRouterRule(const QString &pathPattern, const QHttpServerRequest::Methods methods,
                          const QObject *context, QtPrivate::QSlotObjectBase *slotObjRaw);

public:
#ifdef Q_QDOC
    template <typename Functor>
    QHttpServerRouterRule(
            const QString &pathPattern, const QHttpServerRequest::Methods methods,
            const QObject *receiver,
            Functor &&slot);

    template <typename Functor>
    QHttpServerRouterRule(
            const QString &pathPattern,
            const QObject *receiver,
            Functor &&slot);
#else
    template <typename Handler, if_routerhandler_prototype_compatible<Handler> = true>
    QHttpServerRouterRule(
            const QString &pathPattern,
            const typename QtPrivate::ContextTypeForFunctor<Handler>::ContextType *context,
            Handler &&func)
        : QHttpServerRouterRule(
                  pathPattern, QHttpServerRequest::Method::AnyKnown, context,
                  QtPrivate::makeCallableObject<RouterHandlerPrototype>(std::forward<Handler>(func)))
    {
    }

    template <typename Handler, if_routerhandler_prototype_compatible<Handler> = true>
    QHttpServerRouterRule(
            const QString &pathPattern, const QHttpServerRequest::Methods methods,
            const typename QtPrivate::ContextTypeForFunctor<Handler>::ContextType *context,
            Handler &&func)
        : QHttpServerRouterRule(
                  pathPattern, methods, context,
                  QtPrivate::makeCallableObject<RouterHandlerPrototype>(std::forward<Handler>(func)))
    {
    }
#endif

#ifdef Q_QDOC
    template <typename Functor, typename ViewTraits = QHttpServerRouterViewTraits<Functor>>
    static typename ViewTraits::BindableType bindCaptured(
                    QObject *receiver,
                    Functor &&slot,
                    const QRegularExpressionMatch &match) const;
# else
    template<typename ViewHandler, typename ViewTraits = QHttpServerRouterViewTraits<ViewHandler>>
    static typename ViewTraits::BindableType bindCaptured(
                    const typename QtPrivate::ContextTypeForFunctor<ViewHandler>::ContextType *context,
                    ViewHandler &&handler,
                    const QRegularExpressionMatch &match)
    {
        return bindCapturedImpl<ViewHandler, ViewTraits>(
                context, std::forward<ViewHandler>(handler), match,
                typename ViewTraits::Arguments::CapturableIndexes{});
    }
#endif

    const QObject *contextObject() const;

    virtual ~QHttpServerRouterRule();

protected:
    bool exec(const QHttpServerRequest &request, QHttpServerResponder &responder) const;

    bool hasValidMethods() const;

    bool createPathRegexp(std::initializer_list<QMetaType> metaTypes,
                          const QHash<QMetaType, QString> &converters);

    virtual bool matches(const QHttpServerRequest &request,
                         QRegularExpressionMatch *match) const;

    QHttpServerRouterRule(QHttpServerRouterRulePrivate *d);

    // Implementation of C++20 std::bind_front() in C++17
    template<typename F, typename... Args>
    static auto bind_front(F &&f, Args &&...args)
    {
        return [f = std::forward<F>(f),
                args = std::make_tuple(std::forward<Args>(args)...)](auto &&...callArgs) {
            return std::apply(f,
                              std::tuple_cat(args,
                                             std::forward_as_tuple(std::forward<decltype(callArgs)>(
                                                     callArgs)...)));
        };
    }

    template<typename ViewHandler, typename ViewTraits, size_t... Cx>
    static typename ViewTraits::BindableType bindCapturedImpl(
                const typename QtPrivate::ContextTypeForFunctor<ViewHandler>::ContextType *context,
                ViewHandler &&handler,
                const QRegularExpressionMatch &match,
                std::index_sequence<Cx...>)
    {
        if constexpr (std::is_member_function_pointer_v<ViewHandler>) {
            return bind_front(
                handler, const_cast<typename QtPrivate::ContextTypeForFunctor<ViewHandler>::ContextType*>(context),
                QVariant(match.captured(Cx + 1))
                        .value<typename ViewTraits::Arguments::template Arg<Cx>::CleanType>()...);
        } else {
            Q_UNUSED(context);
            return bind_front(
                    handler,
                    QVariant(match.captured(Cx + 1))
                            .value<typename ViewTraits::Arguments::template Arg<Cx>::CleanType>()...);
        }
    }

private:
    std::unique_ptr<QHttpServerRouterRulePrivate> d_ptr;

    friend class QHttpServerRouter;
};

QT_END_NAMESPACE

#endif // QHTTPSERVERROUTERRULE_H
