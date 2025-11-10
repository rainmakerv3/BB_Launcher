// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QMOCKIODEVICE_P_H
#define QMOCKIODEVICE_P_H

#include <qiodevice.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

class MockIODevice : public QIODevice
{
public:
    using QIODevice::QIODevice;

    qint64 writeData(const char *, qint64 len) override { return len; }

    qint64 readData(char *data, qint64 maxlen) override
    {
        memset(data, 0, maxlen);
        return maxlen;
    }
};

QT_END_NAMESPACE

#endif // QMOCKIODEVICE_P_H
