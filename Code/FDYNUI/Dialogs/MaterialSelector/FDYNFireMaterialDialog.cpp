#include "FDYNFireMaterialDialog.h"
#include "ui_FDYNFireMaterialDialog.h"
#include "FDYNFireAddT2Dialog.h"
#include "Managers/FDYNGUIManager.h"
#include "Base/NESceneManager.h"
#include "Mesh/NEZoneItem.h"
#include "Utility/NEFireMaterial.h"
#include "FDYNHRRCurveWidget.h"

FDYNFireMaterialDialog::FDYNFireMaterialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FDYNFireMaterialDialog)
{
    ui->setupUi(this);
    setStyle( QStyleFactory::create( "Fusion" ) );

    QObject::connect(ui->btnRun, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(reject()));

    QObject::connect(ui->lineFireID, SIGNAL(editingFinished()), this, SLOT(editedLineEdit()));
    QObject::connect(ui->lineC, SIGNAL(editingFinished()), this, SLOT(editedLineEdit()));
    QObject::connect(ui->lineN, SIGNAL(editingFinished()), this, SLOT(editedLineEdit()));
    QObject::connect(ui->lineH, SIGNAL(editingFinished()), this, SLOT(editedLineEdit()));
    QObject::connect(ui->lineCl, SIGNAL(editingFinished()), this, SLOT(editedLineEdit()));
    QObject::connect(ui->lineO, SIGNAL(editingFinished()), this, SLOT(editedLineEdit()));
    QObject::connect(ui->lineCombuHeat, SIGNAL(editingFinished()), this, SLOT(editedLineEdit()));
    QObject::connect(ui->lineRadFraction, SIGNAL(editingFinished()), this, SLOT(editedLineEdit()));
    QObject::connect(ui->tableFireMaterials, SIGNAL(cellChanged(int, int )), this, SLOT(materialInfoChanged(int, int)));

    QObject::connect(ui->btnAddNew, SIGNAL(clicked()), this, SLOT(addNewMaterial()));
    QObject::connect(ui->btnRemove, SIGNAL(clicked()), this, SLOT(removeMaterial()));
    QObject::connect(ui->btnAddT2, SIGNAL(clicked()), this, SLOT(addT2()));
    QObject::connect(ui->listMaterial, SIGNAL(currentRowChanged(int)), this, SLOT(updateMaterial(int)));
    //QObject::connect(ui->comboRefFireID, SIGNAL(currentIndexChanged(int)), this, SLOT(updateMaterial(int)));
    QObject::connect(ui->tableFireMaterials->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(updateRow(const QItemSelection&, const QItemSelection&)));

    QObject::connect(this, SIGNAL(sendAddFireMaterial(FireMaterial)), &FDYNGUI, SLOT(addFireMaterial(FireMaterial)));
    QObject::connect(this, SIGNAL(sendRemoveFireMaterial(FireMaterial)), &FDYNGUI, SLOT(removeFireMaterial(FireMaterial)));
    QObject::connect(this, SIGNAL(sendAssignFireMaterial(NENode *, FireMaterial)),
                     &FDYNGUI, SLOT(assignFireMaterial(NENode *, FireMaterial)));

    m_CurMaterial = nullptr;

    for( int j = 0; j < ui->tableFireMaterials->columnCount(); j++)
    {
        for (int i = 0; i < ui->tableFireMaterials->rowCount(); i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setForeground(QBrush(QColor(255, 255, 255)));
            ui->tableFireMaterials->setItem(i, j, item);
        }
    }
}

FDYNFireMaterialDialog::~FDYNFireMaterialDialog()
{

}

QStringList FDYNFireMaterialDialog::materialNameList()
{
    QStringList list;
    foreach (const FireMaterial mtl, m_MaterialList)
    {
        list.push_back(mtl.m_Name);
    }
    return list;
}

void FDYNFireMaterialDialog::showWithPropertyValues(QWidget *propertyBrowser, QVariant idx, QVariant val, QVariant type)
{
    if(isVisible())
        return;

    NEFireMaterial mtl = val.value<NEFireMaterial>();

    for (int idx = 0; idx < m_MaterialList.size(); idx++)
    {
        FireMaterial mat = m_MaterialList[idx];
        if(mat.m_ID == mtl.uniqueID())
        {
            int ridx = idx-1;
            if (ridx >= 0)
                ui->listMaterial->setCurrentRow(ridx);
            //ui->comboRefFireID->setCurrentIndex(ridx);
            break;
        }
    }

    updateControlsState();

    show();
}

