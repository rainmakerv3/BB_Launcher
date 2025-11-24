// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QOAUTHHTTPSERVERREPLYHANDLER_H
#define QOAUTHHTTPSERVERREPLYHANDLER_H

#include <QtNetworkAuth/qoauthglobal.h>

#ifndef QT_NO_HTTP

#include <QtNetworkAuth/qoauthoobreplyhandler.h>

#include <QtNetwork/qhostaddress.h>

QT_BEGIN_NAMESPACE

class QUrlQuery;
#ifndef QT_NO_SSL
class QSslConfiguration;
#endif

class QOAuthHttpServerReplyHandlerPrivate;
class Q_OAUTH_EXPORT QOAuthHttpServerReplyHandler : public QOAuthOobReplyHandler
{
    Q_OBJECT

public:
    explicit QOAuthHttpServerReplyHandler(QObject *parent = nullptr);
    explicit QOAuthHttpServerReplyHandler(quint16 port, QObject *parent = nullptr);
    explicit QOAuthHttpServerReplyHandler(const QHostAddress &address, quint16 port,
                                          QObject *parent = nullptr);
    ~QOAuthHttpServerReplyHandler();

    QString callback() const override;

    QString callbackPath() const;
    void setCallbackPath(const QString &path);

    QString callbackHost() const;
    void setCallbackHost(const QString &path);

    QString callbackText() const;
    void setCallbackText(const QString &text);

    quint16 port() const;

    bool listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 0);
#ifndef QT_NO_SSL
    bool listen(const QSslConfiguration &configuration,
                const QHostAddress &address = QHostAddress::Any, quint16 port = 0);
#endif
    void close();
    bool isListening() const;

private:
    Q_DECLARE_PRIVATE(QOAuthHttpServerReplyHandler)
    QScopedPointer<QOAuthHttpServerReplyHandlerPrivate> d_ptr;
};

QT_END_NAMESPACE

#endif // QT_NO_HTTP

#endif // QOAUTHHTTPSERVERREPLYHANDLER_H
