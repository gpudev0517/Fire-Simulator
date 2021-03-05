#include "FDYNSystemInspectorWindow.h"
#include "FDYNSystemInspector.h"
#include "FDYNSystemInspectorItem.h"
//#include "ui_NESystemInspectorWindow.h"


class FDYNSystemInspectorWindowPrivate
{
public:
    FDYNSystemInspectorWindowPrivate(FDYNSystemInspectorWindow* s) : S(s) {}

    FDYNSystemInspector* systemInspWidget;
private:
    FDYNSystemInspectorWindow* S;
};

FDYNSystemInspectorWindow::FDYNSystemInspectorWindow(QWidget *parent) :
    QMainWindow(parent)
    //,ui(new Ui::NESystemInspectorWindow)
{
    //ui->setupUi(this);

    P = new FDYNSystemInspectorWindowPrivate(this);

    P->systemInspWidget = new FDYNSystemInspector( this );
    P->systemInspWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    P->systemInspWidget->setMinimumHeight(80);

    setCentralWidget(P->systemInspWidget);
}

FDYNSystemInspectorWindow::~FDYNSystemInspectorWindow()
{
    //delete ui;
    if(P->systemInspWidget)
        delete P->systemInspWidget;
    delete P;
}

void FDYNSystemInspectorWindow::addRoot(QObject *root)
{
    //ui->systemTree->addRoot(root);
    P->systemInspWidget->addRoot(root);
}

void FDYNSystemInspectorWindow::update()
{
    //ui->systemTree->update();
    P->systemInspWidget->update();
}

void FDYNSystemInspectorWindow::setNewCurrentItem(QObject *obj)
{
    //ui->systemTree->setNewCurrentItem(obj);
    P->systemInspWidget->setNewCurrentItem(obj);
}

FDYNSystemInspector *FDYNSystemInspectorWindow::systemInspector()
{
    return P->systemInspWidget;
}
