#include "NECurveScene.h"
#include "Base/NESceneManager.h"
#include "Manager/NECurveManager.h"
#include "Dialogs/Curve/NEKey.h"
#include "Dialogs/Curve/NECurveView.h"
#include "Utility/Curves/NECurve.h"

NECurveScene::NECurveScene( NECurveView * parent ) : QGraphicsScene(parent)
{
    addItem(&m_pathItem);
    m_mode = moveKeyframe;
    m_parent = parent;

    m_currentFrame = NESCENE.CurrentFrame();
    m_currentFrameMarker = addLine(QLineF(QPointF(m_currentFrame, this->sceneRect().y()), QPointF(m_currentFrame, this->sceneRect().y() + this->sceneRect().height())), QPen(Qt::red));


    connect(&NESCENE, SIGNAL(currentFrameChanged(uint)), this, SLOT(drawCurrentframeMarker(uint)));
}

NECurveScene::~NECurveScene()
{

}

void NECurveScene::drawCurve()
{
    QList<NEKey*> tmp = m_keyList.keys();

    if(tmp.size() > 0)
    {
        qSort(tmp.begin(), tmp.end(), NEKey::greaterThan);
//        QTest::qWait(10);

        QPolygonF interpolated;

        for(int count = 0; count < m_keyList.size() - 1; ++count)
        {
            NEKey* keyA = tmp[count];
            NEKey* keyB = tmp[count + 1];

            for(int x = keyA->x(); x <= keyB->x(); ++x)
            {
                interpolated << QPointF(x, m_keyList.value(keyA)->interpolate(keyA->x(), keyA->getValue(), keyB->x(), keyB->getValue(), x).toFloat());
            }
        }

        // add last key to painter path
        interpolated << tmp.last()->point();

        QPainterPath path;
        path.addPolygon(interpolated);
        m_pathItem.setPath(path);
        m_pathItem.setPen(QPen(Qt::blue, this->sceneRect().height()/200.f));
    }
    update();
}

void NECurveScene::drawForeground(QPainter* painter, const QRectF& rect)
{
    m_pathItem.setPen(QPen(Qt::blue, this->sceneRect().height()/200.f));
}


void NECurveScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->save();

    // initialize intervals with minimal lookup value
    int gridInterval_x = 1;
    float gridInterval_y = 0.001f;

    // preferred scales for x and y grids
    float scaleRate_y[] = {0.001f, 0.0025f, 0.005f, 0.0075f, 0.01f, 0.025f, 0.05f, 0.075f, 0.1f, 0.25f, 0.5f, 0.75f, 1.0f, 1.5f,
                           2.0f, 4.0f, 5.0f, 10.0f, 20.0f, 50.0f, 100.0f, 250.0f, 500.0f, 1000.0f};
    int scaleRate_x[] = {1, 2, 5, 10, 25, 50, 75, 100, 150, 250, 500, 1000};

    // compute gridInterval_x
    int count = 0;
    while (gridInterval_x < (this->sceneRect().width() / 8.0)) {
        if(count >= int(sizeof(scaleRate_x) / sizeof(int)))
            gridInterval_x *= 1.5;
        else
            gridInterval_x = scaleRate_x[count];
        ++count;
    }
    if(count == 0)
    {
        while(gridInterval_x > (this->sceneRect().width() / 8.0))
            gridInterval_x /= 2.5;
    }

    // compute gridInterval_y
    count = 0;
    while (gridInterval_y < (this->sceneRect().height() / 8.0)) {
        if(count >= int(sizeof(scaleRate_y) / sizeof(float)))
            gridInterval_y *= 1.5;
        else
            gridInterval_y = scaleRate_y[count];
        ++count;
    }
    if(count == 0)
    {
        while(gridInterval_y > (this->sceneRect().height() / 8.0))
            gridInterval_y /= 2.5;
    }

    // draw background color
    painter->save();
    painter->fillRect(rect, QColor(120,120,120,255));
    painter->restore();

    painter->save();
    painter->setWorldMatrixEnabled(true);
    painter->scale(this->width()/ m_parent->width(), this->height() / m_parent->height());

    // compute left and top values of first grid line in x and y direction
    int result_x = static_cast<int>( rect.left() / gridInterval_x );
    float mod_x = rect.left() - static_cast<float>( result_x ) * gridInterval_x;

    int result_y= static_cast<int>( rect.top() / gridInterval_y );
    float mod_y = rect.top() - static_cast<float>( result_y ) * gridInterval_y;

    float left = 0.0;
    float top = 0.0;

    // for 0.0 values we have to fuzzy compare
    if(!qFuzzyCompare(float(1 + rect.left()), float(1 + mod_x)))
    {
        left = rect.left() - mod_x;
    }
    if(!qFuzzyCompare(float(1 + rect.top()), float(1 + mod_y)))
    {
        top = rect.top() - mod_y;
    }

    // iterate over x and y and add lines and labels
    // labels are immediately painted / lines are stored and painted afterwards
    // in order not to have to change pens too ofen
    QVarLengthArray<QLineF, 100> linesX;
    for (double x = left; x < rect.right(); x += gridInterval_x )
    {
        linesX.append(QLineF((x + (this->width()/(m_parent->width()))) * (m_parent->width()/this->width()), (this->sceneRect().top() + (this->sceneRect().height() / (m_parent->height()))) * (m_parent->height()/this->height()), (x + (this->width()/(m_parent->width()))) * (m_parent->width()/this->width()), (this->sceneRect().bottom() + (this->sceneRect().height() / (m_parent->height()))) * (m_parent->height()/this->height())));
        painter->drawText((x + (this->width()/(m_parent->width() * 0.2))) * (m_parent->width()/this->width()), (this->sceneRect().top() + (this->sceneRect().height() / (m_parent->height() * 0.05))) * (m_parent->height()/this->height()), QString::number(x));
    }

    QVarLengthArray<QLineF, 100> linesY;
    for (double y = top; y < rect.bottom(); y += gridInterval_y )
    {
        linesY.append(QLineF(this->sceneRect().left() * (m_parent->width()/this->width()), (y + (this->sceneRect().height() / (m_parent->height()))) * (m_parent->height()/this->height()), this->sceneRect().right() * (m_parent->width()/this->width()), (y + (this->sceneRect().height() / (m_parent->height()))) * (m_parent->height()/this->height())));
        painter->drawText(this->sceneRect().left() * (m_parent->width()/this->width()), (y + (this->sceneRect().height() / (m_parent->height() * 0.05))) * (m_parent->height()/this->height()), QString::number(y));
    }

    // paint grid lines
    painter->restore();
    painter->save();
    painter->setWorldMatrixEnabled(true);
    painter->scale(this->width()/ m_parent->width(), this->height() / m_parent->height());
    painter->setPen(QColor(230,230,230,255));
    painter->drawLines(linesX.data(), linesX.size());
    painter->drawLines(linesY.data(), linesY.size());
    painter->restore();
    painter->restore();
}

