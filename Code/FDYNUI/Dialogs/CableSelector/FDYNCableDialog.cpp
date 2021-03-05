#include "FDYNCableDialog.h"
#include "ui_FDYNCableDialog.h"
#include "Managers/FDYNGUIManager.h"
#include "Windows/Main/FDYNMainWindow.h"
#include "Base/NESceneManager.h"
#include "Mesh/NEZoneItem.h"

ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex & index ) const
{
    if( index.column() == 2 || index.column() == 3 ) // insulation type or jacket type
    {
        QComboBox *combo = new QComboBox(parent);

        FDYNCableDialog *dlg = FDYNGUI.mainWindow()->cableMaterialDialog();

        if( index.column() == 2)
        {
            foreach (QString item, dlg->m_InsulationTypes)
            {
                combo->addItem(item);
            }
        }
        else
        {
            foreach (QString item, dlg->m_CategoryTypes)
            {
                combo->addItem(item);
            }
        }

        return combo;
//        QSpinBox *editor = new QSpinBox(parent);
//        editor->setFrame(false);
//        editor->setMinimum(0);
//        editor->setMaximum(100);

//        return editor;
    }
    else
    {
        QLineEdit *line = new QLineEdit(parent);
        return line;
    }
    return NULL;

}

void ComboBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    if( index.column() == 2 || index.column() == 3 ) // insulation type or jacket type
    {
        QString val = index.model()->data(index, Qt::EditRole).toString();
        QComboBox *comboBox = static_cast<QComboBox *>(editor);
        comboBox->setCurrentText(val);
    }
    else
    {
        QString val = index.model()->data(index, Qt::EditRole).toString();
        QLineEdit *line = static_cast<QLineEdit *>(editor);
        line->setText(val);
    }



//    int value = index.model()->data(index, Qt::EditRole).toInt();

//    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
//    spinBox->setValue(value);

}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    if( index.column() == 2 || index.column() == 3 ) // insulation type or jacket type
    {
        QComboBox *comboBox = static_cast<QComboBox *>(editor);
        QString val = comboBox->currentText();
        model->setData(index, val, Qt::EditRole);
    }
    else
    {
        QLineEdit *line = static_cast<QLineEdit *>(editor);
        QString val = line->text();
        model->setData(index, val, Qt::EditRole);
    }


//    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
//    spinBox->interpretText();
//    int value = spinBox->value();

//    model->setData(index, value, Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}


FDYNCableDialog::FDYNCableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FDYNCableDialog)
{
    ui->setupUi(this);
    setStyle( QStyleFactory::create( "Fusion" ) );

    QObject::connect(ui->btnAssign, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(reject()));

    QObject::connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(addCableProperty()));
    QObject::connect(ui->btnDuplicate, SIGNAL(clicked()), this, SLOT(duplicateCableProperty()));
    QObject::connect(ui->btnRemove, SIGNAL(clicked()), this, SLOT(removeCableProperty()));

    QObject::connect(ui->tblCable->selectionModel(),
                     SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                     this, SLOT(updateRow(const QItemSelection&, const QItemSelection&)));
    QObject::connect(ui->tblCable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateItem(QTableWidgetItem*)));
    //QObject::connect(ui->tblCable, SIGNAL(cellClicked(int,int)), this, SLOT(updateDynamicComboBox(int, int)));

    QObject::connect(ui->comboCategory, SIGNAL(currentIndexChanged(QString)), this,
                     SLOT(filterCategory(QString)));

    QObject::connect(ui->comboGauge, SIGNAL(currentIndexChanged(QString)), this,
                     SLOT(filterGauge(QString)));

    QObject::connect(ui->comboConductors, SIGNAL(currentIndexChanged(QString)), this,
                     SLOT(filterConductor(QString)));

    QObject::connect(this, SIGNAL(sendAddCableProperty(CableProperty)), &FDYNGUI,
                     SLOT(addCableProperty(CableProperty)));
    QObject::connect(this, SIGNAL(sendRemoveCableProperty(CableProperty)), &FDYNGUI,
                     SLOT(removeCableProperty(CableProperty)));
    QObject::connect(this, SIGNAL(sendAssignCableProperty(NENode*,CableProperty)),
                     &FDYNGUI, SLOT(assignCableProperty(NENode*,CableProperty)));

    m_prevSelectedIndex = 0;
    m_CurCableProperty = nullptr;

    ui->tblCable->setItemDelegate(new ComboBoxDelegate());

    m_CategoryTypes.clear();
    m_InsulationTypes.clear();
}

