#include "Base/NESceneManager.h"
#include "FZoneInspectorWindow.h"
#include "ui_FZoneInspectorWindow.h"
#include "Core/NENode.h"


FZoneInspectorWindow::FZoneInspectorWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::FZoneInspectorWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setVisible(false);
    ui->label->setVisible(false);
}

FZoneInspectorWindow::~FZoneInspectorWindow()
{
    delete ui;
}

void FZoneInspectorWindow::removeSelectedItem()
{
    ui->sceneTree->removeSelectedItem();
}

void FZoneInspectorWindow::setRoot(QObject* root)
{
    ui->sceneTree->setRoot(root);
}

void FZoneInspectorWindow::update()
{
    ui->sceneTree->update();
}

void FZoneInspectorWindow::setNewCurrentItem(QObject* obj)
{
    ui->sceneTree->setNewCurrentItem(obj);
}

void FZoneInspectorWindow::setCurrentItems(QList<NENode*> objList)
{
    ui->sceneTree->setCurrentItems(objList);
}

bool FZoneInspectorWindow::isSelectedObject(NENode* obj)
{
    return ui->sceneTree->isSelectedObect(obj);
}

void FZoneInspectorWindow::setParentCurrentItem()
{
    ui->sceneTree->setParentCurrentItem();
}

void FZoneInspectorWindow::writeGUIState(QXmlStreamWriter* writer)
{
    writer->writeStartElement("ZoneInspector");
    ui->sceneTree->saveGUIStates(writer);
    writer->writeEndElement();
}

void FZoneInspectorWindow::readGUIState(QXmlStreamReader* reader)
{
    ui->sceneTree->readGUIStates(reader);
}

void FZoneInspectorWindow::SetChildsVisibility(QTreeWidgetItem* item, QString exp)
{
    if(item->childCount() > 0)
        for(int i = 0; i < item->childCount(); ++i)
            SetChildsVisibility(item->child(i), exp);
    else if((!item->text(0).contains(exp, Qt::CaseInsensitive)) && (!item->text(1).contains(exp, Qt::CaseInsensitive)))
        item->setHidden(true);
}

void FZoneInspectorWindow::on_lineEdit_textChanged(const QString& arg1)
{
    ui->sceneTree->clear();
    ui->sceneTree->setRoot(&NESCENE);
    ui->sceneTree->update();
    if(ui->lineEdit->text().length() == 0)
        return;
    SetChildsVisibility(ui->sceneTree->invisibleRootItem(), ui->lineEdit->text());
    ui->sceneTree->expandAll();

}
