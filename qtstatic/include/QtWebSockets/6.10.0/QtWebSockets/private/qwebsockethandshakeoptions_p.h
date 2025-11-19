// Copyright (C) 2022 Menlo Systems GmbH, author Arno Rehn <a.rehn@menlosystems.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBSOCKETHANDSHAKEOPTIONS_P_H
#define QWEBSOCKETHANDSHAKEOPTIONS_P_H

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

#include <QSharedData>

#include "qwebsockethandshakeoptions.h"

QT_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT QWebSocketHandshakeOptionsPrivate : public QSharedData
{
public:
    inline bool operator==(const QWebSocketHandshakeOptionsPrivate &other) const
    { return subprotocols == other.subprotocols; }

    QStringList subprotocols;
};

QT_END_NAMESPACE

#endif // QWEBSOCKETHANDSHAKEOPTIONS_P_H
