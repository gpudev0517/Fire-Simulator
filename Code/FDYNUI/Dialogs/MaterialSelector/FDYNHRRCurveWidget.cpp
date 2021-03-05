#include "FDYNHRRCurveWidget.h"

FDYNHRRCurveWidget::FDYNHRRCurveWidget(QWidget *parent)
    : QWidget(parent)
{
    m_titleLabel = new QLabel(this);
    m_titleLabel->setText("New Fire 1");
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_titleLabel, 1, Qt::AlignCenter);
    layout->setSpacing(1);

    m_Plot = new NECustomPlot(this);
    layout->addWidget(m_Plot, 19);

    setDarkTheme();

}

FDYNHRRCurveWidget::~FDYNHRRCurveWidget()
{
    //delete m_Plot;
}


void FDYNHRRCurveWidget::setDarkTheme()
{
    m_Plot->setBackground(QColor(52, 50, 53));
    m_Plot->setAutoFillBackground(true);

    //m_Plot->graph(0)->setData(x, y);
    //m_Plot->graph()->setPen(QPen(Qt::white));
    //m_Plot->graph()->setBrush(Qt::transparent);

//    m_Plot->xAxis->setLabel("Time");
//    m_Plot->yAxis->setLabel("HRR");

    m_Plot->xAxis->setLabelColor(Qt::white);
    m_Plot->yAxis->setLabelColor(Qt::white);

    m_Plot->xAxis->setTickLabelColor(Qt::transparent);
    m_Plot->yAxis->setTickLabelColor(Qt::transparent);

    m_Plot->xAxis->setBasePen(QPen(Qt::white));
    m_Plot->yAxis->setBasePen(QPen(Qt::white));

    m_Plot->xAxis->setTickPen(QPen(Qt::white));
    m_Plot->yAxis->setTickPen(QPen(Qt::white));

    m_Plot->xAxis->setSubTickPen(QPen(Qt::white));
    m_Plot->yAxis->setSubTickPen(QPen(Qt::white));

    m_Plot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    m_Plot->yAxis->grid()->setZeroLinePen(Qt::NoPen);

    m_Plot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    m_Plot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//    m_Plot->xAxis->grid()->setSubGridVisible(true);
//    m_Plot->yAxis->grid()->setSubGridVisible(true);

    m_Plot->xAxis->setRange(0, 900);
    m_Plot->yAxis->setRange(0, 900);
}

void FDYNHRRCurveWidget::drawPlot(QVector<double> &x, QVector<double> &y, double xupper, double yupper, QString title)
{
    m_titleLabel->setText(title);
    m_Plot->clearGraphs();
    setDarkTheme();
    m_Plot->addGraph();
    m_Plot->graph()->setData(x, y);
    m_Plot->graph()->setPen(QPen(Qt::white));
    m_Plot->graph()->setBrush(Qt::transparent);

    if(x.size() > 0 && y.size() > 0){
        m_Plot->xAxis->setTickLabelColor(Qt::white);
        m_Plot->yAxis->setTickLabelColor(Qt::white);
        m_Plot->xAxis->setRange(0, xupper+10);
        bool isSameAll = false;

        for(int i = 0; i < y.size(); i++){
            if( y[0] == y[i])
                isSameAll = true;
            else{
                isSameAll = false;
                break;
            }
        }

        if(isSameAll){
            if( yupper > 0 )
                m_Plot->yAxis->setRange(0, 2*yupper);
            else
                m_Plot->yAxis->setRange(-1, 1);
        }
        else
            m_Plot->yAxis->setRange(-yupper/20, yupper * 1.1);
    }
    else{
        m_Plot->xAxis->setTickLabelColor(Qt::transparent);
        m_Plot->yAxis->setTickLabelColor(Qt::transparent);
    }
    m_Plot->replot();
}