void FDYNFireMaterialDialog::accept()
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
        QVariant val = zitem->property("FireMaterial");
        NEFireMaterial mtl = val.value<NEFireMaterial>();

        mtl.setName(m_CurMaterial->m_Name);
        mtl.setUniqueID(m_CurMaterial->m_ID);
        mtl.setIndex(m_CurMaterial->m_Index);
        mtl.setC(m_CurMaterial->m_C);
        mtl.setN(m_CurMaterial->m_N);
        mtl.setH(m_CurMaterial->m_H);
        mtl.setCI(m_CurMaterial->m_Cl);
        mtl.setO(m_CurMaterial->m_O);
        mtl.setCombuHeat(m_CurMaterial->m_CombuHeat);
        mtl.setRadFraction(m_CurMaterial->m_RadFraction);
        mtl.setInfos(m_CurMaterial->m_Infos);

        zitem->setProperty("FireMaterial", mtl);

//        if( qobject_cast< NENode * >( zitem ) )
//            emit sendAssignFireMaterial( qobject_cast< NENode * >( zitem ), *m_CurMaterial );
    }

    FDYNGUI.updatePropertyEditor();
}

void FDYNFireMaterialDialog::reject()
{
    QDialog::reject();
}

void FDYNFireMaterialDialog::setDefaultMaterialProps(FireMaterial &mat)
{
    mat.m_C = 0.0;
    mat.m_N = 0.0;
    mat.m_H = 4.0;
    mat.m_Cl = 0.0;
    mat.m_O = 0.0;
    mat.m_CombuHeat = 50000.0;
    mat.m_RadFraction = 0.35;
}

void FDYNFireMaterialDialog::clear()
{
    ui->tableFireMaterials->clearContents();
    ui->listMaterial->clear();
    //ui->comboRefFireID->clear();
    m_MaterialList.clear();
}

void FDYNFireMaterialDialog::addRowToTable(FireMaterial &mat)
{
    ui->listMaterial->insertItem(ui->listMaterial->count(), mat.m_Name);
    ui->listMaterial->setCurrentRow(mat.m_Index-1);
    //ui->comboRefFireID->insertItem(ui->comboRefFireID->count(), mat.m_Name);
    //ui->comboRefFireID->setCurrentIndex(mat.m_Index-1);

    updateControlsState();
}

void FDYNFireMaterialDialog::updateControlsState()
{
    if(m_MaterialList.size() <= 0)
    {
        ui->listMaterial->setEnabled(false);
        //ui->comboRefFireID->setEnabled(false);
        ui->lineFireID->setEnabled(false);
        ui->lineC->setEnabled(false);
        ui->lineN->setEnabled(false);
        ui->lineH->setEnabled(false);
        ui->lineCl->setEnabled(false);
        ui->lineO->setEnabled(false);
        ui->lineCombuHeat->setEnabled(false);
        ui->lineRadFraction->setEnabled(false);
        ui->tableFireMaterials->setEnabled(false);
    }
    else
    {
        ui->listMaterial->setEnabled(true);
        //ui->comboRefFireID->setEnabled(true);
        ui->lineFireID->setEnabled(true);
        ui->lineC->setEnabled(true);
        ui->lineN->setEnabled(true);
        ui->lineH->setEnabled(true);
        ui->lineCl->setEnabled(true);
        ui->lineO->setEnabled(true);
        ui->lineCombuHeat->setEnabled(true);
        ui->lineRadFraction->setEnabled(true);
        ui->tableFireMaterials->setEnabled(true);
    }
}

FireMaterial FDYNFireMaterialDialog::createNewMaterial()
{
    FireMaterial mat;
    mat.m_Name = QString("New Fire Material ") + QString::number(m_MaterialList.size());
    mat.m_ID = 0; // Default value, expected to be overwritten by the id assigned by the database
    mat.m_Index = m_MaterialList.size();
    setDefaultMaterialProps(mat);

    return mat;
}

FireMaterial FDYNFireMaterialDialog::createNullMaterial()
{
    FireMaterial mat;
    mat.m_Name = QString("Null");
    mat.m_ID = 0;
    mat.m_Index = m_MaterialList.size();
    setDefaultMaterialProps(mat);

    return mat;
}

