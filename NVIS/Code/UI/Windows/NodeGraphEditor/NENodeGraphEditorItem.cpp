#include "NENodeGraphEditorItem.h"
#include "NEConnectionGEItem.h"
#include "NENode.h"

NENodeGraphEditorItem::NENodeGraphEditorItem(NENode *node, QGraphicsItem *parent) : QGraphicsItem(parent)
{
    setFlags( QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsFocusable );
    setFlag( ItemSendsGeometryChanges );
    m_pNode = node;
    //	mousePressed =
}

QRectF NENodeGraphEditorItem::boundingRect() const
{
    int penWidth;
    if(isSelected())
        penWidth = SelectedPenWidth;
    else
        penWidth = BorderPenWidth;

    return QRectF( -ItemWidth*0.5 - penWidth - OuterBorderWidth,
                   -ItemHeight*0.5 - penWidth - OuterBorderHeight,
                   ItemWidth + penWidth * 2 + OuterBorderWidth,
                   ItemHeight + penWidth * 2 + OuterBorderHeight);
}


bool NENodeGraphEditorItem::advance()
{
    if( m_Position == pos() )
        return false;

    setPos( m_Position );
    return true;
}


void NENodeGraphEditorItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{

    QPen selectedPen( QPen( QGuiApplication::palette().color( QPalette::Highlight ) ) );
    selectedPen.setWidth( 0 );

    if( isSelected() )
        painter->setPen( selectedPen );
    else
        painter->setPen( QGuiApplication::palette().color(QPalette::Window) );

    painter->setBrush( QColor( 200, 200, 200 ) );
    painter->drawRect( -ItemWidth/2 - OuterBorderWidth/2, -ItemHeight/2-OuterBorderHeight/2, ItemWidth+OuterBorderWidth, ItemHeight+OuterBorderHeight );
    painter->setBrush( Qt::black );
    painter->drawRect( -ItemWidth/2 - InnerBorderWidth/2, -ItemHeight/2-InnerBorderHeight/2, ItemWidth+InnerBorderWidth, ItemHeight+InnerBorderHeight );

    // Paint selected items lighter
    QRadialGradient gradient( -20, -20, 100 );
    if( option->state & QStyle::State_Sunken )
    {
        /*
        painter->setBrush( QColor( 160, 160, 160 ) );
        */
        gradient.setCenter( 3, 3 );
        gradient.setFocalPoint( 3, 3 );
        /*
        gradient.setColorAt( 1, QColor( Qt::red ).light(120 ) );
        gradient.setColorAt( 0, QColor( Qt::darkRed ).light(120) );
        */
        gradient.setColorAt( 1, color().light( 120 ) );
        gradient.setColorAt( 0, color().darker().light( 120 ) );
    }
    else
    {
        /*
        painter->setBrush( QColor( 128, 128, 128 ) );
        */
        /*
        gradient.setColorAt(0, Qt::red );
        gradient.setColorAt(1, Qt::darkRed );
        */
        gradient.setColorAt( 0, color() );
        gradient.setColorAt( 1, color().darker() );

    }

    painter->setBrush( gradient );
    //std::cout << "Paint Vertex : " << pos().x() << " " << pos().y() << std::endl;

    painter->drawRect( -ItemWidth/2, -ItemHeight/2, ItemWidth, ItemHeight );

    painter->setPen( QPen( Qt::gray, 0 ) );
    QFont sansFont( "Helvetica [Cronyx]", 8 );

    sansFont.setBold( true );
    sansFont.setStretch( QFont::SemiCondensed );
    painter->setFont( sansFont );

    painter->drawText( QRect( -ItemWidth/2, -ItemHeight/2, ItemWidth, ItemHeight), Qt::AlignCenter, m_pNode->objectName());
}

void NENodeGraphEditorItem::addNodeConnection( NEConnectionGEItem *item )
{
    m_ConnList << item;
    item->adjust();
}

void NENodeGraphEditorItem::setPosition(QPointF r)
{
    m_Position = r;
}

QVariant NENodeGraphEditorItem::itemChange( QGraphicsItem::GraphicsItemChange change, const QVariant &value )
{
    switch (change)
    {
    case ItemPositionHasChanged:
        foreach( NEConnectionGEItem *conn, m_ConnList )
        {
            conn->adjust();
        }
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange( change, value );

}

void NENodeGraphEditorItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent( event );
}

void NENodeGraphEditorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //	if(event->buttonDownScreenPos(Qt::LeftButton) == event->lastScreenPos())
    //		setSelected(true);
    update();
    QGraphicsItem::mouseReleaseEvent( event );
}

void NENodeGraphEditorItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    //update();
    QGraphicsItem::mouseMoveEvent( event );
}
