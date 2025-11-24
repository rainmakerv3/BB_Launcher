// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QOAUTH1SIGNATURE_H
#define QOAUTH1SIGNATURE_H

#include <QtNetworkAuth/qoauthglobal.h>

#ifndef QT_NO_HTTP

#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QUrlQuery;

class QOAuth1SignaturePrivate;
class Q_OAUTH_EXPORT QOAuth1Signature
{
public:
    enum class HttpRequestMethod {
        Head = 1,
        Get,
        Put,
        Post,
        Delete,
        Custom,

        Unknown = 0
    };

    explicit QOAuth1Signature(const QUrl &url = QUrl(),
                              HttpRequestMethod method = HttpRequestMethod::Post,
                              const QMultiMap<QString, QVariant> &parameters = {});
    QOAuth1Signature(const QUrl &url, const QString &clientSharedKey, const QString &tokenSecret,
                     HttpRequestMethod method = HttpRequestMethod::Post,
                     const QMultiMap<QString, QVariant> &parameters = {});
    QOAuth1Signature(const QOAuth1Signature &other);
    QOAuth1Signature(QOAuth1Signature &&other);
    ~QOAuth1Signature();

    HttpRequestMethod httpRequestMethod() const;
    void setHttpRequestMethod(HttpRequestMethod method);

    QByteArray customMethodString() const;
    void setCustomMethodString(const QByteArray &verb);

    QUrl url() const;
    void setUrl(const QUrl &url);

    QMultiMap<QString, QVariant> parameters() const;
    void setParameters(const QMultiMap<QString, QVariant> &parameters);
    void addRequestBody(const QUrlQuery &body);

    void insert(const QString &key, const QVariant &value);
    QList<QString> keys() const;
    QVariant take(const QString &key);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

    QString clientSharedKey() const;
    void setClientSharedKey(const QString &secret);

    QString tokenSecret() const;
    void setTokenSecret(const QString &secret);

    QByteArray hmacSha1() const;
    QByteArray rsaSha1() const;
    QByteArray plainText() const;

    static QByteArray plainText(const QString &clientSharedSecret, const QString &tokenSecret);

    void swap(QOAuth1Signature &other);
    QOAuth1Signature &operator=(const QOAuth1Signature &other);
    QOAuth1Signature &operator=(QOAuth1Signature &&other);

private:
    QSharedDataPointer<QOAuth1SignaturePrivate> d;
};

QT_END_NAMESPACE

#endif // QT_NO_HTTP

#endif // QOAUTH1SIGNATURE_H
