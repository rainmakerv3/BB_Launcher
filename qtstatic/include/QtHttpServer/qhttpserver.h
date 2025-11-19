// Copyright (C) 2020 Mikhail Svetkin <mikhail.svetkin@gmail.com>
// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVER_H
#define QHTTPSERVER_H

#include <QtHttpServer/qabstracthttpserver.h>
#include <QtHttpServer/qhttpserverrouter.h>
#include <QtHttpServer/qhttpserverrouterrule.h>
#include <QtHttpServer/qhttpserverresponse.h>
#include <QtHttpServer/qhttpserverrouterviewtraits.h>

#if QT_CONFIG(future)
#  include <QtCore/qfuture.h>
#endif

#include <functional>
#include <tuple>

QT_BEGIN_NAMESPACE

class QHttpServerRequest;

class QHttpServerPrivate;
class Q_HTTPSERVER_EXPORT QHttpServer final : public QAbstractHttpServer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QHttpServer)

    template <typename...>
    static constexpr bool dependent_false_v = false;

    template<typename T>
    using ResponseType =
#if QT_CONFIG(future)
        std::conditional_t<
            std::is_same_v<QFuture<QHttpServerResponse>, T>,
            QFuture<QHttpServerResponse>,
            QHttpServerResponse
        >;
#else
        QHttpServerResponse;
#endif

    using MissingHandlerPrototype = void (*)(const QHttpServerRequest &request,
                                             QHttpServerResponder &responder);
    template <typename T>
    using if_missinghandler_prototype_compatible = typename std::enable_if<
            QtPrivate::AreFunctionsCompatible<MissingHandlerPrototype, T>::value, bool>::type;

    using AfterRequestPrototype = void (*)(const QHttpServerRequest &request,
                                           QHttpServerResponse &response);
    template <typename T>
    using if_after_request_prototype_compatible = typename std::enable_if<
            QtPrivate::AreFunctionsCompatible<AfterRequestPrototype, T>::value, bool>::type;

public:
    explicit QHttpServer(QObject *parent = nullptr);
    ~QHttpServer() override;

    QHttpServerRouter *router();
    const QHttpServerRouter *router() const;

#ifdef Q_QDOC
    template <typename Rule = QHttpServerRouterRule, typename Functor>
    Rule *route(const QString &pathPattern, QHttpServerRequest::Methods method,
                const QObject *context,
                Functor &&slot);

    template <typename Rule = QHttpServerRouterRule, typename Functor>
    Rule *route(const QString &pathPattern,
                const QObject *context,
                Functor &&slot);

    template <typename Rule = QHttpServerRouterRule, typename Functor>
    Rule *route(const QString &pathPattern, QHttpServerRequest::Methods method,
                Functor &&handler);

    template <typename Rule = QHttpServerRouterRule, typename Functor>
    Rule *route(const QString &pathPattern,
                Functor &&handler);
#else
    template<typename Rule = QHttpServerRouterRule, typename ViewHandler>
    Rule *route(const QString &pathPattern, QHttpServerRequest::Methods method,
                const typename QtPrivate::ContextTypeForFunctor<ViewHandler>::ContextType *context,
                ViewHandler &&viewHandler)
    {
        using ViewTraits = QHttpServerRouterViewTraits<ViewHandler>;
        static_assert(ViewTraits::Arguments::StaticAssert,
                      "ViewHandler arguments are in the wrong order or not supported");
        return routeImpl<Rule, ViewHandler, ViewTraits>(pathPattern, method, context,
                                                        std::forward<ViewHandler>(viewHandler));
    }

    template<typename Rule = QHttpServerRouterRule, typename ViewHandler>
    Rule *route(const QString &pathPattern,
                const typename QtPrivate::ContextTypeForFunctor<ViewHandler>::ContextType *context,
                ViewHandler &&viewHandler)
    {
        using ViewTraits = QHttpServerRouterViewTraits<ViewHandler>;
        static_assert(ViewTraits::Arguments::StaticAssert,
                      "ViewHandler arguments are in the wrong order or not supported");
        return routeImpl<Rule, ViewHandler, ViewTraits>(pathPattern, context,
                                                        std::forward<ViewHandler>(viewHandler));
    }

    template<typename Rule = QHttpServerRouterRule, typename ViewHandler>
    Rule *route(const QString &pathPattern,
                ViewHandler &&viewHandler)
    {
        return route<Rule>(pathPattern, QHttpServerRequest::Method::AnyKnown,
                           this, std::forward<ViewHandler>(viewHandler));
    }

    template<typename Rule = QHttpServerRouterRule, typename ViewHandler>
    Rule *route(const QString &pathPattern, QHttpServerRequest::Methods method,
                ViewHandler &&viewHandler)
    {
        return route<Rule>(pathPattern, method,
                           this, std::forward<ViewHandler>(viewHandler));
    }
#endif

#ifdef Q_QDOC
    template <typename Functor>
    void setMissingHandler(const QObject *context, Functor &&slot);
