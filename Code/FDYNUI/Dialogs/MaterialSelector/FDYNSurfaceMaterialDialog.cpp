#include "FDYNSurfaceMaterialDialog.h"
#include "ui_FDYNSurfaceMaterialDialog.h"
#include "Managers/FDYNGUIManager.h"
#include "Base/NESceneManager.h"
#include "Mesh/NEZoneItem.h"
#include "Utility/NESurfaceMaterial.h"

FDYNSurfaceMaterialDialog::FDYNSurfaceMaterialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FDYNSurfaceMaterialDialog)
{
    ui->setupUi(this);
    setStyle( QStyleFactory::create( "Fusion" ) );

    QObject::connect(ui->addBtn, SIGNAL(clicked()), this, SLOT(addNewMaterial()));
    QObject::connect(ui->duplicateBtn, SIGNAL(clicked()), this, SLOT(duplicateMaterial()));
    QObject::connect(ui->removeBtn, SIGNAL(clicked()), this, SLOT(removeMaterial()));
    QObject::connect(ui->addDefaultBtn, SIGNAL(clicked(bool)), this, SLOT(addDefaultMaterials()));
    QObject::connect(ui->tblMaterialList->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(updateRow(const QItemSelection&, const QItemSelection&)));
    QObject::connect(ui->tblMaterialList, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(updateItem(QTableWidgetItem *)));

    QObject::connect(ui->btnRun, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(reject()));

    QObject::connect(this, SIGNAL(sendAddSurfaceMaterial(SurfaceMaterial)),
                     &FDYNGUI, SLOT(addSurfaceMaterial(SurfaceMaterial)));
    QObject::connect(this, SIGNAL(sendRemoveSurfaceMaterial(SurfaceMaterial)),
                     &FDYNGUI, SLOT(removeSurfaceMaterial(SurfaceMaterial)));
    QObject::connect(this, SIGNAL(sendAssignSurfaceMaterial(NENode *, SurfaceMaterial, QString)),
                     &FDYNGUI, SLOT(assignSurfaceMaterial(NENode *, SurfaceMaterial, QString)));
    QObject::connect(this, SIGNAL(sendSurfaceMaterialChanged(SurfaceMaterial)),
                     &FDYNGUI, SLOT(changeSurfaceMaterial(SurfaceMaterial)));

    m_prevSelectedIndex = 0;
    m_CurMaterial = nullptr;
}

FDYNSurfaceMaterialDialog::~FDYNSurfaceMaterialDialog()
{

}

QStringList FDYNSurfaceMaterialDialog::materialNameList()
{
    QStringList list;
    foreach (const SurfaceMaterial mtl, m_MaterialList)
    {
        list.push_back(mtl.m_Name);
    }
    return list;
}

void FDYNSurfaceMaterialDialog::showWithPropertyValues(QWidget *propertyBrowser, QVariant idx, QVariant val, QVariant type, QVariant vName)
{
    if(isVisible())
        return;

    m_CurrentMaterialKind = vName.value<QString>();

    NESurfaceMaterial mtl = val.value<NESurfaceMaterial>();

    for (int idx = 0; idx < m_MaterialList.size(); idx++)
    {
        SurfaceMaterial mat = m_MaterialList[idx];
        if(mat.m_ID == mtl.uniqueID())
        {
            int ridx = idx-1;
            if (ridx >= 0)
                ui->tblMaterialList->selectRow(ridx);
            break;
        }
    }

    show();
}

