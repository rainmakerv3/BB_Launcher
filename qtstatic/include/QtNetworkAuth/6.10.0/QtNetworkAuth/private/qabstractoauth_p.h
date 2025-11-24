// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the Network Access API.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QABSTRACTQOAUTH_P_H
#define QABSTRACTQOAUTH_P_H

#include <private/qobject_p.h>

#include <QtNetworkAuth/qoauthglobal.h>
#include <QtNetworkAuth/qabstractoauth.h>
#include <QtNetworkAuth/qoauthoobreplyhandler.h>

#include <QtCore/qurl.h>
#include <QtCore/qglobal.h>
#include <QtCore/qvariant.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qloggingcategory.h>

#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qnetworkaccessmanager.h>

#include <QtCore/qpointer.h>

QT_BEGIN_NAMESPACE

class QUrlQuery;

class Q_AUTOTEST_EXPORT QAbstractOAuthPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QAbstractOAuth)

public:
    QAbstractOAuthPrivate(const char *loggingCategory,
                          const QUrl &authorizationUrl,
                          const QString &clientIdentifier,
                          QNetworkAccessManager *manager);
    ~QAbstractOAuthPrivate();

    QNetworkAccessManager *networkAccessManager();
    void setStatus(QAbstractOAuth::Status status);
    static QByteArray generateRandomBase64String(quint8 length);
    void setExtraTokens(const QVariantMap &tokens);

    const QLoggingCategory loggingCategory;
    QString clientIdentifier;
    QString token;

    // Resource Owner Authorization: https://tools.ietf.org/html/rfc5849#section-2.2
    QUrl authorizationUrl;
    QVariantMap extraTokens;
    QAbstractOAuth::Status status = QAbstractOAuth::Status::NotAuthenticated;
    QPointer<QAbstractOAuthReplyHandler> replyHandler;
    QScopedPointer<QOAuthOobReplyHandler> defaultReplyHandler;
    QPointer<QNetworkAccessManager> networkAccessManagerPointer;
    QAbstractOAuth::ModifyParametersFunction modifyParametersFunction;
    QAbstractOAuth::ContentType contentType = QAbstractOAuth::ContentType::WwwFormUrlEncoded;

    QByteArray convertParameters(const QVariantMap &parameters);
    void addContentTypeHeaders(QNetworkRequest *request);

    static QUrlQuery createQuery(const QMultiMap<QString, QVariant> &parameters);
};

QT_END_NAMESPACE

#endif // QABSTRACTQOAUTH_H
