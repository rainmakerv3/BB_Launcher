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

#ifndef QOAUTH1SIGNATURE_P_H
#define QOAUTH1SIGNATURE_P_H

#include <QtNetworkAuth/qoauth1signature.h>

#include <QtCore/qurl.h>
#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>
#include <QtCore/qshareddata.h>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class QOAuth1SignaturePrivate : public QSharedData
{
public:
    QOAuth1SignaturePrivate() = default;
    QOAuth1SignaturePrivate(const QUrl &url, QOAuth1Signature::HttpRequestMethod method,
                            const QMultiMap<QString, QVariant> &parameters,
                            const QString &clientSharedKey = QString(),
                            const QString &tokenSecret = QString());

    QByteArray signatureBaseString() const;
    QByteArray secret() const;
    static QByteArray parameterString(const QMultiMap<QString, QVariant> &parameters);
    static QByteArray encodeHeaders(const QMultiMap<QString, QVariant> &headers);


    QOAuth1Signature::HttpRequestMethod method = QOAuth1Signature::HttpRequestMethod::Post;
    QByteArray customVerb;
    QUrl url;
    QString clientSharedKey;
    QString tokenSecret;
    QMultiMap<QString, QVariant> parameters;

    static QOAuth1SignaturePrivate shared_null;
};

QT_END_NAMESPACE

#endif // QOAUTH1SIGNATURE_P_H