void FDYNSurfaceMaterialDialog::accept()
{
    if(m_MaterialList.size() < 0 || m_prevSelectedIndex <= 0)
        return;

    int ridx = m_prevSelectedIndex-1;
    ui->tblMaterialList->selectRow(ridx);
    QList<QObject *> selectedList;
    if(NEObject *obj = qobject_cast<NEObject*>(NESCENE.selectedObject()))
    {
        if(selectedList.indexOf(obj) == -1)
            selectedList.push_back(obj);
    }

    foreach (NEObject *obj, *NESCENE.selectedObjects())
    {
            if(selectedList.indexOf(obj) == -1)
                selectedList.push_back(obj);
    }

    foreach (QObject *obj, selectedList)
    {
        QVariant val = obj->property("Material");
        NESurfaceMaterial mtl = val.value<NESurfaceMaterial>();

        mtl.setName(m_CurMaterial->m_Name);
        mtl.setUniqueID(m_CurMaterial->m_ID);
        mtl.setIndex(m_CurMaterial->m_Index);
        mtl.setConductivity(m_CurMaterial->m_Conductivity);
        mtl.setHeat(m_CurMaterial->m_Heat);
        mtl.setDensity(m_CurMaterial->m_Density);
        mtl.setThickness(m_CurMaterial->m_Thickness);
        mtl.setEmissivity(m_CurMaterial->m_Emissivity);

        for (int i = 0; i < obj->metaObject()->classInfoCount(); ++i)
        {
            const QMetaClassInfo info = obj->metaObject()->classInfo(i);
            if( info.value() == m_CurrentMaterialKind)
            {
                QString strName(info.name());
                if(strName.startsWith("name-")){
                    strName = strName.split("name-")[1];
                    obj->setProperty(strName.toUtf8().data(), mtl);
                    break;
                }
            }
        }

//        if( qobject_cast< NENode * >( zitem ) )
//            emit sendAssignSurfaceMaterial( qobject_cast< NENode * >( zitem ), *m_CurMaterial, m_CurrentMaterialKind );
    }

    FDYNGUI.updatePropertyEditor();
}

void FDYNSurfaceMaterialDialog::reject()
{
    QDialog::reject();
}

void FDYNSurfaceMaterialDialog::setDefaultMaterialProps(SurfaceMaterial &mat)
{
    mat.m_Conductivity = 0.00016;
    mat.m_Heat = 0.9;
    mat.m_Density = 790;
    mat.m_Thickness = 0.016;
    mat.m_Emissivity = 0.9;
}

void FDYNSurfaceMaterialDialog::clear()
{
    ui->tblMaterialList->clearContents();
    m_MaterialList.clear();
}

void FDYNSurfaceMaterialDialog::addRowToTable(SurfaceMaterial &material)
{
    if(ui->tblMaterialList->rowCount() <= m_MaterialList.size())
        ui->tblMaterialList->insertRow(ui->tblMaterialList->rowCount());

    // Offset the index by one, because the null material (index = 0) is hidden from the table
    int rIdx = material.m_Index-1;

    ui->tblMaterialList->blockSignals(true);

    QTableWidgetItem *item = new QTableWidgetItem(material.m_Name);
    item->setForeground(QBrush(Qt::white));
    ui->tblMaterialList->setItem(rIdx, 0, item);

    item =  new QTableWidgetItem(QString::number(material.m_Conductivity));
    item->setForeground(QBrush(Qt::white));
    ui->tblMaterialList->setItem(rIdx, 1, item);

    item =  new QTableWidgetItem(QString::number(material.m_Heat));
    item->setForeground(QBrush(Qt::white));
    ui->tblMaterialList->setItem(rIdx, 2, item);

    item =  new QTableWidgetItem(QString::number(material.m_Density));
    item->setForeground(QBrush(Qt::white));
    ui->tblMaterialList->setItem(rIdx, 3, item);

    item =  new QTableWidgetItem(QString::number(material.m_Thickness));
    item->setForeground(QBrush(Qt::white));
    ui->tblMaterialList->setItem(rIdx, 4, item);

    item =  new QTableWidgetItem(QString::number(material.m_Emissivity));
    item->setForeground(QBrush(Qt::white));
    ui->tblMaterialList->setItem(rIdx, 5, item);

    ui->tblMaterialList->selectRow(rIdx);

    ui->tblMaterialList->blockSignals(false);

    update();
    ui->tblMaterialList->update();

    ui->tblMaterialList->viewport()->update();
}

