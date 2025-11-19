// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QLANGUAGESERVERPROTOCOL_P_H
#define QLANGUAGESERVERPROTOCOL_P_H

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
#include <QtLanguageServer/private/qlanguageservergen_p.h>

QT_BEGIN_NAMESPACE

class Q_LANGUAGESERVER_EXPORT QLanguageServerProtocol : public QLspSpecification::ProtocolGen
{
public:
    QLanguageServerProtocol(const QJsonRpcTransport::DataHandler &sender);
    void receiveData(const QByteArray &data);
};

QT_END_NAMESPACE

#endif // QLANGUAGESERVER_P_H
