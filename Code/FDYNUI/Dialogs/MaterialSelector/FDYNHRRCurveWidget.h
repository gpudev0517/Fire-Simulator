#ifndef FDYNHRRCURVEWIDGET_H
#define FDYNHRRCURVEWIDGET_H

#include "Dialogs/Plot/NECustomPlot.h"

class FDYNHRRCurveWidget : public QWidget
{
    Q_OBJECT
public:
    FDYNHRRCurveWidget(QWidget *parent = nullptr);
    virtual ~FDYNHRRCurveWidget();

    void setDarkTheme();

    void drawPlot(QVector<double> &x, QVector<double> &y, double xupper, double yupper, QString title);


    NECustomPlot *m_Plot;
    QLabel *m_titleLabel;

signals:

public slots:
private:


};

#endif // FDYNHRRCURVEWIDGET_H
