// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKSATURATIONLIGHTNESSPICKER_P_H
#define QQUICKSATURATIONLIGHTNESSPICKER_P_H

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

#include "qquickabstractcolorpicker_p.h"
#include "qtquickdialogs2quickimplglobal_p.h"

QT_BEGIN_NAMESPACE

class QQuickSaturationLightnessPickerPrivate;

class Q_QUICKDIALOGS2QUICKIMPL_EXPORT QQuickSaturationLightnessPicker
    : public QQuickAbstractColorPicker
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SaturationLightnessPickerImpl)

public:
    explicit QQuickSaturationLightnessPicker(QQuickItem *parent = nullptr);

protected:
    QColor colorAt(const QPointF &pos) override;

private:
    Q_DISABLE_COPY(QQuickSaturationLightnessPicker)
    Q_DECLARE_PRIVATE(QQuickSaturationLightnessPicker)
};

QT_END_NAMESPACE

#endif // QQUICKSATURATIONLIGHTNESSPICKER_P_H
