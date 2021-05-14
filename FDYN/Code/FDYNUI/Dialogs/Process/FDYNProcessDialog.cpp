#include "FDYNProcessDialog.h"
#include "ui_FDYNProcessDialog.h"
#include <QMovie>
#include "Base/NEBase.h"

FDYNLoadingDialog::FDYNLoadingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FDYNLoadingDialog)
{
    ui->setupUi(this);
    //ui->progressBar->setValue(100);
    setWindowFlags(Qt::SplashScreen);

    movie = new QMovie(":/img/loading.gif");
    ui->progressbar->setMovie(movie);
    movie->start();


}

FDYNLoadingDialog::~FDYNLoadingDialog()
{
}


void FDYNLoadingDialog::setMainOperation(QString s)
{
    ui->topLabel->setText(s);
    ui->topLabel->repaint();
}

void FDYNLoadingDialog::setCurrentOperation(QString s)
{
    //ui->progressBar->setFormat(s);
    //ui->progressBar->repaint();
    movie->start();
    ui->progressbar->repaint();
}

void FDYNLoadingDialog::setCurrentSubOperation(QString s)
{
    ui->bottomLabel->setText(s);
    ui->bottomLabel->repaint();

}

void FDYNLoadingDialog::setPercentage(int p)
{
    //ui->progressBar->setValue(p);
    //ui->progressBar->repaint();
}
