#include "PythonHighlighter.h"
#include "Utilities/NEGUIStyle.h"

PythonHighlighter::PythonHighlighter(QTextDocument *parent) :
	QSyntaxHighlighter(parent)
{
    keywords << "and" << "as" << "assert" << "break" << "class" << "continue" << "def"
        << "del" << "elif" << "else" << "except" << "exec" << "finally"
        << "for" << "from" << "global" << "if" << "import" << "in"
        << "is" << "lambda" << "not" << "or" << "pass" << "print" << "raise"
        << "return" << "try" << "while" << "with" <<"yield";

    initTextStyle(NEGUISTYLE.darkTheme());
}


void
PythonHighlighter::highlightBlock(const QString &text)
{
  // Set default font for all text:
  setFormat(0, text.length(), plainFormat);

  // Apply rules
  int currentIndex = 0;
  while (currentIndex < text.length()) {
	// Find first pattern matches
	int index = text.length(), length = 0;
	const HighlightingRule *matched_rule = 0;
	foreach (const HighlightingRule &rule, rules) {
	  // Skip pattern with different block state
	  if (previousBlockState() != rule.inState) { continue; }
	  // If index matches:
	  QRegExp expr(rule.pattern);
	  if (0 > expr.indexIn(text, currentIndex)) { continue; }
	  // Check if match is next:
	  if ((index > expr.pos(rule.group)) || ((index == expr.pos(rule.group)) && (length < expr.cap(rule.group).size()))) {
		matched_rule = &rule;
		index = expr.pos(rule.group);
		length = expr.cap(rule.group).size();
	  }
	}

	// If no rule matched -> done.
	if (0 == matched_rule) { return; }

	// Apply rule:
	setFormat(index, length, matched_rule->format);
	setCurrentBlockState(matched_rule->toState);

	// Update current start index:
	currentIndex = index+length;
  }
}

void PythonHighlighter::initTextStyle(bool dark)
{
    if(dark)
    {
        plainFormat.setForeground(QColor(255,255,255));
        plainFormat.setFontWeight(QFont::Normal);
        //keywordFormat.setFont(*defaultFont);
        keywordFormat.setForeground(QColor(0x8f, 0x60, 0x8f));
        keywordFormat.setFontWeight(QFont::Bold);

        //definitionFormat.setFont(*defaultFont);
        definitionFormat.setFontWeight(QFont::Bold);
        definitionFormat.setForeground(QColor(0x80, 0x80, 0xff));

        //singleLineCommentFormat.setFont(*defaultFont);
        singleLineCommentFormat.setForeground(QColor(0x00, 0xFF, 0x00));
        singleLineCommentFormat.setFontItalic(true);

        //quotationFormat.setFont(*defaultFont);
        //quotationFormat.setForeground(QColor(0x00, 0x55, 0x00));
        quotationFormat.setForeground(QColor(0xff, 0x80, 0x80));

        //numberFormat.setFont(*defaultFont);
        numberFormat.setForeground(QColor(0xFF, 0x80, 0x40));

        //operatorFormat.setFont(*defaultFont);
        operatorFormat.setForeground(QColor(0x65, 0xFF, 0x20));

        functionFormat.setForeground(QColor(0xA0, 0xA0, 0xFF));

        tupleFormat.setForeground(QColor(0x7f, 0xff, 0x7f));

    }
    else
    {
        plainFormat.setForeground(QColor(0,0,0));
        plainFormat.setFontWeight(QFont::Normal);
        //keywordFormat.setFont(*defaultFont);
        keywordFormat.setForeground(QColor(0x00, 0x00, 0x8f));
        keywordFormat.setFontWeight(QFont::Bold);

        //definitionFormat.setFont(*defaultFont);
        definitionFormat.setFontWeight(QFont::Bold);
        definitionFormat.setForeground(QColor(0x7F, 0x7F, 0x00));

        //singleLineCommentFormat.setFont(*defaultFont);
        singleLineCommentFormat.setForeground(QColor(0x00, 0x80, 0x00));
        singleLineCommentFormat.setFontItalic(true);

        //quotationFormat.setFont(*defaultFont);
        //quotationFormat.setForeground(QColor(0x00, 0x55, 0x00));
        quotationFormat.setForeground(QColor(0xff, 0x00, 0x00));

        //numberFormat.setFont(*defaultFont);
        numberFormat.setForeground(QColor(0xDF, 0x5A, 0x00));

        //operatorFormat.setFont(*defaultFont);
        operatorFormat.setForeground(QColor(0x9A, 0x00, 0xDF));

        functionFormat.setForeground(QColor(0x00, 0x00, 0x8f));

        tupleFormat.setForeground(QColor(0x80, 0x00, 0x80));
    }
    rules.clear();


    /* Assemble rules */
    // Keywords
    rules.append(HighlightingRule(QRegExp("\\b(" + keywords.join("|") + ")\\b"), keywordFormat));
    // Numbers
    rules.append(HighlightingRule(QRegExp("\\b\\d+\\b"), numberFormat));
    // Operators
    rules.append(HighlightingRule(QRegExp("[\\\\|\\<|\\>|\\=|\\!|\\+|\\-|\\*|\\/|\\%]"), operatorFormat));
    // Comments
    rules.append(HighlightingRule(QRegExp("#.*$"), singleLineCommentFormat));
    // Single line strings:
    rules.append(HighlightingRule(QRegExp("\"[^\"]*\""), quotationFormat));
    rules.append(HighlightingRule(QRegExp("'[^']*'"), quotationFormat));
    // Functions
    rules.append(HighlightingRule(QRegExp("\\b[A-Za-z0-9_]+(?=\\()"), functionFormat));

    rules.append(HighlightingRule(QRegExp("(\\((-?\\d+(\\.\\d*)?,[ ]*)+-?\\d+(\\.\\d*)?\\))|(\\[(-?\\d+(\\.\\d*)?,[ ]*)+-?\\d+(\\.\\d*)?\\])"), tupleFormat));


    // Multiline strings
    /*rules.append(
          HighlightingRule(
            QRegExp("\"[^\"]*$"), quotationFormat, 0,
            HighlightingRule::DEFAULT, HighlightingRule::QUOTATION_CONTINUATION));
    rules.append(
          HighlightingRule(
            QRegExp("^[^\"]*\""), quotationFormat, 0,
            HighlightingRule::QUOTATION_CONTINUATION, HighlightingRule::DEFAULT));
    rules.append(
          HighlightingRule(
          QRegExp("^[^\"]*$"), quotationFormat, 0,
            HighlightingRule::QUOTATION_CONTINUATION, HighlightingRule::QUOTATION_CONTINUATION));*/

}