void FDYNFireMaterialDialog::addNewMaterial()
{
    if(m_CurMaterial && m_CurMaterial->m_Name.isEmpty())
    {
        QMessageBox msgWarning ;
        msgWarning.setText( QString("Please fill Material name.") );
        msgWarning.setIcon( QMessageBox::Warning );
        msgWarning.setWindowTitle(tr("FRI3D"));
        msgWarning.addButton( QMessageBox::Ok );
        msgWarning.setStyle( QStyleFactory::create ("Fusion" ) );

        msgWarning.exec();

        return;
    }

    FireMaterial mat = createNewMaterial();
    addMaterial(mat);

    emit sendAddFireMaterial(mat);

    FDYNGUI.updatePropertyEditor();
}

void FDYNFireMaterialDialog::addMaterial(FireMaterial& mat)
{
    m_MaterialList.push_back(mat);
    m_CurMaterial = &m_MaterialList.back();

    // Null material should not be visible in the table
    if (mat.m_Index > 0)
        addRowToTable(mat);
}

void FDYNFireMaterialDialog::updateMaterial(FireMaterial& mat)
{
    // Search for the material with the name given my newMat and update all the quantities by the value
    // in newMat
    for(int i = 0; i < m_MaterialList.size(); i++)
    {
        if( m_MaterialList[i].m_Name == mat.m_Name )
        {
            m_MaterialList[i] = mat;
            //updateMaterial(i);
            break;
        }
    }
}

void FDYNFireMaterialDialog::updateMaterial(int ridx)
{
    if(ridx >= 0 && ridx+1 < m_MaterialList.size())
    {
        int idx = ridx+1;
        m_CurMaterial = &m_MaterialList[idx];
        setLineEdits(*m_CurMaterial);
    }
    else
    {
        m_CurMaterial = nullptr;
    }

    ui->tableFireMaterials->clearContents();
    for( int j = 0; j < ui->tableFireMaterials->columnCount(); j++)
    {

        for (int i = 0; i < ui->tableFireMaterials->rowCount(); i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setForeground(QBrush(QColor(255, 255, 255)));
            ui->tableFireMaterials->setItem(i, j, item);
        }
    }

    if (m_CurMaterial != nullptr)
    {
        ui->tableFireMaterials->blockSignals(true);
        for(int i = 0; i < m_CurMaterial->m_Infos.size(); i++){
            //       QTableWidgetItem *item = ui->tableFireMaterials->item(i, 0);
            //       item->setText(QString::number(mat.m_Infos[i].m_Time));
            //       ui->tableFireMaterials->setItem(i, 0, item);

            //        QTableWidgetItem *item = new QTableWidgetItem(QString::number(m_CurMaterial->m_Infos[i].m_Time));
            //        item->setForeground(QBrush(Qt::white));
            //        ui->tableFireMaterials->setItem(i, 0, item);

            //        item = new QTableWidgetItem(QString::number(m_CurMaterial->m_Infos[i].m_HRR));
            //        item->setForeground(QBrush(Qt::white));
            //        ui->tableFireMaterials->setItem(i, 1, item);

            //        item = new QTableWidgetItem(QString::number(m_CurMaterial->m_Infos[i].m_Height));
            //        item->setForeground(QBrush(Qt::white));
            //        ui->tableFireMaterials->setItem(i, 2, item);

            //        item = new QTableWidgetItem(QString::number(m_CurMaterial->m_Infos[i].m_Area));
            //        item->setForeground(QBrush(Qt::white));
            //        ui->tableFireMaterials->setItem(i, 3, item);

            //        item = new QTableWidgetItem(QString::number(m_CurMaterial->m_Infos[i].m_CO));
            //        item->setForeground(QBrush(Qt::white));
            //        ui->tableFireMaterials->setItem(i, 4, item);

            //        item = new QTableWidgetItem(QString::number(m_CurMaterial->m_Infos[i].m_Soot));
            //        item->setForeground(QBrush(Qt::white));
            //        ui->tableFireMaterials->setItem(i, 5, item);

            //        item = new QTableWidgetItem(QString::number(m_CurMaterial->m_Infos[i].m_HCN));
            //        item->setForeground(QBrush(Qt::white));
            //        ui->tableFireMaterials->setItem(i, 6, item);

            //        item = new QTableWidgetItem(QString::number(m_CurMaterial->m_Infos[i].m_TS));
            //        item->setForeground(QBrush(Qt::white));
            //        ui->tableFireMaterials->setItem(i, 7, item);

            //        update();
            //        ui->tableFireMaterials->update();

            ui->tableFireMaterials->item(i, 0)->setForeground(Qt::white);
            ui->tableFireMaterials->item(i, 0)->setText(QString::number(m_CurMaterial->m_Infos[i].m_Time));

            ui->tableFireMaterials->item(i, 1)->setForeground(Qt::white);
            ui->tableFireMaterials->item(i, 1)->setText(QString::number(m_CurMaterial->m_Infos[i].m_HRR));

            ui->tableFireMaterials->item(i, 2)->setForeground(Qt::white);
            ui->tableFireMaterials->item(i, 2)->setText(QString::number(m_CurMaterial->m_Infos[i].m_Height));

            ui->tableFireMaterials->item(i, 3)->setForeground(Qt::white);
            ui->tableFireMaterials->item(i, 3)->setText(QString::number(m_CurMaterial->m_Infos[i].m_Area));

            ui->tableFireMaterials->item(i, 4)->setForeground(Qt::white);
            ui->tableFireMaterials->item(i, 4)->setText(QString::number(m_CurMaterial->m_Infos[i].m_CO));

            ui->tableFireMaterials->item(i, 5)->setForeground(Qt::white);
            ui->tableFireMaterials->item(i, 5)->setText(QString::number(m_CurMaterial->m_Infos[i].m_Soot));

            ui->tableFireMaterials->item(i, 6)->setForeground(Qt::white);
            ui->tableFireMaterials->item(i, 6)->setText(QString::number(m_CurMaterial->m_Infos[i].m_HCN));

            ui->tableFireMaterials->item(i, 7)->setForeground(Qt::white);
            ui->tableFireMaterials->item(i, 7)->setText(QString::number(m_CurMaterial->m_Infos[i].m_TS));
        }
        ui->tableFireMaterials->blockSignals(false);

        drawGraph(0);
    }

    //FDYNGUI.updatePropertyEditor();
}

