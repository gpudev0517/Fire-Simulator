#include "FDYNFireAddT2Dialog.h"
#include "FDYNHRRCurveWidget.h"
#include "ui_FDYNFireAddT2Dialog.h"
#include "Managers/FDYNGUIManager.h"
#include "Base/NESceneManager.h"
#include "Mesh/NEZoneItem.h"
#include "Utility/NEFireMaterial.h"

FDYNFireAddT2Dialog::FDYNFireAddT2Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FDYNFireAddT2Dialog)
{
    ui->setupUi(this);
    setStyle( QStyleFactory::create( "Fusion" ) );

    QObject::connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

    QStringList comboList = {"Custom", "Slow", "Medium", "Fast", "Ultra fast"};
    ui->cbFireGrowthRate->addItems(comboList);
    ui->cbFireGrowthRate->setCurrentIndex(-1);

    QObject::connect(ui->cbFireGrowthRate, SIGNAL(currentIndexChanged(int)), this, SLOT(fireGrowthRateChanged(int)));
    QObject::connect(ui->lineTimeToPeak, SIGNAL(editingFinished()), this, SLOT(lineEditingFinished()));
    QObject::connect(ui->linePeakHRR, SIGNAL(editingFinished()), this, SLOT(lineEditingFinished()));
    QObject::connect(ui->lineSteadyBurningPeriod, SIGNAL(editingFinished()), this, SLOT(lineEditingFinished()));
    QObject::connect(ui->lineDecayTime, SIGNAL(editingFinished()), this, SLOT(lineEditingFinished()));


    setDefaultValues(Medium);
    updateFireMaterialInfos(m_CurInputValue, m_infos);
}

FDYNFireAddT2Dialog::~FDYNFireAddT2Dialog()
{

}

void FDYNFireAddT2Dialog::fireGrowthRateChanged(int index)
{
    if(index != Custom){
        setDefaultValues(index);
        updateFireMaterialInfos(m_CurInputValue, m_infos);
    }
}

void FDYNFireAddT2Dialog::lineEditingFinished()
{
    if(QLineEdit *line = qobject_cast<QLineEdit*>(sender()))
    {
        ui->cbFireGrowthRate->setCurrentIndex(Custom);

        QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");
        int pos = 0;

        QString second_unit("s");
        QString kw_unit("kW");

        if(line == ui->lineTimeToPeak)
        {

            if( (pos = rx.indexIn(ui->lineTimeToPeak->text(), pos)) != -1 )
                m_CurInputValue.timeToPeak = rx.cap(0).toFloat();
            else
                m_CurInputValue.timeToPeak = 0.0f;

            ui->lineTimeToPeak->blockSignals(true);
            ui->lineTimeToPeak->setText(QString::number(m_CurInputValue.timeToPeak) + QChar::Space + second_unit);
            ui->lineTimeToPeak->blockSignals(false);
        }
        else if( line == ui->linePeakHRR )
        {
            if( (pos = rx.indexIn(ui->linePeakHRR->text(), pos)) != -1 )
                m_CurInputValue.peakHRR = rx.cap(0).toFloat();
            else
                m_CurInputValue.peakHRR = 0.0f;

            ui->linePeakHRR->blockSignals(true);
            ui->linePeakHRR->setText(QString::number(m_CurInputValue.peakHRR) + QChar::Space + kw_unit);
            ui->linePeakHRR->blockSignals(false);
        }
        else if( line == ui->lineSteadyBurningPeriod )
        {
            if( (pos = rx.indexIn(ui->lineSteadyBurningPeriod->text(), pos)) != -1 )
                m_CurInputValue.steadyBurningPeriod = rx.cap(0).toFloat();
            else
                m_CurInputValue.steadyBurningPeriod = 0.0f;

            ui->lineSteadyBurningPeriod->blockSignals(true);
            ui->lineSteadyBurningPeriod->setText(QString::number(m_CurInputValue.steadyBurningPeriod) + QChar::Space + second_unit);
            ui->lineSteadyBurningPeriod->blockSignals(false);
        }
        else if( line == ui->lineDecayTime )
        {
            if( (pos = rx.indexIn(ui->lineDecayTime->text(), pos)) != -1 )
                m_CurInputValue.decayTime = rx.cap(0).toFloat();
            else
                m_CurInputValue.decayTime = 0.0f;

            ui->lineDecayTime->blockSignals(true);
            ui->lineDecayTime->setText(QString::number(m_CurInputValue.decayTime) + QChar::Space + second_unit );
            ui->lineDecayTime->blockSignals(false);
        }

        updateFireMaterialInfos(m_CurInputValue, m_infos);
    }
}

