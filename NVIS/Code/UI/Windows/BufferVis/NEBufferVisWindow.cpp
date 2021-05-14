#include "NEBufferVisWindow.h"
#include "ui_NEBufferVisWindow.h"

NEBufferVisWindow::NEBufferVisWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NEBufferVisWindow)
{
    ui->setupUi(this);
}

NEBufferVisWindow::~NEBufferVisWindow()
{
    delete ui;
}

NEGPUBufferVis* NEBufferVisWindow::bufferVis()
{
    return ui->bufferVis;
}