void FDYNSurfaceMaterialDialog::updateRow(const QItemSelection& selected, const QItemSelection& deselected)
{
    if(selected.indexes().size() > 0)
    {
        int ridx = selected.indexes().first().row();
        int idx = ridx+1;
        if(idx < m_MaterialList.size())
        {
            m_CurMaterial = &m_MaterialList[idx];
            m_prevSelectedIndex = idx;
        }
        else{
            m_CurMaterial = nullptr;
        }
    }
}

void FDYNSurfaceMaterialDialog::updateItem(QTableWidgetItem *item)
{
    if(m_MaterialList.size() <= 0)
        return;

    int idx = ui->tblMaterialList->row(item)+1;

    if(idx < m_MaterialList.size())
    {
        ui->tblMaterialList->blockSignals(true);

        m_CurMaterial = &m_MaterialList[idx];
        int cidx = ui->tblMaterialList->column(item);
        if(cidx == 0)
        {
            if(m_CurMaterial->m_ID != 0)
                m_CurMaterial->m_Name = item->text();
            item->setText(m_CurMaterial->m_Name);
        }
        else if( cidx == 1)
        {
            QString unit("kW/(m.K)");
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurMaterial->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurMaterial->m_Conductivity = rx.cap(0).toFloat();
                else
                    m_CurMaterial->m_Conductivity = 0.0f;
            }
            item->setText(QString::number(m_CurMaterial->m_Conductivity));
        }
        else if( cidx == 2)
        {
            QString unit("kJ/(kg.K)");
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurMaterial->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurMaterial->m_Heat = rx.cap(0).toFloat();
                else
                    m_CurMaterial->m_Heat = 0.0f;
            }

            item->setText(QString::number(m_CurMaterial->m_Heat));
        }
        else if( cidx == 3)
        {
            QString unit = QString("kg/m")+QString(QChar(0x00B3));
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurMaterial->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurMaterial->m_Density = rx.cap(0).toFloat();
                else
                    m_CurMaterial->m_Density = 0.0f;
            }

            item->setText(QString::number(m_CurMaterial->m_Density));
        }
        else if( cidx == 4 )
        {
            QString unit("m");
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurMaterial->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurMaterial->m_Thickness = rx.cap(0).toFloat();
                else
                    m_CurMaterial->m_Thickness = 0.0f;
            }

            item->setText(QString::number(m_CurMaterial->m_Thickness));
        }
        else if( cidx == 5 )
        {
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurMaterial->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(item->text(), pos)) != -1 )
                    m_CurMaterial->m_Emissivity = rx.cap(0).toFloat();
                else
                    m_CurMaterial->m_Emissivity = 0.0f;
            }

            item->setText(QString::number(m_CurMaterial->m_Emissivity));
        }

        m_prevSelectedIndex = idx;

        ui->tblMaterialList->blockSignals(false);
    }
    else{
        m_CurMaterial = nullptr;

        ui->tblMaterialList->blockSignals(true);

        item->setText("");

        ui->tblMaterialList->blockSignals(false);
    }

//    if(selected.indexes().size() > 0)
//    {
//        int ridx = selected.indexes().first().row();
//        int idx = ridx+1;
//        if(idx < m_MaterialList.size())
//        {
//            m_CurMaterial = &m_MaterialList[idx];
//            setLineEdits(*m_CurMaterial);
//            m_prevSelectedIndex = idx;
//        }
//        else{
//            m_CurMaterial = nullptr;
//        }
//    }

    if(m_CurMaterial)
    {
        if(m_CurMaterial->m_ID != 0)
        {
            emit sendSurfaceMaterialChanged(*m_CurMaterial);

            FDYNGUI.updatePropertyEditor();
        }
    }
}


SurfaceMaterial FDYNSurfaceMaterialDialog::createNewMaterial()
{
    SurfaceMaterial mat;
    mat.m_Name = QString("New Material ") + QString::number(m_MaterialList.size());
    mat.m_ID = 0; // Default value, expected to be overwritten by the id assigned by the database
    mat.m_Index = m_MaterialList.size();
    setDefaultMaterialProps(mat);

    return mat;
}

