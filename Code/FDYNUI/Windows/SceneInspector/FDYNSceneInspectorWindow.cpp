#include "Base/NESceneManager.h"
#include "FDYNSceneInspectorWindow.h"
#include "Core/NENode.h"
#include "FDYNSceneInspector.h"

#include <QQuickWidget>

class FDYNSceneInspectorWindowPrivate
{
public:
    FDYNSceneInspectorWindowPrivate(FDYNSceneInspectorWindow* s) : S(s) {}

    FDYNSceneInspector* sceneInspWidget;
private:
    FDYNSceneInspectorWindow* S;
};

FDYNSceneInspectorWindow::FDYNSceneInspectorWindow(QWidget* parent): QMainWindow(parent)
{
#if 0
    ui->setupUi(this);
    ui->lineEdit->setVisible(false);
    ui->label->setVisible(false);
#endif

    P = new FDYNSceneInspectorWindowPrivate(this);

    P->sceneInspWidget = new FDYNSceneInspector( this );
    P->sceneInspWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    P->sceneInspWidget->setMinimumHeight(80);

    setCentralWidget(P->sceneInspWidget);
}

FDYNSceneInspectorWindow::~FDYNSceneInspectorWindow()
{
    if( P->sceneInspWidget )
        delete P->sceneInspWidget;
    delete P;
}

FDYNSceneInspector *FDYNSceneInspectorWindow::sceneInspector()
{
    return P->sceneInspWidget;
}

void FDYNSceneInspectorWindow::removeSelectedItem()
{
    P->sceneInspWidget->removeSelectedItem();
}

void FDYNSceneInspectorWindow::setRoot(QObject* root)
{
    P->sceneInspWidget->setRoot(root);
}

void FDYNSceneInspectorWindow::update()
{
    P->sceneInspWidget->update();
}

void FDYNSceneInspectorWindow::setNewCurrentItem(QObject* obj)
{
    P->sceneInspWidget->setNewCurrentItem(obj);
}

void FDYNSceneInspectorWindow::setCurrentItems(QList<NENode*> objList)
{
    P->sceneInspWidget->setCurrentItems(objList);
}

bool FDYNSceneInspectorWindow::isSelectedObject(NENode* obj)
{
    return P->sceneInspWidget->isSelectedObect(obj);
}

void FDYNSceneInspectorWindow::setParentCurrentItem()
{
    P->sceneInspWidget->setParentCurrentItem();
}

void FDYNSceneInspectorWindow::writeGUIState(QXmlStreamWriter* writer)
{
    writer->writeStartElement("SceneInspector");
    P->sceneInspWidget->saveGUIStates(writer);
    writer->writeEndElement();
}

void FDYNSceneInspectorWindow::readGUIState(QXmlStreamReader* reader)
{
    P->sceneInspWidget->readGUIStates(reader);
}

void FDYNSceneInspectorWindow::SetChildsVisibility(FDYNTreeItem* item, QString exp)
{
    if(item->childCount() > 0)
        for(int i = 0; i < item->childCount(); ++i)
            SetChildsVisibility(item->child(i), exp);
    else if((!item->text(0).contains(exp, Qt::CaseInsensitive)) && (!item->text(1).contains(exp, Qt::CaseInsensitive)))
        item->setHidden(true);
}

void FDYNSceneInspectorWindow::on_lineEdit_textChanged(const QString& arg1)
{
    //P->sceneInspWidget->clear();
    //P->sceneInspWidget->setRoot(&NESCENE);
    P->sceneInspWidget->update();
#if 0
    if(ui->lineEdit->text().length() == 0)
        return;
#endif
    //SetChildsVisibility(P->sceneInspWidget>invisibleRootItem(), ui->lineEdit->text());
    //P->sceneInspWidget->expandAll();

}
