// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QERRORINFO_P_H
#define QERRORINFO_P_H

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

#include <QtMultimedia/qtmultimediaglobal.h>
#include <QString>

QT_BEGIN_NAMESPACE

template <typename ErrorCode, ErrorCode NoError = ErrorCode::NoError>
class QErrorInfo
{
public:
    QErrorInfo(ErrorCode error = NoError, QString description = {})
        : m_code(error), m_description(std::move(description))
    {
    }

    template <typename Notifier>
    void setAndNotify(ErrorCode code, QString description, Notifier &notifier)
    {
        const bool changed = code != m_code || description != m_description;

        m_code = code;
        m_description = std::move(description);

        if (code != NoError)
            emit notifier.errorOccurred(m_code, m_description);

        if (changed)
            emit notifier.errorChanged();
    }

    ErrorCode code() const { return m_code; }
    QString description() const { return m_description; };

private:
    ErrorCode m_code;
    QString m_description;
};

QT_END_NAMESPACE

#endif // QERRORINFO_P_H
