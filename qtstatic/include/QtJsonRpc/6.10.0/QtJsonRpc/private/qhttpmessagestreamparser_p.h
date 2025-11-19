// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QHTTPMESSAGESTREAMPARSER_P_H
#define QHTTPMESSAGESTREAMPARSER_P_H

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

#include <QtJsonRpc/qtjsonrpcglobal.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qstring.h>

#include <functional>

QT_BEGIN_NAMESPACE

class Q_JSONRPC_EXPORT QHttpMessageStreamParser
{
public:
    enum class State {
        PreHeader,
        InHeaderField,
        HeaderValueSpace,
        InHeaderValue,
        AfterCr,
        AfterCrLf,
        AfterCrLfCr,
        InBody
    };

    /*!
     * \internal
     * \brief Allows to run the FSM with or without keeping any buffers.
     */
    enum Mode { BUFFERED, UNBUFFERED };

    QHttpMessageStreamParser(
            std::function<void(const QByteArray &, const QByteArray &)> headerHandler,
            std::function<void(const QByteArray &body)> bodyHandler,
            std::function<void(QtMsgType error, QString msg)> errorHandler, Mode mode = BUFFERED);
    void receiveData(QByteArray data);
    bool receiveEof();

    State state() const { return m_state; }

private:
    void callHasHeader();
    void callHasBody();
    void errorMessage(QtMsgType error, QString msg);

    std::function<void(const QByteArray &, const QByteArray &)> m_headerHandler;
    std::function<void(const QByteArray &body)> m_bodyHandler;
    std::function<void(QtMsgType error, QString msg)> m_errorHandler;

    State m_state = State::PreHeader;
    QByteArray m_currentHeaderField;
    QByteArray m_currentHeaderValue;
    QByteArray m_currentPacket;
    int m_contentSize = -1;
    int m_currentPacketSize = 0;
    Mode m_mode;
};

QT_END_NAMESPACE
#endif // QHTTPMESSAGESTREAMPARSER_P_H