FDYNCableDialog::~FDYNCableDialog()
{

}

void FDYNCableDialog::filterByCombos(QString categoryFilter, QString gaugeFilter, QString conductorFilter)
{
    m_filterdCableIndexList.clear();
    foreach (CableProperty cable, m_CablePropertyList)
    {
        if ( (categoryFilter == "All" || cable.m_Category == categoryFilter) &&
             (gaugeFilter == "All" || cable.m_Gauges == gaugeFilter.toFloat())
             && (conductorFilter == "All" || cable.m_Conductors == conductorFilter.toFloat()))
        {
            m_filterdCableIndexList.push_back(cable.m_Index);
        }
    }
}


void FDYNCableDialog::filterCategory(QString index)
{
    filterByCombos(index, ui->comboGauge->currentText(), ui->comboConductors->currentText());
    ui->tblCable->blockSignals(true);

    for( int i =0 ; i < ui->tblCable->rowCount(); i++)
        ui->tblCable->hideRow(i);
    foreach (int index, m_filterdCableIndexList)
    {
        ui->tblCable->showRow(index-1);
    }
    ui->tblCable->blockSignals(false);
}


void FDYNCableDialog::filterGauge(QString index)
{
    filterByCombos(ui->comboCategory->currentText(), index, ui->comboConductors->currentText());
    ui->tblCable->blockSignals(true);

    for( int i =0 ; i < ui->tblCable->rowCount(); i++)
        ui->tblCable->hideRow(i);
    foreach (int index, m_filterdCableIndexList)
    {
        ui->tblCable->showRow(index-1);
    }
    ui->tblCable->blockSignals(false);
}


void FDYNCableDialog::filterConductor(QString index)
{
    filterByCombos(ui->comboCategory->currentText(), ui->comboGauge->currentText(), index);
    ui->tblCable->blockSignals(true);

    for( int i =0 ; i < ui->tblCable->rowCount(); i++)
        ui->tblCable->hideRow(i);
    foreach (int index, m_filterdCableIndexList)
    {
        ui->tblCable->showRow(index-1);
    }
    ui->tblCable->blockSignals(false);
}

QStringList FDYNCableDialog::cablePropertyList()
{
    QStringList list;
    foreach (const CableProperty cable, m_CablePropertyList)
    {
        list.push_back( cable.m_Name);
    }
    return list;
}

void FDYNCableDialog::showWithPropertyValues(QWidget *propertyBrowser, QVariant idx, QVariant val, QVariant type)
{
    if(isVisible())
        return;

//    m_CurrentMaterialKind = vName.value<QString>();

//    NESurfaceMaterial mtl = val.value<NESurfaceMaterial>();

//    for (int idx = 0; idx < m_MaterialList.size(); idx++)
//    {
//        SurfaceMaterial mat = m_MaterialList[idx];
//        if(mat.m_ID == mtl.uniqueID())
//        {
//            int ridx = idx-1;
//            if (ridx >= 0)
//                ui->tblMaterialList->selectRow(ridx);
//            break;
//        }
//    }

    show();
}

void FDYNCableDialog::accept()
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
//        if( NEZoneItem *nezitem = qobject_cast<NEZoneItem *>(zitem)){
//            if( nezitem->factory().nodeName() == "Cables" ){
                QVariant val = zitem->property("CableMaterial");
                NECableProperty cable = val.value<NECableProperty>();

                cable.setName(m_CurCableProperty->m_Name);
                cable.setUniqueID(m_CurCableProperty->m_ID);
                cable.setIndex(m_CurCableProperty->m_Index);
                cable.setModel(m_CurCableProperty->m_Model);
                cable.setInsulationType(m_CurCableProperty->m_InsulationType);
                cable.setJacketType(m_CurCableProperty->m_JacketType);
                cable.setOutsideDiameterMM(m_CurCableProperty->m_OutSideDiameterMM);
                cable.setOutSideDiameterKgPerMeter(m_CurCableProperty->m_OutSideDiameterKgPerMeter);
                cable.setJacketThicknessMM(m_CurCableProperty->m_JacketThicknessMM);
                cable.setGauges(m_CurCableProperty->m_Gauges);
                cable.setConductors(m_CurCableProperty->m_Conductors);
                cable.setDescription(m_CurCableProperty->m_Description);
                cable.setGrade(m_CurCableProperty->m_Grade);
                cable.setPart(m_CurCableProperty->m_Part);
                cable.setItem(m_CurCableProperty->m_Item);

                zitem->setProperty("CableMaterial", cable);

        //        if( qobject_cast< NENode * >( zitem ) )
        //            emit sendAssignFailureCriteria( qobject_cast< NENode * >( zitem ), *m_CurFailureCriteria );
