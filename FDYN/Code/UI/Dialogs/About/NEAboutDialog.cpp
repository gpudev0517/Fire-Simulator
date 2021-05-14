#include "NEAboutDialog.h"
#include "ui_NEAboutDialog.h"

NEAboutDialog::NEAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NEAboutDialog)
{
    ui->setupUi(this);
}

NEAboutDialog::~NEAboutDialog()
{
    delete ui;
}
