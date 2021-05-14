#pragma once

//#include "Manager/NECurveManager.h"
//#include "Utility/Curves/NEKey.h"
#include "Dialogs/Curve/NECurveScene.h"

class NECurveView;
class NEKey;
class NEEnumCurveScene : public NECurveScene
{
	Q_OBJECT

public:
	NEEnumCurveScene( NECurveView * parent, QMap<int, QString> keys ); 
	~NEEnumCurveScene();

	//! Called, if the given key has been moved
	virtual void moveKey(NEKey* point);

private:
	QMap<int, QString> m_enumValues;

protected:
	void drawBackground(QPainter *painter, const QRectF &rect);
};
