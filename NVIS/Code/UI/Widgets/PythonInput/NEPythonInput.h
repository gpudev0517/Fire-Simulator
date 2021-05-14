#pragma once


class QCompleter;
class NEConnectionInspector;
//class NELineNumberArea;
#include "Widgets/LineNumberArea/NELineNumberArea.h"

class NEPythonInput : public QPlainTextEdit
{
    Q_OBJECT
    friend class NELineNumberArea<NEPythonInput>;
    friend class NEConsoleWindow;
    friend class NEConnectionInspector;

public:
    NEPythonInput(QWidget* parent);
    ~NEPythonInput();

public slots:
    //! clear the console
    void clear();
    //! Executes the given string as a python script
    void executeCode(const QString& code);

protected:
    //! handle the pressing of tab
    void handleTabCompletion();
    //! Returns the position of the command prompt
    int commandPromptPosition();
    //! Sets the current font
    void setCurrentFont(const QColor& color = QColor(0,0,0), bool bold = false);
    //! change the history according to _historyPos
    void changeHistory();
    //! flush output that was not yet printed
    void flushStdOut();
    void resizeEvent(QResizeEvent* event);
    //! derived key press event
    void keyPressEvent (QKeyEvent * e);

private slots:
    //! Message output redirection
    void stdOut(const QString& s);
    //! Error output redirection
    void stdErr(const QString& s);
    //! Inserts code completion
    void insertCompletion(const QString&);

    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    bool clearAfterExecute() const;
    void setClearAfterExecute(bool val);
    void init(QPlainTextEdit* outputWin);
    void connectPythonStdOutErr();
    //! returns true if python cerr had an error
    bool hadError() { return m_HadError; }
    void clearError() { m_HadError = false; }
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    //! output the given message to the outputWindow
    void consoleMessage(const QString & message);
    //! get history
    QStringList history() { return m_History; }
    //! set history
    void setHistory(const QStringList& h) { m_History = h; m_HistoryPosition = 0; }

    QStringList m_History;
    int m_HistoryPosition;

    QString m_StdOut;
    QString m_StdErr;

    QTextCharFormat _defaultTextCharacterFormat;
    QCompleter* m_Completer;

    bool m_ClearAfterExecute;
    bool m_HadError;

    QPlainTextEdit* outputWindow;

    NELineNumberArea<NEPythonInput>* m_LineNumberArea;

};

//class LineNumberArea : public QWidget
//{
//public:
//	LineNumberArea(NEPythonInput *editor) : QWidget(editor) {
//		codeEditor = editor;
//	}

//	QSize sizeHint() const {
//		return QSize(codeEditor->lineNumberAreaWidth(), 0);
//	}

//protected:
//	void paintEvent(QPaintEvent *event) {
//		codeEditor->lineNumberAreaPaintEvent(event);
//	}

//private:
//	NEPythonInput *codeEditor;
//};
