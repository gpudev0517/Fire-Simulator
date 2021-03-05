#include "FDYNFailureCriteriaDialog.h"
#include "ui_FDYNFailureCriteriaDialog.h"
#include "Managers/FDYNGUIManager.h"
#include "Base/NESceneManager.h"
#include "Mesh/NEZoneItem.h"
#include "Utility/NEFailureCriteria.h"

FDYNFailureCriteriaDialog::FDYNFailureCriteriaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FDYNFailureCriteriaDialog)
{
    ui->setupUi(this);
    setStyle( QStyleFactory::create( "Fusion" ) );

    QObject::connect(ui->addBtn, SIGNAL(clicked()), this, SLOT(addFailureCriteria()));
    QObject::connect(ui->duplicateBtn, SIGNAL(clicked()), this, SLOT(duplicateFailureCriteria()));
    QObject::connect(ui->removeBtn, SIGNAL(clicked()), this, SLOT(removeFailureCriteria()));
    QObject::connect(ui->tblFailureCriteriaList->selectionModel(),
                     SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                     this, SLOT(updateRow(const QItemSelection&, const QItemSelection&)));
    QObject::connect(ui->tblFailureCriteriaList, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateItem(QTableWidgetItem*)));

    QObject::connect(ui->btnRun, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(reject()));

    QObject::connect(this, SIGNAL(sendAddFailureCriteria(FailureCriteria)), &FDYNGUI,
                     SLOT(addFailureCriteria(FailureCriteria)));
    QObject::connect(this, SIGNAL(sendRemoveFailureCriteria(FailureCriteria)), &FDYNGUI,
                     SLOT(removeFailureCriteria(FailureCriteria)));
    QObject::connect(this, SIGNAL(sendAssignFailureCriteria(NENode *, FailureCriteria)),
                     &FDYNGUI, SLOT(assignFailureCriteria(NENode *, FailureCriteria)));

    m_prevSelectedIndex = 0;
    m_CurFailureCriteria = nullptr;
}

FDYNFailureCriteriaDialog::~FDYNFailureCriteriaDialog()
{

}

QStringList FDYNFailureCriteriaDialog::failureCriteriaList()
{
    QStringList list;
    foreach (const FailureCriteria fail, m_FailureCriteriaList)
    {
        list.push_back( fail.m_Name);
    }
    return list;
}

void FDYNFailureCriteriaDialog::showWithPropertyValues(QWidget *propertyBrowser, QVariant idx,
                                                       QVariant val, QVariant type)
{
    if(isVisible())
        return;

    NEFailureCriteria failure = val.value<NEFailureCriteria>();

    for (int idx = 0; idx < m_FailureCriteriaList.size(); idx++)
    {
        FailureCriteria fail = m_FailureCriteriaList[idx];
        if(fail.m_ID == failure.ID())
        {
            int ridx = idx-1;
            if (ridx >= 0)
                ui->tblFailureCriteriaList->selectRow(ridx);
            break;
        }
    }

    show();
}

void FDYNFailureCriteriaDialog::accept()
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
        QVariant val = zitem->property("Failure");
        NEFailureCriteria failure = val.value<NEFailureCriteria>();
        failure.setName(m_CurFailureCriteria->m_Name);
        failure.setID(m_CurFailureCriteria->m_ID);
        failure.setIndex(m_CurFailureCriteria->m_Index);
        failure.setType(m_CurFailureCriteria->m_Type);
        failure.setThreshold(m_CurFailureCriteria->m_Threshold);

        zitem->setProperty("Failure", failure);

//        if( qobject_cast< NENode * >( zitem ) )
//            emit sendAssignFailureCriteria( qobject_cast< NENode * >( zitem ), *m_CurFailureCriteria );
    }

    FDYNGUI.updatePropertyEditor();
}

void FDYNFailureCriteriaDialog::reject()
{
    QDialog::reject();
}

