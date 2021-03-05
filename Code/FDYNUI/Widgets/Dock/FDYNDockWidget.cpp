#include "FDYNDockWidget.h"
#include "ui_FDYNTitlebarWidget.h"
#include "FDYNTitlebarWidget.h"

#include "NEGLWidget.h"

#include "Windows/Main/FDYNMainWindow.h"
#include "Managers/FDYNGUIManager.h"
#include "Managers/FDYNGUIManager_p.h"
#include "Widgets/MenuBar/FDYNMenuBar.h"
#include "Windows/PropertyManager/FDYNPropertyEditorWindow.h"

FDYNDockWidget::FDYNDockWidget(QWidget *parent): QDockWidget(parent)
{

    m_ContainedMainWindow = 0;
//    m_view = new QQuickWidget( this );
//    m_view->setResizeMode(QQuickWidget::SizeRootObjectToView);

    m_ContainedMainWindow = 0;
    m_Titlebar = new FDYNTitlebarWidget(this);
    setTitleBarWidget(m_Titlebar);
    m_Titlebar->m_DockWidget = this;

    m_ResizeTimer.setParent(this);
    QObject::connect(&m_ResizeTimer, SIGNAL(timeout()), this, SLOT(resizeTimeout()));
    QObject::connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(tlChanged(bool)));

    QString style =
            "QToolButton {"
           "background-color: transparent;"
           "border-width: 2px;"
           "margin: 2px;"
         "}";
    this->setStyleSheet(style);

}

FDYNDockWidget::~FDYNDockWidget()
{
    delete m_Titlebar;
}


void FDYNDockWidget::setMenubar(FDYNMenuBar *bar)
{
    //m_Titlebar->setMenubar(bar);
    m_MenuBar = bar;
    m_Titlebar->ui->horizontalLayout->insertWidget(0, bar);
    //	m_Titlebar->m_DockWidget = this;

    // Add actions in the menubar to the dock
    addActions(bar->actions());

}

void FDYNDockWidget::setTitleText(QString val)
{
    m_Titlebar->ui->label->setText(val);
    QFont f = m_Titlebar->ui->label->font();
    f.setFamily("Segoe UI");
    f.setPointSize(8);
    f.setBold(true);
    m_Titlebar->ui->label->setFont( f );
    this->setWindowTitle(val);
}

QMainWindow* FDYNDockWidget::containedMainWindow()
{
    return m_ContainedMainWindow;
}

FDYNMenuBar* FDYNDockWidget::menubar()
{
    return m_MenuBar;
}

QString FDYNDockWidget::titleText()
{
    return 	m_Titlebar->ui->label->text();
    //return QString ("");
}


void FDYNDockWidget::closeEvent(QCloseEvent *event)
{
    QWidget* w = NULL;
    if( m_ContainedMainWindow )
        if( m_ContainedMainWindow->centralWidget() )
            w = m_ContainedMainWindow->centralWidget()->findChild<NEGLWidget *>();
    if( !w )
        return;
    // Close dock containing glwidgets carefully
    if(NEGLWidget* gw = qobject_cast<NEGLWidget*>(w))
    {
        FDYNGUI.P->closeGLWidget(gw);
    }
    else if(FDYNPropertyEditorWindow* dw = qobject_cast<FDYNPropertyEditorWindow*>(m_ContainedMainWindow))
    {
        FDYNGUI.P->closePropertyEditor(dw);
        emit dockVisible(false);
    }
    else
    {
        QDockWidget::closeEvent(event);
        emit dockVisible(false);
    }
    event->accept();
}

void FDYNDockWidget::hideEvent(QHideEvent *event)
{
    QDockWidget::hideEvent(event);
    emit dockVisible(false);
}

void FDYNDockWidget::showEvent(QShowEvent *event)
{
    QDockWidget::showEvent(event);
    raise();
    emit dockVisible(true);
}

void FDYNDockWidget::resizeEvent(QResizeEvent *event)
{
    //    if(maximumWidth()==minimumWidth())
    //        return;

    if(!NeutrinoInitialized())
    {
        return QDockWidget::resizeEvent(event);
    }


    FDYNMainWindow* mw = reinterpret_cast<FDYNMainWindow*>( FDYNGUI.mainWindow() );

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


void FDYNDockWidget::tlChanged(bool flag)
{
    //qApp->processEvents();
}

void FDYNDockWidget::resizeTimeout()
{
    if(maximumWidth()==minimumWidth())
        return;

    FDYNMainWindow* mw = reinterpret_cast<FDYNMainWindow*>( FDYNGUI.mainWindow() );

    Qt::DockWidgetArea dockArea = mw->dockWidgetArea(this);

    switch (dockArea)
    {
    case Qt::RightDockWidgetArea:
        mw->unlockDocks(Qt::LeftDockWidgetArea);
        break;
    default:
        break;
    }
}

void FDYNDockWidget::setContainedMainWindow(QMainWindow *mw)
{
    setWidget(mw);
    m_ContainedMainWindow = mw;
#if 1
    setMenubar(static_cast<FDYNMenuBar*>(mw->menuBar()));
#endif
    m_Titlebar->ui->iconLabel->setPixmap(mw->windowIcon().pixmap(QSize(16,16)));
    setTitleText(mw->windowTitle());
    m_MenuBar->m_pParentDock = this;
    m_DockedMaxSize = maximumSize();
}
