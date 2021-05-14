#include "NEEnumCurveScene.h"
#include "Dialogs/Curve/NECurveView.h"
#include "Dialogs/Curve/NEKey.h"

NEEnumCurveScene::NEEnumCurveScene( NECurveView * parent, QMap<int, QString> keys ) : NECurveScene(parent)
{
	m_enumValues = keys;
}


void NEEnumCurveScene::drawBackground(QPainter* painter, const QRectF& rect)
{
	painter->save();

	// initialize intervals with minimal lookup value
	int gridInterval_x = 1.0f;

	// preferred scales for x and y grids
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

	float left = 0.0;
//	float top = 0.0;

	// for 0.0 values we have to fuzzy compare
	if(!qFuzzyCompare(float(1 + rect.left()), float(1 + mod_x)))
	{
		left = rect.left() - mod_x;
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
    foreach (int value, m_enumValues.keys())
	{
		linesY.append(QLineF(this->sceneRect().left() * (m_parent->width()/this->width()), (value + (this->sceneRect().height() / (m_parent->height()))) * (m_parent->height()/this->height()), this->sceneRect().right() * (m_parent->width()/this->width()), (value + (this->sceneRect().height() / (m_parent->height()))) * (m_parent->height()/this->height())));
		painter->drawText(this->sceneRect().left() * (m_parent->width()/this->width()), (value + (this->sceneRect().height() / (m_parent->height() * 0.05))) * (m_parent->height()/this->height()), m_enumValues.value(value));
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

NEEnumCurveScene::~NEEnumCurveScene()
{

}

void NEEnumCurveScene::moveKey(NEKey* point)
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

	// make sure, the y value is a valid enum value
	int max = m_enumValues.keys()[0];
	foreach(int val, m_enumValues.keys())
	{
		if( max < val)
			max = val;
	}
	while(!m_enumValues.keys().contains((int) point->y()) && point->y() < max)
	{
		point->setY(point->y() + 1);
	}

	if(point->y() > max)
		point->setY(max);

	drawCurve();
	emit selectedKeyframeMoved(point);
}


