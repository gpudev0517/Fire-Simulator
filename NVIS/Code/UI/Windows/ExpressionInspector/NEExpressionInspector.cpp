#include "NEExpressionInspector.h"
#include "ui_NEExpressionInspector.h"
#include "NENode.h"
#include "Managers/NEGUIManager.h"
#include "Base/NESceneManager.h"

//! This is the node item
class NEExpressionInspectorNodeItem : public QTreeWidgetItem
{
public:
    NEExpressionInspectorNodeItem() : QTreeWidgetItem() {}
    ~NEExpressionInspectorNodeItem() {}

    NENode* node() const { return m_pNode; }
    void setNode(NENode* val) { m_pNode = val; }

private:
    NENode* m_pNode;
};

//! This is the property item
class NEExpressionInspectorPropertyItem : public QTreeWidgetItem
{
public:
    NEExpressionInspectorPropertyItem() : QTreeWidgetItem() {}
};


NEExpressionInspector::NEExpressionInspector(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NEExpressionInspector)
{
    ui->setupUi(this);

    m_SelectedNodeItem = 0;
    m_SelectedPropertyItem = 0;
    m_SelectedNodePos = -1;

    m_ExpressionDeleteAction = new QAction("Delete expression", ui->nodePropertyTree);
    ui->nodePropertyTree->addAction(m_ExpressionDeleteAction);
    ui->nodePropertyTree->setContextMenuPolicy(Qt::ActionsContextMenu);
    // Connect expression delete signal to deletePropertyExpression function
    connect(m_ExpressionDeleteAction, &QAction::triggered, this, &NEExpressionInspector::deletePropertyExpression);

    m_DeleteAllExpressionsAction = new QAction("Delete all expressions", ui->nodeNameTree);
    ui->nodeNameTree->addAction(m_DeleteAllExpressionsAction);
    ui->nodeNameTree->setContextMenuPolicy(Qt::ActionsContextMenu);
    // Connect expression delete signal to deleteAllPropertyExpressionsOfNode function
    connect(m_DeleteAllExpressionsAction, &QAction::triggered, this, &NEExpressionInspector::deleteAllPropertyExpressionsOfNode);

    // Connect itemselectionchaged signals
    connect(ui->nodeNameTree, &QTreeWidget::itemSelectionChanged, this, &NEExpressionInspector::selectNode);
    connect(ui->nodePropertyTree, &QTreeWidget::itemSelectionChanged, this, &NEExpressionInspector::selectProperty);
}

NEExpressionInspector::~NEExpressionInspector()
{
    delete ui;
}

void NEExpressionInspector::update()
{
    // Clear all tree widgets first
    ui->nodeNameTree->clear();
    ui->nodePropertyTree->clear();

    // Start the recursive update
    recursiveUpdate(&NESCENE);
    // After updating, restore the selected node and property

    // Re-select the previously selected node
    if(m_SelectedNodePos >= 0)
        ui->nodeNameTree->setCurrentItem(ui->nodeNameTree->topLevelItem(m_SelectedNodePos));
}

void NEExpressionInspector::recursiveUpdate(QObject *root)
{
    // Get the list of objects
    const QObjectList& objList = root->children();
    // Iterate over the children
    foreach(QObject* child, objList)
    {	// If the node is a NENode
        if(NENode* node = qobject_cast<NENode*>(child))
        {	// If there exists an expression in the node
            if(node->propertyUpdateExpressionsMap().size())
                addNode(node);
        }
        if(child->children().size())	// If the node has child properties, do recursion
            recursiveUpdate(child);
    }
}

void NEExpressionInspector::addNode(NENode *node)
{
    NEExpressionInspectorNodeItem* item = new NEExpressionInspectorNodeItem();
    item->setText(0, node->objectName());
    item->setNode(node);
    ui->nodeNameTree->addTopLevelItem(item);
}

void NEExpressionInspector::showNodeExpressions(NENode *node)
{
    ui->nodePropertyTree->clear();

    QMap<QString, QString>::const_iterator iter;
    for(iter = node->propertyUpdateExpressionsMap().begin(); iter != node->propertyUpdateExpressionsMap().end(); ++iter)
    {
        NEExpressionInspectorPropertyItem* item = new NEExpressionInspectorPropertyItem();
        item->setText(0, iter.key());
        ui->nodePropertyTree->addTopLevelItem(item);
    }
}

void NEExpressionInspector::selectNode()
{
    QList<QTreeWidgetItem*> items = ui->nodeNameTree->selectedItems();
    if(items.isEmpty())
        return;
    NEExpressionInspectorNodeItem* item = static_cast<NEExpressionInspectorNodeItem*>(items.first());

    if(m_SelectedNodeItem && item == m_SelectedNodeItem)
        return; // The node is already selected

    m_SelectedNodeItem = item;

    showNodeExpressions(item->node());

    NEGUI.selectObject(item->node());
}

void NEExpressionInspector::selectProperty()
{
    QList<QTreeWidgetItem*> items = ui->nodePropertyTree->selectedItems();
    if(items.isEmpty())
        return;
    NEExpressionInspectorPropertyItem* item = static_cast<NEExpressionInspectorPropertyItem*>(items.first());

    m_SelectedPropertyItem = item;

    NEGUI.showNodePropertyExpression(m_SelectedNodeItem->node(), m_SelectedPropertyItem->text(0));
}

void NEExpressionInspector::deletePropertyExpression()
{
    // Return if a property item is not selected
    if(!m_SelectedPropertyItem)
        return;

    NEPushAttrib(NE_UI_UPDATE_STATE);
    // Disable UI update in case of calls to NENode functions
    SetUpdateUI(false);
    // Remove the property expression from the node
    m_SelectedNodeItem->node()->removePropertyUpdateExpression(m_SelectedPropertyItem->text(0));
    // Save the node position
    m_SelectedNodePos = ui->nodeNameTree->indexOfTopLevelItem(m_SelectedNodeItem);
    // Reset the selected node pointer
    m_SelectedNodeItem = 0;
    // Reset the selected property pointer
    m_SelectedPropertyItem = 0;
    // Re-enable UI update for calls to NENode functions
    NEPopAttrib(NE_UI_UPDATE_STATE);
    // Update the expression inspector
    update();
}

void NEExpressionInspector::deleteAllPropertyExpressionsOfNode()
{
    // Return if a node item is not selected
    if(!m_SelectedNodeItem)
        return;

    NEPushAttrib(NE_UI_UPDATE_STATE);
    // Disable UI update in case of calls to NENode functions
    SetUpdateUI(false);
    // Remove all property expression of the node
    m_SelectedNodeItem->node()->removeAllPropertyUpdateExpressions();
    // Reset the selected node position
    m_SelectedNodePos = -1;
    // Reset the selected node pointer
    m_SelectedNodeItem = 0;
    // Reset the selected property pointer
    m_SelectedPropertyItem = 0;
    // Re-enable UI update for calls to NENode functions
    NEPopAttrib(NE_UI_UPDATE_STATE);
    // Update the expression inspector
    update();
}




