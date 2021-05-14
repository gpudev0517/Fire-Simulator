#include "NEConnection.h"
#include "NEPort.h"

NEConnection::PathCurveType NEConnection::m_PathCurveType = NEConnection::Cubic;

NEConnection::NEConnection(QGraphicsItem *parent) : QGraphicsPathItem(parent)
{
    setPen(QPen(Qt::gray, 2));
    //    setBrush(Qt::NoBrush);
    setBrush(Qt::gray);
    setZValue(-1);
    m_port1 = 0;
    m_port2 = 0;
    arrowSize = 12;

    m_order = 0;
    orderLabel = new QGraphicsTextItem(this);
    orderLabel->setDefaultTextColor(Qt::gray);
    //    orderLabel->setTextInteractionFlags(Qt::TextEditorInteraction);

    //    setFlag(QGraphicsItem::ItemIsFocusable);
}

NEConnection::~NEConnection()
{
    if (m_port1 != NULL)
        m_port1->connections().remove(m_port1->connections().indexOf(this));
    if (m_port2 != NULL)
        m_port2->connections().remove(m_port2->connections().indexOf(this));
}

void NEConnection::setPos1(const QPointF &p)
{
    pos1 = p;
}

void NEConnection::setPos2(const QPointF &p)
{
    pos2 = p;
}

void NEConnection::setPort1(NEPort *p)
{
    m_port1 = p;

    if (m_port1 != NULL)
        m_port1->connections().append(this);
}

void NEConnection::setPort2(NEPort *p)
{
    m_port2 = p;

    if (m_port2 != NULL)
        m_port2->connections().append(this);
}

void NEConnection::updatePosFromPorts()
{
    if (m_port1 != NULL && m_port2 != NULL)
    {
        pos1 = m_port1->scenePos();
        pos2 = m_port2->scenePos();
    } else {
        pos1 = QPointF(0, 0);
        pos2 = pos1;
    }
}

void NEConnection::updateOrderLabel()
{
    QLineF line( pos1, pos2 );
    if (qFuzzyCompare(1+line.length(), qreal(1)) || m_port1 == NULL || m_port2 == NULL || getCurveType() == Quadratic)
        orderLabel->setPlainText("");
    else
        orderLabel->setPlainText(QString::number(m_order));
}


void NEConnection::updatePath()
{
    updateOrderLabel();
    setPath(QPainterPath());
    int chkRadius = 0;

    if(m_port1 != NULL)
        chkRadius += m_port1->radius();
    if(m_port2 != NULL )
        chkRadius += m_port2->radius();
    QLineF line( pos1, pos2 );
    if (line.length() < chkRadius)
        return;

    qreal dx = pos2.x() - pos1.x();
    qreal dy = pos2.y() - pos1.y();

#ifdef _SIMPLE_CUBIC_PATH
    QPainterPath p;

    p.moveTo(pos1);

    QPointF ctr1(pos1.x() + dx * 0.25, pos1.y() + dy * 0.1);
    QPointF ctr2(pos1.x() + dx * 0.75, pos1.y() + dy * 0.9);

    p.cubicTo(ctr1, ctr2, pos2);

    setPath(p);
#else
    if (line.length() < chkRadius + 30)
    {
        linearPath( pos1 , pos2 , m_Path );
        setPath(m_Path);
    }
    else {
        // set order label pos
        QPointF labelPos;
        if (dx > 0 &&line.length() < 600)
        {
            dx = MAX(MIN(dx * 0.1, 80), 40);
            if (dy < 0) dy = MAX(dy * 0.2, -60) + 25;
            else dy = MIN(dy * 0.2, 60) + 25;
            //            if (getCurveType() == Quadratic)
            //                dy -= 20;
            labelPos = QPointF(pos2.x() - dx, pos2.y() - dy);
        } else {
            dx = dx * 0.6;
            dy = dy * 0.6;
            labelPos = QPointF(pos1.x() + dx, pos1.y() + dy);
        }
        orderLabel->setPos(labelPos);

        double angle = ::acos(line.dx() / line.length());
        if (line.dy() >= 0)
            angle = 2*M_PI - angle;
        QPointF newPos2 = pos2;

        switch(getCurveType())
        {
        case Linear:
            if (m_port2)
                newPos2 -= QPointF(cos(angle) * m_port2->radius(),
                                   - sin(angle) * m_port2->radius());
            linearPath( pos1 , newPos2 , m_Path );
            break;
        case Quadratic:
            quadraticPath( pos1, newPos2, m_Path );
            break;
        case Cubic: default:
            if (m_port2)
                newPos2 -= QPointF(cos(angle) * m_port2->radius(),
                                   - sin(angle) * m_port2->radius());
            cubicPath( pos1, newPos2, m_Path );
            break;
        }
        if (getCurveType() != Quadratic)
        {
            QPointF destArrowP1 = newPos2 + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                                    cos(angle - M_PI / 3) * arrowSize);
            QPointF destArrowP2 = newPos2 + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                                    cos(angle - M_PI + M_PI / 3) * arrowSize);

            m_Path.addPolygon(QPolygonF() << newPos2 << destArrowP1 << destArrowP2);
        }
        setPath(m_Path);
    }
