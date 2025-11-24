// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QABSTRACTOAUTH2_H
#define QABSTRACTOAUTH2_H

#include <QtNetworkAuth/qoauthglobal.h>

#ifndef QT_NO_HTTP

#include <QtCore/qcontainerfwd.h>
#include <QtCore/qdatetime.h>

#include <QtNetworkAuth/qabstractoauth.h>

QT_BEGIN_NAMESPACE

#if !QT_REMOVAL_QT7_DEPRECATED_SINCE(6, 13)
# define QOAUTH2_NO_LEGACY_SCOPE
#endif

class QSslConfiguration;
class QHttpMultiPart;
class QAbstractOAuth2Private;
class Q_OAUTH_EXPORT QAbstractOAuth2 : public QAbstractOAuth
{
    Q_OBJECT
#ifndef QOAUTH2_NO_LEGACY_SCOPE
    Q_PROPERTY(QString scope READ scope WRITE setScope NOTIFY scopeChanged)
#endif
    Q_PROPERTY(QSet<QByteArray> grantedScopeTokens
               READ grantedScopeTokens NOTIFY grantedScopeTokensChanged)
    Q_PROPERTY(QSet<QByteArray> requestedScopeTokens
               READ requestedScopeTokens
               WRITE setRequestedScopeTokens
               NOTIFY requestedScopeTokensChanged)
    Q_PROPERTY(QString userAgent READ userAgent WRITE setUserAgent NOTIFY userAgentChanged)
    Q_PROPERTY(QString clientIdentifierSharedKey
               READ clientIdentifierSharedKey
               WRITE setClientIdentifierSharedKey
               NOTIFY clientIdentifierSharedKeyChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QDateTime expiration READ expirationAt NOTIFY expirationAtChanged)
    Q_PROPERTY(QString refreshToken
               READ refreshToken
               WRITE setRefreshToken
               NOTIFY refreshTokenChanged)
    Q_PROPERTY(std::chrono::seconds refreshLeadTime
               READ refreshLeadTime
               WRITE setRefreshLeadTime
               NOTIFY refreshLeadTimeChanged)
    Q_PROPERTY(bool autoRefresh
               READ autoRefresh
               WRITE setAutoRefresh
               NOTIFY autoRefreshChanged)
    Q_PROPERTY(NonceMode nonceMode READ nonceMode WRITE setNonceMode NOTIFY nonceModeChanged)
    Q_PROPERTY(QString nonce READ nonce WRITE setNonce NOTIFY nonceChanged)
    Q_PROPERTY(QString idToken READ idToken NOTIFY idTokenChanged)
    Q_PROPERTY(QUrl tokenUrl READ tokenUrl WRITE setTokenUrl NOTIFY tokenUrlChanged)

    using NetworkRequestModifierPrototype = void(*)(QNetworkRequest&, QAbstractOAuth::Stage);
    template <typename Functor>
    using ContextTypeForFunctor = typename QtPrivate::ContextTypeForFunctor<Functor>::ContextType;
    template <typename Functor>
    using if_compatible_callback = std::enable_if_t<
        QtPrivate::AreFunctionsCompatible<NetworkRequestModifierPrototype, Functor>::value, bool>;

public:
    enum class NonceMode : quint8 {
        Automatic,
        Enabled,
        Disabled,
    };
    Q_ENUM(NonceMode)

    explicit QAbstractOAuth2(QObject *parent = nullptr);
    explicit QAbstractOAuth2(QNetworkAccessManager *manager, QObject *parent = nullptr);
    ~QAbstractOAuth2();

    Q_INVOKABLE virtual QUrl createAuthenticatedUrl(const QUrl &url,
                                                    const QVariantMap &parameters = QVariantMap());

    QT_DEPRECATED_VERSION_X_6_11("Use QtNetwork classes instead."
                                 "See https://doc.qt.io/qt-6/oauth-http-method-alternatives.html")
    Q_INVOKABLE QNetworkReply *head(const QUrl &url,
                                    const QVariantMap &parameters = QVariantMap()) override;

    QT_DEPRECATED_VERSION_X_6_11("Use QtNetwork classes instead."
                                 "See https://doc.qt.io/qt-6/oauth-http-method-alternatives.html")
    Q_INVOKABLE QNetworkReply *get(const QUrl &url,
                                   const QVariantMap &parameters = QVariantMap()) override;

    QT_DEPRECATED_VERSION_X_6_11("Use QtNetwork classes instead."
                                 "See https://doc.qt.io/qt-6/oauth-http-method-alternatives.html")
    Q_INVOKABLE QNetworkReply *post(const QUrl &url,
                                    const QVariantMap &parameters = QVariantMap()) override;

    QT_DEPRECATED_VERSION_X_6_11("Use QtNetwork classes instead."
                                 "See https://doc.qt.io/qt-6/oauth-http-method-alternatives.html")
    Q_INVOKABLE virtual QNetworkReply *post(const QUrl &url, const QByteArray &data);

    QT_DEPRECATED_VERSION_X_6_11("Use QtNetwork classes instead."
                                 "See https://doc.qt.io/qt-6/oauth-http-method-alternatives.html")
    Q_INVOKABLE virtual QNetworkReply *post(const QUrl &url, QHttpMultiPart *multiPart);

    QT_DEPRECATED_VERSION_X_6_11("Use QtNetwork classes instead."
                                 "See https://doc.qt.io/qt-6/oauth-http-method-alternatives.html")
    Q_INVOKABLE QNetworkReply *put(const QUrl &url,
                                   const QVariantMap &parameters = QVariantMap()) override;

