#ifndef NECurveView_H
#define NECurveView_H

class NECurveView : public QGraphicsView
{
	Q_OBJECT

public:
	NECurveView(QWidget *parent = 0);
	~NECurveView();
protected:
	void mousePressEvent( QMouseEvent * event );
	void mouseMoveEvent( QMouseEvent * event );
	void mouseReleaseEvent( QMouseEvent * event );
	void resizeEvent(QResizeEvent *event);
private:
	enum m_mode { kZoom, kPan, kNone};
	m_mode m_mouseMode;
	QPoint m_mousePositionOld;
signals:
	//! Emitted, if the mouse position in the scene is changed
	void scenePosChanged(QPointF point);

};

#endif // NECurveView_H
