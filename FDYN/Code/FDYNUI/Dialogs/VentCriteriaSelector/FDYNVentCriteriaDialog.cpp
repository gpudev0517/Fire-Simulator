#include "FDYNVentCriteriaDialog.h"
#include "ui_FDYNVentCriteriaDialog.h"
#include "Managers/FDYNGUIManager.h"
#include "Base/NESceneManager.h"
#include "Mesh/NEZoneItem.h"
#include "Utility/NEVentCriteria.h"
#include "FDYNAddVentValuesDialog.h"

FDYNVentCriteriaDialog::FDYNVentCriteriaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FDYNVentCriteriaDialog)
{
    ui->setupUi(this);
    setStyle( QStyleFactory::create( "Fusion" ) );

    QObject::connect(ui->addBtn, SIGNAL(clicked()), this, SLOT(addVentCriteria()));
    QObject::connect(ui->duplicateBtn, SIGNAL(clicked()), this, SLOT(duplicateVentCriteria()));
    QObject::connect(ui->removeBtn, SIGNAL(clicked()), this, SLOT(removeVentCriteria()));
    QObject::connect(ui->tblVentCriteriaList->selectionModel(),
                     SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                     this, SLOT(updateRow(const QItemSelection&, const QItemSelection&)));
    QObject::connect(ui->tblVentCriteriaList, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateItem(QTableWidgetItem*)));

    QObject::connect(ui->btnRun, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(reject()));

    QObject::connect(this, SIGNAL(sendAddVentCriteria(VentCriteria)), &FDYNGUI,
                     SLOT(addVentCriteria(VentCriteria)));
    QObject::connect(this, SIGNAL(sendRemoveVentCriteria(VentCriteria)), &FDYNGUI,
                     SLOT(removeVentCriteria(VentCriteria)));
    QObject::connect(this, SIGNAL(sendAssignVentCriteria(NENode *, VentCriteria, QString)),
                     &FDYNGUI, SLOT(assignVentCriteria(NENode *, VentCriteria, QString)));

    m_prevSelectedIndex = 0;
    m_CurVentCriteria = nullptr;
}

FDYNVentCriteriaDialog::~FDYNVentCriteriaDialog()
{

}

QStringList FDYNVentCriteriaDialog::ventCriteriaList()
{
    QStringList list;
    foreach (const VentCriteria vent, m_VentCriteriaList)
    {
        list.push_back( vent.m_Name);
    }
    return list;
}

void FDYNVentCriteriaDialog::showWithPropertyValues(QWidget *propertyBrowser, QVariant idx, QVariant val, QVariant type, QVariant vName)
{
    if(isVisible())
        return;

    m_CurCriterionKind = vName.value<QString>();
    m_CurCriterionKind.remove(" ");

    NEVentCriteria criterion = val.value<NEVentCriteria>();

    for (int idx = 0; idx < m_VentCriteriaList.size(); idx++)
    {
        VentCriteria crit = m_VentCriteriaList[idx];
        if(crit.m_ID == criterion.ID())
        {
            int ridx = idx-1;
            if (ridx >= 0)
                ui->tblVentCriteriaList->selectRow(ridx);
            break;
        }
    }

    show();
}

void FDYNVentCriteriaDialog::accept()
{
    QList<QObject *> selectedList;
    if(NEZoneItem *zitem = qobject_cast<NEZoneItem*>(NESCENE.selectedObject()))
    {
        if(selectedList.indexOf(zitem) == -1)
            selectedList.push_back(zitem);
    }

    foreach (NEObject *obj, *NESCENE.selectedObjects())
    {
        if(NEZoneItem *zitem = qobject_cast<NEZoneItem*>(obj))
        {
            if(selectedList.indexOf(zitem) == -1)
                selectedList.push_back(zitem);
        }
    }

    foreach (QObject *zitem, selectedList)
    {
        QVariant val = zitem->property(m_CurCriterionKind.toStdString().c_str());
        NEVentCriteria criterion = val.value<NEVentCriteria>();

        criterion.setName(m_CurVentCriteria->m_Name);
        criterion.setID(m_CurVentCriteria->m_ID);
        criterion.setIndex(m_CurVentCriteria->m_Index);
        criterion.setType(m_CurVentCriteria->m_Type);
        criterion.setValues(m_CurVentCriteria->m_Values);

        zitem->setProperty(m_CurCriterionKind.toStdString().c_str(), criterion);

//        if( qobject_cast< NENode * >( zitem ) )
//            emit sendAssignVentCriteria( qobject_cast< NENode * >( zitem ), *m_CurVentCriteria);
    }

    FDYNGUI.updatePropertyEditor();
}

