// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QOAUTHURISCHEMEREPLYHANDLER_H
#define QOAUTHURISCHEMEREPLYHANDLER_H

#include <QtNetworkAuth/qoauthglobal.h>
#include <QtNetworkAuth/qoauthoobreplyhandler.h>

#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

class QOAuthUriSchemeReplyHandlerPrivate;
class Q_OAUTH_EXPORT QOAuthUriSchemeReplyHandler : public QOAuthOobReplyHandler
{
    Q_OBJECT
    Q_PROPERTY(QUrl redirectUrl READ redirectUrl WRITE setRedirectUrl NOTIFY redirectUrlChanged FINAL)
public:
    Q_IMPLICIT QOAuthUriSchemeReplyHandler() : QOAuthUriSchemeReplyHandler(nullptr) {}
    explicit QOAuthUriSchemeReplyHandler(QObject *parent);
    explicit QOAuthUriSchemeReplyHandler(const QUrl &redirectUrl, QObject *parent = nullptr);
    ~QOAuthUriSchemeReplyHandler() override;

    QString callback() const override;

    void setRedirectUrl(const QUrl &url);
    QUrl redirectUrl() const;

    bool handleAuthorizationRedirect(const QUrl &url);

    bool listen();
    void close();
    bool isListening() const noexcept;

Q_SIGNALS:
    void redirectUrlChanged();

private:
    Q_DISABLE_COPY(QOAuthUriSchemeReplyHandler)
    Q_DECLARE_PRIVATE(QOAuthUriSchemeReplyHandler)
    // Private slot for providing a callback slot for QDesktopServices::setUrlHandler
    Q_PRIVATE_SLOT(d_func(), bool _q_handleRedirectUrl(const QUrl &url))
};

QT_END_NAMESPACE

#endif // QOAUTHURISCHEMEREPLYHANDLER_H
