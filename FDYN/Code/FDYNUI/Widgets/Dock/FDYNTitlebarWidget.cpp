#include "FDYNTitlebarWidget.h"
#include "FDYNDockWidget.h"
#include "ui_FDYNTitlebarWidget.h"
#include "Windows/Main/FDYNMainWindow.h"
#include "Managers/FDYNGUIManager.h"


FDYNTitlebarWidget::FDYNTitlebarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDYNTitlebarWidget)
{
    ui->setupUi(this);
}

FDYNTitlebarWidget::~FDYNTitlebarWidget()
{
    delete ui;
}

void FDYNTitlebarWidget::dock(bool val)
{
    m_DockWidget->setFloating(!val);
}

void FDYNTitlebarWidget::maximizeDock(bool val)
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
            uiStateBeforeMaxed = FDYNGUI.mainWindow()->saveState();
            foreach(QObject* obj, FDYNGUI.mainWindow()->children())
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
            FDYNGUI.mainWindow()->restoreState(uiStateBeforeMaxed);
    }
}

void FDYNTitlebarWidget::hideDock()
{
    m_DockWidget->hide();
}

void FDYNTitlebarWidget::closeDock()
{
    m_DockWidget->close();
}



