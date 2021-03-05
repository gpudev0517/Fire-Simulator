#include "Base/NESceneManager.h"
#include "FDYNScenarioInspectorWindow.h"
#include "FDYNScenarioInspector.h"
#include "Core/NENode.h"



class FDYNScenarioInspectorWindowPrivate
{
public:
    FDYNScenarioInspectorWindowPrivate(FDYNScenarioInspectorWindow* s) : S(s) {}

    FDYNScenarioInspector* scenarioInspWidget;
private:
    FDYNScenarioInspectorWindow* S;
};


FDYNScenarioInspectorWindow::FDYNScenarioInspectorWindow(QWidget* parent) :
    QMainWindow(parent)
{
    //ui->setupUi(this);
    //ui->lineEdit->setVisible(false);
    //ui->label->setVisible(false);


    P = new FDYNScenarioInspectorWindowPrivate(this);

    P->scenarioInspWidget = new FDYNScenarioInspector( this );
    P->scenarioInspWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    P->scenarioInspWidget->setMinimumHeight(80);

    setCentralWidget(P->scenarioInspWidget);
}

FDYNScenarioInspectorWindow::~FDYNScenarioInspectorWindow()
{
    if(P->scenarioInspWidget)
        delete P->scenarioInspWidget;
    delete P;
}

void FDYNScenarioInspectorWindow::removeSelectedItem()
{
    P->scenarioInspWidget->removeSelectedItem();
}

void FDYNScenarioInspectorWindow::setRoot(QObject* root)
{
    //ui->sceneTree->setRoot(root);
    P->scenarioInspWidget->setRoot(root);
}

void FDYNScenarioInspectorWindow::update()
{
    //ui->sceneTree->update();
    P->scenarioInspWidget->update();
}

void FDYNScenarioInspectorWindow::setNewCurrentItem(QObject* obj)
{
    //ui->sceneTree->setNewCurrentItem(obj);
    P->scenarioInspWidget->setNewCurrentItem(obj);
}

void FDYNScenarioInspectorWindow::setCurrentItems(QList<NENode*> objList)
{
    //ui->sceneTree->setCurrentItems(objList);
    P->scenarioInspWidget->setCurrentItems(objList);
}

bool FDYNScenarioInspectorWindow::isSelectedObject(NENode* obj)
{
    return P->scenarioInspWidget->isSelectedObect(obj);
    //return ui->sceneTree->isSelectedObect(obj);
}

void FDYNScenarioInspectorWindow::setParentCurrentItem()
{
    //ui->sceneTree->setParentCurrentItem();
    P->scenarioInspWidget->setParentCurrentItem();

}

void FDYNScenarioInspectorWindow::writeGUIState(QXmlStreamWriter* writer)
{
    writer->writeStartElement("ScenarioInspector");
    P->scenarioInspWidget->saveGUIStates(writer);
    writer->writeEndElement();
}

void FDYNScenarioInspectorWindow::readGUIState(QXmlStreamReader* reader)
{
    //ui->sceneTree->readGUIStates(reader);
    P->scenarioInspWidget->readGUIStates(reader);
}

FDYNScenarioInspector *FDYNScenarioInspectorWindow::scenarioInspector()
{
    return P->scenarioInspWidget;
}


void FDYNScenarioInspectorWindow::SetChildsVisibility(FDYNTreeItem* item, QString exp)
{
    if(item->childCount() > 0)
        for(int i = 0; i < item->childCount(); ++i)
            SetChildsVisibility(item->child(i), exp);
    else if((!item->text(0).contains(exp, Qt::CaseInsensitive)) && (!item->text(1).contains(exp, Qt::CaseInsensitive)))
        item->setHidden(true);
}

void FDYNScenarioInspectorWindow::on_lineEdit_textChanged(const QString& arg1)
{
    //ui->sceneTree->clear();
    //ui->sceneTree->setRoot(&NESCENE);
    //ui->sceneTree->update();
    P->scenarioInspWidget->update();
    //if(ui->lineEdit->text().length() == 0)
    //    return;
    //SetChildsVisibility(ui->sceneTree->invisibleRootItem(), ui->lineEdit->text());
    //ui->sceneTree->expandAll();

}
