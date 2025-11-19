// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBSOCKETDATAPROCESSOR_P_H
#define QWEBSOCKETDATAPROCESSOR_P_H

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

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringDecoder>
#include <QtCore/QChronoTimer>
#include "qwebsocketframe_p.h"
#include "qwebsocketprotocol.h"
#include "qwebsocketprotocol_p.h"

QT_BEGIN_NAMESPACE

class QIODevice;
class QWebSocketFrame;

const quint64 MAX_MESSAGE_SIZE_IN_BYTES = std::numeric_limits<int>::max() - 1;

class Q_AUTOTEST_EXPORT QWebSocketDataProcessor : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QWebSocketDataProcessor)

public:
    explicit QWebSocketDataProcessor(QObject *parent = nullptr);
    ~QWebSocketDataProcessor() override;

    void setMaxAllowedFrameSize(quint64 maxAllowedFrameSize);
    quint64 maxAllowedFrameSize() const;
    void setMaxAllowedMessageSize(quint64 maxAllowedMessageSize);
    quint64 maxAllowedMessageSize() const;
    static quint64 maxMessageSize();
    static quint64 maxFrameSize();

    void setIdleTimeout(std::chrono::milliseconds timeout);
    std::chrono::milliseconds idleTimeout() const;

Q_SIGNALS:
    void pingReceived(const QByteArray &data);
    void pongReceived(const QByteArray &data);
    void closeReceived(QWebSocketProtocol::CloseCode closeCode, const QString &closeReason);
    void textFrameReceived(const QString &frame, bool lastFrame);
    void binaryFrameReceived(const QByteArray &frame, bool lastFrame);
    void textMessageReceived(const QString &message);
    void binaryMessageReceived(const QByteArray &message);
    void errorEncountered(QWebSocketProtocol::CloseCode code, const QString &description);

public Q_SLOTS:
    bool process(QIODevice *pIoDevice);
    void clear();

private:
    enum
    {
        PS_READ_HEADER,
        PS_READ_PAYLOAD_LENGTH,
        PS_READ_BIG_PAYLOAD_LENGTH,
        PS_READ_MASK,
        PS_READ_PAYLOAD,
        PS_DISPATCH_RESULT
    } m_processingState;

    bool m_isFinalFrame;
    bool m_isFragmented;
    QWebSocketProtocol::OpCode m_opCode;
    bool m_isControlFrame;
    bool m_hasMask;
    quint32 m_mask;
    QByteArray m_binaryMessage;
    QString m_textMessage;
    quint64 m_payloadLength;
    QStringDecoder m_decoder;
    QWebSocketFrame frame;
    QChronoTimer *m_waitTimer;
    quint64 m_maxAllowedMessageSize = MAX_MESSAGE_SIZE_IN_BYTES;

    bool processControlFrame(const QWebSocketFrame &frame);
    void timeout();
};

QT_END_NAMESPACE

#endif // QWEBSOCKETDATAPROCESSOR_P_H