void FDYNFireMaterialDialog::removeMaterial()
{
    if(m_MaterialList.size() <= 0)
        return;

    int ridx = ui->listMaterial->currentIndex().row();
    if( ridx < 0 )
        return;

    int idx = ridx + 1;
    FireMaterial mat = m_MaterialList[idx];
    m_MaterialList.removeAt(idx);
    ui->listMaterial->takeItem(ridx);
    if(ridx >= 1)
        ui->listMaterial->setCurrentRow(ridx-1);
    else if(ui->listMaterial->count() > 0)
        ui->listMaterial->setCurrentRow(ridx);
    else
    {
        m_CurMaterial = nullptr;
        ui->listMaterial->clearSelection();;
    }


    for(int i = idx; i < m_MaterialList.size(); i++)
        m_MaterialList[i].m_Index--;

    updateControlsState();

    emit sendRemoveFireMaterial(mat);

    FDYNGUI.updatePropertyEditor();
}

void FDYNFireMaterialDialog::updateRow(const QItemSelection& selected, const QItemSelection& deselected)
{
//    if(selected.size() <= 0 )
//        return;
//    QModelIndex idx = selected.indexes().first();
//    if(idx.row() < m_MaterialList.size()){
//        m_CurMaterial = &m_MaterialList[idx.row()];
//        setLineEdits(*m_CurMaterial);
//        m_prevSelectedIndex = idx.row();
//    }
//    else{
//        m_CurMaterial = nullptr;
//    }

    if(selected.indexes().size() > 0)
        drawGraph(selected.indexes().first().column());
}