SurfaceMaterial FDYNSurfaceMaterialDialog::createNullMaterial()
{
    SurfaceMaterial mat;
    mat.m_Name = QString("Null");
    mat.m_ID = 0;
    mat.m_Index = m_MaterialList.size();
    setDefaultMaterialProps(mat);

    return mat;
}

void FDYNSurfaceMaterialDialog::addNewMaterial()
{
    if(m_CurMaterial && m_CurMaterial->m_Name.isEmpty())
    {
        QMessageBox::warning(this, QString("FRI3D"),
                                     QString("Please fill material name."),
                                     QMessageBox::Yes);

        return;
    }

    SurfaceMaterial mat = createNewMaterial();
    addMaterial(mat);

    emit sendAddSurfaceMaterial(mat);

    FDYNGUI.updatePropertyEditor();
}

void FDYNSurfaceMaterialDialog::addMaterial(SurfaceMaterial& mat)
{
    m_MaterialList.push_back(mat);

    // Null material should not be visible in the table
    if (mat.m_Index > 0)
        addRowToTable(mat);
}

void FDYNSurfaceMaterialDialog::updateMaterial(SurfaceMaterial& mat)
{
    // Search for the material with the name given my newMat and update all the quantities by the value
    // in newMat
    for(int i = 0; i < m_MaterialList.size(); i++)
    {
        if( m_MaterialList[i].m_Name == mat.m_Name )
        {
            m_MaterialList[i] = mat;
            break;
        }
    }

    ui->tblMaterialList->viewport()->update();
}

void FDYNSurfaceMaterialDialog::duplicateMaterial()
{
    int ridx = ui->tblMaterialList->selectionModel()->selectedRows().first().row();
    int idx = ridx+1;
    if(idx >= m_MaterialList.size())
        idx = m_prevSelectedIndex;

    m_CurMaterial = &m_MaterialList[idx];

    if(m_CurMaterial->m_Name.isEmpty())
    {
        QMessageBox::warning(this, QString("FRI3D"),
                                     QString("Please fill material name."),
                                     QMessageBox::Yes);
        return;
    }

    SurfaceMaterial newMat;
    newMat = *m_CurMaterial;
    newMat.m_Index = m_MaterialList.size();

    newMat.m_Name = QString("New Material ") + QString::number(m_MaterialList.size());
    newMat.m_ID = 0;

    m_MaterialList.push_back(newMat);

    addRowToTable(newMat);

    emit sendAddSurfaceMaterial(newMat);

    FDYNGUI.updatePropertyEditor();
}

void FDYNSurfaceMaterialDialog::removeMaterial()
{
    if(m_MaterialList.size() <= 0)
        return;

    int ridx = ui->tblMaterialList->selectionModel()->selectedRows().first().row();
    int idx = ridx+1;
    if(idx >= m_MaterialList.size())
        idx = m_prevSelectedIndex;

    SurfaceMaterial mat = m_MaterialList[idx];

    m_MaterialList.removeAt(idx);
    ui->tblMaterialList->removeRow(ridx);
    if(ridx >= 1)
        ui->tblMaterialList->selectRow(ridx - 1);
    else if(ui->tblMaterialList->rowCount() > 0)
        ui->tblMaterialList->selectRow(ridx);
    else
    {
        m_CurMaterial = nullptr;
        ui->tblMaterialList->selectionModel()->clearSelection();
    }

    for(int i = idx; i < m_MaterialList.size(); i++)
        m_MaterialList[i].m_Index--;

    emit sendRemoveSurfaceMaterial(mat);

    FDYNGUI.updatePropertyEditor();
}

void FDYNSurfaceMaterialDialog::addDefaultMaterials()
{
    FDYNGUI.loadDefaultSurfaceMaterials();
}