void FDYNFireAddT2Dialog::updateFireMaterialInfos(T2Input inputValue, QList<FireMaterialInfo> &infos)
{
    if(infos.size() > 0)
        infos.clear();
    for(int i = 0; i <= 22; i++)
    {
        FireMaterialInfo info;
        infos.append(info);
    }

    float AlphaGrowth = 0.0f;
    float AlphaDecay = 0.0f;

    float aTimetoPeak = 0.0f;
    float aPeakHRR = 0.0f;
    float aSteadyBurningTime = 0.0f;
    float aDecayTime = 0.0f;

    aTimetoPeak = inputValue.timeToPeak;
    aPeakHRR = inputValue.peakHRR * 1000;
    aSteadyBurningTime = inputValue.steadyBurningPeriod;
    aDecayTime = inputValue.decayTime;

    for(int ir = 0; ir <= 22; ir ++)
    {
        infos[ir].m_Height = 0.0;
        infos[ir].m_Area = 0.09;
        infos[ir].m_CO = 0.01;
        infos[ir].m_Soot = 0.01;
        infos[ir].m_HCN = 0.0;
        infos[ir].m_TS = 0.0;
    }

    infos[0].m_Time = 0.0;
    infos[0].m_HRR = 0.0;
    infos[0].m_Area = 0.001;
    AlphaGrowth = aPeakHRR /  pow( aTimetoPeak, 2);

    for(int ir = 1; ir <= 10; ir ++)
    {
        infos[ir].m_Time = ir / 10.0 * aTimetoPeak;
        infos[ir].m_HRR = AlphaGrowth * pow(infos[ir].m_Time, 2);
        infos[ir].m_Area = M_PI * pow((pow((infos[ir].m_HRR / (352.981915 / 293.15 * 1012 * 293.15 * sqrt(9.80665))), 0.4) / 2), 2);
    }
    AlphaDecay = aPeakHRR / pow(aDecayTime,2);
    for(int ir = 11; ir <= 21; ir++)
    {
        infos[ir].m_Time = aTimetoPeak + aSteadyBurningTime + (10 - (21 - ir)) / 10.0 * aDecayTime;
        infos[ir].m_HRR = AlphaDecay * pow((aTimetoPeak + aSteadyBurningTime + aDecayTime - infos[ir].m_Time), 2);
        infos[ir].m_Area = M_PI * pow((pow((infos[ir].m_HRR / (352.981915 / 293.15 * 1012 * 293.15 * sqrt(9.80665))), 0.4) / 2), 2);
    }

    infos[22].m_Time = aTimetoPeak + aSteadyBurningTime + aDecayTime + 10.0;
    infos[22].m_HRR = 0;
    infos[22].m_Area = 0.001;

    for(int ir = 0; ir <= 22; ir ++)
    {
        infos[ir].m_HRR /= 1000.0;
    }


    QVector<double> x(23), y(23);

    for(int i = 0; i < 23; i++)
    {
        x[i] = infos[i].m_Time;
        y[i] = infos[i].m_HRR;
    }

    ui->hrrCurveWidget->clearGraphs();

    ui->hrrCurveWidget->setBackground(QColor(52, 50, 53));
    ui->hrrCurveWidget->setAutoFillBackground(true);

    ui->hrrCurveWidget->addGraph();
    ui->hrrCurveWidget->graph()->setData(x, y);
    ui->hrrCurveWidget->graph()->setPen(QPen(Qt::white));
    ui->hrrCurveWidget->graph()->setBrush(Qt::transparent);

    ui->hrrCurveWidget->xAxis->setLabel("Time");
    ui->hrrCurveWidget->yAxis->setLabel("HRR");

    ui->hrrCurveWidget->xAxis->setLabelColor(Qt::white);
    ui->hrrCurveWidget->yAxis->setLabelColor(Qt::white);

    ui->hrrCurveWidget->xAxis->setTickLabelColor(Qt::white);
    ui->hrrCurveWidget->yAxis->setTickLabelColor(Qt::white);

    ui->hrrCurveWidget->xAxis->setBasePen(QPen(Qt::white));
    ui->hrrCurveWidget->yAxis->setBasePen(QPen(Qt::white));

    ui->hrrCurveWidget->xAxis->setTickPen(QPen(Qt::white));
    ui->hrrCurveWidget->yAxis->setTickPen(QPen(Qt::white));

    ui->hrrCurveWidget->xAxis->setSubTickPen(QPen(Qt::white));
    ui->hrrCurveWidget->yAxis->setSubTickPen(QPen(Qt::white));

    ui->hrrCurveWidget->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->hrrCurveWidget->yAxis->grid()->setZeroLinePen(Qt::NoPen);

    ui->hrrCurveWidget->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->hrrCurveWidget->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    ui->hrrCurveWidget->xAxis->setRange(0, aTimetoPeak + aSteadyBurningTime + aDecayTime + 10);
    ui->hrrCurveWidget->yAxis->setRange(-50, aPeakHRR/1000.0+50);

    ui->hrrCurveWidget->replot();

}

