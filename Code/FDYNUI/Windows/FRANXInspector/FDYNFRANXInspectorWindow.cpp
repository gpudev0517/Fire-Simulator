#include "Base/NESceneManager.h"
#include "FDYNFRANXInspectorWindow.h"
#include "FDYNFRANXInspector.h"
#include "Core/NENode.h"



class FDYNFRANXInspectorWindowPrivate
{
public:
    FDYNFRANXInspectorWindowPrivate(FDYNFRANXInspectorWindow* s) : S(s) {}

    FDYNFRANXInspector* franxInspWidget;
private:
    FDYNFRANXInspectorWindow* S;
};


FDYNFRANXInspectorWindow::FDYNFRANXInspectorWindow(QWidget* parent) :
    QMainWindow(parent)
{
    //ui->setupUi(this);
    //ui->lineEdit->setVisible(false);
    //ui->label->setVisible(false);


    P = new FDYNFRANXInspectorWindowPrivate(this);

    P->franxInspWidget = new FDYNFRANXInspector( this );
    P->franxInspWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    P->franxInspWidget->setMinimumHeight(80);

    setCentralWidget(P->franxInspWidget);
}

FDYNFRANXInspectorWindow::~FDYNFRANXInspectorWindow()
{
    if(P->franxInspWidget)
        delete P->franxInspWidget;
    delete P;
}


void FDYNFRANXInspectorWindow::setRoot(QObject* root)
{
    //ui->sceneTree->setRoot(root);
    P->franxInspWidget->setRoot(root);
}

void FDYNFRANXInspectorWindow::update()
{
    //ui->sceneTree->update();
    P->franxInspWidget->update();
}

void FDYNFRANXInspectorWindow::setNewCurrentItem(QObject* obj)
{
    //ui->sceneTree->setNewCurrentItem(obj);
    P->franxInspWidget->setNewCurrentItem(obj);
}

void FDYNFRANXInspectorWindow::setCurrentItems(QList<NENode*> objList)
{
    //ui->sceneTree->setCurrentItems(objList);
    P->franxInspWidget->setCurrentItems(objList);
}

bool FDYNFRANXInspectorWindow::isSelectedObject(NENode* obj)
{
    return P->franxInspWidget->isSelectedObect(obj);
    //return ui->sceneTree->isSelectedObect(obj);
}

void FDYNFRANXInspectorWindow::setParentCurrentItem()
{
    //ui->sceneTree->setParentCurrentItem();
    P->franxInspWidget->setParentCurrentItem();

}

void FDYNFRANXInspectorWindow::writeGUIState(QXmlStreamWriter* writer)
{
    writer->writeStartElement("FRANXInspector");
    P->franxInspWidget->saveGUIStates(writer);
    writer->writeEndElement();
}

void FDYNFRANXInspectorWindow::readGUIState(QXmlStreamReader* reader)
{
    //ui->sceneTree->readGUIStates(reader);
    P->franxInspWidget->readGUIStates(reader);
}

FDYNFRANXInspector *FDYNFRANXInspectorWindow::franxInspector()
{
    return P->franxInspWidget;
}


void FDYNFRANXInspectorWindow::SetChildsVisibility(FDYNTreeItem* item, QString exp)
{
    if(item->childCount() > 0)
        for(int i = 0; i < item->childCount(); ++i)
            SetChildsVisibility(item->child(i), exp);
    else if((!item->text(0).contains(exp, Qt::CaseInsensitive)) && (!item->text(1).contains(exp, Qt::CaseInsensitive)))
        item->setHidden(true);
}

void FDYNFRANXInspectorWindow::on_lineEdit_textChanged(const QString& arg1)
{
    //ui->sceneTree->clear();
    //ui->sceneTree->setRoot(&NESCENE);
    //ui->sceneTree->update();
    P->franxInspWidget->update();
    //if(ui->lineEdit->text().length() == 0)
    //    return;
    //SetChildsVisibility(ui->sceneTree->invisibleRootItem(), ui->lineEdit->text());
    //P->zoneInspWidget->expandAll();

}