void FDYNFailureCriteriaDialog::setDefaultFailureCriteriaProps(FailureCriteria &fail)
{
    fail.m_Threshold = 100.0;
}

void FDYNFailureCriteriaDialog::clear()
{
    ui->tblFailureCriteriaList->clearContents();
    m_FailureCriteriaList.clear();
}

void FDYNFailureCriteriaDialog::addRowToTable(FailureCriteria &fail)
{
    if(ui->tblFailureCriteriaList->rowCount() <= m_FailureCriteriaList.size())
        ui->tblFailureCriteriaList->insertRow(ui->tblFailureCriteriaList->rowCount());

    // Offset the index by one, because the null failure criteria (index = 0) is hidden from the table
    int rIdx = fail.m_Index-1;

    ui->tblFailureCriteriaList->blockSignals(true);

    QTableWidgetItem *item = new QTableWidgetItem(fail.m_Name);
    item->setForeground(QBrush(Qt::white));
    ui->tblFailureCriteriaList->setItem(rIdx, 0, item);

    item =  new QTableWidgetItem(QString::number(fail.m_Threshold));
    item->setForeground(QBrush(Qt::white));
    ui->tblFailureCriteriaList->setItem(rIdx, 2, item);

    QComboBox *combo = new QComboBox();
    combo->addItem("Temperature");
    combo->addItem("Time");
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(criteriaTypeChanged(int)));
    ui->tblFailureCriteriaList->setCellWidget(rIdx, 1, combo);

    ui->tblFailureCriteriaList->selectRow(rIdx);

    ui->tblFailureCriteriaList->blockSignals(false);

    update();
    ui->tblFailureCriteriaList->update();

    ui->tblFailureCriteriaList->viewport()->update();
}

void FDYNFailureCriteriaDialog::criteriaTypeChanged(int idx)
{
    if(m_CurFailureCriteria->m_ID != 0){
        m_CurFailureCriteria->m_Type = static_cast<NEFailureCriteria::CriteriaType>(idx);
    }

    if(m_CurFailureCriteria)
    {
        if(m_CurFailureCriteria->m_ID != 0)
            emit sendAddFailureCriteria(*m_CurFailureCriteria);

        FDYNGUI.updatePropertyEditor();
    }
}
void FDYNFailureCriteriaDialog::updateRow(const QItemSelection& selected, const QItemSelection& deselected)
{
    if(selected.indexes().size() > 0)
    {
        int ridx = selected.indexes().first().row();
        int idx = ridx+1;
        if(idx < m_FailureCriteriaList.size())
        {
            ui->tblFailureCriteriaList->blockSignals(true);

            m_CurFailureCriteria = &m_FailureCriteriaList[idx];
            m_prevSelectedIndex = idx;

            ui->tblFailureCriteriaList->blockSignals(false);
        }
        else{
            m_CurFailureCriteria = nullptr;
        }
    }

//    if(m_CurFailureCriteria)
//    {
//        if(m_CurFailureCriteria->m_ID != 0){
//            emit sendAddFailureCriteria(*m_CurFailureCriteria);

//            FDYNGUI.updatePropertyEditor();
//        }
//    }
}


