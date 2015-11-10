#include "sourcesyntaxhighlighter.h"
#include "settings.h"

#include <insituc/parser/parser.hpp>

#include <QStringList>
#include <QDebug>
#include <QTime>

#include <utility>

using namespace insituc;

static
QRegExp
enclose_word(QString const & word)
{
    return QRegExp(QString(R"(\b%1\b)").arg(word));
}

SourceSyntaxHighlighter::SourceSyntaxHighlighter(QTextDocument * parent)
    : QSyntaxHighlighter(parent)
{
    globalSymbolsFormat.setForeground(Qt::red);
    selectedGlobalVariablesFormat.setBackground(Qt::lightGray);
    selectedGlobalVariablesFormat.setFontWeight(QFont::Bold);
    functionExpression = QRegExp(R"(\b[A-Za-z_][A-Za-z0-9_]*\s*(?=\())");
    {
        keywordFormat.setForeground(Qt::blue);
        for (string_type const & reserved_symbol_ : parser::get_constants()) {
            keywordExpressions.append(enclose_word(QString::fromStdString(reserved_symbol_)));
        }
        for (string_type const & reserved_symbol_ : parser::get_intrinsics()) {
            keywordExpressions.append(enclose_word(QString::fromStdString(reserved_symbol_)));
        }
        for (string_type const & reserved_symbol_ : parser::get_keywords()) {
            keywordExpressions.append(enclose_word(QString::fromStdString(reserved_symbol_)));
        }
    }
    {
        literalFormat.setForeground(Qt::darkCyan);
    }
    {
        commentFormat.setForeground(Qt::gray);
        commentFormat.setFontItalic(true);
        commentSingleLineExpression = QRegExp(R"(//[^\n]*)");
        commentStartExpression = QRegExp(R"(/\*)");
        commentEndExpression = QRegExp(R"(\*/)");
    }
}

void SourceSyntaxHighlighter::highlight(QString const & text, QRegExp const & pattern, QTextCharFormat const & format)
{
    int index = pattern.indexIn(text);
    while (!(index < 0)) {
        int length = pattern.matchedLength();
        setFormat(index, length, format);
        index = pattern.indexIn(text, index + length);
    }
}

void SourceSyntaxHighlighter::highlightCommentStart(QString const & text, int & startIndex)
{
    int singleLineIndex = commentSingleLineExpression.indexIn(text, startIndex);
    startIndex = commentStartExpression.indexIn(text, startIndex);
    if (startIndex < 0) {
        startIndex = text.length();
    }
    while (!(singleLineIndex < 0)) {
        if (singleLineIndex < startIndex) {
            int commentLength = commentSingleLineExpression.matchedLength();
            setFormat(singleLineIndex, commentLength, commentFormat);
            singleLineIndex += commentLength;
            singleLineIndex = commentSingleLineExpression.indexIn(text, singleLineIndex);
        } else {
            break;
        }
    }
    startIndex = commentStartExpression.indexIn(text, startIndex);
}

void SourceSyntaxHighlighter::highlightBlock(QString const & text)
{
    highlight(text, Settings::getPersistentSettings().floatingPointExpression, literalFormat);
    highlight(text, Settings::getPersistentSettings().symbolExpression, identifierFormat);
    highlight(text, functionExpression, globalSymbolsFormat);
    for (QRegExp const & pattern : globalVariablesExpressions) {
        highlight(text, pattern, globalSymbolsFormat);
    }
    for (QRegExp const & pattern : selectedGlobalVariablesExpressions) {
        highlight(text, pattern, selectedGlobalVariablesFormat);
    }
    highlight(text, Settings::getPersistentSettings().dummySymbolExpression, literalFormat);
    for (QRegExp const & pattern : keywordExpressions) {
        highlight(text, pattern, keywordFormat);
    }
    { // comments
        setCurrentBlockState(0);
        int startIndex = 0;
        bool commentContinue = (previousBlockState() == 1);
        if (!commentContinue) {
            highlightCommentStart(text, startIndex);
        }
        while (!(startIndex < 0)) {
            int commentLength = commentContinue ? 0 : commentStartExpression.matchedLength();
            int endIndex = commentEndExpression.indexIn(text, startIndex + commentLength);
            if (endIndex < 0) {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            } else {
                commentLength = (endIndex + commentEndExpression.matchedLength()) - startIndex;
            }
            setFormat(startIndex, commentLength, commentFormat);
            startIndex += commentLength;
            if (!(startIndex < text.length())) {
                break;
            }
            highlightCommentStart(text, startIndex);
        }
    }
}

void SourceSyntaxHighlighter::on_globalVariablesChanged(QList< QString > const & globalVariablesNames)
{
    globalVariablesExpressions.clear();
    for (QString const & globalVariable : globalVariablesNames) {
        if (Settings::getPersistentSettings().symbolExpression.exactMatch(globalVariable)) {
            globalVariablesExpressions.append(enclose_word(globalVariable));
        }
    }
    return rehighlight();
}

void SourceSyntaxHighlighter::on_globalVariablesSelectionChanged(QList< QString > const & selectedGlobalVariablesNames)
{
    selectedGlobalVariablesExpressions.clear();
    for (QString const & selectedGlobalVariable : selectedGlobalVariablesNames) {
        if (Settings::getPersistentSettings().symbolExpression.exactMatch(selectedGlobalVariable)) {
           selectedGlobalVariablesExpressions.append(enclose_word(selectedGlobalVariable));
        }
    }
    return rehighlight();
}
