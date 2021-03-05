#include "FDYNCreateProjectDialog.h"
#include "ui_FDYNCreateProjectDialog.h"
#include "Managers/FDYNGUIManager.h"
#include "Dialogs/File/FDYNFileDialog.h"
#include "Windows/Main/FDYNMainWindow.h"

FDYNCreateProjectDialog::FDYNCreateProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FDYNCreateProjectDialog)
{
    ui->setupUi(this);
    setStyle( QStyleFactory::create( "Fusion" ) );

    QObject::connect(ui->btnCreate, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

    QObject::connect(ui->btnNewProject, SIGNAL(clicked(bool)), this, SLOT(newProject()));
    QObject::connect(ui->btnFRANXLocation, SIGNAL(clicked(bool)), this, SLOT(chooseFRANXLocation()));
    QObject::connect(ui->btnPDMSLocation, SIGNAL(clicked(bool)), this, SLOT(choosePDMSLocation()));

}

FDYNCreateProjectDialog::~FDYNCreateProjectDialog()
{
    delete ui;
}

void FDYNCreateProjectDialog::newProject()
{

    QSettings settings( "Fire Dynamics", "FRI3D" );

    FDYNFileDialog dlg(FDYNGUI.mainWindow(), settings.value("LastLoadDB").toByteArray(),
                     tr("New Database Location"), "*.*", FDYNFileDialog::SAVE,
                     QFileDialog::AnyFile);

    if ( dlg.exec() == QDialog::Accepted)
    {
        if( dlg.selectedFiles().size() > 0)
        {
            ui->lineNewProject->setText(dlg.selectedFiles()[0]);
        }
        else
        {
            ui->lineNewProject->setText("");
        }
    }
    else
    {
        ui->lineNewProject->setText("");
    }
}


void FDYNCreateProjectDialog::chooseFRANXLocation()
{
    QSettings settings( "Fire Dynamics", "FRI3D" );

    FDYNFileDialog dlg(FDYNGUI.mainWindow(), settings.value("LastLoadFRANX").toByteArray(),
                     tr("FRANX Location"), "*.*", FDYNFileDialog::OPEN,
                     QFileDialog::AnyFile);

    if ( dlg.exec() == QDialog::Accepted)
    {
        if( dlg.selectedFiles().size() > 0)
        {
            ui->lineFRANXLoaction->setText(dlg.selectedFiles()[0]);
        }
        else
        {
            ui->lineFRANXLoaction->setText("");
        }
    }
    else
    {
        ui->lineFRANXLoaction->setText("");
    }

}


void FDYNCreateProjectDialog::choosePDMSLocation()
{
    QSettings settings( "Fire Dynamics", "FRI3D" );

    FDYNFileDialog dlg(FDYNGUI.mainWindow(), settings.value("LastLoadPDMS").toByteArray(),
                     tr("FRANX Location"), "*.*", FDYNFileDialog::OPEN,
                     QFileDialog::AnyFile);

    if ( dlg.exec() == QDialog::Accepted)
    {
        if( dlg.selectedFiles().size() > 0)
        {
            ui->linePDMSLocation->setText(dlg.selectedFiles()[0]);
        }
        else
        {
            ui->linePDMSLocation->setText("");
        }
    }
    else{
        ui->linePDMSLocation->setText("");
    }
}

QString FDYNCreateProjectDialog::selectedProjectFile()
{
    return ui->lineNewProject->text();
}

QString FDYNCreateProjectDialog::selectedFRANXFile()
{
    return ui->lineFRANXLoaction->text();
}

QString FDYNCreateProjectDialog::selectedPDMSFile()
{
    return ui->linePDMSLocation->text();
}
