// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWEBVIEWPLUGIN_H
#define QWEBVIEWPLUGIN_H

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

#include "qwebview_global.h"
#include "qabstractwebview_p.h"

#include <QtCore/qobject.h>

#define QWebViewPluginInterface_iid "org.qt-project.Qt.QWebViewPluginInterface"

QT_BEGIN_NAMESPACE

class Q_WEBVIEW_EXPORT QWebViewPlugin : public QObject
{
    Q_OBJECT
public:
    explicit QWebViewPlugin(QObject *parent = nullptr);
    virtual ~QWebViewPlugin();

    virtual QAbstractWebView *create(const QString &key) const = 0;

    virtual void prepare() const;
};

QT_END_NAMESPACE

#endif // QWEBVIEWPLUGIN_H