void FDYNVentCriteriaDialog::reject()
{
    QDialog::reject();
}

void FDYNVentCriteriaDialog::setDefaultVentCriteriaProps(VentCriteria &criterion)
{
    criterion.m_Values = QList<NEVentCriteria::VentValues>();
}

void FDYNVentCriteriaDialog::clear()
{
    ui->tblVentCriteriaList->clearContents();
    m_VentCriteriaList.clear();
}

void FDYNVentCriteriaDialog::addRowToTable(VentCriteria &criterion)
{
    if(ui->tblVentCriteriaList->rowCount() <= m_VentCriteriaList.size())
        ui->tblVentCriteriaList->insertRow(ui->tblVentCriteriaList->rowCount());

    // Offset the index by one, because the null vent criteria (index = 0) is hidden from the table
    int rIdx = criterion.m_Index-1;

    ui->tblVentCriteriaList->blockSignals(true);

    QTableWidgetItem *item = new QTableWidgetItem(criterion.m_Name);
    item->setForeground(QBrush(Qt::white));
    ui->tblVentCriteriaList->setItem(rIdx, 0, item);

    QComboBox *combo = new QComboBox();
    combo->addItem("Temperature");
    combo->addItem("Time");
    combo->setCurrentIndex(criterion.m_Type);
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(criteriaTypeChanged(int)));
    ui->tblVentCriteriaList->setCellWidget(rIdx, 1, combo);

    QPushButton *btn = new QPushButton();
    btn->setText("Edit");
    connect(btn, SIGNAL(clicked()), this, SLOT(criteriaValueClicked()));
    ui->tblVentCriteriaList->setCellWidget(rIdx, 2, btn);

    ui->tblVentCriteriaList->selectRow(rIdx);

    ui->tblVentCriteriaList->blockSignals(false);

    update();
    ui->tblVentCriteriaList->update();

    ui->tblVentCriteriaList->viewport()->update();
}

void FDYNVentCriteriaDialog::criteriaTypeChanged(int idx)
{
    if(m_CurVentCriteria->m_ID != 0){
        m_CurVentCriteria->m_Type = static_cast<NEVentCriteria::CriteriaType>(idx);
    }

    if(m_CurVentCriteria)
    {
        if(m_CurVentCriteria->m_ID != 0)
        {
            emit sendAddVentCriteria(*m_CurVentCriteria);

            FDYNGUI.updatePropertyEditor();
        }
    }
}

void FDYNVentCriteriaDialog::criteriaValueClicked()
{
    FDYNAddVentValuesDialog dlg(m_CurVentCriteria->m_Type, m_CurVentCriteria->m_Values, this);
    dlg.exec();
    m_CurVentCriteria->m_Values = dlg.m_Values;


//    if(m_CurVentCriteria->m_ID != 0){
//        m_CurVentCriteria->m_Type = static_cast<NEVentCriteria::CriteriaType>(idx);
//    }

    if(m_CurVentCriteria)
    {
        if(m_CurVentCriteria->m_ID != 0)
        {
            emit sendAddVentCriteria(*m_CurVentCriteria);

            FDYNGUI.updatePropertyEditor();
        }
    }
}

void FDYNVentCriteriaDialog::updateRow(const QItemSelection& selected, const QItemSelection& deselected)
{
    if(selected.indexes().size() > 0)
    {
        int ridx = selected.indexes().first().row();
        int idx = ridx+1;
        if(idx < m_VentCriteriaList.size())
        {
            ui->tblVentCriteriaList->blockSignals(true);

            m_CurVentCriteria = &m_VentCriteriaList[idx];
            m_prevSelectedIndex = idx;

            ui->tblVentCriteriaList->blockSignals(false);
        }
        else{
            m_CurVentCriteria = nullptr;
        }
    }

//    if(m_CurVentCriteria)
//    {
//        if(m_CurVentCriteria->m_ID != 0)
//    {
//            emit sendAddVentCriteria(*m_CurVentCriteria);

//            FDYNGUI.updatePropertyEditor();
//        }
//    }
}


