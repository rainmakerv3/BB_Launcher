// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKSOUNDEFFECT_H
#define QQUICKSOUNDEFFECT_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QSoundEffect>
#include <QtQml/qqml.h>
#include <qtmultimediaquickexports.h>
#include <qurl.h>

QT_BEGIN_NAMESPACE

class Q_MULTIMEDIAQUICK_EXPORT QQuickSoundEffect : public QSoundEffect
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ qmlSource WRITE qmlSetSource NOTIFY sourceChanged)
    QML_NAMED_ELEMENT(SoundEffect)

public:
    QQuickSoundEffect(QObject *parent = nullptr);

    void qmlSetSource(const QUrl &source);

    QUrl qmlSource() const;

Q_SIGNALS:
    void sourceChanged(const QUrl &source);

private:
    QUrl m_source;
};

QT_END_NAMESPACE

#endif
