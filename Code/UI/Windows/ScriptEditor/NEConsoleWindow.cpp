#include "Managers/NEGUIManager.h"
#include "Base/NEBase.h"
#include "NEConsoleWindow.h"
#include "Base/NESceneManager.h"
#include "ui_NEConsoleWindow.h"
#include "Widgets/PythonInput/PythonHighlighter/PythonHighlighter.h"
#include "Utilities/NEGUIStyle.h"

NEConsoleWindow::NEConsoleWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConsoleWindow)
{
    ui->setupUi(this);

    connect(ui->actionNew, &QAction::triggered, this, &NEConsoleWindow::newScript);
    connect(ui->actionSave, &QAction::triggered, this, &NEConsoleWindow::saveScript);
    connect(ui->actionSaveAs, &QAction::triggered, this, &NEConsoleWindow::saveScriptAs);
}

uint NEConsoleWindow::init()
{
    ui->pythonConsoleInput->init(ui->pythonOutput);	// Initialize the Console widget
    ui->pythonConsoleInput->connectPythonStdOutErr();
    ui->pythonConsoleInput->setClearAfterExecute(true);

    ui->pythonPreprocess->init(ui->pythonOutput);
    ui->pythonPostprocess->init(ui->pythonOutput);

    ui->pythonConsoleInput->setFont(*NEGUISTYLE.MonoNormalFont());
    ui->pythonOutput->setFont(*NEGUISTYLE.MonoNormalFont());
    ui->pythonPreprocess->setFont(*NEGUISTYLE.MonoNormalFont());
    ui->pythonPostprocess->setFont(*NEGUISTYLE.MonoNormalFont());

    m_pConsoleHighlighter = new PythonHighlighter(ui->pythonConsoleInput->document());
    m_pOutputHighlighter = new PythonHighlighter(ui->pythonOutput->document());
    m_pPreProHighlighter = new PythonHighlighter(ui->pythonPreprocess->document());
    m_pPosProHighlighter = new PythonHighlighter(ui->pythonPostprocess->document());

    return NE::kReturnSuccess;
}

void NEConsoleWindow::executeCode(const QString &script)
{
    ui->pythonConsoleInput->executeCode(script);
    NESCENE.cleanup();
}

NEConsoleWindow::~NEConsoleWindow()
{
    delete ui;
}

void NEConsoleWindow::reinitStyle(bool dark)
{
    m_pConsoleHighlighter->initTextStyle(dark);
    m_pOutputHighlighter->initTextStyle(dark);
    m_pPreProHighlighter->initTextStyle(dark);
    m_pPosProHighlighter->initTextStyle(dark);
}

QPlainTextEdit *NEConsoleWindow::outputWindow()
{
    return ui->pythonOutput;
}

NEPythonInput* NEConsoleWindow::getInputWindow()
{
    switch(ui->tabWidget->currentIndex())
    {
    case 0:	return ui->pythonConsoleInput;
    case 1:	return ui->pythonPreprocess;
    case 2:	return ui->pythonPostprocess;
    default:return 0;
    }
}

QString &NEConsoleWindow::getInputFilePath()
{
    switch(ui->tabWidget->currentIndex())
    {
    case 0:	return m_ActiveConsoleScriptFilePath;
    case 1:	return m_ActivePreprocessScriptFilePath;
    case 2:	return m_ActivePostprocessScriptFilePath;
    default:return m_ActiveConsoleScriptFilePath;
    }
}


uint NEConsoleWindow::openScript()
{
    QString openStr = QFileDialog::getOpenFileName( reinterpret_cast< QMainWindow* >( NEGUI.mainWindow()),
                                                    "Open script file", ".", "Python Script (*.py)");

    if(!openStr.isEmpty())
    {
        QFile file(openStr);
        if(!file.open(QIODevice::ReadOnly))
        {
            Log() << "Cannot load python script." << openStr; EndLog(NE::kLogError);
            return NE::kReturnCannotOpenFile;
        }

        QTextStream in(&file);
        QString fileContents = in.readAll();
        NEPythonInput* pyInput = getInputWindow();
        pyInput->clear();
        pyInput->appendPlainText(fileContents);
        pyInput->document()->setModified(false);
        pyInput->scroll(0,0);
        getInputFilePath() = openStr;
        file.close();
        return NE::kReturnSuccess;
    }
    return NE::kReturnCannotOpenFile;
}

uint NEConsoleWindow::newScript()
{
    if(getInputWindow()->document()->isModified())
    {
        int choice = QMessageBox::question(this, "", "Do you want to save the changes?", QMessageBox::Yes, QMessageBox::No);

        if(choice == QMessageBox::Yes)
        {
            if(getInputFilePath().size())
                saveScript();
            else
                saveScriptAs();
        }
    }
    getInputFilePath().clear();
    getInputWindow()->clear();

    return NE::kReturnSuccess;
}

uint NEConsoleWindow::saveScript()
{
    QString scriptPath = getInputFilePath();
    if(scriptPath.isEmpty())
        saveScriptAs();
    else
    {
        QFile file(scriptPath);
        if(!file.open(QIODevice::WriteOnly))
        {
            Log() << "Cannot save python script." << scriptPath; EndLog(NE::kLogError);
            return NE::kReturnCannotSaveFile;
        }
        QTextStream out(&file);
        out << getInputWindow()->toPlainText();
        file.close();
        return NE::kReturnSuccess;
    }
    return NE::kReturnCannotSaveFile;
}

uint NEConsoleWindow::saveScriptAs()
{
    QString saveAsStr = QFileDialog::getSaveFileName(reinterpret_cast< QMainWindow* >( NEGUI.mainWindow()),
                                                     "Save script file", ".", "Python Script (*.py)");
    if(!saveAsStr.isEmpty())
    {
        QFile file(saveAsStr);
        if(!file.open(QIODevice::WriteOnly))
        {
            Log() << "Cannot save python script." << saveAsStr; EndLog(NE::kLogError);
            return NE::kReturnCannotSaveFile;
        }
        QTextStream out(&file);
        out << getInputWindow()->toPlainText();
        getInputFilePath() = saveAsStr;
        file.close();
        return NE::kReturnSuccess;
    }
    return NE::kReturnCannotSaveFile;
}

uint NEConsoleWindow::clearHistory()
{
    ui->pythonOutput->clear();
    return NE::kReturnSuccess;
}

void NEConsoleWindow::undo()
{
    getInputWindow()->undo();
}

void NEConsoleWindow::redo()
{
    getInputWindow()->redo();
}

void NEConsoleWindow::copy()
{
    getInputWindow()->copy();
}

void NEConsoleWindow::cut()
{
    getInputWindow()->cut();
}

void NEConsoleWindow::paste()
{
    getInputWindow()->paste();
}