void FDYNVentCriteriaDialog::updateItem(QTableWidgetItem *item)
{
    if(m_VentCriteriaList.size() <= 0)
        return;

    int idx = ui->tblVentCriteriaList->row(item)+1;

    if(idx < m_VentCriteriaList.size())
    {
        ui->tblVentCriteriaList->blockSignals(true);

        m_CurVentCriteria = &m_VentCriteriaList[idx];
        int cidx = ui->tblVentCriteriaList->column(item);
        if(cidx == 0)
        {
            if(m_CurVentCriteria->m_ID != 0)
                m_CurVentCriteria->m_Name = item->text();
            item->setText(m_CurVentCriteria->m_Name);
        }
        else if( cidx == 1)
        {
            QComboBox *cb = qobject_cast<QComboBox*>(ui->tblVentCriteriaList->cellWidget(idx, cidx));

            if(m_CurVentCriteria->m_ID != 0){
                m_CurVentCriteria->m_Type = static_cast<NEVentCriteria::CriteriaType>(cb->currentIndex());
            }
        }
        else if( cidx == 2){

        }

        m_prevSelectedIndex = idx;

        ui->tblVentCriteriaList->blockSignals(false);
    }
    else{
        m_CurVentCriteria = nullptr;

        ui->tblVentCriteriaList->blockSignals(true);

        item->setText("");

        ui->tblVentCriteriaList->blockSignals(false);
    }

    if(m_CurVentCriteria)
    {
        if(m_CurVentCriteria->m_ID != 0)
        {
            emit sendAddVentCriteria(*m_CurVentCriteria);

            FDYNGUI.updatePropertyEditor();
        }
    }
}


void FDYNVentCriteriaDialog::editedLineEdit()
{
//    QLineEdit *line = qobject_cast<QLineEdit *>(sender());

//    if(!line)
//        return;

//    if(line->text().isEmpty())
//    {
//        QMessageBox::warning(this, QString("FRI3D"),
//                                     QString("Text can't be empty string. please fill value."),
//                                     QMessageBox::Yes);
//        line->setFocus();
//        return;
//    }

//    if(m_CurVentCriteria == nullptr)
//        return;

//    int ridx = ui->tblVentCriteriaList->selectionModel()->selectedRows().first().row();
//    int idx = ridx+1;
//    if(idx >= m_VentCriteriaList.size())
//        idx = m_prevSelectedIndex;

//    if(QLineEdit *line = qobject_cast<QLineEdit *>(sender()))
//    {
//        ui->tblVentCriteriaList->blockSignals(true);

//        if(line == ui->lineCriteriaName)
//        {
//            if(m_CurVentCriteria ->m_ID != 0)
//                m_CurVentCriteria->m_Name = ui->lineCriteriaName->text();
//            ui->tblVentCriteriaList->item(ridx, 0)->setText(m_CurVentCriteria->m_Name);
//        }
//        else if( line == ui->lineThreshold)
//        {
//            QString unit("K");
//            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

//            if(m_CurVentCriteria ->m_ID != 0){
//                int pos = 0;
//                if( (pos = rx.indexIn(ui->lineThreshold->text(), pos)) != -1 )
//                    m_CurVentCriteria->m_Threshold = rx.cap(0).toFloat();
//                else
//                    m_CurVentCriteria->m_Threshold = 0.0f;
//            }

//            ui->lineThreshold->setText(QString::number(m_CurVentCriteria->m_Threshold) + QChar::Space + unit);
//            ui->tblVentCriteriaList->item(ridx, 1)->setText(QString::number(m_CurVentCriteria->m_Threshold));
//        }

//        ui->tblVentCriteriaList->blockSignals(false);
//    }

//    ui->tblVentCriteriaList->selectRow(ridx);

    if(m_CurVentCriteria->m_ID != 0)
        emit sendAddVentCriteria(*m_CurVentCriteria);

    FDYNGUI.updatePropertyEditor();
}