void FDYNFireMaterialDialog::drawGraph(int index)
{
    if(m_CurMaterial){
        QVector<double> x(m_CurMaterial->m_Infos.size()), y(m_CurMaterial->m_Infos.size());
        double xupper, yupper;
        xupper = yupper = 0;
        for(int i = 0; i < m_CurMaterial->m_Infos.size(); i++){
            if(index < 2){
                x[i] = m_CurMaterial->m_Infos[i].m_Time;
                y[i] = m_CurMaterial->m_Infos[i].m_HRR;
            }
            else{
                x[i] = m_CurMaterial->m_Infos[i].m_Time;
                y[i] = *(&(m_CurMaterial->m_Infos[i].m_Time)+index);
            }

            xupper = xupper < x[i] ? x[i] : xupper;
            yupper = yupper < y[i] ? y[i] : yupper;
        }
        if( m_CurMaterial->m_Infos.size() <= 0)
            ui->widgetGraph->drawPlot(x, y, xupper, yupper, "");
        else{
            QString title = m_CurMaterial->m_Name + ": ";
            switch (index) {
            case 0:
            case 1:
                title = title + "HRR(kW)";
                break;
            case 2:
                title = title + "Height(m)";
                break;
            case 3:
                title = title + "Area(m²)";
                break;
            case 4:
                title = title + "CO Yield";
                break;
            case 5:
                title = title + "Soot Yield";
                break;
            case 6:
                title = title + "HCN Yield";
                break;
            case 7:
                title = title + "TS Yield";
                break;
            default:
                break;
            }
            ui->widgetGraph->drawPlot(x, y, xupper, yupper, title);
        }
    }
}

void FDYNFireMaterialDialog::setLineEdits(FireMaterial &mat)
{
    ui->lineFireID->blockSignals(true);
    ui->lineFireID->setText(mat.m_Name);
    ui->lineFireID->blockSignals(false);

    ui->lineC->blockSignals(true);
    ui->lineC->setText(QString::number(mat.m_C));
    ui->lineC->blockSignals(false);

    ui->lineN->blockSignals(true);
    ui->lineN->setText(QString::number(mat.m_N));
    ui->lineN->blockSignals(false);

    ui->lineH->blockSignals(true);
    ui->lineH->setText(QString::number(mat.m_H));
    ui->lineH->blockSignals(false);

    ui->lineCl->blockSignals(true);
    ui->lineCl->setText(QString::number(mat.m_Cl));
    ui->lineCl->blockSignals(false);

    ui->lineO->blockSignals(true);
    ui->lineO->setText(QString::number(mat.m_O));
    ui->lineO->blockSignals(false);

    ui->lineCombuHeat->blockSignals(true);
    QString unit("kJ/kg");
    ui->lineCombuHeat->setText(QString::number(mat.m_CombuHeat) + QChar::Space + unit);
    ui->lineCombuHeat->blockSignals(false);

    ui->lineRadFraction->blockSignals(true);
    ui->lineRadFraction->setText(QString::number(mat.m_RadFraction));
    ui->lineRadFraction->blockSignals(false);
}

void FDYNFireMaterialDialog::materialInfoChanged(int row, int column)
{
    QTableWidgetItem *item = ui->tableFireMaterials->item(row, 0);
    if(item && !item->text().isEmpty()){

        if(m_CurMaterial->m_Infos.size() <= row ){
            int i = m_CurMaterial->m_Infos.size();
            while( i <= row){
                FireMaterialInfo info;
                m_CurMaterial->m_Infos.insert(i, info);
                i++;
            }
        }

        ui->tableFireMaterials->blockSignals(true);

        FireMaterialInfo &info = m_CurMaterial->m_Infos[row];

        if(column == 0){
            info.m_Time = item->text().toDouble();
            item->setText(QString::number(info.m_Time));
        }
        else if(column == 1){
            item = ui->tableFireMaterials->item(row, 1);
            if(item)
                info.m_HRR = item->text().toDouble();

            item->setText(QString::number(info.m_HRR));
        }
        else if(column == 2){

            item = ui->tableFireMaterials->item(row, 2);
            if(item)
                info.m_Height = item->text().toDouble();

            item->setText(QString::number(info.m_Height));
        }
        else if( column == 3){
            item = ui->tableFireMaterials->item(row, 3);
            if(item)
                info.m_Area = item->text().toDouble();

            item->setText(QString::number(info.m_Area));
        }
        else if( column == 4){
            item = ui->tableFireMaterials->item(row, 4);
            if(item)
                info.m_CO = item->text().toDouble();

            item->setText(QString::number(info.m_CO));
        }
        else if(column == 5){
            item = ui->tableFireMaterials->item(row, 5);
            if(item)
                info.m_Soot = item->text().toDouble();

            item->setText(QString::number(info.m_Soot));
        }
        else if(column == 6){
            item = ui->tableFireMaterials->item(row, 6);
            if(item)
                info.m_HCN = item->text().toDouble();

            item->setText(QString::number(info.m_HCN));
        }
        else if(column == 7){
            item = ui->tableFireMaterials->item(row, 7);
            if(item)
                info.m_TS = item->text().toDouble();

            item->setText(QString::number(info.m_TS));
        }

        m_CurMaterial->m_Infos.replace(row, info);

        ui->tableFireMaterials->blockSignals(false);

        drawGraph(column);

        emit sendAddFireMaterial(*m_CurMaterial);

        FDYNGUI.updatePropertyEditor();
    }
}

