// Copyright (C) 2024 The Qt Company Ltd.
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

#ifndef QOAUTH2DEVICEAUTHORIZATIONFLOW_P_H
#define QOAUTH2DEVICEAUTHORIZATIONFLOW_P_H

#include <private/qabstractoauth2_p.h>

#include <QtNetworkAuth/qoauth2deviceauthorizationflow.h>
#include <QtNetworkAuth/qoauthglobal.h>

#include <QtCore/qchronotimer.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qpointer.h>
#include <QtCore/qstring.h>

#include <chrono>

QT_BEGIN_NAMESPACE

class QRestAccessManager;
class QRestReply;

class Q_AUTOTEST_EXPORT QOAuth2DeviceAuthorizationFlowPrivate : public QAbstractOAuth2Private
{
public:
    Q_DECLARE_PUBLIC(QOAuth2DeviceAuthorizationFlow)

    QOAuth2DeviceAuthorizationFlowPrivate(QNetworkAccessManager *manager = nullptr);
    ~QOAuth2DeviceAuthorizationFlowPrivate() override;

    void authorizationReplyFinished(QRestReply &reply);
    void tokenReplyFinished(QRestReply &reply);

    void handleTokenSuccessResponse(const QJsonObject &data);
    void handleTokenErrorResponse(const QJsonObject &data);

    void tokenAcquisitionFailed(QAbstractOAuth::Error error, const QString &errorString = {});

    void setUserCode(const QString &code);
    void setVerificationUrl(const QUrl &url);
    void setVerificationUrlComplete(const QUrl &url);
    void setUserCodeExpiration(const QDateTime &expiration);

    bool startTokenPolling();
    void stopTokenPolling();
    void pollTokens();
    bool isNextPollAfterExpiration() const;

    void reset();
    void resetCurrentTokenReply();
    void resetCurrentAuthorizationReply();

    QRestAccessManager *network();

    // https://datatracker.ietf.org/doc/html/rfc8628#section-3.2
    static inline constexpr auto defaultPollingInterval = std::chrono::seconds(5);
    bool useAutoTestDurations = false;
    QString userCode;
    QString deviceCode;
    QDateTime userCodeExpirationUtc; // When devicecode and usercode expire
    QUrl verificationUrl;
    QUrl completeVerificationUrl;
    QRestAccessManager *restAccessManager = nullptr;
    QChronoTimer tokenPollingTimer;
    QPointer<QNetworkReply> currentAuthorizationReply;
    QPointer<QNetworkReply> currentTokenReply;
};

QT_END_NAMESPACE

#endif // QOAUTH2DEVICEAUTHORIZATIONFLOW_P_H
