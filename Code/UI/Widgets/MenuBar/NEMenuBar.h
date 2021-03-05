#pragma once

class NEDockWidget;

class NEMenuBar : public QMenuBar
{
	Q_OBJECT
	friend class NEDockWidget;
public:
	explicit NEMenuBar(QWidget *parent = 0);

signals:

public slots:

protected:
	void mouseDoubleClickEvent(QMouseEvent * event);

	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);

private:
	NEDockWidget* m_pParentDock;
	QPoint offset;
	bool moving;
	//bool docked;

};


