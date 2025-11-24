// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQMLSEMANTICTOKENS_P_H
#define QQMLSEMANTICTOKENS_P_H

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

#include <QtLanguageServer/private/qlanguageserverspec_p.h>
#include <QtQmlDom/private/qqmldomitem_p.h>
#include <QtCore/qlist.h>
#include <QtCore/qmap.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(semanticTokens)

namespace HighlightingUtils {
Q_NAMESPACE

// Protocol agnostic highlighting kinds
// Use this enum while visiting dom tree to define the highlighting kinds for the semantic tokens
// Then map it to the protocol specific token types and modifiers
// This can be as much as detailed as needed
enum class QmlHighlightKind {
    QmlKeyword, // Qml keyword
    QmlType, // Qml type name
    QmlImportId, // Qml import module name
    QmlNamespace, // Qml module namespace, i.e import QtQuick as Namespace
    QmlLocalId, // Object id within the same file
    QmlExternalId, // Object id defined in another file. [UNUSED FOR NOW]
    QmlProperty, // Qml property. For now used for all kind of properties
    QmlScopeObjectProperty, // Qml property defined in the current scope
    QmlRootObjectProperty, // Qml property defined in the parent scopes
    QmlExternalObjectProperty, // Qml property defined in the root object of another file
    QmlMethod,
    QmlMethodParameter,
    QmlSignal,
    QmlSignalHandler,
    QmlEnumName, // Enum type name
    QmlEnumMember, // Enum field names
    QmlPragmaName, // Qml pragma name
    QmlPragmaValue, // Qml pragma value
    QmlTypeModifier, // list<QtObject>, list is the modifier, QtObject is the type
    JsImport, // Js imported name
    JsGlobalVar, // Js global variable or objects
    JsGlobalMethod, // Js global method
    JsScopeVar, // Js variable defined in the current scope
    JsLabel, // js label
    Number,
    String,
    Comment,
    Operator,
    Field, // Used for the field names in the property chains,
    Unknown, // Used for the unknown tokens
};

enum class QmlHighlightModifier {
    None = 0,
    QmlPropertyDefinition = 1 << 0,
    QmlDefaultProperty = 1 << 1,
    QmlFinalProperty = 1 << 2,
    QmlRequiredProperty = 1 << 3,
    QmlReadonlyProperty = 1 << 4,
};
Q_DECLARE_FLAGS(QmlHighlightModifiers, QmlHighlightModifier)
Q_DECLARE_OPERATORS_FOR_FLAGS(QmlHighlightModifiers)

enum class HighlightingMode { Default, QtCHighlighting };

// Protocol specific token types
// The values in this enum are converted to relevant strings and sent to the client as server
// capabilities The convention is that the first letter in the enum value is decapitalized and the
// rest is unchanged i.e Namespace -> "namespace" This is handled in enumToByteArray() helper
// function.
enum class SemanticTokenProtocolTypes {
    // Subset of the QLspSpefication::SemanticTokenTypes enum
    // We register only the token types used in the qml semantic highlighting
    Namespace,
    Type,
    Enum,
    Parameter,
    Variable,
    Property,
    EnumMember,
    Method,
    Keyword,
    Comment,
    String,
    Number,
    Regexp,
    Operator,
    Decorator,

    // Additional token types for the extended semantic highlighting
    QmlLocalId, // object id within the same file
    QmlExternalId, // object id defined in another file
    QmlRootObjectProperty, // qml property defined in the parent scopes
    QmlScopeObjectProperty, // qml property defined in the current scope
    QmlExternalObjectProperty, // qml property defined in the root object of another file
    JsScopeVar, // js variable defined in the current file
    JsImportVar, // js import name that is imported in the qml file
    JsGlobalVar, // js global variables
    QmlStateName, // name of a qml state
    Field, // used for the field names in the property chains
    Unknown, // used for the unknown contexts
};
Q_ENUM_NS(SemanticTokenProtocolTypes)

} // namespace HighlightingUtils

// Represents a semantic highlighting token
// startLine and startColumn are 0-based as in LSP spec.
struct Token
{
    Token() = default;
    Token(const QQmlJS::SourceLocation &loc, int tokenType, int tokenModifier = 0)
        : offset(loc.offset),
          length(loc.length),
          startLine(loc.startLine - 1),
          startColumn(loc.startColumn - 1),
          tokenType(tokenType),
          tokenModifier(tokenModifier)
    {
    }

    inline friend bool operator<(const Token &lhs, const Token &rhs)
    {
        return lhs.offset < rhs.offset;
    }

