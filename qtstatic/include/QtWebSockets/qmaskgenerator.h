// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QMASKGENERATOR_H
#define QMASKGENERATOR_H

#include <QtCore/QObject>
#include "QtWebSockets/qwebsockets_global.h"

QT_BEGIN_NAMESPACE

class Q_WEBSOCKETS_EXPORT QMaskGenerator : public QObject
{
    Q_DISABLE_COPY(QMaskGenerator)

public:
    explicit QMaskGenerator(QObject *parent = nullptr);
    ~QMaskGenerator() override;

    virtual bool seed() = 0;
    virtual quint32 nextMask() = 0;
};

QT_END_NAMESPACE

#endif // QMASKGENERATOR_H
