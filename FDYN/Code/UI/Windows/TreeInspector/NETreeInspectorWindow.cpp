#include "Base/NESceneManager.h"
#include "NETreeInspectorWindow.h"
#include "ui_NETreeInspectorWindow.h"
#include "Core/NENode.h"


NETreeInspectorWindow::NETreeInspectorWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::NETreeInspectorWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setVisible(false);
    ui->label->setVisible(false);
}

NETreeInspectorWindow::~NETreeInspectorWindow()
{
    delete ui;
}

void NETreeInspectorWindow::removeSelectedItem()
{
    ui->sceneTree->removeSelectedItem();
}

void NETreeInspectorWindow::setRoot(QObject* root)
{
    ui->sceneTree->setRoot(root);
}

void NETreeInspectorWindow::update()
{
    ui->sceneTree->update();
}

void NETreeInspectorWindow::setNewCurrentItem(QObject* obj)
{
    ui->sceneTree->setNewCurrentItem(obj);
}

void NETreeInspectorWindow::setCurrentItems(QList<NENode*> objList)
{
    ui->sceneTree->setCurrentItems(objList);
}

bool NETreeInspectorWindow::isSelectedObject(NENode* obj)
{
    return ui->sceneTree->isSelectedObect(obj);
}

void NETreeInspectorWindow::setParentCurrentItem()
{
    ui->sceneTree->setParentCurrentItem();
}

void NETreeInspectorWindow::writeGUIState(QXmlStreamWriter* writer)
{
    writer->writeStartElement("SceneInspector");
    ui->sceneTree->saveGUIStates(writer);
    writer->writeEndElement();
}

void NETreeInspectorWindow::readGUIState(QXmlStreamReader* reader)
{
    ui->sceneTree->readGUIStates(reader);
}

void NETreeInspectorWindow::SetChildsVisibility(QTreeWidgetItem* item, QString exp)
{
    if(item->childCount() > 0)
        for(int i = 0; i < item->childCount(); ++i)
            SetChildsVisibility(item->child(i), exp);
    else if((!item->text(0).contains(exp, Qt::CaseInsensitive)) && (!item->text(1).contains(exp, Qt::CaseInsensitive)))
        item->setHidden(true);
}

void NETreeInspectorWindow::on_lineEdit_textChanged(const QString& arg1)
{
    ui->sceneTree->clear();
    ui->sceneTree->setRoot(&NESCENE);
    ui->sceneTree->update();
    if(ui->lineEdit->text().length() == 0)
        return;
    SetChildsVisibility(ui->sceneTree->invisibleRootItem(), ui->lineEdit->text());
    ui->sceneTree->expandAll();

}
