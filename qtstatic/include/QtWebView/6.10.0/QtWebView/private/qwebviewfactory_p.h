// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWEBVIEWFACTORY_H
#define QWEBVIEWFACTORY_H

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

#include "qabstractwebview_p.h"

QT_BEGIN_NAMESPACE

class QWebViewPlugin;

namespace QWebViewFactory
{
    QWebViewPlugin *getPlugin();
    QAbstractWebView *createWebView();
    bool requiresExtraInitializationSteps();
    Q_WEBVIEW_EXPORT bool loadedPluginHasKey(const QString key);
};

QT_END_NAMESPACE

#endif // QWEBVIEWFACTORY_H
