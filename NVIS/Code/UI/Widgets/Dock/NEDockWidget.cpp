#include "NEDockWidget.h"
#include "Widgets/MenuBar/NEMenuBar.h"
#include "NETitlebarWidget.h"
#include "ui_NETitlebarWidget.h"
#include "NEGLWidget.h"
#include "Windows/Main/NEMainWindow.h"
#include "Managers/NEGUIManager.h"
#include "Managers/NEGUIManager_p.h"
#include "Windows/PropertyManager/NEPropertyEditorWindow.h"

NEDockWidget::NEDockWidget(QWidget *parent) :
    QDockWidget(parent)
{
    m_ContainedMainWindow = 0;
    m_Titlebar = new NETitlebarWidget(this);
    setTitleBarWidget(m_Titlebar);
    m_Titlebar->m_DockWidget = this;

    m_ResizeTimer.setParent(this);
    connect(&m_ResizeTimer, SIGNAL(timeout()), this, SLOT(resizeTimeout()));

    connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(tlChanged(bool)));
}

//NEDockWidget::~NEDockWidget()
//{
//    //delete m_Titlebar;
//}

void NEDockWidget::setMenubar(NEMenuBar *bar)
{
    //m_Titlebar->setMenubar(bar);
    m_MenuBar = bar;
    m_Titlebar->ui->horizontalLayout->insertWidget(0, bar);
    //	m_Titlebar->m_DockWidget = this;

    // Add actions in the menubar to the dock
    addActions(bar->actions());

}

void NEDockWidget::setTitleText(QString val)
{
    m_Titlebar->ui->label->setText(val);
    this->setWindowTitle(val);
}

QMainWindow* NEDockWidget::containedMainWindow()
{
    return m_ContainedMainWindow;
}

NEMenuBar* NEDockWidget::menubar()
{
    return m_MenuBar;
}

QString NEDockWidget::titleText()
{
    return 	m_Titlebar->ui->label->text();
}

void NEDockWidget::closeEvent(QCloseEvent *event)
{
    emit dockVisible(false);
    QWidget* w = NULL;
    if( m_ContainedMainWindow )
        if( m_ContainedMainWindow->centralWidget() )
            w = m_ContainedMainWindow->centralWidget()->childAt(0,0);
    if( !w )
        return;
    // Close dock containing glwidgets carefully
    if(NEGLWidget* gw = qobject_cast<NEGLWidget*>(w))
    {
        NEGUI.P->closeGLWidget(gw);
    }
    else if(NEPropertyEditorWindow* dw = qobject_cast<NEPropertyEditorWindow*>(m_ContainedMainWindow))
    {
        NEGUI.P->closePropertyEditor(dw);
    }
    else
    {
        QDockWidget::closeEvent(event);
    }
    event->accept();
}

void NEDockWidget::hideEvent(QHideEvent *event)
{
    emit dockVisible(false);
    QDockWidget::hideEvent(event);
}

void NEDockWidget::showEvent(QShowEvent *event)
{
    emit dockVisible(true);
    QDockWidget::showEvent(event);
    raise();
}

void NEDockWidget::resizeEvent(QResizeEvent *event)
{
    //    if(maximumWidth()==minimumWidth())
    //        return;

    if(!NeutrinoInitialized())
    {
        return QDockWidget::resizeEvent(event);
    }


    NEMainWindow* mw = reinterpret_cast<NEMainWindow*>( NEGUI.mainWindow() );

    Qt::DockWidgetArea dockArea = mw->dockWidgetArea(this);

    switch (dockArea)
    {
    case Qt::RightDockWidgetArea:
        mw->lockDocks(Qt::LeftDockWidgetArea);
        break;
        //    case Qt::LeftDockWidgetArea:
        //        mw->lockDocks(Qt::RightDockWidgetArea);
        //        break;
    default:
        break;
    }
    if(maximumWidth()!=minimumWidth())
        m_ResizeTimer.start(500);

    QDockWidget::resizeEvent(event);
}


void NEDockWidget::tlChanged(bool flag)
{

    if(flag)
    {
        m_Titlebar->ui->dockButton->blockSignals(true);
        m_Titlebar->ui->dockButton->setChecked(false);
        m_Titlebar->ui->dockButton->blockSignals(false);
        //this->setTitleBarWidget( NULL );
        //setMaximumSize(32000, 32000);
    }

    else
    {
        m_Titlebar->ui->dockButton->blockSignals(true);
        m_Titlebar->ui->dockButton->setChecked(true);
        m_Titlebar->ui->dockButton->blockSignals(false);
        //this->setTitleBarWidget( m_Titlebar );
        //setMaximumSize(m_DockedMaxSize);
    }

    //qApp->processEvents();
}

void NEDockWidget::resizeTimeout()
{
    if(maximumWidth()==minimumWidth())
        return;

    NEMainWindow* mw = reinterpret_cast<NEMainWindow*>( NEGUI.mainWindow() );

    Qt::DockWidgetArea dockArea = mw->dockWidgetArea(this);

    switch (dockArea)
    {
    case Qt::RightDockWidgetArea:
        mw->unlockDocks(Qt::LeftDockWidgetArea);
        break;
        //    case Qt::LeftDockWidgetArea:
        //        mw->unlockDocks(Qt::RightDockWidgetArea);
        //break;
    default:
        break;
    }
}

void NEDockWidget::setContainedMainWindow(QMainWindow *mw)
{
    setWidget(mw);
    m_ContainedMainWindow = mw;
    setMenubar(static_cast<NEMenuBar*>(mw->menuBar()));
    m_Titlebar->ui->iconLabel->setPixmap(mw->windowIcon().pixmap(QSize(16,16)));
    //m_Titlebar->ui->horizontalLayout->insertWidget(0, bar);
    setTitleText(mw->windowTitle());
    m_MenuBar->m_pParentDock = this;
    m_DockedMaxSize = maximumSize();
    //m_ContainedMainWindow->setMenuBar(0);
}
