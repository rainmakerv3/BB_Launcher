// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBSOCKETHANDSHAKEREQUEST_P_H
#define QWEBSOCKETHANDSHAKEREQUEST_P_H
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

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtNetwork/private/qhttpheaderparser_p.h>
#include <QtNetwork/qhttpheaders.h>

#include "qwebsocketprotocol.h"

QT_BEGIN_NAMESPACE

class QTextStream;

class Q_AUTOTEST_EXPORT QWebSocketHandshakeRequest
{
    Q_DISABLE_COPY(QWebSocketHandshakeRequest)

public:
    QWebSocketHandshakeRequest(int port, bool isSecure);
    virtual ~QWebSocketHandshakeRequest();

    void clear();

    int port() const;
    bool isSecure() const;
    bool isValid() const;
    QHttpHeaders headers() const;
    bool hasHeader(const QByteArray &name) const;
    QList<QWebSocketProtocol::Version> versions() const;
    QString key() const;
    QString origin() const;
    QList<QString> protocols() const;
    QList<QString> extensions() const;
    QUrl requestUrl() const;
    QString resourceName() const;
    QString host() const;

    void readHandshake(QByteArrayView header, int maxHeaderLineLength);

private:

    int m_port;
    bool m_isSecure;
    bool m_isValid;
    QHttpHeaderParser m_parser;
    QList<QWebSocketProtocol::Version> m_versions;
    QString m_key;
    QString m_origin;
    QList<QString> m_protocols;
    QList<QString> m_extensions;
    QUrl m_requestUrl;
};

QT_END_NAMESPACE

#endif // QWEBSOCKETHANDSHAKEREQUEST_P_H
