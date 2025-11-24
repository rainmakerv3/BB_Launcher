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

#ifndef QOAUTH1_P_H
#define QOAUTH1_P_H

#include <private/qabstractoauth_p.h>

#include <QtNetworkAuth/qoauth1.h>
#include <QtNetworkAuth/qoauthglobal.h>

#include <QtCore/qurl.h>
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkaccessmanager.h>

#include <utility>

QT_BEGIN_NAMESPACE

class QOAuth1Signature;

class QOAuth1Private : public QAbstractOAuthPrivate
{
public:
    Q_DECLARE_PUBLIC(QOAuth1)

    QOAuth1Private(const std::pair<QString, QString> &clientCredentials,
                   QNetworkAccessManager *networkAccessManager = nullptr);

    void appendCommonHeaders(QVariantMap *headers);
    void appendSignature(QAbstractOAuth::Stage stage,
                         QVariantMap *headers,
                         const QUrl &url,
                         QNetworkAccessManager::Operation operation,
                         const QMultiMap<QString, QVariant> parameters);

    QNetworkReply *requestToken(QNetworkAccessManager::Operation operation,
                                const QUrl &url,
                                const std::pair<QString, QString> &token,
                                const QVariantMap &additionalParameters);

    QString signatureMethodString() const;
    QByteArray generateSignature(const QMultiMap<QString, QVariant> &parameters,
                                 const QUrl &url,
                                 QNetworkAccessManager::Operation operation) const;
    QByteArray generateSignature(const QMultiMap<QString, QVariant> &parameters,
                                 const QUrl &url,
                                 const QByteArray &verb) const;
    QByteArray formatSignature(const QOAuth1Signature &signature) const;

    QVariantMap createOAuthBaseParams() const;

    void _q_onTokenRequestError(QNetworkReply::NetworkError error);
    void _q_tokensReceived(const QVariantMap &tokens);

    QString clientIdentifierSharedKey;
    QString tokenSecret;
    QString verifier;
    QUrl temporaryCredentialsUrl;
    QUrl tokenCredentialsUrl;
    QOAuth1::SignatureMethod signatureMethod = QOAuth1::SignatureMethod::Hmac_Sha1;
    const QString oauthVersion = QStringLiteral("1.0");
    bool tokenRequested = false;

    struct OAuth1KeyString
    {
        static const QString oauthCallback;
        static const QString oauthCallbackConfirmed;
        static const QString oauthConsumerKey;
        static const QString oauthNonce;
        static const QString oauthSignature;
        static const QString oauthSignatureMethod;
        static const QString oauthTimestamp;
        static const QString oauthToken;
        static const QString oauthTokenSecret;
        static const QString oauthVerifier;
        static const QString oauthVersion;
    };
};

QT_END_NAMESPACE

#endif // QOAUTH1_P_H
