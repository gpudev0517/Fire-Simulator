#include "NETitlebarWidget.h"
#include "ui_NETitlebarWidget.h"
#include "Widgets/MenuBar/NEMenuBar.h"
#include "NEDockWidget.h"
#include "Windows/Main/NEMainWindow.h"
#include "Managers/NEGUIManager.h"


NETitlebarWidget::NETitlebarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NETitlebarWidget)
{
    ui->setupUi(this);


}

NETitlebarWidget::~NETitlebarWidget()
{
    delete ui;
}

void NETitlebarWidget::dock(bool val)
{
    m_DockWidget->setFloating(!val);
}

void NETitlebarWidget::maximizeDock(bool val)
{
    if(m_DockWidget->isFloating())
    {
        if(val)
            m_DockWidget->showMaximized();
        else
            m_DockWidget->showNormal();
    }
    else
    {
        if(val)
        {
            uiStateBeforeMaxed = NEGUI.mainWindow()->saveState();
            foreach(QObject* obj, NEGUI.mainWindow()->children())
            {
                QDockWidget* dock = qobject_cast<QDockWidget*>(obj);
                if(dock && dock != m_DockWidget)
                {
                    if(dock->isVisible())
                    {
                        dock->hide();
                        //listOfHiddenWhenMaxed.push_back(dock);
                    }
                }
            }
        }
        else
            NEGUI.mainWindow()->restoreState(uiStateBeforeMaxed);
    }
}

void NETitlebarWidget::hideDock()
{
    m_DockWidget->hide();
}

void NETitlebarWidget::closeDock()
{
    m_DockWidget->close();



}



