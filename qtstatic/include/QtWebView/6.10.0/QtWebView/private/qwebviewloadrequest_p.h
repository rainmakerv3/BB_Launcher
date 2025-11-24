// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only


#ifndef QWEBVIEWLOADREQUESTPRIVATE_H
#define QWEBVIEWLOADREQUESTPRIVATE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtWebView/private/qwebview_p.h>
#include <QtCore/qstring.h>
#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

class Q_WEBVIEW_EXPORT QWebViewLoadRequestPrivate
{
public:
    QWebViewLoadRequestPrivate();
    QWebViewLoadRequestPrivate(const QUrl &url,
                               QWebView::LoadStatus status,
                               const QString &errorString);
    ~QWebViewLoadRequestPrivate();

    QUrl m_url;
    QWebView::LoadStatus m_status;
    QString m_errorString;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QWebViewLoadRequestPrivate)

#endif // QWEBVIEWLOADREQUESTPRIVATE_H
