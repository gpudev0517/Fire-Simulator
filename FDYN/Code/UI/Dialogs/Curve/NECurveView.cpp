#include "Dialogs//Curve/NECurveView.h"

NECurveView::NECurveView(QWidget *parent) : QGraphicsView(parent)
{
    this->setMouseTracking(true);
    m_mouseMode = kNone;
}

NECurveView::~NECurveView()
{
}

void NECurveView::mousePressEvent(QMouseEvent *event)
{


    if(event->modifiers() == Qt::AltModifier)
    {
        switch(event->button())
        {
        case Qt::RightButton:
            m_mouseMode = kZoom;
            break;
        case Qt::MiddleButton:
            m_mouseMode = kPan;
            break;
        default:break;
        }
        m_mousePositionOld = event->pos();
    }
    else
    {
        QGraphicsView::mousePressEvent(event);
    }

}

void NECurveView::mouseMoveEvent(QMouseEvent *event) 
{
    emit scenePosChanged(mapToScene(event->pos()));

    QGraphicsView::mouseMoveEvent(event);

    const QPointF delta = mapToScene(event->pos()) - mapToScene(m_mousePositionOld);
    m_mousePositionOld = event->pos();

    if (this->scene() == NULL)
        return;

    QRectF rectF = sceneRect();
    if(m_mouseMode == kPan)
    {
        this->scene()->setSceneRect(rectF.x() - delta.x(), rectF.y() - delta.y(),
                                    rectF.width(), rectF.height());
    }

    if(m_mouseMode == kZoom)
    {
        this->scene()->setSceneRect(rectF.x() + (delta.x()/2.0), rectF.y() + (delta.y()/2.0),
                                    rectF.width() - delta.x(), rectF.height() - delta.y());
        this->fitInView(sceneRect());
    }
}

void NECurveView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);

    m_mouseMode = kNone;
}


void NECurveView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    this->fitInView(sceneRect());
}
