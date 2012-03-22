#ifndef SYNTAX_HIGHLIGHTER_H
#define SYNTAX_HIGHLIGHTER_H

#include <QSyntaxHighlighter>

class QTextDocument;
class SyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	SyntaxHighlighter(QTextDocument *parent = 0);

	static QStringList javascript_keyword_patterns();

protected:
	void highlightBlock(const QString &text);

private:
	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> highlightingRules;

	QRegExp commentStartExpression;
	QRegExp commentEndExpression;

	QTextCharFormat keywordFormat;
	QTextCharFormat classFormat;
	QTextCharFormat singleLineCommentFormat;
	QTextCharFormat multiLineCommentFormat;
	QTextCharFormat quotationFormat;
	QTextCharFormat functionFormat;
        QTextCharFormat pscriptKeywordFormat;
        QTextCharFormat pscriptFunctionFormat;
};
#endif // SYNTAX_HIGHLIGHTER_H
