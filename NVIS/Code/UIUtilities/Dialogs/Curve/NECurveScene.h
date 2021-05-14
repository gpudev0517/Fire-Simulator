#pragma once

class NECurveManager;
class NECurveView;
class NECurve;
class NEKey;

class NECEXP_NEUIUTILITIES NECurveScene : public QGraphicsScene
{
	Q_OBJECT

public:
	enum selectionMode {
		moveKeyframe,
		addKeyframe,
		deleteKeyframe,
	};

	NECurveScene( NECurveView * parent ); 
	~NECurveScene();

	//! Adds a key to this scene
	void addKey(NEKey* key, NECurve* curve);
	//! Called, if the given key has been moved
	virtual void moveKey(NEKey* point);
	//! Sets different modes for selecting keys, see selectionMode enum
	void setMode(selectionMode newMode);
	//! Returns all keys and curves in this scene (e.g. for persisting data)
	QMap<NEKey*, NECurve*> getKeylist();

	double getValueForX(double x);
private:
	QGraphicsPathItem m_pathItem;
	selectionMode m_mode;
	NEKey* m_selectedKey;
	uint m_currentFrame;
	QGraphicsLineItem* m_currentFrameMarker;
	QVariant::Type m_dataType;
protected slots:
	//! Called for drawing the curve for the keyList
	virtual void drawCurve();
public slots:
	//! Changes the curvetype of the selectedKey
	void changeCurveType(const QString typeName);
	//! Draws the currentFrameMarker at the given frame
	void drawCurrentframeMarker(uint frame);
signals:
	void selectedKeyframeChanged(NEKey* key, NECurve* curve);
	void selectedKeyframeMoved(NEKey* key);
protected:
	QMap<NEKey*, NECurve*> m_keyList;
	NECurveView* m_parent;
	void mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent );
	void mousePressEvent( QGraphicsSceneMouseEvent * mouseEvent );
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * mouseEvent );
	virtual void drawBackground(QPainter *painter, const QRectF &rect);
	virtual void drawForeground(QPainter *painter, const QRectF &rect);
};
