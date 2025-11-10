// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWINDOWS_PROPERTYSTORE_P_H
#define QWINDOWS_PROPERTYSTORE_P_H

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

#include <QtCore/qglobal.h>
#include <QtCore/private/qcomptr_p.h>
#include <QtCore/private/qexpected_p.h>
#include <QtCore/private/qsystemerror_p.h>

struct IMMDevice;
struct IPropertyStore;
typedef struct _tagpropertykey PROPERTYKEY;

QT_BEGIN_NAMESPACE

namespace QtMultimediaPrivate {

class PropertyStoreHelper
{
public:
    explicit PropertyStoreHelper(ComPtr<IPropertyStore>);
    static q23::expected<PropertyStoreHelper, QString> open(const ComPtr<IMMDevice> &);

    std::optional<QString> getString(const PROPERTYKEY &);
    std::optional<uint32_t> getUInt32(const PROPERTYKEY &);

private:
    ComPtr<IPropertyStore> m_props;
};

} // namespace QtMultimediaPrivate

QT_END_NAMESPACE

#endif // QWINDOWS_PROPERTYSTORE_P_H
