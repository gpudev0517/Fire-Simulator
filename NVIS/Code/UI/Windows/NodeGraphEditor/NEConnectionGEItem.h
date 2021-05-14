#pragma once

class NENodeGraphEditorItem;

class NEConnectionGEItem : public QGraphicsItem
{
public:

	enum CurveType
	{
		Linear,
		Quadratic,
		Cubic
	};

	NEConnectionGEItem(NENodeGraphEditorItem* source, NENodeGraphEditorItem* target, QGraphicsItem *parent = 0);

	QRectF boundingRect() const;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
			   QWidget *widget);

	void adjust();

	NENodeGraphEditorItem* sourceNode();
	NENodeGraphEditorItem* targetNode();

	static CurveType curveType() { return m_CurveType; }
	static void setCurveType(CurveType type) { m_CurveType = type; }

private:
	NENodeGraphEditorItem* m_pSource;
	NENodeGraphEditorItem* m_pTarget;
	QPointF m_SourcePoint;
	QPointF m_TargetPoint;

	//NENode::NEConnection* m_pConn;
	mutable QPainterPath m_Path;
	double arrowSize;
	static CurveType m_CurveType;

	void linearPath(const QPointF& start, const QPointF& end,  QPainterPath& path);
	void cubicPath(const QPointF& start, const QPointF& end,  QPainterPath& path);
	void quadraticPath(const QPointF& start, const QPointF& end,  QPainterPath& path);
};


