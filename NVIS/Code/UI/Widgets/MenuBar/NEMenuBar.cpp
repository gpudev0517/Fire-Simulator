#include "NEMenuBar.h"
#include "Widgets/Dock/NEDockWidget.h"
#include "Managers/NEGUIManager.h"
#include "Utilities//NEGUIStyle.h"

NEMenuBar::NEMenuBar(QWidget *parent) :
    QMenuBar(parent)
{
    m_pParentDock = 0;
    setFont(*NEGUISTYLE.NormalSmallerFont());
    //docked = true;
    moving = false;
    setMouseTracking(false);
    setNativeMenuBar( false );
}

void NEMenuBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(m_pParentDock)
        m_pParentDock->setFloating(!m_pParentDock->isFloating());
    //	NEGUI.mainWindow()->consoleWindow()->titleBarWidget();
    //	QMouseEvent* evnt = event;

    //	QWidget* parent = parentWidget();
    //	QApplication::sendEvent(parent, evnt);
    //	m_pParentDock = parentWidget()->parentWidget();
    //NEGUI.mainWindow()->consoleWindow->setFloating(!(NEGUI.mainWindow()->consoleWindow->isFloating()));
    //docked = !docked;


    //QApplication::sendEvent(NEGUI.mainWindow()->consoleWindow, event);
    //static_cast<NEDockWidget*>(parentWidget()->parentWidget())->titleBarWidget()->mouseDoubleClickEvent(event);
}

//void NEMenuBar::dragMoveEvent(QDragMoveEvent * event)
//{
//	QApplication::sendEvent(NEGUI.mainWindow()->consoleWindow, event);
//}

void NEMenuBar::mouseMoveEvent(QMouseEvent * event)
{
    QMenuBar::mouseMoveEvent(event);
    if(moving && m_pParentDock->isFloating())
        m_pParentDock->move(event->globalPos() - offset);

}

void NEMenuBar::mousePressEvent(QMouseEvent * event)
{
    QMenuBar::mousePressEvent(event);
    if((event->button() == Qt::LeftButton) && !actionAt(event->pos()))
    {
        moving = true;
        offset = event->pos();
    }

}

void NEMenuBar::mouseReleaseEvent(QMouseEvent * event)
{
    QMenuBar::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton)
    {
        moving = false;
    }
}
