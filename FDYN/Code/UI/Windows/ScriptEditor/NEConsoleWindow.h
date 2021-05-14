#pragma once

class PythonHighlighter;

namespace Ui {
class ConsoleWindow;
}

class NEPythonInput;

class NEConsoleWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NEConsoleWindow(QWidget *parent = 0);
    uint init();

    void executeCode(const QString& script);

    ~NEConsoleWindow();
    void reinitStyle(bool dark);

    QPlainTextEdit* outputWindow();

public slots:

    uint openScript();
    uint newScript();
    uint saveScript();
    uint saveScriptAs();
    uint clearHistory();
    void undo();
    void redo();
    void copy();
    void cut();
    void paste();

private:
    PythonHighlighter* m_pConsoleHighlighter;
    PythonHighlighter* m_pOutputHighlighter;
    PythonHighlighter* m_pPreProHighlighter;
    PythonHighlighter* m_pPosProHighlighter;

    //! Returns the active script tab
    NEPythonInput* getInputWindow();
    //! Returns the file path of the active tab
    QString& getInputFilePath();

    QString m_ActiveConsoleScriptFilePath;
    QString m_ActivePreprocessScriptFilePath;
    QString m_ActivePostprocessScriptFilePath;

    Ui::ConsoleWindow *ui;
};



