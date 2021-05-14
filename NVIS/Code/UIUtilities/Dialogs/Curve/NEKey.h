#pragma once


class NECurveScene;

class NECEXP_NEUIUTILITIES NEKey : public QGraphicsItem
{

public:
    enum { Type = UserType + 1 };
    NEKey(QPointF Spline, QVariant::Type type,  NECurveScene* scene);
    ~NEKey();

    //! Returns the QPointF of this NEKey (only used for graph editor)
    QPointF point() { return scenePos(); }

    //! Overriden paint function
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //! Returns the Bounding Rect of this NEKey
    QRectF boundingRect() const;
    static bool greaterThan(const NEKey* keyA, const NEKey* keyB);

    //! Returns the QVariant value of this NEKey (as x() is only returning
    //!  float and is not aware of different Datatypes
    QVariant getValue();
    QVariant::Type dataType() { return m_type; }

    //! Define QGraphicsItem type for NEKey needs this
    int type() const
    {
        return Type;
    }

private:
    NECurveScene * m_parent;
    QColor m_background;
    QVariant::Type m_type;
protected:
    void mouseMoveEvent( QGraphicsSceneMouseEvent * event );
    void mousePressEvent( QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event);
};
