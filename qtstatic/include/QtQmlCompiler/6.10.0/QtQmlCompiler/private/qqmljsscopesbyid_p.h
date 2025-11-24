// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#ifndef QQMLJSSCOPESBYID_P_H
#define QQMLJSSCOPESBYID_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.


#include "qqmljsscope_p.h"

#include <QtCore/qhash.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

enum QQmlJSScopesByIdOption: char {
    Default = 0,
    AssumeComponentsAreBound = 1,
};
Q_DECLARE_FLAGS(QQmlJSScopesByIdOptions, QQmlJSScopesByIdOption);

class QQmlJSScopesById
{
public:
    bool componentsAreBound() const { return m_componentsAreBound; }
    void setComponentsAreBound(bool bound) { m_componentsAreBound = bound; }

    void setSignaturesAreEnforced(bool enforced) { m_signaturesAreEnforced = enforced; }
    bool signaturesAreEnforced() const { return m_signaturesAreEnforced; }

    void setValueTypesAreAddressable(bool addressable) { m_valueTypesAreAddressable = addressable; }
    bool valueTypesAreAddressable() const { return m_valueTypesAreAddressable; }

    QString id(const QQmlJSScope::ConstPtr &scope, const QQmlJSScope::ConstPtr &referrer,
               QQmlJSScopesByIdOptions options = Default) const
    {
        const QQmlJSScope::ConstPtr referrerRoot = componentRoot(referrer);
        for (auto it = m_scopesById.begin(), end = m_scopesById.end(); it != end; ++it) {
            if (*it == scope && isComponentVisible(componentRoot(*it), referrerRoot, options))
                return it.key();
        }
        return QString();
    }

    /*!
        \internal
        Returns the scope that has id \a id in the component to which \a referrer belongs to.
        If no such scope exists, a null scope is returned.
     */
    QQmlJSScope::ConstPtr scope(const QString &id, const QQmlJSScope::ConstPtr &referrer,
                                QQmlJSScopesByIdOptions options = Default) const
    {
        Q_ASSERT(!id.isEmpty());
        const auto range =  m_scopesById.equal_range(id);
        if (range.first == range.second)
            return QQmlJSScope::ConstPtr();
        const QQmlJSScope::ConstPtr referrerRoot = componentRoot(referrer);

        for (auto it = range.first; it != range.second; ++it) {
            if (isComponentVisible(componentRoot(*it), referrerRoot, options))
                return *it;
        }

        return QQmlJSScope::ConstPtr();
    }

    void insert(const QString &id, const QQmlJSScope::ConstPtr &scope)
    {
        Q_ASSERT(!id.isEmpty());
        m_scopesById.insert(id, scope);
    }

    void clear() { m_scopesById.clear(); }

    /*!
        \internal
        Returns \c true if \a id exists anywhere in the current document.
        This is still allowed if the other occurrence is in a different (inline) component.
        Check the return value of scope to know whether the id has already been assigned
        in a givne scope.
    */
    bool existsAnywhereInDocument(const QString &id) const { return m_scopesById.contains(id); }

private:
    static QQmlJSScope::ConstPtr componentRoot(const QQmlJSScope::ConstPtr &inner)
    {
        QQmlJSScope::ConstPtr scope = inner;
        while (scope
                && scope->componentRootStatus() == QQmlJSScope::IsComponentRoot::No
                && !scope->isInlineComponent()) {
            if (QQmlJSScope::ConstPtr parent = scope->parentScope())
                scope = parent;
            else
                break;
        }
        return scope;
    }

    bool isComponentVisible(const QQmlJSScope::ConstPtr &observed,
                            const QQmlJSScope::ConstPtr &observer,
                            QQmlJSScopesByIdOptions options) const
    {
        if (!m_componentsAreBound && !options.testAnyFlag(AssumeComponentsAreBound))
            return observed == observer;

        for (QQmlJSScope::ConstPtr scope = observer; scope; scope = scope->parentScope()) {
            if (scope == observed)
                return true;
        }

        return false;
    }

    QMultiHash<QString, QQmlJSScope::ConstPtr> m_scopesById;
    bool m_componentsAreBound = false;
    bool m_signaturesAreEnforced = true;
    bool m_valueTypesAreAddressable = false;
};

QT_END_NAMESPACE

#endif // QQMLJSSCOPESBYID_P_H
