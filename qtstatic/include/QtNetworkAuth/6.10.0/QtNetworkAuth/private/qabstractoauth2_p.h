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

#ifndef QABSTRACTOAUTH2_P_H
#define QABSTRACTOAUTH2_P_H

#include <chrono>
#include <optional>

#include <private/qabstractoauth_p.h>

#include <QtNetworkAuth/qoauthglobal.h>
#include <QtNetworkAuth/qabstractoauth2.h>

#include <QtCore/qchronotimer.h>
#include <QtCore/qurl.h>
#include <QtCore/qstring.h>
#include <QtCore/qpointer.h>

#include <QtNetwork/qnetworkreply.h>

#include <utility>

QT_BEGIN_NAMESPACE

class QNetworkAccessManager;

class QAbstractOAuth2Private : public QAbstractOAuthPrivate
{
    Q_DECLARE_PUBLIC(QAbstractOAuth2)

public:
    QAbstractOAuth2Private(const std::pair<QString, QString> &clientCredentials,
                           const QUrl &authorizationUrl, QNetworkAccessManager *manager = nullptr);
    ~QAbstractOAuth2Private();

    void setExpiresAt(const QDateTime &expiration);
    void setGrantedScopeTokens(const QSet<QByteArray> &tokens);
    static QString joinedScope(const QSet<QByteArray> &scopeTokens);
    static QSet<QByteArray> splitScope(QStringView scope);
    static bool checkRequestedScopeTokensValid(const QSet<QByteArray> &tokens);
    static bool checkRequestedScopeTokenValid(QByteArrayView token);
    static void warnOnInvalidScopeToken(QStringView token);
    static QString generateRandomState();
    static QString generateNonce();
    QNetworkRequest createRequest(QUrl url, const QVariantMap *parameters = nullptr);
    bool authorizationShouldIncludeNonce() const;
    void setIdToken(const QString &token);
    void _q_tokenRequestFailed(QAbstractOAuth::Error error, const QString& errorString);
    void _q_tokenRequestFinished(const QVariantMap &values);
    bool handleRfcErrorResponseIfPresent(const QVariantMap &data);
    struct RequestAndBody
    {
        QNetworkRequest request;
        QByteArray body;
    };
    [[nodiscard]] RequestAndBody createRefreshRequestAndBody(const QUrl &url);

    Q_DECL_COLD_FUNCTION
    void logAuthorizationStageWarning(QLatin1StringView message);
    Q_DECL_COLD_FUNCTION
    void logAuthorizationStageWarning(QLatin1StringView message, int detail);
    Q_DECL_COLD_FUNCTION
    void logTokenStageWarning(QLatin1StringView message);

    struct CallerInfo {
        QPointer<const QObject> contextObject = nullptr;
        QtPrivate::SlotObjUniquePtr slot;
    };
    CallerInfo networkRequestModifier;
    void callNetworkRequestModifier(QNetworkRequest &request, QAbstractOAuth::Stage stage);
    bool verifyThreadAffinity(const QObject *contextObject);

    void initializeRefreshHandling();
    void updateRefreshTimer(bool clientSideUpdate);

    QString clientIdentifierSharedKey;
    QSet<QByteArray> requestedScopeTokens;
    QSet<QByteArray> grantedScopeTokens;
    QString state = generateRandomState();
    QString userAgent = QStringLiteral("QtOAuth/1.0 (+https://www.qt.io)");
    QString responseType;
    const QString bearerFormat = QStringLiteral("Bearer %1"); // Case sensitive
    QDateTime expiresAtUtc;
    QString refreshToken;
    std::chrono::seconds refreshLeadTime = std::chrono::seconds::zero();
    QChronoTimer refreshTimer;
#ifndef QOAUTH2_NO_LEGACY_SCOPE
    QString legacyScope;
    bool legacyScopeWasSetByUser = false;
#endif
    bool autoRefresh = false;
    QAbstractOAuth2::NonceMode nonceMode = QAbstractOAuth2::NonceMode::Automatic;
    QString nonce;
    QString idToken;
    QString tokenType;
    QUrl tokenUrl;
    // RFC (6749) doesn't state any maximum value for the lifetime, use long just in case
    qlonglong tokenLifetime = 0;
#ifndef QT_NO_SSL
    std::optional<QSslConfiguration> sslConfiguration;
#endif
};

namespace QtOAuth2RfcKeywords
{
    inline constexpr auto accessToken = QLatin1StringView("access_token");
    inline constexpr auto apiKey = QLatin1StringView("api_key");
    inline constexpr auto clientIdentifier = QLatin1StringView("client_id");
    inline constexpr auto clientSharedSecret = QLatin1StringView("client_secret");
    inline constexpr auto code = QLatin1StringView("code");
    inline constexpr auto error = QLatin1StringView("error");
    inline constexpr auto errorDescription = QLatin1StringView("error_description");
    inline constexpr auto errorUri = QLatin1StringView("error_uri");
    inline constexpr auto expiresIn = QLatin1StringView("expires_in");
    inline constexpr auto grantType = QLatin1StringView("grant_type");
    inline constexpr auto redirectUri = QLatin1StringView("redirect_uri");
    inline constexpr auto refreshToken = QLatin1StringView("refresh_token");
    inline constexpr auto responseType = QLatin1StringView("response_type");
    inline constexpr auto scope = QLatin1StringView("scope");
    inline constexpr auto state = QLatin1StringView("state");
    inline constexpr auto tokenType = QLatin1StringView("token_type");
    inline constexpr auto codeVerifier = QLatin1StringView("code_verifier");
    inline constexpr auto codeChallenge = QLatin1StringView("code_challenge");
    inline constexpr auto codeChallengeMethod = QLatin1StringView("code_challenge_method");
    inline constexpr auto nonce = QLatin1StringView("nonce");
    inline constexpr auto idToken = QLatin1StringView("id_token");
    inline constexpr auto deviceCode = QLatin1StringView("device_code");
    inline constexpr auto userCode = QLatin1StringView("user_code");
    // RFC keyword is verification_uri[_complete], but some servers use 'url' (note L)
    // https://datatracker.ietf.org/doc/html/rfc8628#section-3.2
    inline constexpr auto verificationUri = QLatin1StringView("verification_uri");
    inline constexpr auto verificationUrl = QLatin1StringView("verification_url");
    inline constexpr auto completeVerificationUri = QLatin1StringView("verification_uri_complete");
    inline constexpr auto completeVerificationUrl = QLatin1StringView("verification_url_complete");
    inline constexpr auto interval = QLatin1StringView("interval");
}

QT_END_NAMESPACE

#endif // QABSTRACTOAUTH2_P_H