void FDYNFailureCriteriaDialog::updateItem(QTableWidgetItem *item)
{
    if(m_FailureCriteriaList.size() <= 0)
        return;

    int idx = ui->tblFailureCriteriaList->row(item)+1;

    if(idx < m_FailureCriteriaList.size())
    {
        ui->tblFailureCriteriaList->blockSignals(true);

        m_CurFailureCriteria = &m_FailureCriteriaList[idx];
        int cidx = ui->tblFailureCriteriaList->column(item);
        if(cidx == 0)
        {
            if(m_CurFailureCriteria->m_ID != 0)
                m_CurFailureCriteria->m_Name = item->text();
            item->setText(m_CurFailureCriteria->m_Name);
        }
        else if( cidx == 1)
        {
            QComboBox *cb = qobject_cast<QComboBox*>(ui->tblFailureCriteriaList->cellWidget(idx, cidx));

            if(m_CurFailureCriteria->m_ID != 0){
                m_CurFailureCriteria->m_Type = static_cast<NEFailureCriteria::CriteriaType>(cb->currentIndex());
            }

        }
        else if( cidx == 2)
        {
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurFailureCriteria->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurFailureCriteria->m_Threshold = rx.cap(0).toFloat();
                else
                    m_CurFailureCriteria->m_Threshold = 0.0f;
            }

            item->setText(QString::number(m_CurFailureCriteria->m_Threshold));
        }

        m_prevSelectedIndex = idx;

        ui->tblFailureCriteriaList->blockSignals(false);
    }
    else{
        m_CurFailureCriteria = nullptr;

        ui->tblFailureCriteriaList->blockSignals(true);

        item->setText("");

        ui->tblFailureCriteriaList->blockSignals(false);
    }

    if(m_CurFailureCriteria)
    {
        if(m_CurFailureCriteria->m_ID != 0)
            emit sendAddFailureCriteria(*m_CurFailureCriteria);

        FDYNGUI.updatePropertyEditor();
    }
}


void FDYNFailureCriteriaDialog::editedLineEdit()
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

//    if(m_CurFailureCriteria == nullptr)
//        return;

//    int ridx = ui->tblFailureCriteriaList->selectionModel()->selectedRows().first().row();
//    int idx = ridx+1;
//    if(idx >= m_FailureCriteriaList.size())
//        idx = m_prevSelectedIndex;

//    if(QLineEdit *line = qobject_cast<QLineEdit *>(sender()))
//    {
//        ui->tblFailureCriteriaList->blockSignals(true);

//        if(line == ui->lineCriteriaName)
//        {
//            if(m_CurFailureCriteria ->m_ID != 0)
//                m_CurFailureCriteria->m_Name = ui->lineCriteriaName->text();
//            ui->tblFailureCriteriaList->item(ridx, 0)->setText(m_CurFailureCriteria->m_Name);
//        }
//        else if( line == ui->lineThreshold)
//        {
//            QString unit("K");
//            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

//            if(m_CurFailureCriteria ->m_ID != 0){
//                int pos = 0;
//                if( (pos = rx.indexIn(ui->lineThreshold->text(), pos)) != -1 )
//                    m_CurFailureCriteria->m_Threshold = rx.cap(0).toFloat();
//                else
//                    m_CurFailureCriteria->m_Threshold = 0.0f;
//            }

//            ui->lineThreshold->setText(QString::number(m_CurFailureCriteria->m_Threshold) + QChar::Space + unit);
//            ui->tblFailureCriteriaList->item(ridx, 1)->setText(QString::number(m_CurFailureCriteria->m_Threshold));
//        }

//        ui->tblFailureCriteriaList->blockSignals(false);
//    }

//    ui->tblFailureCriteriaList->selectRow(ridx);

    if(m_CurFailureCriteria->m_ID != 0)
        emit sendAddFailureCriteria(*m_CurFailureCriteria);

    FDYNGUI.updatePropertyEditor();
}

FailureCriteria FDYNFailureCriteriaDialog::createFailureCriteria()
{
    FailureCriteria fail;
    fail.m_Name = QString("New Failure Criterion ") + QString::number(m_FailureCriteriaList.size());
    fail.m_ID = 0;
    fail.m_Index = m_FailureCriteriaList.size();
    setDefaultFailureCriteriaProps(fail);

    return fail;
}

FailureCriteria FDYNFailureCriteriaDialog::createNullFailureCriteria()
{
    FailureCriteria fail;
    fail.m_Name = QString("Null");
    fail.m_ID = 0;
    fail.m_Index = m_FailureCriteriaList.size();
    setDefaultFailureCriteriaProps(fail);

    return fail;
}

