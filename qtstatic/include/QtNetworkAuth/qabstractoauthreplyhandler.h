// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QABSTRACTOAUTHREPLYHANDLER_H
#define QABSTRACTOAUTHREPLYHANDLER_H

#include <QtNetworkAuth/qoauthglobal.h>

#ifndef QT_NO_HTTP

#include <QtNetworkAuth/qabstractoauth.h>

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class Q_OAUTH_EXPORT QAbstractOAuthReplyHandler : public QObject
{
    Q_OBJECT

public:
    explicit QAbstractOAuthReplyHandler(QObject *parent = nullptr);
    virtual ~QAbstractOAuthReplyHandler();

    virtual QString callback() const = 0;

public Q_SLOTS:
    virtual void networkReplyFinished(QNetworkReply *reply) = 0;

Q_SIGNALS:
    void callbackReceived(const QVariantMap &values);
    void tokensReceived(const QVariantMap &tokens);
    void tokenRequestErrorOccurred(QAbstractOAuth::Error error, const QString& errorString);

    void replyDataReceived(const QByteArray &data);
    void callbackDataReceived(const QByteArray &data);

protected:
    QAbstractOAuthReplyHandler(QObjectPrivate &d, QObject *parent = nullptr);

private:
    Q_DISABLE_COPY(QAbstractOAuthReplyHandler)
};

QT_END_NAMESPACE

#endif // QT_NO_HTTP

#endif // QABSTRACTOAUTHREPLYHANDLER_H
