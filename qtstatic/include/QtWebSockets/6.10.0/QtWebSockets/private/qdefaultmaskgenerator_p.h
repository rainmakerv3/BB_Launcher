// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QDEFAULTMASKGENERATOR_P_H
#define QDEFAULTMASKGENERATOR_P_H

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

#include <QtWebSockets/qmaskgenerator.h>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE
class QObject;

class Q_AUTOTEST_EXPORT QDefaultMaskGenerator : public QMaskGenerator
{
    Q_DISABLE_COPY(QDefaultMaskGenerator)

public:
    explicit QDefaultMaskGenerator(QObject *parent = nullptr);
    ~QDefaultMaskGenerator() override;

    bool seed() noexcept override;
    quint32 nextMask() noexcept override;
};

QT_END_NAMESPACE

#endif // QDEFAULTMASKGENERATOR_P_H