#endif
}

NEPort* NEConnection::port1() const
{
    return m_port1;
}

NEPort* NEConnection::port2() const
{
    return m_port2;
}

void NEConnection::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
//    if( m_port1 == NULL || m_port2 == NULL )
//        return;

    //    std::cout << "Paint Edge: " << pos1.x() << " " << pos1.y() << " To " << pos2.x() << " " <<pos2.y() << std::endl;

    //    setBrush(Qt::gray);
    NE::IoTypes m_type = NE::kIONone;
    if( m_port1 != NULL )
    {
        int enumIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
        QMetaEnum ioEnum = NE::Instance().metaObject()->enumerator(enumIndex);
        std::string charName = m_port1->portName().toStdString();
        m_type = (NE::IoTypes)ioEnum.keyToValue(charName.c_str());
        switch (m_type)
        {
        case NE::kIOParticles:
            painter->setPen(QPen(Qt::green, 4));
            break;
        case NE::kIOTriangleMesh:
            painter->setPen(QPen(Qt::darkGreen, 4));
            break;
        case NE::kIOForceField:
            painter->setPen(QPen(Qt::red, 4));
            break;
        case NE::kIOExtent:
            painter->setPen(QPen(Qt::blue, 4));
            break;
        case NE::kIOVolume:
            painter->setPen(QPen(Qt::magenta, 4));
            break;
        case NE::kIOFlowRate:
            painter->setPen(QPen(Qt::cyan, 4));
            break;
        case NE::kIOFluidHeight:
            painter->setPen(QPen(Qt::yellow, 4));
            break;
        case NE::kIONumParticles:
            painter->setPen(QPen(Qt::darkMagenta, 4));
            break;
        case NE::kIONumParticlesDrain:
            painter->setPen(QPen(Qt::darkYellow, 4));
            break;
        case NE::kIOHydrostaticPressure:
            painter->setPen(QPen(Qt::darkCyan, 4));
            break;
        case NE::kIOFluid:
            painter->setPen(QPen(Qt::lightGray, 4));
            break;
        case NE::kIORigid:
            painter->setPen(QPen(Qt::darkRed, 4));
            break;
        case NE::kIOMeasurement:
            painter->setPen(QPen(Qt::darkGray, 4));
            break;
        case NE::kIOGizmo:
            painter->setPen(QPen(Qt::darkBlue, 4));
            break;
        default:
            painter->setPen(QPen(Qt::gray, 4));
            break;
        }
    }
    painter->drawPath( m_Path );

    // Draw the arrows
    QLineF line( pos1, pos2 );
    if (line.length() < 20)
        return;
    if (getCurveType() == Quadratic)
        return;

    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = 2*M_PI - angle;
    QPointF newPos2 = pos2;
    if (m_port2)
        newPos2 -= QPointF(cos(angle) * m_port2->radius(), - sin(angle) * m_port2->radius());
    QPointF destArrowP1 = newPos2 + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                            cos(angle - M_PI / 3) * arrowSize);
    QPointF destArrowP2 = newPos2 + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                            cos(angle - M_PI + M_PI / 3) * arrowSize);

    //        painter->setBrush(Qt::red);
    //        painter->drawPolygon( QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2 );

    //    setBrush(Qt::gray);
    //    painter->drawPolygon( QPolygonF() << line.p2() << destArrowP1 << destArrowP2 );
    QPainterPath ArrowPath;
    ArrowPath.addPolygon(QPolygonF() << newPos2 << destArrowP1 << destArrowP2);
    switch (m_type)
    {
    case NE::kIOParticles:
        painter->fillPath(ArrowPath, Qt::green);
        break;
    case NE::kIOTriangleMesh:
        painter->fillPath(ArrowPath, Qt::darkGreen);
        break;
    case NE::kIOForceField:
        painter->fillPath(ArrowPath, Qt::red);
        break;
    case NE::kIOExtent:
        painter->fillPath(ArrowPath, Qt::blue);
        break;
    case NE::kIOVolume:
        painter->fillPath(ArrowPath, Qt::magenta);
        break;
    case NE::kIOFlowRate:
        painter->fillPath(ArrowPath, Qt::cyan);
        break;
    case NE::kIOFluidHeight:
        painter->fillPath(ArrowPath, Qt::yellow);
        break;
    case NE::kIONumParticles:
        painter->fillPath(ArrowPath, Qt::darkMagenta);
        break;
    case NE::kIONumParticlesDrain:
        painter->fillPath(ArrowPath, Qt::darkYellow);
        break;
    case NE::kIOHydrostaticPressure:
        painter->fillPath(ArrowPath, Qt::darkCyan);
        break;
    case NE::kIOFluid:
        painter->fillPath(ArrowPath, Qt::lightGray);
        break;
    case NE::kIORigid:
        painter->fillPath(ArrowPath, Qt::darkRed);
        break;
    case NE::kIOMeasurement:
        painter->fillPath(ArrowPath, Qt::darkGray);
        break;
    case NE::kIOGizmo:
        painter->fillPath(ArrowPath, Qt::darkBlue);
        break;
    default:
        painter->fillPath(ArrowPath, Qt::gray);
        break;
    }
}