void FDYNFailureCriteriaDialog::addFailureCriteria()
{
    if(m_CurFailureCriteria && m_CurFailureCriteria->m_Name.isEmpty())
    {
        QMessageBox msgWarning ;
        msgWarning.setText( QString("Please fill failure criterion name.") );
        msgWarning.setIcon( QMessageBox::Warning );
        msgWarning.setWindowTitle(tr("FRI3D"));
        msgWarning.addButton( QMessageBox::Ok );
        msgWarning.setStyle( QStyleFactory::create ("Fusion" ) );
        msgWarning.exec();

        return;

    }

    FailureCriteria fail = createFailureCriteria();
    addFailureCriteria(fail);

    emit sendAddFailureCriteria(fail);

    FDYNGUI.updatePropertyEditor();
}

void FDYNFailureCriteriaDialog::addFailureCriteria(FailureCriteria& fail)
{
    m_FailureCriteriaList.push_back(fail);

    // Null failure criterion should not be visible in the table
    if (fail.m_Index > 0)
        addRowToTable(fail);
}

void FDYNFailureCriteriaDialog::updateFailureCriteria(FailureCriteria& fail)
{
    // Search for the failure criterion with the name given my fail and update all the quantities by the value
    // in fail
    for(int i = 0; i < m_FailureCriteriaList.size(); i++)
    {
        if( m_FailureCriteriaList[i].m_Name == fail.m_Name )
        {
            m_FailureCriteriaList[i] = fail;
            break;
        }
    }

    ui->tblFailureCriteriaList->viewport()->update();
}

void FDYNFailureCriteriaDialog::duplicateFailureCriteria()
{

    int ridx = ui->tblFailureCriteriaList->selectionModel()->selectedRows().first().row();
    int idx = ridx+1;
    if(idx >= m_FailureCriteriaList.size())
        idx = m_prevSelectedIndex;

    m_CurFailureCriteria = &m_FailureCriteriaList[idx];

    if(m_CurFailureCriteria->m_Name.isEmpty())
    {

        QMessageBox msgWarning ;
        msgWarning.setText( QString("Please fill failure  name.") );
        msgWarning.setIcon( QMessageBox::Warning );
        msgWarning.setWindowTitle(tr("FRI3D"));
        msgWarning.addButton( QMessageBox::Ok );
        msgWarning.setStyle( QStyleFactory::create ("Fusion" ) );
        msgWarning.exec();
        return;

    }

    FailureCriteria fail;
    fail = *m_CurFailureCriteria;
    fail.m_Index = m_FailureCriteriaList.size();

    fail.m_Name = QString("New Failure Criterion ") + QString::number(m_FailureCriteriaList.size());
    fail.m_ID = 0;

    m_FailureCriteriaList.push_back(fail);

    addRowToTable(fail);

    emit sendAddFailureCriteria(fail);

    FDYNGUI.updatePropertyEditor();
}

void FDYNFailureCriteriaDialog::removeFailureCriteria()
{
    if(m_FailureCriteriaList.size() <= 0)
        return;

    int ridx = ui->tblFailureCriteriaList->selectionModel()->selectedRows().first().row();
    int idx = ridx+1;
    if(idx >= m_FailureCriteriaList.size())
        idx = m_prevSelectedIndex;

    FailureCriteria fail = m_FailureCriteriaList[idx];

    m_FailureCriteriaList.removeAt(idx);
    ui->tblFailureCriteriaList->removeRow(ridx);
    if(ridx >= 1)
        ui->tblFailureCriteriaList->selectRow(ridx - 1);
    else if(ui->tblFailureCriteriaList->rowCount() > 0)
        ui->tblFailureCriteriaList->selectRow(ridx);
    else
    {
        m_CurFailureCriteria = nullptr;
        ui->tblFailureCriteriaList->selectionModel()->clearSelection();
    }

    for(int i = idx; i < m_FailureCriteriaList.size(); i++)
        m_FailureCriteriaList[i].m_Index--;

    emit sendRemoveFailureCriteria(fail);

    FDYNGUI.updatePropertyEditor();
}
