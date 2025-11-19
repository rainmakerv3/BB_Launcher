// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QLANGUAGESERVERBASE_P_H
#define QLANGUAGESERVERBASE_P_H

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

#include <QtLanguageServer/qtlanguageserverglobal.h>
#include <QtLanguageServer/private/qlanguageserverspec_p.h>
#include <QtJsonRpc/private/qjsonrpcprotocol_p.h>
#include <QtJsonRpc/private/qjsonrpctransport_p.h>

#include <QtCore/QByteArray>

#include <functional>
#include <memory>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(lspLog);

namespace QLspSpecification {

class ProtocolBasePrivate;
class Q_LANGUAGESERVER_EXPORT ProtocolBase
{
    Q_DISABLE_COPY_MOVE(ProtocolBase)
public:
    ~ProtocolBase();
    using GenericRequestHandler = std::function<void(const QJsonRpc::IdType &, const QByteArray &,
                                                     const QLspSpecification::RequestParams &,
                                                     QJsonRpc::TypedResponse &&)>;
    using GenericNotificationHandler =
            std::function<void(const QByteArray &, const QLspSpecification::NotificationParams &)>;
    using ResponseErrorHandler = std::function<void(const QLspSpecification::ResponseError &)>;

    // generated, defined in qlanguageservergen.cpp
    static QByteArray requestMethodToBaseCppName(const QByteArray &);

    // generated, defined in qlanguageservergen.cpp
    static QByteArray notificationMethodToBaseCppName(const QByteArray &);

    static void defaultUndispatchedRequestHandler(
            const QJsonRpc::IdType &id, const QByteArray &method,
            const QLspSpecification::RequestParams &params, QJsonRpc::TypedResponse &&response);
    static void defaultUndispatchedNotificationHandler(
            const QByteArray &method, const QLspSpecification::NotificationParams &params);
    static void defaultResponseErrorHandler(const QLspSpecification::ResponseError &err);

    void registerResponseErrorHandler(const ResponseErrorHandler &h);
    void registerUndispatchedRequestHandler(const GenericRequestHandler &handler);
    void registerUndispatchedNotificationHandler(const GenericNotificationHandler &handler);

    void handleResponseError(const ResponseError &err);
    void handleUndispatchedRequest(const QJsonRpc::IdType &id, const QByteArray &method,
                                   const QLspSpecification::RequestParams &params,
                                   QJsonRpc::TypedResponse &&response);
    void handleUndispatchedNotification(const QByteArray &method,
                                        const QLspSpecification::NotificationParams &params);
    QJsonRpc::TypedRpc *typedRpc();

protected:
    std::unique_ptr<ProtocolBasePrivate> d_ptr;

    ProtocolBase(std::unique_ptr<ProtocolBasePrivate> &&priv);
    QJsonRpcTransport *transport();

private:
    void registerMethods(QJsonRpc::TypedRpc *);
    Q_DECLARE_PRIVATE(ProtocolBase)
};

template<typename T, typename F>
void decodeAndCall(QJsonValue value, F funct,
                   ProtocolBase::ResponseErrorHandler errorHandler =
                           &ProtocolBase::defaultResponseErrorHandler)
{
    using namespace Qt::StringLiterals;

    T result;
    QTypedJson::Reader r(value);
    doWalk(r, result);
    if (!r.errorMessages().isEmpty()) {
        errorHandler(QLspSpecification::ResponseError {
                int(QLspSpecification::ErrorCodes::ParseError),
                u"Errors decoding data:\n    %1"_s.arg(r.errorMessages().join(u"\n    ")).toUtf8(),
                value });
        r.clearErrorMessages();
    } else {
        if constexpr (std::is_same_v<T, std::nullptr_t> && std::is_invocable_v<F>) {
            funct();
        } else {
            funct(result);
        }
    }
}

} // namespace QLspSpecification
QT_END_NAMESPACE
#endif // QLANGUAGESERVERBASE_P_H