void NEConnection::linearPath(const QPointF &start, const QPointF &end, QPainterPath &path)
{
    path = QPainterPath(start);
    path.lineTo(end);
}

void NEConnection::cubicPath(const QPointF &start, const QPointF &end, QPainterPath &path)
{
    //    QPointF center = (end + start)*0.5f;
    //    QPointF clCenter = (center + start)*0.5f;
    //    QPointF frCenter = (center + end)*0.5f;
    //    QPointF diff1 = clCenter - center;
    //    QPointF diff2 = frCenter - center;

    //    QPointF clCenterCP(-diff1.y(), diff1.x());
    //    clCenterCP = clCenterCP + clCenter;
    //    QPointF frCenterCP(-diff2.y(), diff2.x());
    //    frCenterCP = frCenterCP + frCenter;

    //    path = QPainterPath(start);
    //    path.cubicTo(clCenter, clCenterCP, center);
    //    path.cubicTo(frCenterCP, frCenter, end);

    path = QPainterPath(start);

    qreal dx = end.x() - start.x();
    qreal dy = end.y() - start.y();

    QPointF ctr1(start.x() + dx * 0.25, start.y() + dy * 0.1);
    QPointF ctr2(start.x() + dx * 0.75, start.y() + dy * 0.9);

    path.cubicTo(ctr1, ctr2, end);
}

void NEConnection::quadraticPath(const QPointF &start, const QPointF &end, QPainterPath &path)
{
    QPointF center = (end + start)*0.5f;
    QPointF clCenter = (center + start)*0.5f;
    QPointF frCenter = (center + end)*0.5f;
    QPointF diff1 = clCenter - center;
    QPointF diff2 = frCenter - center;

    QPointF clCenterCP(-diff1.y(), diff1.x());
    clCenterCP = clCenterCP + clCenter;
    QPointF frCenterCP(-diff2.y(), diff2.x());
    frCenterCP = frCenterCP + frCenter;

    path = QPainterPath(start);
    path.quadTo(clCenterCP, center);
    path.quadTo(frCenterCP, end);
}

QVariant NEConnection::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemScenePositionHasChanged)
    {
        //        foreach(NEConnection *conn, m_connections)
        //        {
        //            conn->updatePosFromPorts();
        //            conn->updatePath();
        //        }
    }
    return value;
}
