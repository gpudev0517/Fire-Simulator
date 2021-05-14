#include "FDYNFRANXCalcDialog.h"
#include "ui_FDYNFRANXCalcDialog.h"
#include "Managers/FDYNGUIManager.h"

#include "Dialogs/File/FDYNFileDialog.h"
#include "Windows/Main/FDYNMainWindow.h"

FDYNFRANXCalcDialog::FDYNFRANXCalcDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FDYNFRANXCalcDialog)
{
    ui->setupUi(this);
    setStyle( QStyleFactory::create( "Fusion" ) );

    ui->tblCable->resizeColumnsToContents();
    ui->tblCable->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->btnTempDataDir->setAutoDefault( false );

    QObject::connect( this, &FDYNFRANXCalcDialog::sendFranxCalc, &FDYNGUI, &FDYNGUIManager::computeFRANX );
    QObject::connect( this, &FDYNFRANXCalcDialog::sendUpdateComputeScenario, &FDYNGUI,
                     &FDYNGUIManager::updateComputeScenario );

    QObject::connect( ui->lineNewProject, SIGNAL(editingFinished()), this, SLOT(franxDataFinished()));

    QObject::connect( ui->btnTempDataDir, SIGNAL(clicked(bool)), this, SLOT(setTempDataDirAccept()));
    QObject::connect( ui->btnOK, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect( ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

    QObject::connect( ui->tblCable, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(updateItem(QTableWidgetItem *)));

}

FDYNFRANXCalcDialog::~FDYNFRANXCalcDialog()
{
    delete ui;
}

void FDYNFRANXCalcDialog::setTempDataDirAccept()
{
    QSettings settings( QCoreApplication::organizationName(), QCoreApplication::applicationName() );

    FDYNFileDialog dlg( FDYNGUI.mainWindow(), settings.value("LastLoadFRANX").toByteArray(),
                     tr("FRANX File"), "FRANX (*.franx);;All (*.*);", FDYNFileDialog::OPEN,
                     QFileDialog::AnyFile );

    if ( dlg.exec() == QDialog::Accepted)
    {
        if( dlg.selectedFiles().size() > 0)
        {
            ui->lineNewProject->setText( dlg.selectedFiles()[0] );
            FRANXProperties fp;
            fp.m_ID = -1;
            fp.m_CleanName = "";
            fp.m_FranxFile = dlg.selectedFiles()[0];
            QFileInfo qff( fp.m_FranxFile );

            settings.setValue( "LastLoadFRANX", qff.absolutePath() );
            emit sendUpdateComputeScenario( fp );
        }
    }
}

void FDYNFRANXCalcDialog::franxDataFinished()
{
    QString text = ui->lineNewProject->text();

    setTempDataDir( text );
}

void FDYNFRANXCalcDialog::setTempDataDir(const QString tempDataDir)
{
    qDebug() << "Data Dir " << tempDataDir;

    FRANXProperties fp;
    fp.m_ID = -1;
    fp.m_CleanName = "";
    fp.m_FranxFile = tempDataDir;
    emit sendUpdateComputeScenario( fp );
}



QString FDYNFRANXCalcDialog::selectedTempDataDir()
{
    return ui->lineNewProject->text();
}


QStringList FDYNFRANXCalcDialog::scenarioNameList()
{
    QStringList list;
    foreach( const FRANXProperties mtl, m_ScenarioList)
    {
        list.push_back( mtl.m_Name );
    }
    return list;
}


void FDYNFRANXCalcDialog::showWithPropertyValues(QWidget *propertyBrowser, QVariant idx, QVariant val,
                                                 QVariant type, QVariant vName)
{
    if(isVisible())
        return;

    m_CurrentScenarioKind = vName.value<QString>();

    NESurfaceMaterial mtl = val.value<NESurfaceMaterial>();

    for (int idx = 0; idx < m_ScenarioList.size(); idx++)
    {
        FRANXProperties mat = m_ScenarioList[idx];
        if(mat.m_ID == mtl.uniqueID())
        {
            int ridx = idx-1;
            if (ridx >= 0)
                ui->tblCable->selectRow(ridx);
            break;
        }
    }

    show();
}

void FDYNFRANXCalcDialog::accept()
{
    if(m_ScenarioList.size() < 0 || m_prevSelectedIndex <= 0)
        return;

    emit sendFranxCalc();

    FDYNGUI.updatePropertyEditor();
}


void FDYNFRANXCalcDialog::reject()
{
    QDialog::reject();
}


void FDYNFRANXCalcDialog::setDefaultProps(FRANXProperties &mat)
{
    mat.m_Name = "";
    mat.m_Desc = "";
}

void FDYNFRANXCalcDialog::clear()
{
    ui->tblCable->clearContents();
    m_ScenarioList.clear();
}

void FDYNFRANXCalcDialog::addRowToTable( FRANXProperties &material )
{
    ui->tblCable->insertRow( ui->tblCable->rowCount() );

    // Offset the index by one, because the null material (index = 0) is hidden from the table
    int rIdx = material.m_Index-1;

    ui->tblCable->blockSignals(true);

    QTableWidgetItem *item = new QTableWidgetItem();
    if( material.m_Selected == true )
        item->setCheckState( Qt::Checked );
    else
        item->setCheckState( Qt::Unchecked );

    item->setForeground( QBrush(Qt::white) );
    ui->tblCable->setItem( rIdx, 0, item );

    item = new QTableWidgetItem( material.m_Name );
    item->setForeground( QBrush(Qt::white) );
    ui->tblCable->setItem( rIdx, 1, item );
    item->setFlags( item->flags() & ~Qt::ItemIsEditable);

    item =  new QTableWidgetItem( material.m_Desc );
    item->setForeground( QBrush( Qt::white ) );
    ui->tblCable->setItem( rIdx, 2, item );
    item->setFlags( item->flags() & ~Qt::ItemIsEditable);

//    ui->tblCable->selectRow(rIdx);

    ui->tblCable->blockSignals(false);

    update();
    ui->tblCable->update();

    ui->tblCable->viewport()->update();
}



void FDYNFRANXCalcDialog::updateItem(QTableWidgetItem *item)
{
    if(m_ScenarioList.size() <= 0)
        return;

    int idx = ui->tblCable->row(item);

    if(idx < m_ScenarioList.size())
    {
        ui->tblCable->blockSignals(true);

        m_CurScenario = &m_ScenarioList[idx];
        int cidx = ui->tblCable->column(item);
        if( cidx == 0 )
        {
            //qDebug() << "IS Selected " << item->checkState();
            m_CurScenario->m_Selected = item->checkState();
        }
        else if( cidx == 1)
        {
            {
                m_CurScenario->m_Name = item->text();
                m_CurScenario->m_CleanName = item->text();
            }
        }
        else if( cidx == 2)
        {
            m_CurScenario->m_Desc = item->text();
        }

        m_prevSelectedIndex = idx;

        ui->tblCable->blockSignals(false);
    }
    else
    {
        m_CurScenario = nullptr;

        ui->tblCable->blockSignals( true );

        item->setText("");

        ui->tblCable->blockSignals( false );
    }

    if( m_CurScenario )
    {
        if( m_CurScenario->m_ID != 0 )
        {
            //qDebug() << "Updating " << m_CurScenario->m_Name << " " << m_CurScenario->m_Selected;
            m_CurScenario->m_FranxFile = ui->lineNewProject->text();
            emit sendUpdateComputeScenario( *m_CurScenario );

            FDYNGUI.updatePropertyEditor();
        }
    }
}



void FDYNFRANXCalcDialog::addScenario(FRANXProperties& fp)
{
    m_ScenarioList.push_back(fp);

    // Null material should not be visible in the table
    fp.m_Index = m_ScenarioList.size();

    addRowToTable( fp );
}

void FDYNFRANXCalcDialog::updateScenarios(QList<FRANXProperties>& fplist)
{
    clear();

    if( fplist.size() )
        ui->lineNewProject->setText( fplist[0].m_FranxFile );
//    qDebug() << "Setting " << ui->lineNewProject->text();

    for( unsigned i = 0; i < fplist.size(); ++i )
    {
//        qDebug() << "Adding Scenario " << fplist[i].m_CleanName;
        addScenario( fplist[i] );
    }

    ui->tblCable->viewport()->update();

}

//void FDYNFRANXCalcDialog::updateScenarios(FRANXProperties& fp)
//{
//    // Search for the material with the name given my newMat and update all the quantities by the value
//    // in newMat
//    for(int i = 0; i < m_ScenarioList.size(); i++)
//    {
//        if( m_ScenarioList[i].m_Name == fp.m_Name )
//        {
//            m_ScenarioList[i] = fp;
//            break;
//        }
//    }
//    if( fp.m_FranxFile != "" )
//    {
//        setTempDataDir( fp.m_FranxFile );
//    }

//    ui->tblCable->viewport()->update();
//}