void FDYNFireMaterialDialog::editedLineEdit()
{
    QLineEdit *line = qobject_cast<QLineEdit *>(sender());

    if(!line)
        return;

    if(line->text().isEmpty())
    {
        QMessageBox msgWarning ;
        msgWarning.setText( QString("Text can't be empty. Please Enter a Value ") );
        msgWarning.setIcon( QMessageBox::Warning );
        msgWarning.setWindowTitle(tr("FRI3D"));
        msgWarning.addButton( QMessageBox::Ok );
        msgWarning.setStyle( QStyleFactory::create ("Fusion" ) );

        msgWarning.exec();

        line->setFocus();
        return;
    }

    if(m_CurMaterial == nullptr)
        return;

    if(QLineEdit *line = qobject_cast<QLineEdit *>(sender()))
    {
        if(line == ui->lineFireID)
        {
            if(m_CurMaterial->m_ID != 0)
                m_CurMaterial->m_Name = ui->lineFireID->text();
            ui->listMaterial->item(ui->listMaterial->currentRow())->setText(m_CurMaterial->m_Name);
        }
        else if( line == ui->lineC)
        {
            if(m_CurMaterial->m_ID != 0)
                m_CurMaterial->m_C = ui->lineC->text().toFloat();
            ui->lineC->setText(QString::number(m_CurMaterial->m_C));
        }
        else if( line == ui->lineN)
        {
            if(m_CurMaterial->m_ID != 0)
                m_CurMaterial->m_N = ui->lineN->text().toFloat();
            ui->lineN->setText(QString::number(m_CurMaterial->m_N));
        }
        else if( line == ui->lineH)
        {
            if(m_CurMaterial->m_ID != 0)
                m_CurMaterial->m_H = ui->lineH->text().toFloat();
            ui->lineH->setText(QString::number(m_CurMaterial->m_H));
        }
        else if( line == ui->lineCl)
        {
            if(m_CurMaterial->m_ID != 0)
                m_CurMaterial->m_Cl = ui->lineCl->text().toFloat();
            ui->lineCl->setText(QString::number(m_CurMaterial->m_Cl));
        }
        else if( line == ui->lineO)
        {
            if(m_CurMaterial->m_ID != 0)
                m_CurMaterial->m_O = ui->lineO->text().toFloat();
            ui->lineO->setText(QString::number(m_CurMaterial->m_O));
        }
        else if( line == ui->lineCombuHeat)
        {
            QString unit("kJ/kg");
            QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

            if(m_CurMaterial->m_ID != 0){
                int pos = 0;
                if( (pos = rx.indexIn(ui->lineCombuHeat->text(), pos)) != -1 )
                    m_CurMaterial->m_CombuHeat = rx.cap(0).toFloat();
                else
                    m_CurMaterial->m_CombuHeat = 0.0f;
            }

            ui->lineCombuHeat->setText(QString::number(m_CurMaterial->m_CombuHeat) + QChar::Space + unit);
        }
        else if( line == ui->lineRadFraction)
        {
            if(m_CurMaterial->m_ID != 0)
                m_CurMaterial->m_RadFraction = ui->lineRadFraction->text().toFloat();
            ui->lineRadFraction->setText(QString::number(m_CurMaterial->m_RadFraction));
        }
    }

    if(m_CurMaterial->m_ID != 0)
        emit sendAddFireMaterial(*m_CurMaterial);

    FDYNGUI.updatePropertyEditor();
}

void FDYNFireMaterialDialog::addT2()
{
    FDYNFireAddT2Dialog dlg(this);
    if( dlg.exec() == QDialog::Accepted){
        if(m_MaterialList.size() <= 0 || !m_CurMaterial){
            addNewMaterial();
        }

        m_CurMaterial->m_Infos.clear();
        m_CurMaterial->m_Infos = dlg.m_infos;
        updateMaterial(ui->listMaterial->currentRow());
        //updateMaterial(ui->comboRefFireID->currentIndex());

    }

    emit sendAddFireMaterial(*m_CurMaterial);

    FDYNGUI.updatePropertyEditor();
}
