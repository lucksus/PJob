#include "SyntaxHighlighter.h"

QStringList SyntaxHighlighter::javascript_keyword_patterns(){
	QStringList keywordPatterns;
	keywordPatterns
		<< "\\babstract\\b" << "\\bboolean\\b" << "\\bbreak\\b" << "\\bbyte\\b" << "\\bcase\\b"
		<< "\\bcatch\\b" << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b" << "\\bcontinue\\b"
		<< "\\bdebugger\\b" << "\\bdefault\\b" << "\\bdelete\\b" << "\\bdo\\b" << "\\bdouble\\b"
		<< "\\belse\\b" << "\\benum\\b" << "\\bexport\\b" << "\\bextends\\b" << "\\bfalse\\b"
		<< "\\bfinal\\b" << "\\bfinally\\b" << "\\bfloat\\b" << "\\bfor\\b" << "\\bfunction\\b"
		<< "\\bgoto\\b" << "\\bif\\b" << "\\bimplements\\b" << "\\bimport\\b" << "\\bin\\b"
		<< "\\binstanceof\\b" << "\\bint\\b" << "\\binterface\\b" << "\\blong\\b" << "\\bnative\\b"
		<< "\\bnew\\b" << "\\bnull\\b" << "\\bpackage\\b" << "\\bprivate\\b" << "\\bprotected\\b"
		<< "\\bpublic\\b" << "\\breturn\\b" << "\\bshort\\b" << "\\bstatic\\b" << "\\bsuper\\b"
		<< "\\bswitch\\b" << "\\bsynchronized\\b" << "\\bthis\\b" << "\\bthrow\\b" << "\\bthrows\\b"
		<< "\\btransient\\b" << "\\btrue\\b" << "\\btry\\b" << "\\btypeof\\b" << "\\bvar\\b"
		<< "\\bvoid\\b" << "\\bvolatile\\b" << "\\bwhile\\b" << "\\bwith\\b";
	return keywordPatterns;
}

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
: QSyntaxHighlighter(parent)
{
	HighlightingRule rule;

	keywordFormat.setForeground(Qt::darkBlue);
	keywordFormat.setFontWeight(QFont::Bold);
	QStringList keywordPatterns = javascript_keyword_patterns();

	foreach (const QString &pattern, keywordPatterns) 
	{
		rule.pattern = QRegExp(pattern);
		rule.format = keywordFormat;
		highlightingRules.append(rule);
	}


        classFormat.setForeground(Qt::darkRed);
        rule.pattern = QRegExp("\\b[0-9]+(.[0-9]+)?\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);


        pscriptKeywordFormat.setForeground(Qt::black);
        pscriptKeywordFormat.setFontWeight(QFont::Bold);

        QStringList pscriptObjectPatterns;
        pscriptObjectPatterns
        << "\\bapplication\\b" << "\\bcomponentObject\\b" << "\\bglobalVariableObject\\b" << "\\bMath\\b"
        << "\\bmatlab\\b" << "\\bparameterObject\\b" << "\\bpscript\\b" << "\\bPScriptSettings\\b"
        << "\\bresultObject\\b" << "\\brng\\b" << "\\bsimulationObject\\b" << "\\bsimulationParameterObject\\b"
        << "\\biteratorComponent\\b" << "\\bphotoss\\b" << "\\bexe\\b" << "\\bparameters\\b";

        foreach (const QString &pattern, pscriptObjectPatterns)
        {
                rule.pattern = QRegExp(pattern);
                rule.format = pscriptKeywordFormat;
                highlightingRules.append(rule);
        }

        foreach (QString pattern, pscriptObjectPatterns)
        {
                pattern[2] = pattern[2].toUpper();
                rule.pattern = QRegExp(pattern);
                rule.format = pscriptKeywordFormat;
                highlightingRules.append(rule);
        }


	classFormat.setFontWeight(QFont::Bold);
	classFormat.setForeground(Qt::darkMagenta);
	rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
	rule.format = classFormat;
	highlightingRules.append(rule);



        quotationFormat.setForeground(Qt::red);
	rule.pattern = QRegExp("\".*\"");
	rule.format = quotationFormat;
	highlightingRules.append(rule);

	functionFormat.setFontItalic(true);
        //functionFormat.setForeground(Qt::darkCyan);
	rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
	rule.format = functionFormat;
	highlightingRules.append(rule);


        pscriptFunctionFormat.setFontItalic(false);
        pscriptFunctionFormat.setFontWeight(QFont::Bold);
        pscriptFunctionFormat.setForeground(Qt::blue);

        QStringList pscriptFunctionPatterns;
        pscriptFunctionPatterns
        << "\\bclear" << "\\bclos)" << "\\bcontainsComponent" << "\\bgetBypass" << "\\bgetComponent"
        << "\\bgetComponentsByType" << "\\bgetComponentType" << "\\bgetGlobalVariable" << "\\bgetGlobalVariables"
        << "\\bgetGlobalVariableUnit" << "\\bgetGlobalVariableValue" << "\\bgetIterationResultValues"
        << "\\bgetName" << "\\bgetParameter" << "\\bgetParameterNames" << "\\bgetParameters"
        << "\\bgetParameterUnit" << "\\bgetParameterUnits" << "\\bgetParameterValue" << "\\bgetParameterValues"
        << "\\bgetPMDPath" << "\\bgetResult" << "\\bgetResultNames" << "\\bgetResults" << "\\bgetResultUnit"
        << "\\bgetResultUnits" << "\\bgetResultValue" << "\\bgetResultValues" << "\\bgetSimulationParameters"
        << "\\bgetSimulationParameterUnit" << "\\bgetSimulationParameterValue" << "\\bgetSimulationParameterValues"
        << "\\bgetUnit" << "\\bsetUnit" << "\\bgetValue" << "\\bisComponentOfType" << "\\bisIterator"
        << "\\bisNetwork" << "\\bisOpen" << "\\blistComponents" << "\\blistGlobalVariables" << "\\blistParameters"
        << "\\blistResults" << "\\blistSimulationParameters" << "\\brun" << "\\bsave" << "\\bsaveAs"
        << "\\bsetBypass" << "\\bsetGlobalVariableValue" << "\\bsetParameterValue" << "\\bsetName" << "\\bsetPMDPath"
        << "\\bsetSimulationParameterValue" << "\\bisGlobalVariableOfTypeVariation" << "\\bgetParameterDescription"
        << "\\binvestigate" << "\\bdeleteComponent" << "\\bdeleteAllComponents" << "\\blinkComponents"
        << "\\bunlinkComponents";

        pscriptFunctionPatterns
        << "\\bclose" << "\\bcloseSimulations" << "\\bdeleteFile" << "\\bgetApplicationDataDirectory"
        << "\\bgetCurrentDirectory" << "\\bgetPhotossExecutableDirectory" << "\\bgetScriptDirectory"
        << "\\bgetSimulations" << "\\bgetUserHomeDirectory" << "\\binfo" << "\\bopenSimulation"
        << "\\bsetCurrentDirectory" << "\\bclearWorkspace" << "\\brunAutoIncludes" << "\\bresetEngine"
        << "\\bcollectGarbage" << "\\bdeleteFileType" << "\\binclude" << "\\blistPHOTOSSOptions"
        << "\\bgetPHOTOSSOptionDescription" << "\\bgetPHOTOSSOptionValue" << "\\bsetPHOTOSSOptionValue"
        << "\\bsetPHOTOSSDefaultOptions" << "\\bnewSimulation";

        foreach (const QString &pattern, pscriptFunctionPatterns)
        {
                rule.pattern = QRegExp(pattern + "(?=\\()");
                rule.format = pscriptFunctionFormat;
                highlightingRules.append(rule);
        }

        singleLineCommentFormat.setForeground(Qt::darkGreen);
        rule.pattern = QRegExp("//[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setForeground(Qt::darkGreen);

	commentStartExpression = QRegExp("/\\*");
	commentEndExpression = QRegExp("\\*/");
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, highlightingRules)
	{
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0)
		{
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}
	setCurrentBlockState(0);

	int startIndex = 0;
	if (previousBlockState() != 1)
	startIndex = commentStartExpression.indexIn(text);

	while (startIndex >= 0) 
	{
		int endIndex = commentEndExpression.indexIn(text, startIndex);
		int commentLength;
		if (endIndex == -1)
		{
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		}
		else
		{
			commentLength = endIndex - startIndex
							+ commentEndExpression.matchedLength();
		}
	setFormat(startIndex, commentLength, multiLineCommentFormat);
	startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
	}
}
