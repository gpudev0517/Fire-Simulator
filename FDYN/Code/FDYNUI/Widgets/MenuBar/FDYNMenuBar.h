#pragma once

#include <QQuickWidget>

class FDYNDockWidget;

class FDYNMenuBar : public QMenuBar
{
    friend class FDYNDockWidget;
    Q_OBJECT
public:
    explicit FDYNMenuBar(QWidget *parent = 0);

signals:

public slots:

protected:
    void mouseDoubleClickEvent(QMouseEvent * event);

    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

private:
    FDYNDockWidget* m_pParentDock;
    QPoint offset;
    bool moving;
    //bool docked;

};


