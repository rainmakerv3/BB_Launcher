// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QABSTRACTOAUTH_H
#define QABSTRACTOAUTH_H

#include <QtNetworkAuth/qoauthglobal.h>

#ifndef QT_NO_HTTP

#include <QtCore/qurl.h>
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>
#include <QtCore/qmap.h>

#include <functional>

QT_BEGIN_NAMESPACE

class QString;
class QByteArray;
class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;
class QAbstractOAuthReplyHandler;

class QAbstractOAuthPrivate;
class Q_OAUTH_EXPORT  QAbstractOAuth : public QObject
{
    Q_OBJECT

    Q_ENUMS(Status)
    Q_ENUMS(Stage)
    Q_ENUMS(Error)
    Q_PROPERTY(QString clientIdentifier
               READ clientIdentifier
               WRITE setClientIdentifier
               NOTIFY clientIdentifierChanged)
    Q_PROPERTY(QString token READ token WRITE setToken NOTIFY tokenChanged)
    Q_PROPERTY(Status status  READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariantMap extraTokens READ extraTokens NOTIFY extraTokensChanged)
    Q_PROPERTY(QUrl authorizationUrl
               READ authorizationUrl
               WRITE setAuthorizationUrl
               NOTIFY authorizationUrlChanged)
    Q_PROPERTY(QAbstractOAuth::ContentType contentType
               READ contentType
               WRITE setContentType
               NOTIFY contentTypeChanged)

public:
    enum class Status {
        NotAuthenticated,
        TemporaryCredentialsReceived,
        Granted,
        RefreshingToken
    };

    enum class Stage {
        RequestingTemporaryCredentials,
        RequestingAuthorization,
        RequestingAccessToken,
        RefreshingAccessToken
    };

    enum class Error {
        NoError,
        NetworkError,
        ServerError,

        OAuthTokenNotFoundError,
        OAuthTokenSecretNotFoundError,
        OAuthCallbackNotVerified,

        ClientError,
        ExpiredError,
    };

    enum class ContentType {
        WwwFormUrlEncoded,
        Json
    };

    typedef std::function<void(Stage, QMultiMap<QString, QVariant>*)> ModifyParametersFunction;

    virtual ~QAbstractOAuth();

    QString clientIdentifier() const;
    void setClientIdentifier(const QString &clientIdentifier);

    QString token() const;
    void setToken(const QString &token);

    QNetworkAccessManager *networkAccessManager() const;
    void setNetworkAccessManager(QNetworkAccessManager *networkAccessManager);

    Status status() const;

    QUrl authorizationUrl() const;
    void setAuthorizationUrl(const QUrl &url);

    QVariantMap extraTokens() const;

    QAbstractOAuthReplyHandler *replyHandler() const;
    void setReplyHandler(QAbstractOAuthReplyHandler *handler);

    Q_INVOKABLE virtual QNetworkReply *head(const QUrl &url,
                                            const QVariantMap &parameters = QVariantMap()) = 0;
    Q_INVOKABLE virtual QNetworkReply *get(const QUrl &url,
                                           const QVariantMap &parameters = QVariantMap()) = 0;
    Q_INVOKABLE virtual QNetworkReply *post(const QUrl &url,
                                            const QVariantMap &parameters = QVariantMap()) = 0;
    Q_INVOKABLE virtual QNetworkReply *put(const QUrl &url,
                                           const QVariantMap &parameters = QVariantMap()) = 0;
    Q_INVOKABLE virtual QNetworkReply *deleteResource(
            const QUrl &url, const QVariantMap &parameters = QVariantMap()) = 0;

    virtual void prepareRequest(QNetworkRequest *request, const QByteArray &verb,
                                const QByteArray &body = QByteArray()) = 0;

    ModifyParametersFunction modifyParametersFunction() const;
    void setModifyParametersFunction(const ModifyParametersFunction &modifyParametersFunction);

    ContentType contentType() const;
    void setContentType(ContentType contentType);

public Q_SLOTS:
    virtual void grant() = 0;

Q_SIGNALS:
    void clientIdentifierChanged(const QString &clientIdentifier);
    void tokenChanged(const QString &token);
    void statusChanged(Status status);
    void authorizationUrlChanged(const QUrl &url);
    void extraTokensChanged(const QVariantMap &tokens);
    void contentTypeChanged(ContentType contentType);

    void requestFailed(const Error error);
    void authorizeWithBrowser(const QUrl &url);
    void granted();
    void finished(QNetworkReply *reply);
    void replyDataReceived(const QByteArray &data);

protected:
    explicit QAbstractOAuth(QAbstractOAuthPrivate &, QObject *parent = nullptr);

    void setStatus(Status status);

    QString callback() const;

    virtual void resourceOwnerAuthorization(const QUrl &url, const QMultiMap<QString, QVariant> &parameters);
    static QByteArray generateRandomString(quint8 length);

private:
    Q_DISABLE_COPY(QAbstractOAuth)
    Q_DECLARE_PRIVATE(QAbstractOAuth)
};

QT_END_NAMESPACE

#endif // QT_NO_HTTP

#endif // QABSTRACTOAUTH_H
