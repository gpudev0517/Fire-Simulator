#pragma once


class HighlightingRule
{
public:
    typedef enum {
        DEFAULT = -1,
        QUOTATION_CONTINUATION = 1
    } State;

public:
    QRegExp pattern;
    QTextCharFormat format;
    int group;
    /** If this pattern matches, the current block gets this state. */
    State toState;
    /** Matches only if the last block is in state. */
    State inState;

    HighlightingRule(const QRegExp &p, const QTextCharFormat &f, int g=0, State is=DEFAULT, State ts=DEFAULT)
        : pattern(p), format(f), group(g), toState(ts), inState(is)
    {
        // Pass...
    }

    HighlightingRule(const HighlightingRule &other)
        : pattern(other.pattern), format(other.format), group(other.group), toState(other.toState), inState(other.inState)
    {
        // Pass...
    }
};


class PythonHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    friend class NEConsoleWindow;
    friend class NEExpressionEditorWindow;

protected:
    QLinkedList<HighlightingRule> rules;

    //QFont defaultFont;
    QTextCharFormat plainFormat;
    QTextCharFormat keywordFormat;
    QTextCharFormat definitionFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat operatorFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat tupleFormat;


public:
    explicit PythonHighlighter(QTextDocument *parent = 0);


protected:
    virtual void highlightBlock(const QString &text);


signals:
public slots:

private:
    void initTextStyle(bool dark);

    QStringList keywords;
};
