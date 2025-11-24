// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QOAUTH1_H
#define QOAUTH1_H

#include <QtNetworkAuth/qoauthglobal.h>

#ifndef QT_NO_HTTP

#include <QtNetworkAuth/qabstractoauth.h>

#include <QtNetwork/qnetworkaccessmanager.h>

QT_BEGIN_NAMESPACE

// ### Qt 7 consider removing the support for OAuth1 (QTBUG-124329)
class QOAuth1Private;
class Q_OAUTH_EXPORT QOAuth1: public QAbstractOAuth
{
    Q_OBJECT

public:
    enum class SignatureMethod {
        Hmac_Sha1,
        Rsa_Sha1,
        PlainText
    };

    Q_ENUM(SignatureMethod)

    explicit QOAuth1(QObject *parent = nullptr);
    explicit QOAuth1(QNetworkAccessManager *manager,
                     QObject *parent = nullptr);

    QOAuth1(const QString &clientIdentifier,
            const QString &clientSharedSecret,
            QNetworkAccessManager *manager,
            QObject *parent = nullptr);

    QString clientSharedSecret() const;
    void setClientSharedSecret(const QString &clientSharedSecret);
    std::pair<QString, QString> clientCredentials() const;
    void setClientCredentials(const std::pair<QString, QString> &clientCredentials);
    void setClientCredentials(const QString &clientIdentifier, const QString &clientSharedSecret);

    // Token credentials: https://tools.ietf.org/html/rfc5849#section-2.3
    QString tokenSecret() const;
    void setTokenSecret(const QString &tokenSecret);
    std::pair<QString, QString> tokenCredentials() const;
    void setTokenCredentials(const std::pair<QString, QString> &tokenCredentials);
    void setTokenCredentials(const QString &token, const QString &tokenSecret);

    // Temporary Credentials: https://tools.ietf.org/html/rfc5849#section-2.1
    QUrl temporaryCredentialsUrl() const;
    void setTemporaryCredentialsUrl(const QUrl &url);

    // Token Credentials: https://tools.ietf.org/html/rfc5849#section-2.3
    QUrl tokenCredentialsUrl() const;
    void setTokenCredentialsUrl(const QUrl &url);

    // Signature method: https://tools.ietf.org/html/rfc5849#section-3.4
    SignatureMethod signatureMethod() const;
    void setSignatureMethod(SignatureMethod value);

    void prepareRequest(QNetworkRequest *request, const QByteArray &verb,
                                const QByteArray &body = QByteArray()) override;

    QNetworkReply *head(const QUrl &url, const QVariantMap &parameters = QVariantMap()) override;
    QNetworkReply *get(const QUrl &url, const QVariantMap &parameters = QVariantMap()) override;

    QNetworkReply *post(const QUrl &url, const QVariantMap &parameters = QVariantMap()) override;
    QNetworkReply *put(const QUrl &url, const QVariantMap &parameters = QVariantMap()) override;
    QNetworkReply *deleteResource(const QUrl &url,
                                  const QVariantMap &parameters = QVariantMap()) override;

public Q_SLOTS:
    void grant() override;
    void continueGrantWithVerifier(const QString &verifier);

Q_SIGNALS:
    void signatureMethodChanged(QOAuth1::SignatureMethod method);
    void clientSharedSecretChanged(const QString &credential);
    void tokenSecretChanged(const QString &token);
    void temporaryCredentialsUrlChanged(const QUrl &url);
    void tokenCredentialsUrlChanged(const QUrl &url);

protected:
    QNetworkReply *requestTemporaryCredentials(QNetworkAccessManager::Operation operation,
                                               const QUrl &url,
                                               const QVariantMap &parameters = QVariantMap());

    QNetworkReply *requestTokenCredentials(QNetworkAccessManager::Operation operation,
                                           const QUrl &url,
                                           const std::pair<QString, QString> &temporaryToken,
                                           const QVariantMap &parameters = QVariantMap());

    void setup(QNetworkRequest *request,
               const QVariantMap &signingParameters,
               QNetworkAccessManager::Operation operation);
    void setup(QNetworkRequest *request,
               const QVariantMap &signingParameters,
               const QByteArray &operationVerb);

    static QByteArray nonce();
    static QByteArray generateAuthorizationHeader(const QVariantMap &oauthParams);

private:
    Q_DISABLE_COPY(QOAuth1)
    Q_DECLARE_PRIVATE(QOAuth1)
};

QT_END_NAMESPACE

#endif // QT_NO_HTTP

#endif // QOAUTH1_H
