#pragma once

#include <QDialog>
NE_FWD_DECL_1(class, NESpatial)

namespace Ui {
class NEPropertyShortCut;
}

class NEPropertyShortCut : public QDialog
{
    Q_OBJECT

public:
    explicit NEPropertyShortCut(QWidget *parent = 0);
    ~NEPropertyShortCut();

    void setMode(QWidget *glWidget, NESpatial* spatial, uint mode);

    virtual void accept();
    virtual void reject();

private:
    Ui::NEPropertyShortCut *ui;

    NESpatial *m_CurSpatial;
    QWidget *m_CurGLWidget;
    uint m_curMode;

public slots:
    void performChangedX();
    void performChangedY();
    void performChangedZ();
};
