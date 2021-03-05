#include "FDYNSettingsDialog.h"
#include "ui_FDYNSettingsDialog.h"
#include "Managers/FDYNGUIManager.h"
#include "Dialogs/File/FDYNFileDialog.h"
#include "Windows/Main/FDYNMainWindow.h"

FDYNSettingsDialog::FDYNSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FDYNSettingsDialog)
{
    ui->setupUi(this);
    setStyle( QStyleFactory::create( "Fusion" ) );


    QObject::connect(ui->btnWorkingDataDir, SIGNAL(clicked(bool)), this, SLOT(setTempDataDirAccept()));
    QObject::connect(ui->btnUIDataDir, SIGNAL(clicked(bool)), this, SLOT(setUIDataDirAccept()));

    QObject::connect(ui->btnOK, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(accept()));

//    QObject::connect(ui->btnLoadUIData, SIGNAL(clicked()), this, SLOT(loadUIData()));
//    QObject::connect(ui->btnSaveUIData, SIGNAL(clicked()), this, SLOT(saveUIData()));

}

FDYNSettingsDialog::~FDYNSettingsDialog()
{
    delete ui;
}

void FDYNSettingsDialog::setTempDataDirAccept()
{

    QSettings settings( "Fire Dynamics", "FRI3D" );

    FDYNFileDialog dlg(FDYNGUI.mainWindow(), settings.value("LastTempDataDir").toByteArray(),
                     tr("Temp Data Dir"), "*.*", FDYNFileDialog::OPEN,
                     QFileDialog::DirectoryOnly);

    if ( dlg.exec() == QDialog::Accepted)
    {
        if( dlg.selectedFiles().size() > 0)
        {
            ui->lineWorkingDataDir->setText(dlg.selectedFiles()[0]);
        }
        else{
            ui->lineWorkingDataDir->setText("");
        }
    }
    else{
        ui->lineWorkingDataDir->setText("");
    }
}


void FDYNSettingsDialog::setTempDataDir(QString tempDataDir)
{
    ui->lineWorkingDataDir->setText( tempDataDir );
}

bool FDYNSettingsDialog::loadUIData()
{
    return ( ui->loadUIData->checkState() == Qt::Checked );
}

bool FDYNSettingsDialog::saveUIData()
{
    return ( ui->saveUIData->checkState() == Qt::Checked );
}

void FDYNSettingsDialog::setUIDataDir(QString uiDataDir)
{
    ui->lineUIDataDir->setText( uiDataDir );
}


QString FDYNSettingsDialog::selectedTempDataDir()
{
    return ui->lineWorkingDataDir->text();
}


QString FDYNSettingsDialog::selectedUIDataDir()
{
    return ui->lineUIDataDir->text();
}


void FDYNSettingsDialog::setUIDataDirAccept()
{

    QSettings settings( "Fire Dynamics", "FRI3D" );

    FDYNFileDialog dlg(FDYNGUI.mainWindow(), settings.value("LastUIDataDir").toByteArray(),
                     tr("UI Data Dir"), "*.*", FDYNFileDialog::OPEN,
                     QFileDialog::DirectoryOnly);

    if ( dlg.exec() == QDialog::Accepted)
    {
        if( dlg.selectedFiles().size() > 0)
        {
            ui->lineUIDataDir->setText(dlg.selectedFiles()[0]);
        }
        else{
            ui->lineUIDataDir->setText("");
        }
    }
    else
    {
        ui->lineUIDataDir->setText("");
    }
}
