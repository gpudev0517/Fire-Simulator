#include "Base/NESceneManager.h"
#include "NESceneInspectorWindow.h"
#include "ui_NESceneInspectorWindow.h"
#include "Core/NENode.h"


NESceneInspectorWindow::NESceneInspectorWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::NESceneInspectorWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setVisible(false);
    ui->label->setVisible(false);
}

NESceneInspectorWindow::~NESceneInspectorWindow()
{
    delete ui;
}

void NESceneInspectorWindow::removeSelectedItem()
{
    ui->sceneTree->removeSelectedItem();
}

void NESceneInspectorWindow::setRoot(QObject* root)
{
    ui->sceneTree->setRoot(root);
}

void NESceneInspectorWindow::update()
{
    ui->sceneTree->update();
}

void NESceneInspectorWindow::setNewCurrentItem(QObject* obj)
{
    ui->sceneTree->setNewCurrentItem(obj);
}

void NESceneInspectorWindow::setCurrentItems(QList<NENode*> objList)
{
    ui->sceneTree->setCurrentItems(objList);
}

bool NESceneInspectorWindow::isSelectedObject(NENode* obj)
{
    return ui->sceneTree->isSelectedObect(obj);
}

void NESceneInspectorWindow::setParentCurrentItem()
{
    ui->sceneTree->setParentCurrentItem();
}

void NESceneInspectorWindow::writeGUIState(QXmlStreamWriter* writer)
{
    writer->writeStartElement("SceneInspector");
    ui->sceneTree->saveGUIStates(writer);
    writer->writeEndElement();
}

void NESceneInspectorWindow::readGUIState(QXmlStreamReader* reader)
{
    ui->sceneTree->readGUIStates(reader);
}

void NESceneInspectorWindow::SetChildsVisibility(QTreeWidgetItem* item, QString exp)
{
    if(item->childCount() > 0)
        for(int i = 0; i < item->childCount(); ++i)
            SetChildsVisibility(item->child(i), exp);
    else if((!item->text(0).contains(exp, Qt::CaseInsensitive)) && (!item->text(1).contains(exp, Qt::CaseInsensitive)))
        item->setHidden(true);
}

void NESceneInspectorWindow::on_lineEdit_textChanged(const QString& arg1)
{
    ui->sceneTree->clear();
    ui->sceneTree->setRoot(&NESCENE);
    ui->sceneTree->update();
    if(ui->lineEdit->text().length() == 0)
        return;
    SetChildsVisibility(ui->sceneTree->invisibleRootItem(), ui->lineEdit->text());
    ui->sceneTree->expandAll();

}