#else
    template <typename Handler, if_missinghandler_prototype_compatible<Handler> = true>
    void setMissingHandler(const typename QtPrivate::ContextTypeForFunctor<Handler>::ContextType *context,
                           Handler &&handler)
    {
        setMissingHandlerImpl(context,
                              QtPrivate::makeCallableObject<MissingHandlerPrototype>(
                                  std::forward<Handler>(handler)));
    }
#endif

#ifdef Q_QDOC
    template <typename Functor>
    void addAfterRequestHandler(const QObject *context, Functor &&slot);
#else
    template <typename Handler, if_after_request_prototype_compatible<Handler> = true>
    void addAfterRequestHandler(const typename QtPrivate::ContextTypeForFunctor<Handler>::ContextType *context,
                                Handler &&handler)
    {
        addAfterRequestHandlerImpl(context,
                                   QtPrivate::makeCallableObject<AfterRequestPrototype>(
                                   std::forward<Handler>(handler)));
    }
#endif

    void clearMissingHandler();

private:
    void setMissingHandlerImpl(const QObject *context, QtPrivate::QSlotObjectBase *handler);

    void addAfterRequestHandlerImpl(const QObject *context, QtPrivate::QSlotObjectBase *handler);

    template<typename ViewHandler, typename ViewTraits>
    auto createRouteHandler(const typename QtPrivate::ContextTypeForFunctor<ViewHandler>::ContextType *context,
                            ViewHandler &&viewHandler)
    {
        return [this, context, viewHandler](
                       const QRegularExpressionMatch &match,
                       const QHttpServerRequest &request,
                       QHttpServerResponder &responder) mutable {
                auto boundViewHandler = QHttpServerRouterRule::bindCaptured<ViewHandler, ViewTraits>(context,
                                                               std::forward<ViewHandler>(viewHandler), match);
                responseImpl<ViewTraits>(boundViewHandler, request, std::move(responder));
        };
    }

    template<typename Rule, typename ViewHandler, typename ViewTraits>
    Rule *routeImpl(const QString &pathPattern,
                    const typename QtPrivate::ContextTypeForFunctor<ViewHandler>::ContextType *context,
                    ViewHandler &&viewHandler)
    {
        auto routerHandler = createRouteHandler<ViewHandler, ViewTraits>(context,
                                                                         std::forward<ViewHandler>(viewHandler));
        auto rule = std::make_unique<Rule>(pathPattern, context, std::move(routerHandler));
        return reinterpret_cast<Rule*>(router()->addRule<ViewHandler, ViewTraits>(std::move(rule)));
    }

    template<typename Rule, typename ViewHandler, typename ViewTraits>
    Rule *routeImpl(const QString &pathPattern, QHttpServerRequest::Methods method,
                    const typename QtPrivate::ContextTypeForFunctor<ViewHandler>::ContextType *context,
                    ViewHandler &&viewHandler)
    {
        auto routerHandler = createRouteHandler<ViewHandler, ViewTraits>(context,
                                                                         std::forward<ViewHandler>(viewHandler));
        auto rule = std::make_unique<Rule>(pathPattern, method, context, std::move(routerHandler));
        return reinterpret_cast<Rule*>(router()->addRule<ViewHandler, ViewTraits>(std::move(rule)));
    }

    template<typename ViewTraits, typename T>
    void responseImpl(T &boundViewHandler, const QHttpServerRequest &request,
                      QHttpServerResponder &&responder)
    {
        if constexpr (ViewTraits::Arguments::SpecialsCount == 0) {
            ResponseType<typename ViewTraits::ReturnType> response(boundViewHandler());
            sendResponse(std::move(response), request, std::move(responder));
        } else if constexpr (ViewTraits::Arguments::SpecialsCount == 1) {
            if constexpr (ViewTraits::Arguments::Last::IsRequest::Value) {
                ResponseType<typename ViewTraits::ReturnType> response(boundViewHandler(request));
                sendResponse(std::move(response), request, std::move(responder));
            } else {
                static_assert(std::is_same_v<typename ViewTraits::ReturnType, void>,
                    "Handlers with responder argument must have void return type.");
                boundViewHandler(responder);
            }
        } else if constexpr (ViewTraits::Arguments::SpecialsCount == 2) {
            static_assert(std::is_same_v<typename ViewTraits::ReturnType, void>,
                "Handlers with responder argument must have void return type.");
            if constexpr (ViewTraits::Arguments::Last::IsRequest::Value) {
                boundViewHandler(responder, request);
            } else {
                boundViewHandler(request, responder);
            }
        } else {
            static_assert(dependent_false_v<ViewTraits>);
        }
    }

    bool handleRequest(const QHttpServerRequest &request,
                       QHttpServerResponder &responder) override;
    void missingHandler(const QHttpServerRequest &request,
                        QHttpServerResponder &responder) override;

    void sendResponse(QHttpServerResponse &&response, const QHttpServerRequest &request,
                      QHttpServerResponder &&responder);

#if QT_CONFIG(future)
    void sendResponse(QFuture<QHttpServerResponse> &&response, const QHttpServerRequest &request,
                      QHttpServerResponder &&responder);
#endif
};

QT_END_NAMESPACE

#endif  // QHTTPSERVER_H
