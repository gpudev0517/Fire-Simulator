#include "FDYNAddVentValuesDialog.h"
#include "ui_FDYNAddVentValuesDialog.h"
#include "Managers/FDYNGUIManager.h"
#include "Base/NESceneManager.h"
#include "Mesh/NEZoneItem.h"
#include "Utility/NEVentCriteria.h"
#include "FDYNVentValueCurveWidget.h"


FDYNAddVentValuesDialog::FDYNAddVentValuesDialog(NEVentCriteria::CriteriaType type,
                                                 QList<NEVentCriteria::VentValues> values,
                                                 QWidget *parent/* = 0*/):
    QDialog(parent),
    ui(new Ui::FDYNAddVentValuesDialog)
{
    ui->setupUi(this);
    setStyle( QStyleFactory::create( "Fusion" ) );

    m_Values = values;
    m_Type = type;

    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(accept()));

    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(addValue()));
    connect(ui->btnRemove, SIGNAL(clicked()), this, SLOT(removeValue()));
    connect(ui->tblValues, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateValues(QTableWidgetItem*)));

    if(m_Type == NEVentCriteria::CriteriaType::Temperature)
    {
        ui->tblValues->horizontalHeaderItem(0)->setText("Temperature");
    }
    else{
        if(m_Type == NEVentCriteria::CriteriaType::Temperature)
        {
            ui->tblValues->horizontalHeaderItem(0)->setText("Time(s)");
        }
    }

    ui->tblValues->clearContents();

    ui->tblValues->blockSignals(true);
    for (int i = 0; i < ui->tblValues->rowCount(); ++i)
    {
        for (int j = 0; j < ui->tblValues->columnCount(); ++j)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            if(i < m_Values.size())
                item->setText(j == 0 ? QString::number(m_Values[i].m_Threshold) : QString::number(m_Values[i].m_Percent));
            item->setForeground(QBrush(Qt::white));
            ui->tblValues->setItem(i , j, item);

        }
    }
    ui->tblValues->blockSignals(false);

    drawGraph();


}


void FDYNAddVentValuesDialog::accept()
{
    QDialog::accept();

}

void FDYNAddVentValuesDialog::reject()
{
    QDialog::reject();

}

void FDYNAddVentValuesDialog::drawGraph()
{
    QVector<double> xx, yy;
    double upperX, upperY;
    upperX = upperY = 0.0;

    foreach (auto& value, m_Values)
    {
        xx.append(value.m_Threshold);
        yy.append(value.m_Percent);

        if( upperX < value.m_Threshold)
            upperX = value.m_Threshold;

        if( upperY < value.m_Percent)
            upperY = value.m_Percent;
    }

    QString xLabel = (m_Type == NEVentCriteria::Temperature) ? "Temperature" : "Time(s)";

    ui->plotGraph->drawPlot(xx, yy, upperX, upperY, "", xLabel, "%");
}


void FDYNAddVentValuesDialog::addValue()
{
    int rlast = ui->tblValues->rowCount();
    if(ui->tblValues->rowCount() <= m_Values.size())
        ui->tblValues->insertRow(rlast);

    for (int j = 0; j < ui->tblValues->columnCount(); ++j)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setForeground(QBrush(Qt::white));
        ui->tblValues->setItem(rlast , j, item);
    }

    drawGraph();


}


void FDYNAddVentValuesDialog::removeValue()
{
    int ridx = ui->tblValues->selectedItems().first()->row();
    ui->tblValues->removeRow(ridx);
    if( m_Values.size() > ridx)
        m_Values.removeAt(ridx);

}

void FDYNAddVentValuesDialog::updateValues(QTableWidgetItem* item)
{
    int ridx = ui->tblValues->row(item);
    int cidx = ui->tblValues->column(item);

    while (m_Values.size() <= ridx) {
        m_Values.append(NEVentCriteria::VentValues());
    }

    NEVentCriteria::VentValues vv = m_Values[ridx];

    QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

    int pos = 0;
    if( (pos = rx.indexIn(item->text(), pos)) != -1 )
    {
        float v = rx.cap(0).toFloat();

        if(cidx == 0)
        {
            vv.m_Threshold = v;
        }
        else{
            vv.m_Percent = v;
        }
    }
    else{
        if(cidx == 0)
        {
            vv.m_Threshold = 0.0f;
        }
        else{
            vv.m_Percent = 0.0f;
        }

    }



    m_Values.replace(ridx, vv);
    item->setText(QString::number( cidx == 0 ? m_Values[ridx].m_Threshold : m_Values[ridx].m_Percent));
}
