// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default
#ifndef QWEBSOCKETCORSAUTHENTICATOR_P_H
#define QWEBSOCKETCORSAUTHENTICATOR_P_H

#include <QtCore/private/qglobal_p.h>
#include <QtCore/QString>

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
QT_BEGIN_NAMESPACE

class QWebSocketCorsAuthenticatorPrivate
{
public:
    QWebSocketCorsAuthenticatorPrivate(const QString &origin, bool allowed);
    ~QWebSocketCorsAuthenticatorPrivate();

    QString m_origin;
    bool m_isAllowed;
};

QT_END_NAMESPACE

#endif // QWEBSOCKETCORSAUTHENTICATOR_P_H
