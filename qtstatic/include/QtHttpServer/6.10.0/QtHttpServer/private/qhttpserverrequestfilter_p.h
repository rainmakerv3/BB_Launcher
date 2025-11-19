// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERREQUESTFILTER_P_H
#define QHTTPSERVERREQUESTFILTER_P_H

#include <QtHttpServer/qthttpserverglobal.h>
#include <QtHttpServer/qhttpserverconfiguration.h>

#include <QtCore/qhash.h>
#include <QtNetwork/qhostaddress.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QHttpServer. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

QT_BEGIN_NAMESPACE

namespace QHttpServerRequestFilterPrivate {
extern const Q_HTTPSERVER_EXPORT int cPeriodDurationMSec;
}

class QHttpServerRequestFilter
{
public:
    // Rule Of Zero applies!

    Q_HTTPSERVER_EXPORT void setConfiguration(const QHttpServerConfiguration &config);

    Q_HTTPSERVER_EXPORT bool isRequestAllowed(const QHostAddress &peerAddress) const;

    Q_HTTPSERVER_EXPORT bool isRequestWithinRate(const QHostAddress &peerAddress);
    Q_HTTPSERVER_EXPORT bool isRequestWithinRate(const QHostAddress &peerAddress, qint64 currTimeMSec);

private:
    struct IpInfo
    {
        IpInfo() : m_thisPeriodEnd(0) {}
        IpInfo(qint64 thisPeriodEnd) : m_thisPeriodEnd(thisPeriodEnd) {}

        bool isGarbage(qint64 currTime) const;

        qint64 m_thisPeriodEnd = 0;
        unsigned m_nRequests = 0;
    };

    void cleanIpInfoGarbage(QHash<QHostAddress, IpInfo>::iterator it, qint64 currTime);

    unsigned maxRequestPerPeriod() const;

    QHttpServerConfiguration m_config;
    QHash<QHostAddress, IpInfo> ipInfo;
};

QT_END_NAMESPACE

#endif // QHTTPSERVERREQUESTFILTER_P_H
