// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:critical reason:network-protocol

#ifndef QWEBSOCKETFRAME_P_H
#define QWEBSOCKETFRAME_P_H

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

#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QCoreApplication>
#include <limits>

#include "qwebsockets_global.h"
#include "qwebsocketprotocol.h"
#include "qwebsocketprotocol_p.h"

QT_BEGIN_NAMESPACE

class QIODevice;

const quint64 MAX_FRAME_SIZE_IN_BYTES = std::numeric_limits<int>::max() - 1;

class Q_AUTOTEST_EXPORT QWebSocketFrame
{
    Q_DECLARE_TR_FUNCTIONS(QWebSocketFrame)

public:
    QWebSocketFrame() = default;

    void setMaxAllowedFrameSize(quint64 maxAllowedFrameSize);
    quint64 maxAllowedFrameSize() const;
    static quint64 maxFrameSize();

    QWebSocketProtocol::CloseCode closeCode() const;
    QString closeReason() const;
    bool isFinalFrame() const;
    bool isControlFrame() const;
    bool isDataFrame() const;
    bool isContinuationFrame() const;
    bool hasMask() const;
    quint32 mask() const;    //returns 0 if no mask
    inline bool rsv1() const { return m_rsv1; }
    inline bool rsv2() const { return m_rsv2; }
    inline bool rsv3() const { return m_rsv3; }
    QWebSocketProtocol::OpCode opCode() const;
    QByteArray payload() const;

    void clear();

    bool isValid() const;
    bool isDone() const;

    void readFrame(QIODevice *pIoDevice);

private:
    QString m_closeReason;
    QByteArray m_payload;
    quint64 m_length = 0;
    quint32 m_mask = 0;
    QWebSocketProtocol::CloseCode m_closeCode = QWebSocketProtocol::CloseCodeNormal;
    QWebSocketProtocol::OpCode m_opCode = QWebSocketProtocol::OpCodeReservedC;

    enum ProcessingState
    {
        PS_READ_HEADER,
        PS_READ_PAYLOAD_LENGTH,
        PS_READ_MASK,
        PS_READ_PAYLOAD,
        PS_DISPATCH_RESULT,
        PS_WAIT_FOR_MORE_DATA
    } m_processingState = PS_READ_HEADER;

    bool m_isFinalFrame = true;
    bool m_rsv1 = false;
    bool m_rsv2 = false;
    bool m_rsv3 = false;
    bool m_isValid = false;
    quint64 m_maxAllowedFrameSize = MAX_FRAME_SIZE_IN_BYTES;

    ProcessingState readFrameHeader(QIODevice *pIoDevice);
    ProcessingState readFramePayloadLength(QIODevice *pIoDevice);
    ProcessingState readFrameMask(QIODevice *pIoDevice);
    ProcessingState readFramePayload(QIODevice *pIoDevice);

    void setError(QWebSocketProtocol::CloseCode code, const QString &closeReason);
    bool checkValidity();
};

QT_END_NAMESPACE

#endif // QWEBSOCKETFRAME_P_H
