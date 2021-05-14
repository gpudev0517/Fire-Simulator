#ifndef FDYNHRRCURVEWIDGET_H
#define FDYNHRRCURVEWIDGET_H

#include "Dialogs/Plot/NECustomPlot.h"

class FDYNFireMaterialDialog;

class FDYNHRRCurveWidget : public QWidget
{
    Q_OBJECT
public:
    FDYNHRRCurveWidget(QWidget *parent = nullptr);
    virtual ~FDYNHRRCurveWidget();

    void setDarkTheme();

    void drawPlot(QVector<double> &x, QVector<double> &y, double xupper, double yupper, QString title, bool bRescale);


    NECustomPlot *m_Plot;
    QLabel *m_titleLabel;
    FDYNFireMaterialDialog *m_ParentDialog;


signals:

public slots:
    void selectionChanged();
    void mousePressed(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);
    void mouseReleased(QMouseEvent *event);

private:
};

#endif // FDYNHRRCURVEWIDGET_H
