#include "Core/Base/NEBase.h"
#include "NESystemInspector.h"
#include "Utilities/NEGUIStyle.h"
#include "Core/Base/NESceneManager.h"
#include "Managers/NEGUIManager.h"
#include "Core/NENode.h"
#include "Core/Base/NENodeFactory.h"

class NESystemInspectorItem : public QTreeWidgetItem
{
public:
    NESystemInspectorItem() : QTreeWidgetItem() {}
    ~NESystemInspectorItem() {}

    QObject* Object() const { return m_pObject; }
    void setObject(QObject* val) { m_pObject = val; }

private:
    QObject* m_pObject;
};



NESystemInspector::NESystemInspector(QWidget* parent) : QTreeWidget(parent)
{
    m_pBoldFont = const_cast<QFont*>(NEGUISTYLE.BoldFont());
    m_pBrushLight = const_cast<QBrush*>(NEGUISTYLE.GrayFontBrush());
    m_pBrushBlack = const_cast<QBrush*>(NEGUISTYLE.NormalFontBrush());
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    // Make necessary connections
    QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectItems()));

    //m_pBrush
}

NESystemInspector::~NESystemInspector()
{
}

void NESystemInspector::addRoot(QObject *root)
{
    m_Roots.append(root);
}

void NESystemInspector::removeRoot(QObject *root)
{
    m_Roots.removeAll(root);
}

void NESystemInspector::update()
{
    this->clear();
    //Q_ASSERT_X(m_pRoot, " NEGUIInspector::update", "root pointer is null");
    //const QObjectList& objList = m_pRoot->children();


    //foreach(const QObject* child, objList)
    foreach(const QObject* child, m_Roots)
    {
        NESystemInspectorItem* tmpItem = 0;
        if(child->metaObject()->className()[0]=='N' && child->metaObject()->className()[1]=='E')
        {
            tmpItem = new NESystemInspectorItem();
            tmpItem->setForeground(0, *m_pBrushBlack);
            tmpItem->setFont(0, *m_pBoldFont);
            tmpItem->setText(0, child->objectName());
            //tmpItem->setText(1, child->metaObject()->className());
            tmpItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
            tmpItem->setObject(const_cast<QObject*>(child));
            this->addTopLevelItem(tmpItem);
            tmpItem->setExpanded(true);
        }
        if(tmpItem && child->children().size())
            traverseTree(child, tmpItem);
    }
    //reset current item
    setCurrentItem(NULL);
    resizeColumnToContents(0);
}

void NESystemInspector::traverseTree(const QObject* obj, NESystemInspectorItem* parent)
{
    const QObjectList& objList = obj->children();
    foreach(const QObject* child, objList)
    {
        NESystemInspectorItem* tmpItem = 0;
        tmpItem = new NESystemInspectorItem();
        tmpItem->setText(0, child->objectName());
        //tmpItem->setText(1, child->metaObject()->className());
        tmpItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
        tmpItem->setObject(const_cast<QObject*>(child));
        tmpItem->setExpanded(true);

        parent->addChild(tmpItem);
        //            }
        //		}

        // Recursion
        if(tmpItem && child->children().size())
        {
            traverseTree(child, tmpItem);
        }
        else if(child->children().size())
        {
            traverseTree(child, parent);
        }
    }
}

void NESystemInspector::mousePressEvent(QMouseEvent *event)
{
    QTreeWidget::mousePressEvent(event);
    //right clicks should open a pop-up

    switch(event->button())
    {
    case Qt::RightButton:
        createContextMenu();
        break;
    default:break;
        //default:
        //	QTreeWidget::mousePressEvent(event);
    }



    //if (event->button()== Qt::RightButton)
    //	createContextMenu();

    //QTreeWidget::mousePressEvent(event);


}

void NESystemInspector::createContextMenu()
{
    // DELETING OLD ACTIONS FROM CONTEXT MENU
    QList<QAction*> currentActions = actions();
    QListIterator<QAction*> i(currentActions);

    while(i.hasNext())
    {
        removeAction(i.next());
    }

    // Get current (selected) item
    NESystemInspectorItem* item = (NESystemInspectorItem*) currentItem();
    if(item != NULL)
    {
        QObject* object = item->Object();

        //NEObject* tmpObject = NULL;
        //if(item != NULL)
        // tmpObject = qobject_cast<NENode*> (object);

        if(NEObject* tmpObject =  qobject_cast<NEObject*> (object))
        {
            //populate context menu with actions
            const QList<QObject*> &objectActions = *tmpObject->actions();
            foreach(QObject* action, objectActions)
            {
                addAction(qobject_cast<QAction *>(action));
            }
            //set context policy
            this->setContextMenuPolicy(Qt::ActionsContextMenu);
        }
    }
}

void NESystemInspector::removeSelectedItem()
{
    QMap<NEObject*,NEObject*> selObjs = *NESCENE.selectedObjects();
    selObjs[NESCENE.selectedObject()] = NESCENE.selectedObject();

    QString script;

    foreach(NEObject* o, selObjs)
    {
        if(o == 0)	// Sometimes null objects leaks into this map, this is a hack to omit it
        {
            selObjs.remove(o);
            continue;
        }

        if(NENode* node = qobject_cast<NENode*>(o))
        {
            QString name = node->objectName();
            QString type = node->factory().nodeName();//tmpObject->metaObject()->className();
            //assemble the python script
            script += NESCENE.objectName()+".deleteObject(\"" + type + "\", \""+name+"\", True, True)\n";
        }
    }

    if(selObjs.size())
    {
        NEGUI.EvalPyGUI(script);
        update();
        setCurrentItem(NULL);
        NEGUI.selectGUIObject(NULL);
    }
}


bool NESystemInspector::isSelectedObect(QObject* object)
{
    const NESystemInspectorItem* item = (NESystemInspectorItem*) currentItem();
    if(item)
        return (object == item->Object());
    else
        return false;
}

void NESystemInspector::selectItems()
{
    const QList<QTreeWidgetItem *>& items = selectedItems();

    if(items.size() == 1)
    {
        NESystemInspectorItem* sItem =  (NESystemInspectorItem*)items.first();
        QObject* object = sItem->Object();//((NEGUIInspectorItem*)item)->Object();
        NEGUI.selectGUIObject(object);
    }

    else
    {
        QList<NENode*> objects;
        foreach(QTreeWidgetItem* item2, items)
        {
            NESystemInspectorItem* sItem =  (NESystemInspectorItem*)item2;
            NENode* casted = qobject_cast<NENode*>(sItem->Object());

            if(casted)
                objects.push_back(casted);
        }
        NEGUI.selectObjects(objects);
    }
}



/*!
 If we would like to make the parent of the deleted instance the current object this method should be called.
 However, than we should also be able to keep the selected/current item when updating the qtreewidget.
 This is not possible yet.
*/
void NESystemInspector::setParentCurrentItem()
{
    NESystemInspectorItem* item = (NESystemInspectorItem*) currentItem();
    NESystemInspectorItem* parent = (NESystemInspectorItem*)item->parent();
    setCurrentItem(parent);
    NEGUI.selectGUIObject(parent->Object());
}


void NESystemInspector::setNewCurrentItem(QObject* object)
{
    if(object)
    {
        QList<QTreeWidgetItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
        QListIterator<QTreeWidgetItem*> i(items);

        if(i.hasNext())
        {
            NESystemInspectorItem* item = (NESystemInspectorItem*) (i.next());
            setCurrentItem(item);
            //selectItem(i.next(),0);
        }
    }
}