//            }
//        }

    }

    FDYNGUI.updatePropertyEditor();
}

void FDYNCableDialog::reject()
{
    QDialog::reject();
}

void FDYNCableDialog::setDefaultCableProps(CableProperty &p)
{
    p.m_Manufacturer = "Manufacturer";
    p.m_Category = "Custom";
    p.m_Model = "Model";
    p.m_Issues = QString();
    p.m_InsulationType = 0;
    p.m_JacketType = 0;
    p.m_OutSideDiameterMM = 0.0;
    p.m_OutSideDiameterKgPerMeter = 0.0;
    p.m_JacketThicknessMM = 0.0;
    p.m_Gauges = 0.0;
    p.m_Conductors = 0.0;
    p.m_Description = "";
    p.m_Grade = 0.0;
    p.m_Part = 0.0;
    p.m_Item = 0.0;
}

void FDYNCableDialog::addRowToTable(CableProperty &cable)
{
    if(ui->tblCable->rowCount() <= m_CablePropertyList.size())
        ui->tblCable->insertRow(ui->tblCable->rowCount());

    // Offset the index by one, because the null cable property (index = 0) is hidden from the table
    int rIdx = cable.m_Index-1;

    ui->tblCable->blockSignals( true );

    QTableWidgetItem *item = new QTableWidgetItem(cable.m_Name);
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem( rIdx, 0, item );

    item = new QTableWidgetItem(cable.m_Model);
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem( rIdx, 1, item );

    item = new QTableWidgetItem(m_InsulationTypes.at(cable.m_InsulationType));
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem( rIdx, 2, item );

    item = new QTableWidgetItem(cable.m_Category);
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem(rIdx, 3, item);

    item = new QTableWidgetItem(QString::number(cable.m_OutSideDiameterMM));
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem(rIdx, 4, item);

    item = new QTableWidgetItem(QString::number(cable.m_OutSideDiameterKgPerMeter));
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem(rIdx, 5, item);

    item = new QTableWidgetItem(QString::number(cable.m_JacketThicknessMM));
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem(rIdx, 6, item);

    item = new QTableWidgetItem(QString::number(cable.m_Gauges));
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem(rIdx, 7, item);

    item = new QTableWidgetItem(QString::number(cable.m_Conductors));
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem(rIdx, 8, item);

    item = new QTableWidgetItem(cable.m_Description);
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem(rIdx, 9, item);

    item = new QTableWidgetItem(QString::number(cable.m_Grade));
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem(rIdx, 10, item);

    item = new QTableWidgetItem(QString::number(cable.m_Part));
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem(rIdx, 11, item);

    item = new QTableWidgetItem(QString::number(cable.m_Item));
    item->setForeground(QBrush(Qt::white));
    ui->tblCable->setItem(rIdx, 12, item);

    ui->tblCable->selectRow(rIdx);

    ui->tblCable->blockSignals(false);

    update();
    ui->tblCable->update();

    ui->tblCable->viewport()->update();
}

void FDYNCableDialog::clear()
{
    ui->tblCable->clearContents();
    m_CablePropertyList.clear();
}

void FDYNCableDialog::updateFilterCombos(std::vector<QList<QString>> options)
{
    ui->comboCategory->blockSignals(true);
    ui->comboCategory->clear();
    QStringList newOptionCategory;
    newOptionCategory.append("All");
    newOptionCategory.append(options[0]);
    ui->comboCategory->addItems(newOptionCategory);
    ui->comboCategory->blockSignals(false);

    ui->comboGauge->blockSignals(true);
    ui->comboGauge->clear();
    QStringList newOptionGauge;
    newOptionGauge.append("All");
    newOptionGauge.append(options[2]);
    ui->comboGauge->addItems(newOptionGauge);
    ui->comboGauge->blockSignals(false);

    ui->comboConductors->blockSignals(true);
    ui->comboConductors->clear();
    QStringList newOptionConductor;
    newOptionConductor.append("All");
    newOptionConductor.append(options[3]);
    ui->comboConductors->addItems(newOptionConductor);
    ui->comboConductors->blockSignals(false);

    m_CategoryTypes = options[0];
    m_InsulationTypes = options[1];
}

