// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QDECLARATIVEPLUGINPARAMETER_P_H
#define QDECLARATIVEPLUGINPARAMETER_P_H

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

#include <QtPositioningQuick/private/qpositioningquickglobal_p.h>
#include <QtQml/qqml.h>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

class Q_POSITIONINGQUICK_EXPORT QDeclarativePluginParameter : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PluginParameter)
    QML_ADDED_IN_VERSION(5, 14)

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit QDeclarativePluginParameter(QObject *parent = 0);
    ~QDeclarativePluginParameter();

    void setName(const QString &name);
    QString name() const;

    void setValue(const QVariant &value);
    QVariant value() const;

    bool isInitialized() const;

Q_SIGNALS:
    void nameChanged(const QString &name);
    void valueChanged(const QVariant &value);
    void initialized();

private:
    QString name_;
    QVariant value_;
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePluginParameter)

#endif // QDECLARATIVEPLUGINPARAMETER_P_H