void FDYNFireAddT2Dialog::setDefaultValues(int index)
{
    if(index == Slow)
    {
        m_CurInputValue.timeToPeak = 600;
        m_CurInputValue.peakHRR = 1054;
        m_CurInputValue.steadyBurningPeriod = 300;
        m_CurInputValue.decayTime = 600;
    }
    else if(index == Medium)
    {
        m_CurInputValue.timeToPeak = 300;
        m_CurInputValue.peakHRR = 1054;
        m_CurInputValue.steadyBurningPeriod = 300;
        m_CurInputValue.decayTime = 300;
    }
    else if(index == Fast)
    {
        m_CurInputValue.timeToPeak = 150;
        m_CurInputValue.peakHRR = 1054;
        m_CurInputValue.steadyBurningPeriod = 300;
        m_CurInputValue.decayTime = 150;
    }
    else if(index == UltraFast)
    {
        m_CurInputValue.timeToPeak = 75;
        m_CurInputValue.peakHRR = 1054;
        m_CurInputValue.steadyBurningPeriod = 300;
        m_CurInputValue.decayTime = 75;
    }

    QString second_unit("s");
    QString kw_unit("kW");

    ui->lineTimeToPeak->blockSignals(true);
    ui->lineTimeToPeak->setText(QString::number(m_CurInputValue.timeToPeak) + QChar::Space + second_unit);
    ui->lineTimeToPeak->blockSignals(false);

    ui->linePeakHRR->blockSignals(true);
    ui->linePeakHRR->setText(QString::number(m_CurInputValue.peakHRR) + QChar::Space + kw_unit);
    ui->linePeakHRR->blockSignals(false);

    ui->lineSteadyBurningPeriod->blockSignals(true);
    ui->lineSteadyBurningPeriod->setText(QString::number(m_CurInputValue.steadyBurningPeriod) + QChar::Space + second_unit);
    ui->lineSteadyBurningPeriod->blockSignals(false);

    ui->lineDecayTime->blockSignals(true);
    ui->lineDecayTime->setText(QString::number(m_CurInputValue.decayTime) + QChar::Space + second_unit);
    ui->lineDecayTime->blockSignals(false);
}