VentCriteria FDYNVentCriteriaDialog::createVentCriteria()
{
    VentCriteria criterion;
    criterion.m_Name = QString("New Vent Criterion ") + QString::number(m_VentCriteriaList.size());
    criterion.m_ID = 0;
    criterion.m_Index = m_VentCriteriaList.size();
    setDefaultVentCriteriaProps(criterion);

    return criterion;
}

VentCriteria FDYNVentCriteriaDialog::createNullVentCriteria()
{
    VentCriteria criterion;
    criterion.m_Name = QString("Null");
    criterion.m_ID = 0;
    criterion.m_Index = m_VentCriteriaList.size();
    setDefaultVentCriteriaProps(criterion);

    return criterion;
}

void FDYNVentCriteriaDialog::addVentCriteria()
{
    if(m_CurVentCriteria && m_CurVentCriteria->m_Name.isEmpty())
    {
        QMessageBox::warning(this, QString("FRI3D"),
                                     QString("Please fill criterion name."),
                                     QMessageBox::Yes);
        return;

    }

    VentCriteria criterion = createVentCriteria();
    addVentCriteria(criterion);

    emit sendAddVentCriteria(criterion);

    FDYNGUI.updatePropertyEditor();
}

void FDYNVentCriteriaDialog::addVentCriteria(VentCriteria& criterion)
{
    m_VentCriteriaList.push_back(criterion);

    // Null vent criterion should not be visible in the table
    if (criterion.m_Index > 0)
        addRowToTable(criterion);
}

void FDYNVentCriteriaDialog::updateVentCriteria(VentCriteria& criterion)
{
    // Search for the vent criterion with the name given my vent and update all the quantities by the value
    // in vent
    for(int i = 0; i < m_VentCriteriaList.size(); i++)
    {
        if( m_VentCriteriaList[i].m_Name == criterion.m_Name )
        {
            m_VentCriteriaList[i] = criterion;
            break;
        }
    }

    ui->tblVentCriteriaList->viewport()->update();
}

void FDYNVentCriteriaDialog::duplicateVentCriteria()
{

    int ridx = ui->tblVentCriteriaList->selectionModel()->selectedRows().first().row();
    int idx = ridx+1;
    if(idx >= m_VentCriteriaList.size())
        idx = m_prevSelectedIndex;

    m_CurVentCriteria = &m_VentCriteriaList[idx];

    if(m_CurVentCriteria->m_Name.isEmpty())
    {
        QMessageBox::warning(this, QString("FRI3D"),
                                     QString("Please fill vent name."),
                                     QMessageBox::Yes);
        return;

    }

    VentCriteria criterion;
    criterion = *m_CurVentCriteria;
    criterion.m_Index = m_VentCriteriaList.size();

    criterion.m_Name = QString("New Vent Criterion ") + QString::number(m_VentCriteriaList.size());
    criterion.m_ID = 0;

    m_VentCriteriaList.push_back(criterion);

    addRowToTable(criterion);

    emit sendAddVentCriteria(criterion);

    FDYNGUI.updatePropertyEditor();
}

void FDYNVentCriteriaDialog::removeVentCriteria()
{
    if(m_VentCriteriaList.size() <= 0)
        return;

    int ridx = ui->tblVentCriteriaList->selectionModel()->selectedRows().first().row();
    int idx = ridx+1;
    if(idx >= m_VentCriteriaList.size())
        idx = m_prevSelectedIndex;

    VentCriteria criterion = m_VentCriteriaList[idx];

    m_VentCriteriaList.removeAt(idx);
    ui->tblVentCriteriaList->removeRow(ridx);
    if(ridx >= 1)
        ui->tblVentCriteriaList->selectRow(ridx - 1);
    else if(ui->tblVentCriteriaList->rowCount() > 0)
        ui->tblVentCriteriaList->selectRow(ridx);
    else
    {
        m_CurVentCriteria = nullptr;
        ui->tblVentCriteriaList->selectionModel()->clearSelection();
    }

    for(int i = idx; i < m_VentCriteriaList.size(); i++)
        m_VentCriteriaList[i].m_Index--;

    emit sendRemoveVentCriteria(criterion);

    FDYNGUI.updatePropertyEditor();
}