CableProperty FDYNCableDialog::createCableProperty()
{
    CableProperty cable;
    cable.m_Name = QString("New Cable Property ") + QString::number(m_CablePropertyList.size());
    cable.m_CleanName = cable.m_Name;
    cable.m_ID = 0;
    cable.m_Index = m_CablePropertyList.size();
    setDefaultCableProps(cable);

    return cable;
}

CableProperty FDYNCableDialog::createNullCableProperty()
{
    CableProperty cable;
    cable.m_Name = QString("Null");
    cable.m_CleanName = cable.m_Name;
    cable.m_ID = 0;
    cable.m_Index = m_CablePropertyList.size();
    setDefaultCableProps(cable);

    return cable;
}

void FDYNCableDialog::addCableProperty()
{
    if(m_CurCableProperty && m_CurCableProperty->m_Name.isEmpty())
    {
        QMessageBox msgWarning ;
        msgWarning.setText( QString("Please fill Cable property name.") );
        msgWarning.setIcon( QMessageBox::Warning );
        msgWarning.setWindowTitle(tr("FRI3D"));
        msgWarning.addButton( QMessageBox::Ok );
        msgWarning.setStyle( QStyleFactory::create ("Fusion" ) );

        msgWarning.exec();

        return;

    }

    CableProperty cable = createCableProperty();
    addCableProperty(cable);

    emit sendAddCableProperty(cable);

    FDYNGUI.updatePropertyEditor();
}

void FDYNCableDialog::addCableProperty(CableProperty& cable)
{
    m_CablePropertyList.push_back(cable);

    // Null cable property should not be visible in the table
    if (cable.m_Index > 0)
        addRowToTable(cable);
}

void FDYNCableDialog::updateCableProperty(CableProperty& cable)
{
    // Search for the cable property with the name given to cable and update all the quantities by the value
    // in cable
    for(int i = 0; i < m_CablePropertyList.size(); i++)
    {
        if( m_CablePropertyList[i].m_CleanName == cable.m_CleanName )
        {
            m_CablePropertyList[i] = cable;
            break;
        }
    }

    ui->tblCable->viewport()->update();
}


void FDYNCableDialog::duplicateCableProperty()
{

    int ridx = ui->tblCable->selectionModel()->selectedRows().first().row();
    int idx = ridx+1;
    if(idx >= m_CablePropertyList.size())
        idx = m_prevSelectedIndex;

    m_CurCableProperty = &m_CablePropertyList[idx];

    if(m_CurCableProperty->m_Name.isEmpty())
    {

        QMessageBox msgWarning ;
        msgWarning.setText( QString("Please fill Cable property name.") );
        msgWarning.setIcon( QMessageBox::Warning );
        msgWarning.setWindowTitle(tr("FRI3D"));
        msgWarning.addButton( QMessageBox::Ok );
        msgWarning.setStyle( QStyleFactory::create ("Fusion" ) );

        msgWarning.exec();
        return;

    }

    CableProperty cable;
    cable = *m_CurCableProperty;
    cable.m_Index = m_CablePropertyList.size();

    cable.m_Name = QString("New Cable Property ") + QString::number(m_CablePropertyList.size());
    cable.m_ID = 0;

    m_CablePropertyList.push_back(cable);

    addRowToTable(cable);

    emit sendAddCableProperty(cable);

    FDYNGUI.updatePropertyEditor();
}

void FDYNCableDialog::removeCableProperty()
{
    if(m_CablePropertyList.size() <= 1) //only null cable property
        return;

    int ridx = ui->tblCable->selectionModel()->selectedRows().first().row();
    int idx = ridx+1;
    if(idx >= m_CablePropertyList.size()){
        idx = m_prevSelectedIndex;
        ridx = idx - 1;
    }

    CableProperty cable = m_CablePropertyList[idx];

    m_CablePropertyList.removeAt(idx);
    ui->tblCable->removeRow(ridx);
    if(ridx >= 1)
        ui->tblCable->selectRow(ridx - 1);
    else if(ui->tblCable->rowCount() > 0)
        ui->tblCable->selectRow(ridx);
    else
    {
        m_CurCableProperty = nullptr;
        ui->tblCable->selectionModel()->clearSelection();
    }

    for(int i = idx; i < m_CablePropertyList.size(); i++)
        m_CablePropertyList[i].m_Index--;

    emit sendRemoveCableProperty(cable);

    FDYNGUI.updatePropertyEditor();
}

