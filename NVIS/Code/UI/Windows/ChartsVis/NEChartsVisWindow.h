#pragma once

class QCPItemTracer;
class QCPGraph;

namespace Ui {
    class NEChartsVisWindow;
}

class NEChartsVisWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NEChartsVisWindow( QWidget *parent = 0 );
    ~NEChartsVisWindow();

    void setupPlot( QString chartName );
    void setPlotType( QString plotType );
    void clearPlots();
    void updatePlot(QString chartName, QVector<double> &x0, QVector<double> &y0);

    void updatePlots();
    void removePlot(QString chartName);
    void clearPlot(QString chartName);
public slots:
    uint saveChartAs();
    uint saveAnimChartAs();
    uint clear();

private slots:
;
    void screenShot( QString& fileName );

private:

    Ui::NEChartsVisWindow *ui;
    QCPItemTracer *itemDemoPhaseTracer;


    QMap< QString, QCPGraph* > m_graphMap1;
    QMap< QString, int > m_graphIndexMap;


};


