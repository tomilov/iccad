#ifndef SOURCESYNTAXHIGHLIGHTER_H
#define SOURCESYNTAXHIGHLIGHTER_H

#include "base_types.hpp"

#include <QSyntaxHighlighter>

class SourceSyntaxHighlighter
        : public QSyntaxHighlighter
{

    Q_OBJECT

public :

    explicit
    SourceSyntaxHighlighter(QTextDocument * parent = nullptr);
    ~SourceSyntaxHighlighter() Q_DECL_OVERRIDE = default;

protected :

    void highlightBlock(QString const & text) Q_DECL_OVERRIDE;

private :

    QTextCharFormat identifierFormat;
    QTextCharFormat globalSymbolsFormat;
    QTextCharFormat selectedGlobalVariablesFormat;
    QTextCharFormat keywordFormat;
    QTextCharFormat literalFormat;

    QRegExp functionExpression;
    QVector< QRegExp > globalVariablesExpressions;
    QVector< QRegExp > selectedGlobalVariablesExpressions;
    QVector< QRegExp > keywordExpressions;


    QTextCharFormat commentFormat;

    QRegExp commentSingleLineExpression;
    QRegExp commentStartExpression;
    QRegExp commentEndExpression;


    void highlight(QString const & text, QRegExp const & pattern, QTextCharFormat const & format);
    void highlightCommentStart(QString const & text, int & startIndex);

public slots :

    void on_globalVariablesChanged(QList< QString > const & globalVariablesNames);
    void on_globalVariablesSelectionChanged(QList< QString > const & selectedGlobalVariablesNames);

};

#endif // SOURCESYNTAXHIGHLIGHTER_H
