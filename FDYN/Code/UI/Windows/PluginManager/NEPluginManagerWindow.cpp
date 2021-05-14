#include "NEPluginManagerWindow.h"
#include "ui_NEPluginManagerWindow.h"

NEPluginManagerWindow::NEPluginManagerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NEPluginManagerWindow)
{
    ui->setupUi(this);
}

NEPluginManagerWindow::~NEPluginManagerWindow()
{
    delete ui;
}
