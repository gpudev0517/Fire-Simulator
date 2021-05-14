#include "NEPluginInfoDialog.h"
#include "ui_NEPluginInfoDialog.h"
#include "Dialogs/Preferences/NEPreferencesDialog.h"


NEPluginInfoDialog::NEPluginInfoDialog(QWidget *parent) :
    QDialog(parent),
    m_pPD(static_cast<NEPreferencesDialog*>(parent)),
    ui(new Ui::NEPluginInfoDialog)
{
    ui->setupUi(this);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->reloadButton, &QAbstractButton::clicked, this, &NEPluginInfoDialog::reloadPlugin);

}

NEPluginInfoDialog::~NEPluginInfoDialog()
{
    delete ui;
}

void NEPluginInfoDialog::setName(QString s)
{
    ui->nameEdit->setText(s);
}

void NEPluginInfoDialog::setPath(QString s)
{
    ui->pathEdit->setText(s);
}

void NEPluginInfoDialog::setHelp(QString s)
{
    ui->textBrowser->setText(s);
}

void NEPluginInfoDialog::setVendor(QString s)
{
    ui->vendorEdit->setText(s);
}

void NEPluginInfoDialog::setFilename(QString s)
{
    ui->filenameEdit->setText(s);
}

void NEPluginInfoDialog::setVersion(QString s)
{
    ui->versionEdit->setText(s);
}

void NEPluginInfoDialog::reloadPlugin()
{
    m_pPD->reloadPlugin();
    m_pPD->showPluginInfo();
}


