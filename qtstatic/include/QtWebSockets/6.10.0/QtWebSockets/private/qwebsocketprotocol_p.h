// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBSOCKETPROTOCOL_P_H
#define QWEBSOCKETPROTOCOL_P_H

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

#include <QtCore/private/qglobal_p.h>
#include "QtWebSockets/qwebsocketprotocol.h"

#include <QtCore/qstringview.h>

QT_BEGIN_NAMESPACE

class QByteArray;

namespace QWebSocketProtocol
{
enum OpCode
{
    OpCodeContinue    = 0x0,
    OpCodeText        = 0x1,
    OpCodeBinary      = 0x2,
    OpCodeReserved3   = 0x3,
    OpCodeReserved4   = 0x4,
    OpCodeReserved5   = 0x5,
    OpCodeReserved6   = 0x6,
    OpCodeReserved7   = 0x7,
    OpCodeClose       = 0x8,
    OpCodePing        = 0x9,
    OpCodePong        = 0xA,
    OpCodeReservedB   = 0xB,
    OpCodeReservedC   = 0xC,
    OpCodeReservedD   = 0xD,
    OpCodeReservedE   = 0xE,
    OpCodeReservedF   = 0xF
};

inline bool isOpCodeReserved(OpCode code)
{
    return ((code > OpCodeBinary) && (code < OpCodeClose)) || (code > OpCodePong);
}

inline bool isCloseCodeValid(int closeCode)
{
    return  (closeCode > 999) && (closeCode < 5000) &&
            (closeCode != CloseCodeReserved1004) &&          //see RFC6455 7.4.1
            (closeCode != CloseCodeMissingStatusCode) &&
            (closeCode != CloseCodeAbnormalDisconnection) &&
            ((closeCode >= 3000) || (closeCode < 1012));
}

inline Version currentVersion() { return VersionLatest; }
Version Q_AUTOTEST_EXPORT versionFromString(QStringView versionString);

void Q_AUTOTEST_EXPORT mask(QByteArray *payload, quint32 maskingKey);
void Q_AUTOTEST_EXPORT mask(char *payload, quint64 size, quint32 maskingKey);
}	//end namespace QWebSocketProtocol

QT_END_NAMESPACE

#endif // QWEBSOCKETPROTOCOL_P_H
