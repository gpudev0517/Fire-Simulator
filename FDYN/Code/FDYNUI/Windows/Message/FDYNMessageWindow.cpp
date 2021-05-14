#include "FDYNMessageWindow.h"
#include "ui_FDYNMessageWindow.h"
//#include "Utilities/NEGUIStyle.h"

FDYNMessageWindow::FDYNMessageWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FDYNMessageWindow)
{
    ui->setupUi(this);
    m_Debug = 0;
    this->setBackgroundRole(QPalette::Dark);
}

FDYNMessageWindow::~FDYNMessageWindow()
{
    delete ui;
    delete m_Debug;
}

QDebug &FDYNMessageWindow::getNewDebugStream()
{
    if(m_Debug)
    {
        delete m_Debug;
        m_LogBuffer.clear();
    }
    m_Debug = new QDebug(&m_LogBuffer);
    return *m_Debug;
}

void FDYNMessageWindow::pushLogInformation(NE::NELogType type)
{
    QString strOut = "[" + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
    strOut += "]:   ";

    //if(NEGUISTYLE.darkTheme())
    {
        switch(type)
        {
        case NE::kLogInfo:
            ui->messageWidget->appendHtml("<font color=White>" + strOut + "</font> <font color=#9090DA>"+m_LogBuffer+"</font>");
            break;
        case NE::kLogWarning:
            ui->messageWidget->appendHtml("<font color=White>" + strOut + "</font> <font color=#DDDD00>"+m_LogBuffer+"</font>");
            break;
        case NE::kLogError:
            ui->messageWidget->appendHtml("<font color=White>" + strOut + "</font> <font color=#FF8080>"+m_LogBuffer+"</font>");
            break;
        default:
            return;
        }
    }
//    else
//    {
//        switch(type)
//        {
//        case NE::kLogInfo:
//            ui->messageWidget->appendHtml("<font color=Black>" + strOut + "</font> <font color=#0000BA>"+m_LogBuffer+"</font>");
//            break;
//        case NE::kLogWarning:
//            ui->messageWidget->appendHtml("<font color=Black>" + strOut + "</font> <font color=Chocolate>"+m_LogBuffer+"</font>");
//            break;
//        case NE::kLogError:
//            ui->messageWidget->appendHtml("<font color=Black>" + strOut + "</font> <font color=Red>"+m_LogBuffer+"</font>");
//            break;
//        default:
//            return;
//        }
//    }

    ui->messageWidget->verticalScrollBar()->setSliderPosition(ui->messageWidget->verticalScrollBar()->maximum());
}

void FDYNMessageWindow::saveToFile(QString path)
{
    QString messagesHTML = ui->messageWidget->document()->toHtml();
    QString dateTime = QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
    dateTime.replace('/', '-');
    dateTime.replace(':', '-');
    dateTime += ".htm";
    QFile fout(/*"\"" + */path + dateTime/* + "\""*/);
    if(!fout.open(QFile::WriteOnly))
    {
        printf("\nCannot save messages log\n");
        return;
    }

    QTextStream tout(&fout);
    tout << messagesHTML;
}

void FDYNMessageWindow::clear()
{
    ui->messageWidget->clear();
}
