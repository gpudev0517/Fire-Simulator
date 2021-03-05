#include "Base/NESceneManager.h"
#include "FDYNZoneInspectorWindow.h"
#include "FDYNZoneInspector.h"
#include "Core/NENode.h"

class FDYNZoneInspectorWindowPrivate
{
public:
    FDYNZoneInspectorWindowPrivate();

    FDYNZoneInspector* zoneInspWidget;

};

FDYNZoneInspectorWindowPrivate::FDYNZoneInspectorWindowPrivate() {}


FDYNZoneInspectorWindow::FDYNZoneInspectorWindow(QWidget* parent) :
    QMainWindow(parent)
{
    //ui->setupUi(this);
    //ui->lineEdit->setVisible(false);
    //ui->label->setVisible(false);

    P = new FDYNZoneInspectorWindowPrivate();

    P->zoneInspWidget = new FDYNZoneInspector( this );
    P->zoneInspWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    P->zoneInspWidget->setMinimumHeight(80);

    setCentralWidget(P->zoneInspWidget);
}

FDYNZoneInspectorWindow::~FDYNZoneInspectorWindow()
{
    if(P->zoneInspWidget)
        delete P->zoneInspWidget;
    delete P;
}

void FDYNZoneInspectorWindow::removeSelectedItem()
{
    P->zoneInspWidget->removeSelectedItem();
}

void FDYNZoneInspectorWindow::setRoot(QObject* root)
{
    //ui->sceneTree->setRoot(root);
    P->zoneInspWidget->setRoot(root);
}

QObject *FDYNZoneInspectorWindow::root(){
    return P->zoneInspWidget->root();
}

void FDYNZoneInspectorWindow::update()
{
    //ui->sceneTree->update();
    P->zoneInspWidget->update();
}

void FDYNZoneInspectorWindow::setNewCurrentItem(QObject* obj)
{
    //ui->sceneTree->setNewCurrentItem(obj);
    P->zoneInspWidget->setNewCurrentItem(obj);
}

void FDYNZoneInspectorWindow::setCurrentItems(QList<NENode*> objList)
{
    //ui->sceneTree->setCurrentItems(objList);
    P->zoneInspWidget->setCurrentItems(objList);
}

bool FDYNZoneInspectorWindow::isSelectedObject(NENode* obj)
{
    return P->zoneInspWidget->isSelectedObect(obj);
    //return ui->sceneTree->isSelectedObect(obj);
}

void FDYNZoneInspectorWindow::setParentCurrentItem()
{
    //ui->sceneTree->setParentCurrentItem();
    P->zoneInspWidget->setParentCurrentItem();
}

void FDYNZoneInspectorWindow::writeGUIState(QXmlStreamWriter* writer)
{
    writer->writeStartElement("ZoneInspector");
    P->zoneInspWidget->saveGUIStates(writer);
    writer->writeEndElement();
}

void FDYNZoneInspectorWindow::readGUIState(QXmlStreamReader* reader)
{
    //ui->sceneTree->readGUIStates(reader);
    P->zoneInspWidget->readGUIStates(reader);
}

FDYNZoneInspector *FDYNZoneInspectorWindow::zoneInspector()
{
    return P->zoneInspWidget;
}

void FDYNZoneInspectorWindow::SetChildsVisibility(FDYNTreeItem* item, QString exp)
{
    if(item->childCount() > 0)
        for(int i = 0; i < item->childCount(); ++i)
            SetChildsVisibility(item->child(i), exp);
    else if((!item->text(0).contains(exp, Qt::CaseInsensitive)) && (!item->text(1).contains(exp, Qt::CaseInsensitive)))
        item->setHidden(true);
}

void FDYNZoneInspectorWindow::on_lineEdit_textChanged(const QString& arg1)
{
    //ui->sceneTree->clear();
    //ui->sceneTree->setRoot(&NESCENE);
    //ui->sceneTree->update();
    P->zoneInspWidget->update();
    //if(ui->lineEdit->text().length() == 0)
    //    return;
    //SetChildsVisibility(ui->sceneTree->invisibleRootItem(), ui->lineEdit->text());
    //P->zoneInspWidget->expandAll();

}

