#include "NEShortcutWidget.h"
#include "ui_NEShortcutWidget.h"

NEShortcutWidget::NEShortcutWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NEShortcutWidget)
{
    ui->setupUi(this);
}

NEShortcutWidget::~NEShortcutWidget()
{
    delete ui;
}