void NECurveScene::drawCurrentframeMarker(uint frame)
{
    removeItem(m_currentFrameMarker);
    m_currentFrame = frame;
    m_currentFrameMarker = addLine(QLineF(QPointF(frame, this->sceneRect().y()), QPointF(frame, this->sceneRect().y() + this->sceneRect().height())), QPen(Qt::red));
}

void NECurveScene::moveKey(NEKey* point)
{
    QList<uint> xMap;

    // we have to make sure, that no two keyframes with same x exist
    foreach(NEKey* key, m_keyList.keys())
    {
        if(point != key)
        {
            xMap.append(key->x());
        }
    }
    while(xMap.contains(point->x()))
        point->setX(point->x() + 1);

    drawCurve();
    emit selectedKeyframeMoved(point);
}

void NECurveScene::addKey(NEKey* key, NECurve* curve)
{
    m_dataType = key->dataType();
    m_keyList.insert(key, curve);
    addItem(key);
    drawCurve();
}

QMap<NEKey*, NECurve*> NECurveScene::getKeylist()
{
    return m_keyList;
}

void NECurveScene::setMode(selectionMode newMode)
{
    m_mode = newMode;
}

void NECurveScene::mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void NECurveScene::mousePressEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
    NEKey* key = qgraphicsitem_cast<NEKey *>(itemAt(mouseEvent->scenePos(), m_parent->transform()));

    if(m_mode == deleteKeyframe)
    {
        if(key)
        {
            m_keyList.remove(key);
            removeItem(key);
            drawCurve();
            QGraphicsScene::mousePressEvent(mouseEvent);
            m_selectedKey = NULL;
            emit selectedKeyframeChanged(NULL, NULL);
        }
    }
    else if(m_mode == addKeyframe)
    {
        NEKey* newKey = new NEKey(mouseEvent->scenePos(), m_dataType, this);
        addKey(newKey, NECURVE.createCurve());
        moveKey(newKey);

        foreach(NEKey* k, m_keyList.keys())
        {
            k->setSelected(false);
        }
        newKey->setSelected(true);
        m_selectedKey = newKey;
        emit selectedKeyframeChanged(newKey, m_keyList.value(newKey));

        drawCurve();
    }
    else if(m_mode == moveKeyframe)
    {
        if(key)
        {
            foreach(NEKey* k, m_keyList.keys())
            {
                k->setSelected(false);
            }
            key->setSelected(true);
            m_selectedKey = key;
            emit selectedKeyframeChanged(key, m_keyList.value(key));
        } else
        {
            m_selectedKey = NULL;
            emit selectedKeyframeChanged(NULL, NULL);
        }
        QGraphicsScene::mousePressEvent(mouseEvent);
    }
}
void NECurveScene::mouseReleaseEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void NECurveScene::changeCurveType(const QString typeName)
{
    if(m_selectedKey)
    {
        m_keyList.insert(m_selectedKey, NECURVE.createCurve(typeName));
        drawCurve();
    }
}

double NECurveScene::getValueForX(double x)
{
    QList<NEKey*> tmp = m_keyList.keys();

    if(tmp.size() > 1)
    {
        qSort(tmp.begin(), tmp.end(), NEKey::greaterThan);

        for(int count = 0; count < m_keyList.size() - 1; ++count)
        {
            NEKey* keyA = tmp[count];
            NEKey* keyB = tmp[count + 1];


            if(keyA->x() < x && keyB->x() > x)
            {
                return m_keyList.value(keyA)->interpolate(keyA->x(), keyA->getValue(), keyB->x(), keyB->getValue(), x).toFloat();
            }
            else if(keyA->x() == x)
            {
                return keyA->y();
            }
            else if(keyB->x() == x)
            {
                return keyB->y();
            }
        }

        // if we are here, the value is below or above the first, last keyframe
        if(x < tmp.first()->x())
            return tmp.first()->y();
        else
            return tmp.last()->y();

    }
    else if(tmp.size() > 0)
    {
        return tmp.first()->y();
    }
    else
        return 0.0;
}

