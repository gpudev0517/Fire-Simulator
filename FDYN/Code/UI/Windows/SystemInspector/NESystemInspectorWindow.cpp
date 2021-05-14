#include "NESystemInspectorWindow.h"
#include "ui_NESystemInspectorWindow.h"

NESystemInspectorWindow::NESystemInspectorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NESystemInspectorWindow)
{
    ui->setupUi(this);
}

NESystemInspectorWindow::~NESystemInspectorWindow()
{
    delete ui;
}

void NESystemInspectorWindow::addRoot(QObject *root)
{
    ui->systemTree->addRoot(root);
}

void NESystemInspectorWindow::update()
{
    ui->systemTree->update();
}

void NESystemInspectorWindow::setNewCurrentItem(QObject *obj)
{
    ui->systemTree->setNewCurrentItem(obj);
}
