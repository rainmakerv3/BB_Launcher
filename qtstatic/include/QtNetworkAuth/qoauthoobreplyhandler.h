// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QOAUTHOOBREPLYHANDLER_H
#define QOAUTHOOBREPLYHANDLER_H

#include <QtNetworkAuth/qoauthglobal.h>

#ifndef QT_NO_HTTP

#include <QtNetworkAuth/qabstractoauthreplyhandler.h>

QT_BEGIN_NAMESPACE

// ### Qt 7 remove this undocumented class and arrange it's functionality otherwise (QTBUG-124329)
class QOAuthOobReplyHandlerPrivate;
class Q_OAUTH_EXPORT QOAuthOobReplyHandler : public QAbstractOAuthReplyHandler
{
    Q_OBJECT

public:
    explicit QOAuthOobReplyHandler(QObject *parent = nullptr);
    ~QOAuthOobReplyHandler() override;

    QString callback() const override;

protected:
    void networkReplyFinished(QNetworkReply *reply) override;
    explicit QOAuthOobReplyHandler(QOAuthOobReplyHandlerPrivate &, QObject *parent = nullptr);

private:
    QVariantMap parseResponse(const QByteArray &response);
};

QT_END_NAMESPACE

#endif // QT_NO_HTTP

#endif // QOAUTHOOBREPLYHANDLER_H
