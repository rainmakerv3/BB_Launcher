// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMEDIAINPUTENCODERINTERFACE_P_H
#define QMEDIAINPUTENCODERINTERFACE_P_H

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

#include <QtMultimedia/qtmultimediaglobal.h>

QT_BEGIN_NAMESPACE

class QMediaInputEncoderInterface
{
public:
    virtual ~QMediaInputEncoderInterface() = default;
    virtual bool canPushFrame() const = 0;
};

QT_END_NAMESPACE

#endif // QMEDIAINPUTENCODERINTERFACE_P_H
