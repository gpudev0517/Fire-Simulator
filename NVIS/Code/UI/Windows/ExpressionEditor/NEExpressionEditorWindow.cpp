#include "Base/NEBase.h"
#include "NENode.h"
#include "NEExpressionEditorWindow.h"
#include "ui_NEExpressionEditorWindow.h"
#include "Windows/ScriptEditor/NEConsoleWindow.h"
#include "Managers/NEGUIManager.h"
#include "Utilities/NEGUIStyle.h"
#include "Widgets/PythonInput/PythonHighlighter/PythonHighlighter.h"

NEExpressionEditorWindow::NEExpressionEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NEExpressionEditorWindow)
{
    m_Node = 0;
    ui->setupUi(this);

    connect(ui->actionSave_to_file, &QAction::triggered, this, &NEExpressionEditorWindow::saveNodePropertyExpressionToAFile);
    connect(ui->actionLoad_from_file, &QAction::triggered, this, &NEExpressionEditorWindow::loadNodePropertyExpressionFromAFile);
}

uint NEExpressionEditorWindow::init()
{
    expEditor()->init(NEGUI.consoleWindow()->outputWindow());
    expEditor()->setFont(*NEGUISTYLE.MonoNormalFont());

    m_pHighlighter = new PythonHighlighter(expEditor()->document());

    return NE::kReturnSuccess;
}

NEExpressionEditorWindow::~NEExpressionEditorWindow()
{
    delete ui;
}

void NEExpressionEditorWindow::showNodePropertyExpression(const NENode *val, const QString prop)
{
    expEditor()->clear();
    m_Node = const_cast<NENode*>(val);
    m_Property = prop;
    if(m_Node->hasUpdateExpression(prop))
        expEditor()->appendPlainText(m_Node->propertyUpdateExpression(prop));
}

void NEExpressionEditorWindow::saveNodePropertyExpression()
{
    if(!m_Node)
        return;
    QString txt = expEditor()->toPlainText();
    if(txt.isEmpty())
        m_Node->removePropertyUpdateExpression(m_Property);
    else
        m_Node->setPropertyUpdateExpression(m_Property, txt);
}

uint NEExpressionEditorWindow::saveNodePropertyExpressionToAFile()
{
    QString saveAsStr = QFileDialog::getSaveFileName( reinterpret_cast< QMainWindow* >( NEGUI.mainWindow()),
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
        out << expEditor()->toPlainText();
        file.close();
        return NE::kReturnSuccess;
    }
    return NE::kReturnCannotSaveFile;
}

uint NEExpressionEditorWindow::loadNodePropertyExpressionFromAFile()
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
        NEExpressionEditor* pyInput = expEditor();
        pyInput->clear();
        pyInput->appendPlainText(fileContents);
        pyInput->document()->setModified(false);
        pyInput->scroll(0,0);
        file.close();
        return NE::kReturnSuccess;
    }
    return NE::kReturnCannotOpenFile;
}

NEExpressionEditor *NEExpressionEditorWindow::expEditor()
{
    return ui->expressionEditor;
}

void NEExpressionEditorWindow::reinitStyle(bool dark)
{
    m_pHighlighter->initTextStyle(dark);
}
