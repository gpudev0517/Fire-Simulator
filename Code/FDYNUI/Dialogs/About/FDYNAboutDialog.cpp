#include "FDYNAboutDialog.h"
#include "ui_FDYNAboutDialog.h"

FDYNAboutDialog::FDYNAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FDYNAboutDialog)
{
    ui->setupUi(this);
}

FDYNAboutDialog::~FDYNAboutDialog()
{
    delete ui;
}
