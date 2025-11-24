// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QOAUTH2AUTHORIZATIONCODEFLOW_H
#define QOAUTH2AUTHORIZATIONCODEFLOW_H

#include <QtNetworkAuth/qoauthglobal.h>

#ifndef QT_NO_HTTP

#include <QtNetworkAuth/qabstractoauth2.h>

QT_BEGIN_NAMESPACE

class QUrl;
class QString;
class QNetworkAccessManager;

class QOAuth2AuthorizationCodeFlowPrivate;
class Q_OAUTH_EXPORT QOAuth2AuthorizationCodeFlow : public QAbstractOAuth2
{
    Q_OBJECT
#if QT_DEPRECATED_SINCE(6, 13)
    Q_PROPERTY(QUrl accessTokenUrl
               READ accessTokenUrl
               WRITE setAccessTokenUrl
               NOTIFY accessTokenUrlChanged)
#endif
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

public:
    explicit QOAuth2AuthorizationCodeFlow(QObject *parent = nullptr);
    explicit QOAuth2AuthorizationCodeFlow(QNetworkAccessManager *manager,
                                          QObject *parent = nullptr);

    QOAuth2AuthorizationCodeFlow(const QString &clientIdentifier,
                                 QNetworkAccessManager *manager,
                                 QObject *parent = nullptr);

    QOAuth2AuthorizationCodeFlow(const QUrl &authorizationUrl,
                                 const QUrl &accessTokenUrl,
                                 QNetworkAccessManager *manager,
                                 QObject *parent = nullptr);

    QOAuth2AuthorizationCodeFlow(const QString &clientIdentifier,
                                 const QUrl &authorizationUrl,
                                 const QUrl &accessTokenUrl,
                                 QNetworkAccessManager *manager,
                                 QObject *parent = nullptr);

    ~QOAuth2AuthorizationCodeFlow();

#if QT_DEPRECATED_SINCE(6, 13)
    QT_DEPRECATED_VERSION_X_6_13("Use QAbstractOAuth2::tokenUrl() instead.")
    QUrl accessTokenUrl() const;
    QT_DEPRECATED_VERSION_X_6_13("Use QAbstractOAuth2::setTokenUrl() instead.")
    void setAccessTokenUrl(const QUrl &accessTokenUrl);
#endif

    enum class PkceMethod : quint8 {
        S256,
        Plain,
        None = 255,
    };
    Q_ENUM(PkceMethod)

    void setPkceMethod(PkceMethod method, qsizetype length = 43) ;
    PkceMethod pkceMethod() const noexcept;

public Q_SLOTS:
    void grant() override;
#if QT_DEPRECATED_SINCE(6, 13)
    QT_DEPRECATED_VERSION_X_6_13("Use QAbstractOAuth2::refreshTokens() instead")
    void refreshAccessToken();
#endif

protected Q_SLOTS:
    void refreshTokensImplementation() QT7_ONLY(override);

protected:
    QUrl buildAuthenticateUrl(const QMultiMap<QString, QVariant> &parameters = {});
    void requestAccessToken(const QString &code);
    void resourceOwnerAuthorization(const QUrl &url,
                                    const QMultiMap<QString, QVariant> &parameters = {}) override;

Q_SIGNALS:
#if QT_DEPRECATED_SINCE(6, 13)
    QT_MOC_COMPAT
    QT_DEPRECATED_VERSION_X_6_13("Use QAbstractOAuth2::tokenUrlChanged() instead.")
    void accessTokenUrlChanged(const QUrl &accessTokenUrl);
#endif

private:
    Q_DISABLE_COPY(QOAuth2AuthorizationCodeFlow)
    Q_DECLARE_PRIVATE(QOAuth2AuthorizationCodeFlow)
};

QT_END_NAMESPACE

#endif // QT_NO_HTTP

#endif // QOAUTH2AUTHORIZATIONCODEFLOW_H
