#include "NEProcessDialog.h"
#include "ui_NEProcessDialog.h"

NESceneLoadingDialog::NESceneLoadingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NESceneLoadingDialog)
{
    ui->setupUi(this);
    ui->progressBar->setValue(100);
    setWindowFlags(Qt::SplashScreen);
}

NESceneLoadingDialog::~NESceneLoadingDialog()
{
    delete ui;
}


void NESceneLoadingDialog::setMainOperation(QString s)
{
    ui->topLabel->setText(s);
    ui->topLabel->repaint();
}

void NESceneLoadingDialog::setCurrentOperation(QString s)
{
    ui->progressBar->setFormat(s);
    ui->progressBar->repaint();
}

void NESceneLoadingDialog::setCurrentSubOperation(QString s)
{
    ui->bottomLabel->setText(s);
    ui->bottomLabel->repaint();
}

void NESceneLoadingDialog::setPercentage(int p)
{
    ui->progressBar->setValue(p);
    ui->progressBar->repaint();
}