    QT_DEPRECATED_VERSION_X_6_11("Use QtNetwork classes instead."
                                 "See https://doc.qt.io/qt-6/oauth-http-method-alternatives.html")
    Q_INVOKABLE virtual QNetworkReply *put(const QUrl &url, const QByteArray &data);

    QT_DEPRECATED_VERSION_X_6_11("Use QtNetwork classes instead."
                                 "See https://doc.qt.io/qt-6/oauth-http-method-alternatives.html")
    Q_INVOKABLE virtual QNetworkReply *put(const QUrl &url, QHttpMultiPart *multiPart);

    QT_DEPRECATED_VERSION_X_6_11("Use QtNetwork classes instead."
                                 "See https://doc.qt.io/qt-6/oauth-http-method-alternatives.html")
    Q_INVOKABLE QNetworkReply *deleteResource(const QUrl &url,
                                              const QVariantMap &parameters = QVariantMap()) override;
#ifndef QOAUTH2_NO_LEGACY_SCOPE
    QT_DEPRECATED_VERSION_X_6_13("Use requestedScopeTokens and grantedScopeTokens properties instead.")
    QString scope() const;
    QT_DEPRECATED_VERSION_X_6_13("Use requestedScopeTokens and grantedScopeTokens properties instead.")
    void setScope(const QString &scope);
#endif

    QSet<QByteArray> grantedScopeTokens() const;

    QSet<QByteArray> requestedScopeTokens() const;
    void setRequestedScopeTokens(const QSet<QByteArray> &tokens);

    QString userAgent() const;
    void setUserAgent(const QString &userAgent);

    QString responseType() const;

    QString clientIdentifierSharedKey() const;
    void setClientIdentifierSharedKey(const QString &clientIdentifierSharedKey);

    QString state() const;
    void setState(const QString &state);

    QDateTime expirationAt() const;

    QString refreshToken() const;
    void setRefreshToken(const QString &refreshToken);

    std::chrono::seconds refreshLeadTime() const;
    void setRefreshLeadTime(std::chrono::seconds leadTime);

    bool autoRefresh() const;
    void setAutoRefresh(bool enable);

    NonceMode nonceMode() const;
    void setNonceMode(NonceMode mode);

    QString nonce() const;
    void setNonce(const QString &nonce);

    QString idToken() const;

    QUrl tokenUrl() const;
    void setTokenUrl(const QUrl &tokenUrl);

#ifndef QT_NO_SSL
    QSslConfiguration sslConfiguration() const;
    void setSslConfiguration(const QSslConfiguration &configuration);
#endif

    void prepareRequest(QNetworkRequest *request, const QByteArray &verb,
                        const QByteArray &body = QByteArray()) override;

    template <typename Functor, if_compatible_callback<Functor> = true>
    void setNetworkRequestModifier(const ContextTypeForFunctor<Functor> *context,
                                 Functor &&callback) {
        setNetworkRequestModifierImpl(
            context,
            QtPrivate::makeCallableObject<NetworkRequestModifierPrototype>(
                std::forward<Functor>(callback)));
    }
    void clearNetworkRequestModifier();

protected Q_SLOTS:
    QT7_ONLY(virtual) void refreshTokensImplementation() QT7_ONLY(= 0);

public Q_SLOTS:
   void refreshTokens();

Q_SIGNALS:
#ifndef QOAUTH2_NO_LEGACY_SCOPE
    QT_MOC_COMPAT
    QT_DEPRECATED_VERSION_X_6_13("Use requestedScopeTokens and grantedScopeTokens properties instead.")
    void scopeChanged(const QString &scope);
#endif
    void grantedScopeTokensChanged(const QSet<QByteArray> &tokens);
    void requestedScopeTokensChanged(const QSet<QByteArray> &tokens);
    void userAgentChanged(const QString &userAgent);
    void responseTypeChanged(const QString &responseType);
    void clientIdentifierSharedKeyChanged(const QString &clientIdentifierSharedKey);
    void stateChanged(const QString &state);
    void expirationAtChanged(const QDateTime &expiration);
    void refreshTokenChanged(const QString &refreshToken);
    void accessTokenAboutToExpire();
    void refreshLeadTimeChanged(std::chrono::seconds leadTime);
    void autoRefreshChanged(bool enable);
    void nonceModeChanged(NonceMode mode);
    void nonceChanged(const QString &nonce);
    void idTokenChanged(const QString &idToken);
    void tokenUrlChanged(const QUrl &tokenUrl);
#ifndef QT_NO_SSL
    void sslConfigurationChanged(const QSslConfiguration &configuration);
#endif

#if QT_DEPRECATED_SINCE(6, 13)
    QT_MOC_COMPAT
    QT_DEPRECATED_VERSION_X_6_13("Use serverReportedErrorOccurred instead.")
    void error(const QString &error, const QString &errorDescription, const QUrl &uri);
#endif
    void serverReportedErrorOccurred(const QString &error, const QString &errorDescription,
                                     const QUrl &uri);
    void authorizationCallbackReceived(const QVariantMap &data);

protected:
    explicit QAbstractOAuth2(QAbstractOAuth2Private &, QObject *parent = nullptr);

    void setResponseType(const QString &responseType);

private:
    void setNetworkRequestModifierImpl(const QObject* context, QtPrivate::QSlotObjectBase *slot);
    Q_DECLARE_PRIVATE(QAbstractOAuth2)
};

QT_END_NAMESPACE

#endif // QT_NO_HTTP

#endif // QABSTRACTOAUTH2_H