    inline friend bool operator==(const Token &lhs, const Token &rhs)
    {
        return lhs.offset == rhs.offset && lhs.length == rhs.length
                && lhs.startLine == rhs.startLine && lhs.startColumn == rhs.startColumn
                && lhs.tokenType == rhs.tokenType && lhs.tokenModifier == rhs.tokenModifier;
    }

    int offset;
    int length;
    int startLine;
    int startColumn;
    int tokenType;
    int tokenModifier;
};

using HighlightsContainer = QMap<int, QT_PREPEND_NAMESPACE(Token)>;

/*!
\internal
Offsets start from zero.
*/
struct HighlightsRange
{
    int startOffset;
    int endOffset;
};

class Highlights
{
public:
    using QmlHighlightKindToLspKind = int (*)(HighlightingUtils::QmlHighlightKind);
    Highlights(HighlightingUtils::HighlightingMode mode = HighlightingUtils::HighlightingMode::Default);
    void addHighlight(const QQmlJS::SourceLocation &loc, HighlightingUtils::QmlHighlightKind,
                      HighlightingUtils::QmlHighlightModifiers =
                              HighlightingUtils::QmlHighlightModifier::None);
    HighlightsContainer &tokens() { return m_highlights; }
    const HighlightsContainer &tokens() const { return m_highlights; }

private:
    void addHighlightImpl(const QQmlJS::SourceLocation &loc, int tokenType, int tokenModifier = 0);
    HighlightsContainer m_highlights;
    QmlHighlightKindToLspKind m_mapToProtocol;
};

namespace HighlightingUtils
{
    QList<int> encodeSemanticTokens(Highlights &highlights);
    QList<QQmlJS::SourceLocation>
    sourceLocationsFromMultiLineToken(QStringView code,
                                      const QQmlJS::SourceLocation &tokenLocation);
    void addModifier(QLspSpecification::SemanticTokenModifiers modifier, int *baseModifier);
    bool rangeOverlapsWithSourceLocation(const QQmlJS::SourceLocation &loc, const HighlightsRange &r);
    QList<QLspSpecification::SemanticTokensEdit> computeDiff(const QList<int> &, const QList<int> &);
    void updateResultID(QByteArray &resultID);
    QList<int> collectTokens(const QQmlJS::Dom::DomItem &item,
                             const std::optional<HighlightsRange> &range,
                             HighlightingMode mode = HighlightingMode::Default);
    Highlights visitTokens(const QQmlJS::Dom::DomItem &item,
                                 const std::optional<HighlightsRange> &range,
                                 HighlightingMode mode = HighlightingMode::Default);
} // namespace HighlightingUtils

class HighlightingVisitor
{
public:
    HighlightingVisitor(const QQmlJS::Dom::DomItem &item,
                        const std::optional<HighlightsRange> &range,
                        HighlightingUtils::HighlightingMode mode =
                                HighlightingUtils::HighlightingMode::Default);
    const Highlights &hightights() const { return m_highlights; }
    Highlights &highlights() { return m_highlights; }

private:
    bool visitor(QQmlJS::Dom::Path, const QQmlJS::Dom::DomItem &item, bool);
    void highlightComment(const QQmlJS::Dom::DomItem &item);
    void highlightImport(const QQmlJS::Dom::DomItem &item);
    void highlightBinding(const QQmlJS::Dom::DomItem &item);
    void highlightPragma(const QQmlJS::Dom::DomItem &item);
    void highlightEnumItem(const QQmlJS::Dom::DomItem &item);
    void highlightEnumDecl(const QQmlJS::Dom::DomItem &item);
    void highlightQmlObject(const QQmlJS::Dom::DomItem &item);
    void highlightComponent(const QQmlJS::Dom::DomItem &item);
    void highlightPropertyDefinition(const QQmlJS::Dom::DomItem &item);
    void highlightMethod(const QQmlJS::Dom::DomItem &item);
    void highlightScriptLiteral(const QQmlJS::Dom::DomItem &item);
    void highlightIdentifier(const QQmlJS::Dom::DomItem &item);
    void highlightBySemanticAnalysis(const QQmlJS::Dom::DomItem &item, QQmlJS::SourceLocation loc);
    void highlightScriptExpressions(const QQmlJS::Dom::DomItem &item);
    void highlightCallExpression(const QQmlJS::Dom::DomItem &item);
    void highlightFieldMemberAccess(const QQmlJS::Dom::DomItem &item, QQmlJS::SourceLocation loc);

private:
    Highlights m_highlights;
    std::optional<HighlightsRange> m_range;
};

QT_END_NAMESPACE

#endif // QQMLSEMANTICTOKENS_P_H
