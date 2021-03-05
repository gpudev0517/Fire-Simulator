#pragma once

#include <QDialog>
#include "GLI/NEGLWidget.h"

NE_FWD_DECL_1(class, NESpatial)

namespace Ui {
class FDYNPropertyShortCut;
}

class FDYNPropertyShortCut : public QDialog
{
    Q_OBJECT

public:
    explicit FDYNPropertyShortCut(QWidget *parent = 0);
    ~FDYNPropertyShortCut();

    void setMode(QWidget *glWidget, NESpatial* spatial, uint mode);
    void setSnapUnitMode(QWidget *glWidget);
    void setGridMode(vec3f selPt);
private:
    Ui::FDYNPropertyShortCut *ui;

    NESpatial *m_CurSpatial;
    QWidget *m_CurGLWidget;
    uint m_curMode;
    NEGLWidget* m_GLWidget;

    QPoint m_pressPos;

public slots:
    void performChangedX();
    void performChangedY();
    void performChangedZ();

    virtual void accept();
    virtual void reject();

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
};
