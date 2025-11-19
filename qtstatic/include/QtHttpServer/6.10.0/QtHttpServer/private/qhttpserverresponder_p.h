// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QHTTPSERVERRESPONDER_P_H
#define QHTTPSERVERRESPONDER_P_H

#include <QtHttpServer/qthttpserverglobal.h>
#include <QtHttpServer/qhttpserverrequest.h>
#include <QtHttpServer/qhttpserverresponder.h>

#include <private/qhttpserverstream_p.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qpair.h>
#include <QtCore/qpointer.h>
#include <QtCore/qsysinfo.h>

#include <type_traits>

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

class QHttpServerResponderPrivate
{
public:
    QHttpServerResponderPrivate(QHttpServerStream *stream);
    ~QHttpServerResponderPrivate();

    void write(const QByteArray &body, const QHttpHeaders &headers,
               QHttpServerResponder::StatusCode status);
    void write(QHttpServerResponder::StatusCode status);
    void write(QIODevice *data, const QHttpHeaders &headers,
               QHttpServerResponder::StatusCode status);
    void writeBeginChunked(const QHttpHeaders &headers, QHttpServerResponder::StatusCode status);
    void writeChunk(const QByteArray &body);
    void writeEndChunked(const QByteArray &data, const QHttpHeaders &trailers);

#if defined(QT_DEBUG)
    const QPointer<QHttpServerStream> stream;
#else
    QHttpServerStream *const stream;
#endif
    quint32 m_streamId = 0;
};

QT_END_NAMESPACE

#endif // QHTTPSERVERRESPONDER_P_H
