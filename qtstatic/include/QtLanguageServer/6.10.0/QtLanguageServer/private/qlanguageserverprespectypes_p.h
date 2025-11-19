// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QLANGUAGESERVERPRESPECTYPES_P_H
#define QLANGUAGESERVERPRESPECTYPES_P_H

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

#include <QtJsonRpc/private/qtypedjson_p.h>
#include <QtJsonRpc/private/qjsontypedrpc_p.h>
#include <QtLanguageServer/qtlanguageserverglobal.h>
#include <QtCore/QByteArray>
#include <QtCore/QMetaEnum>
#include <QtCore/QString>

#include <variant>
#include <type_traits>

QT_BEGIN_NAMESPACE
namespace QLspSpecification {

using ProgressToken = std::variant<int, QByteArray>;

class Q_LANGUAGESERVER_EXPORT StringAndLanguage
{
public:
    QString language;
    QString value;

    template<typename W>
    void walk(W &w)
    {
        field(w, "language", language);
        field(w, "value", value);
    }
};

using MarkedString = std::variant<QByteArray, StringAndLanguage>;

template<typename RType>
class LSPResponse : public QJsonRpc::TypedResponse
{
    Q_DISABLE_COPY(LSPResponse)
public:
    LSPResponse() = default;
    LSPResponse(LSPResponse &&o) noexcept = default;
    LSPResponse &operator=(LSPResponse &&o) noexcept = default;
    using ResponseType = RType;

    LSPResponse(QJsonRpc::TypedResponse &&r) : QJsonRpc::TypedResponse(std::move(r)) { }

    void sendResponse(const RType &r) { sendSuccessfullResponse(r); }
    auto sendResponse()
    {
        if constexpr (std::is_same_v<std::decay_t<RType>, std::nullptr_t>)
            sendSuccessfullResponse(nullptr);
        else
            Q_ASSERT(false);
    }
};

template<typename RType, typename PRType>
class LSPPartialResponse : public LSPResponse<RType>
{
    Q_DISABLE_COPY(LSPPartialResponse)
public:
    using PartialResponseType = PRType;

    LSPPartialResponse() = default;
    LSPPartialResponse(LSPPartialResponse &&o) noexcept = default;
    LSPPartialResponse &operator=(LSPPartialResponse &&o) noexcept = default;

    LSPPartialResponse(QJsonRpc::TypedResponse &&r) : LSPResponse<RType>(std::move(r)) { }

    void sendPartialResponse(const PRType &r)
    {
        // using Notifications::Progress here would require to split out the *RequestType aliases
        sendNotification("$/progress", r);
    }
};

} // namespace QLspSpecification
QT_END_NAMESPACE
#endif // QLANGUAGESERVERPRESPECTYPES_P_H
