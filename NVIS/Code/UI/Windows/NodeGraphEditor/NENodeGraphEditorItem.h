#pragma once

class NENode;
class NEConnectionGEItem;

class NENodeGraphEditorItem : public QGraphicsItem
{
public:
    NENodeGraphEditorItem(NENode* node = 0, QGraphicsItem* parent=0);

    static const int ItemWidth = 100;
    static const int ItemHeight = 100;
    static const int ItemRoundRadius = 10;
    static const int BorderPenWidth = 1;
    static const int SelectedPenWidth = 3;
    static const int OuterBorderWidth = 16;
    static const int OuterBorderHeight = 16;
    static const int InnerBorderWidth = 8;
    static const int InnerBorderHeight = 8;


    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    void addNodeConnection(NEConnectionGEItem* item);

    void setPosition( QPointF r );
    void setColor( QColor c )
    {
        m_Color = c;
    }

    QColor color()
    {
        return m_Color;
    }

    bool advance();
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    NENode* m_pNode;

    //QList<NENode::NEConnection*> m_NodeConnList;
    QList<NEConnectionGEItem*> m_ConnList;


    mutable bool mousePressed;

    QPointF m_Position;
    QColor m_Color;
};
