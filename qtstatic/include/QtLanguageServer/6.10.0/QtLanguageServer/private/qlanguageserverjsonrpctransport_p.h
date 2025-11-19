// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QLANGUAGESERVERJSONRPCTRANSPORT_P_H
#define QLANGUAGESERVERJSONRPCTRANSPORT_P_H

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
#include <QtJsonRpc/private/qhttpmessagestreamparser_p.h>
#include <QtJsonRpc/private/qjsonrpctransport_p.h>

QT_BEGIN_NAMESPACE

class Q_LANGUAGESERVER_EXPORT QLanguageServerJsonRpcTransport : public QJsonRpcTransport
{
public:
    QLanguageServerJsonRpcTransport() noexcept;
    void sendMessage(const QJsonDocument &packet) override;
    void receiveData(const QByteArray &data) override;

private:
    void hasHeader(const QByteArray &field, const QByteArray &value);
    void hasBody(const QByteArray &body);

    QHttpMessageStreamParser m_messageStreamParser;
};

QT_END_NAMESPACE

#endif // QLANGUAGESERVERJSONRPCTRANSPORT_P_H
