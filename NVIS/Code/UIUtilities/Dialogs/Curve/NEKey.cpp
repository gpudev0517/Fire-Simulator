#include "NEKey.h"
#include "Dialogs/Curve/NECurveScene.h"

NEKey::NEKey(QPointF point, QVariant::Type type,  NECurveScene* scene) : m_parent(scene)
{
    m_type = type;
    setPos(qRound(point.x()), point.y());
    setFlag(ItemIsMovable,true);
    setFlag(ItemIsFocusable, true);
    setFlag(ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    m_background = QColor("blue");
}

NEKey::~NEKey()
{

}

QVariant NEKey::getValue()
{
    QVariant value(this->y());
    value.convert(m_type);
    return value;
}

QRectF NEKey::boundingRect() const
{
    return QRectF(-5, -5, 10, 10);
}

bool NEKey::greaterThan(const NEKey* keyA, const NEKey* keyB) {
    return keyA->x() < keyB->x();
}

void NEKey::paint( QPainter * painter, const QStyleOptionGraphicsItem * , QWidget *){
    if(isSelected())
        painter->setBrush(QColor("yellow"));
    else
        painter->setBrush(QColor("blue"));
    painter->drawRoundedRect(-5, -5, 10, 10, 5, 5);
}

void NEKey::mousePressEvent( QGraphicsSceneMouseEvent* event)
{
    update();
}

void NEKey::mouseReleaseEvent( QGraphicsSceneMouseEvent* event)
{
    update();
}

void NEKey::mouseMoveEvent( QGraphicsSceneMouseEvent * event )
{
    QVariant v(event->scenePos().y());
    v.convert(m_type);
    setPos(qRound(event->scenePos().x()), v.toFloat());
    m_parent->moveKey(this);

}

