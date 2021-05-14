#include "FDYNMenuBar.h"
#include "Widgets/Dock/FDYNDockWidget.h"
#include "Managers/FDYNGUIManager.h"

FDYNMenuBar::FDYNMenuBar(QWidget *parent) :
    QMenuBar(parent)
{
    m_pParentDock = 0;
    //setFont(*NEGUISTYLE.NormalSmallerFont());
    //docked = true;
    moving = false;
    setMouseTracking(false);
}

void FDYNMenuBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(m_pParentDock)
        m_pParentDock->setFloating(!m_pParentDock->isFloating());
    //	NEGUI.mainWindow()->consoleWindow()->titleBarWidget();
}

//void NEMenuBar::dragMoveEvent(QDragMoveEvent * event)
//{
//	QApplication::sendEvent(NEGUI.mainWindow()->consoleWindow, event);
//}

void FDYNMenuBar::mouseMoveEvent(QMouseEvent * event)
{
    QMenuBar::mouseMoveEvent(event);
    if(moving && m_pParentDock->isFloating())
        m_pParentDock->move(event->globalPos() - offset);

}

void FDYNMenuBar::mousePressEvent(QMouseEvent * event)
{
    QMenuBar::mousePressEvent(event);
    if((event->button() == Qt::LeftButton) && !actionAt(event->pos()))
    {
        moving = true;
        offset = event->pos();
    }
}

void FDYNMenuBar::mouseReleaseEvent(QMouseEvent * event)
{
    QMenuBar::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton)
    {
        moving = false;
    }
}