void FDYNCableDialog::updateRow(const QItemSelection& selected, const QItemSelection& deselected)
{
    if(selected.indexes().size() > 0)
    {
        int ridx = selected.indexes().first().row();
        int idx = ridx+1;
        if(idx < m_CablePropertyList.size())
        {
            ui->tblCable->blockSignals(true);

            m_CurCableProperty = &m_CablePropertyList[idx];
            m_prevSelectedIndex = idx;

            ui->tblCable->blockSignals(false);
        }
        else{
            m_CurCableProperty = nullptr;
        }
    }

//    if(m_CurCableProperty)
//    {
//        if(m_CurCableProperty->m_ID != 0){
//            emit sendAddCableProperty(*m_CurCableProperty);

//            FDYNGUI.updatePropertyEditor();
//        }
//    }
}


void FDYNCableDialog::updateItem(QTableWidgetItem *item)
{
    if(m_CablePropertyList.size() <= 0)
        return;

    int idx = ui->tblCable->row(item)+1;

    if(idx < m_CablePropertyList.size())
    {
        ui->tblCable->blockSignals(true);

        m_CurCableProperty = &m_CablePropertyList[idx];
        int cidx = ui->tblCable->column(item);
        if(cidx == 0)
        {
            if(m_CurCableProperty->m_ID != 0)
                m_CurCableProperty->m_Name = item->text();
            item->setText(m_CurCableProperty->m_Name);
        }
        if(cidx == 1)
        {
            if(m_CurCableProperty->m_ID != 0)
                m_CurCableProperty->m_Model= item->text();
            item->setText(m_CurCableProperty->m_Model);
        }
        else if( cidx == 2)
        {
//            QComboBox *cb = qobject_cast<QComboBox*>(ui->tblCable->cellWidget(idx, cidx));

//            if(m_CurCableProperty->m_ID != 0){
//                m_CurCableProperty->m_InsulationType = cb->currentIndex();
//            }

        }
        else if( cidx == 3)
        {
//            QComboBox *cb = qobject_cast<QComboBox*>(ui->tblCable->cellWidget(idx, cidx));

//            if(m_CurCableProperty->m_ID != 0){
//                m_CurCableProperty->m_JacketType = cb->currentIndex();
//            }

        }
        else if( cidx == 4)
        {
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurCableProperty->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurCableProperty->m_OutSideDiameterMM = rx.cap(0).toFloat();
                else
                    m_CurCableProperty->m_OutSideDiameterMM = 0.0f;
            }

            item->setText(QString::number(m_CurCableProperty->m_OutSideDiameterMM));
        }
        else if( cidx == 5)
        {
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurCableProperty->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurCableProperty->m_OutSideDiameterKgPerMeter = rx.cap(0).toFloat();
                else
                    m_CurCableProperty->m_OutSideDiameterKgPerMeter = 0.0f;
            }

            item->setText(QString::number(m_CurCableProperty->m_OutSideDiameterKgPerMeter));
        }
        else if( cidx == 6)
        {
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurCableProperty->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurCableProperty->m_JacketThicknessMM = rx.cap(0).toFloat();
                else
                    m_CurCableProperty->m_JacketThicknessMM = 0.0f;
            }

            item->setText(QString::number(m_CurCableProperty->m_JacketThicknessMM));
        }
        else if( cidx == 7)
        {
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurCableProperty->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurCableProperty->m_Gauges = rx.cap(0).toFloat();
                else
                    m_CurCableProperty->m_Gauges = 0.0f;
            }

            item->setText(QString::number(m_CurCableProperty->m_Gauges));
        }
        else if( cidx == 8)
        {
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurCableProperty->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurCableProperty->m_Conductors = rx.cap(0).toFloat();
                else
                    m_CurCableProperty->m_Conductors = 0.0f;
            }

            item->setText(QString::number(m_CurCableProperty->m_Conductors));
        }
        else if(cidx == 9)
        {
            if(m_CurCableProperty->m_ID != 0)
                m_CurCableProperty->m_Description= item->text();
            item->setText(m_CurCableProperty->m_Description);
        }
        else if( cidx == 10)
        {
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurCableProperty->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurCableProperty->m_Grade = rx.cap(0).toFloat();
                else
                    m_CurCableProperty->m_Grade = 0.0f;
            }

            item->setText(QString::number(m_CurCableProperty->m_Grade));
        }
        else if( cidx == 11)
        {
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurCableProperty->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurCableProperty->m_Part = rx.cap(0).toFloat();
                else
                    m_CurCableProperty->m_Part = 0.0f;
            }

            item->setText(QString::number(m_CurCableProperty->m_Part));
        }
        else if( cidx == 12)
        {
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurCableProperty->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurCableProperty->m_Item = rx.cap(0).toFloat();
                else
                    m_CurCableProperty->m_Item = 0.0f;
            }

            item->setText(QString::number(m_CurCableProperty->m_Item));
        }

        m_prevSelectedIndex = idx;

        ui->tblCable->blockSignals(false);
    }
    else{
        m_CurCableProperty = nullptr;

        ui->tblCable->blockSignals(true);

        item->setText("");

        ui->tblCable->blockSignals(false);
    }

    if(m_CurCableProperty)
    {
        if(m_CurCableProperty->m_ID != 0)
            emit sendAddCableProperty(*m_CurCableProperty);

        FDYNGUI.updatePropertyEditor();
    }
}

