#include "NEConnectionGEItem.h"

NEConnectionGEItem::CurveType NEConnectionGEItem::m_CurveType = NEConnectionGEItem::Quadratic;


NEConnectionGEItem::NEConnectionGEItem( NENodeGraphEditorItem* source, NENodeGraphEditorItem* target, QGraphicsItem *parent )
    :QGraphicsItem(parent), arrowSize( 10 )
{
    m_pSource = source;
    m_pTarget = target;
    m_CurveType = Linear;
    setZValue(-1);
    adjust();
}

QRectF NEConnectionGEItem::boundingRect() const
{
    if ( !m_pSource || !m_pTarget )
        return QRectF();

    qreal penWidth = 1;
    qreal extra = ( penWidth + arrowSize ) / 2.0;

    //return QRectF( QPointF( -1000, -1000 ), QSizeF( 2000, 2000 ) );

    return QRectF(m_SourcePoint, QSizeF( m_TargetPoint.x() - m_SourcePoint.x(), m_TargetPoint.y() - m_SourcePoint.y()))
            .normalized()
            .adjusted(-extra, -extra, extra, extra);
}

void NEConnectionGEItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    if( !m_pSource || !m_pTarget )
        return;

    //std::cout << "Paint Edge: " << m_SourcePoint.x() << " " << m_SourcePoint.y() << " To " << m_TargetPoint.x() << " " <<m_TargetPoint.y() << std::endl;

    //m_SourcePoint = QPointF( -100, -100  );
    //m_TargetPoint = QPointF( 100, 100 );


    //m_SourcePoint = mapToScene( ((QGraphicsItem*)m_pSource)->pos() ) ;
    //m_TargetPoint = mapToScene( ((QGraphicsItem*)m_pTarget)->pos() ) ;

    switch(curveType())
    {
    case Linear:
        linearPath( m_SourcePoint , m_TargetPoint , m_Path );
        break;
    case Quadratic:
        quadraticPath( m_SourcePoint, m_TargetPoint, m_Path );
        break;
    case Cubic: default:
        cubicPath( m_SourcePoint, m_TargetPoint, m_Path );
        break;
    }

    /*
        if(qFuzzyCompare(m_Path.length(), double(0.0)))
                return;

    painter->setPen( QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
    painter->drawPath( m_Path );
*/

    QLineF line( m_SourcePoint, m_TargetPoint );
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    painter->setPen( QPen( Qt::yellow, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

    //painter->drawLine( line );
    painter->drawPath( m_Path );

    // Draw the arrows
    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = 2*M_PI - angle;

    QPointF sourceArrowP1 = m_SourcePoint + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                                    cos(angle + M_PI / 3) * arrowSize);
    QPointF sourceArrowP2 = m_SourcePoint + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                                    cos(angle + M_PI - M_PI / 3) * arrowSize);
    QPointF destArrowP1 = m_TargetPoint + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                                  cos(angle - M_PI / 3) * arrowSize);
    QPointF destArrowP2 = m_TargetPoint + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                                  cos(angle - M_PI + M_PI / 3) * arrowSize);

    painter->setBrush(Qt::red);
    //painter->drawPolygon( QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2 );
    painter->drawPolygon( QPolygonF() << line.p2() << destArrowP1 << destArrowP2 );
}

void NEConnectionGEItem::adjust()
{
    if(!m_pSource || !m_pTarget)
        return;

    //std::cout << "ADjust " << std::endl;
    QLineF line( mapFromItem((QGraphicsItem*)m_pSource, 0, 0), mapFromItem((QGraphicsItem*)m_pTarget, 0, 0 ) );
    float length = line.length();

    prepareGeometryChange();

    if( length > qreal(20.) )
    {
        QPointF edgeOffset( (line.dx() * 55) / length, (line.dy() * 55) / length);
        m_SourcePoint = line.p1() + edgeOffset;
        m_TargetPoint = line.p2() - edgeOffset;
    }
    else
    {
        m_SourcePoint = m_TargetPoint = line.p1();
    }
}

NENodeGraphEditorItem *NEConnectionGEItem::sourceNode()
{
    return m_pSource;
}

NENodeGraphEditorItem *NEConnectionGEItem::targetNode()
{
    return m_pTarget;
}

void NEConnectionGEItem::linearPath(const QPointF &start, const QPointF &end, QPainterPath &path)
{
    path = QPainterPath(start);
    path.lineTo(end);
}

void NEConnectionGEItem::cubicPath(const QPointF &start, const QPointF &end, QPainterPath &path)
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
    path.cubicTo(clCenter, clCenterCP, center);
    path.cubicTo(frCenterCP, frCenter, m_TargetPoint);
}

void NEConnectionGEItem::quadraticPath(const QPointF &start, const QPointF &end, QPainterPath &path)
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
