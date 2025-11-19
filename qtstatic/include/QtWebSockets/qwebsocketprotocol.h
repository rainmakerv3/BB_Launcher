// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBSOCKETPROTOCOL_H
#define QWEBSOCKETPROTOCOL_H

#if 0
#  pragma qt_class(QWebSocketProtocol)
#endif

#include <QtCore/qglobal.h>
#include "QtWebSockets/qwebsockets_global.h"

QT_BEGIN_NAMESPACE

class QString;

namespace QWebSocketProtocol
{
enum Version
{
    VersionUnknown = -1,
    Version0 = 0,
    //hybi-01, hybi-02 and hybi-03 not supported
    Version4 = 4,
    Version5 = 5,
    Version6 = 6,
    Version7 = 7,
    Version8 = 8,
    Version13 = 13,
    VersionLatest = Version13
};

enum CloseCode
{
    CloseCodeNormal                 = 1000,
    CloseCodeGoingAway              = 1001,
    CloseCodeProtocolError          = 1002,
    CloseCodeDatatypeNotSupported   = 1003,
    CloseCodeReserved1004           = 1004,
    CloseCodeMissingStatusCode      = 1005,
    CloseCodeAbnormalDisconnection  = 1006,
    CloseCodeWrongDatatype          = 1007,
    CloseCodePolicyViolated         = 1008,
    CloseCodeTooMuchData            = 1009,
    CloseCodeMissingExtension       = 1010,
    CloseCodeBadOperation           = 1011,
    CloseCodeTlsHandshakeFailed     = 1015
};

}	//end namespace QWebSocketProtocol

QT_END_NAMESPACE

#endif // QWEBSOCKETPROTOCOL_H
