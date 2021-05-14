#ifndef FDYNHRRCURVEWIDGET_H
#define FDYNHRRCURVEWIDGET_H

#include "Dialogs/Plot/NECustomPlot.h"

class FDYNVentValueCurveWidget : public QWidget
{
    Q_OBJECT
public:
    FDYNVentValueCurveWidget(QWidget *parent = nullptr);
    virtual ~FDYNVentValueCurveWidget();

    void setDarkTheme(QString xLabel, QString yLabel);

    void drawPlot(QVector<double> &x, QVector<double> &y, double xupper, double yupper, QString title, QString xLabel, QString yLabel);


    NECustomPlot *m_Plot;
    //QLabel *m_titleLabel;

signals:

public slots:
private:


};

#endif // FDYNHRRCURVEWIDGET_H
