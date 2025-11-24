// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWEBVIEWGLOBAL_H
#define QWEBVIEWGLOBAL_H

#include <QtCore/qglobal.h>
#include <QtWebView/qtwebview-config.h>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#  if defined(QT_BUILD_WEBVIEW_LIB)
#    define Q_WEBVIEW_EXPORT Q_DECL_EXPORT
#  else
#    define Q_WEBVIEW_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_WEBVIEW_EXPORT
#endif

QT_END_NAMESPACE
#endif // QWEBVIEWGLOBAL_H
