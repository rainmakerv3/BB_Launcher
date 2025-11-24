// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QOAUTHGLOBAL_H
#define QOAUTHGLOBAL_H

#include <QtCore/qglobal.h>
#include <QtNetwork/qtnetworkglobal.h>

QT_BEGIN_NAMESPACE

#if defined(QT_SHARED) || !defined(QT_STATIC)
#  if defined(QT_BUILD_NETWORKAUTH_LIB)
#    define Q_OAUTH_EXPORT Q_DECL_EXPORT
#  else
#    define Q_OAUTH_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_OAUTH_EXPORT
#endif

QT_END_NAMESPACE

#endif // QOAUTHGLOBAL_H