void FDYNCableDialog::insulationTypeChanged(int _idx)
{
    if(m_CurCableProperty->m_ID != 0){
        m_CurCableProperty->m_InsulationType= _idx;
    }

    if(m_CurCableProperty)
    {
        if(m_CurCableProperty->m_ID != 0)
            emit sendAddCableProperty(*m_CurCableProperty);

        FDYNGUI.updatePropertyEditor();
    }
}

void FDYNCableDialog::jacketTypeChanged(int _idx)
{
    if(m_CurCableProperty->m_ID != 0){
        m_CurCableProperty->m_JacketType= _idx;
    }

    if(m_CurCableProperty)
    {
        if(m_CurCableProperty->m_ID != 0)
            emit sendAddCableProperty(*m_CurCableProperty);

        FDYNGUI.updatePropertyEditor();
    }
}

void FDYNCableDialog::categoryTypeChanged(int _idx)
{

}

void FDYNCableDialog::gaugeTypeChanged(int _idx)
{

}

void FDYNCableDialog::conductorsTypeChanged(int _idx)
{

}

CableProperty::operator NECableProperty()
{
    NECableProperty sm;


    sm.setName( m_Name );
    sm.setCleanName( m_CleanName );

    sm.setUniqueID( m_ID );
    sm.setIndex( m_Index );
    sm.setCategory( m_Category );
    sm.setIssues( m_Issues );
    sm.setManufacturer( m_Manufacturer );

    sm.setModel(m_Model);
    sm.setInsulationType(m_InsulationType);
    sm.setJacketType(m_JacketType);
    sm.setOutsideDiameterMM(m_OutSideDiameterMM);
    sm.setOutSideDiameterKgPerMeter(m_OutSideDiameterKgPerMeter);
    sm.setJacketThicknessMM(m_JacketThicknessMM);
    sm.setGauges(m_Gauges);
    sm.setConductors(m_Conductors);
    sm.setDescription(m_Description);
    sm.setGrade(m_Grade);
    sm.setPart(m_Part);
    sm.setItem(m_Item);

    return sm;

}

CableProperty::CableProperty(const NECableProperty &mtl)
{
    copy(mtl);
}

void CableProperty::operator =(const NECableProperty &mtl)
{
    copy( mtl );
}

void CableProperty::copy(const NECableProperty &mtl)
{
    m_Name = mtl.name();
    m_ID = mtl.uniqueID();
    m_Index = mtl.index();

    m_Manufacturer = mtl.manufacturer();
    m_Category     = mtl.category();
    m_Issues       = mtl.issues();
    m_CleanName     = mtl.cleanName();

    m_Model = mtl.model();
    m_InsulationType = mtl.insulationType();
    m_JacketType = mtl.jacketType();
    m_OutSideDiameterMM = mtl.outSideDiameterMM();
    m_OutSideDiameterKgPerMeter= mtl.outSideDiameterKgPerMeter();
    m_JacketThicknessMM = mtl.jacketThicknesMM();
    m_Gauges = mtl.gauges();
    m_Conductors = mtl.conductors();
    m_Description = mtl.description();
    m_Grade = mtl.grade();
    m_Part = mtl.part();
    m_Item = mtl.item();

}
