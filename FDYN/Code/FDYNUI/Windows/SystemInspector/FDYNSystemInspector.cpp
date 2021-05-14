#include "Core/Base/NEBase.h"
#include "FDYNSystemInspector.h"
//#include "Utilities/NEGUIStyle.h"
#include "Core/Base/NESceneManager.h"
#include "Managers/FDYNGUIManager.h"
#include "Core/NENode.h"
#include "Core/Base/NENodeFactory.h"
#include "FDYNSystemInspectorItem.h"




FDYNSystemInspector::FDYNSystemInspector(QWidget* parent) : FDYNTreeWidget(parent)
{
//    m_pBoldFont = const_cast<QFont*>(NEGUISTYLE.BoldFont());
//    m_pBrushLight = const_cast<QBrush*>(NEGUISTYLE.GrayFontBrush());
//    m_pBrushBlack = const_cast<QBrush*>(NEGUISTYLE.NormalFontBrush());
//    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    // Make necessary connections
    QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectItems()));

    //m_pBrush

    //context menu
    connect(treeview, SIGNAL(sigCreateContextMenu()), this, SLOT(createContextMenu()));
}

FDYNSystemInspector::~FDYNSystemInspector()
{
}

void FDYNSystemInspector::addRoot(QObject *root)
{
    m_Roots.append(root);
}

void FDYNSystemInspector::removeRoot(QObject *root)
{
    m_Roots.removeAll(root);
}

void FDYNSystemInspector::update()
{
    this->clear();
    //Q_ASSERT_X(m_pRoot, " NEGUIInspector::update", "root pointer is null");
    //const QObjectList& objList = m_pRoot->children();


    //foreach(const QObject* child, objList)
    foreach(const QObject* child, m_Roots)
    {
        FDYNSystemInspectorItem* tmpItem = 0;
        if((child->metaObject()->className()[0]=='N' && child->metaObject()->className()[1]=='E') ||
                (child->metaObject()->className()[0]=='F' && child->metaObject()->className()[1]=='D' && child->metaObject()->className()[2]=='Y' && child->metaObject()->className()[3]=='N'))
        {
            tmpItem = new FDYNSystemInspectorItem();
#if 0
            tmpItem->setForeground(0, *m_pBrushBlack);
            tmpItem->setFont(0, *m_pBoldFont);
#endif
            tmpItem->setText(0, child->objectName());
            //tmpItem->setText(1, child->metaObject()->className());
#if 0
            tmpItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
#endif
            tmpItem->setObject(const_cast<QObject*>(child));
            this->addTopLevelItem(tmpItem);
            tmpItem->setExpanded(true);
        }
        if(tmpItem && child->children().size())
            traverseTree(child, tmpItem);
    }
    //reset current item
    setCurrentItem(NULL);
    FDYNTreeWidget::update();
#if 0
    resizeColumnToContents(0);
#endif
}

void FDYNSystemInspector::traverseTree(const QObject* obj, FDYNSystemInspectorItem* parent)
{
    const QObjectList& objList = obj->children();
    foreach(const QObject* child, objList)
    {
        FDYNSystemInspectorItem* tmpItem = 0;
        tmpItem = new FDYNSystemInspectorItem();
        tmpItem->setText(0, child->objectName());
        //tmpItem->setText(1, child->metaObject()->className());
#if 0
        tmpItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
#endif
        tmpItem->setObject(const_cast<QObject*>(child));
        tmpItem->setExpanded(true);

        parent->addChild(tmpItem);
//                    }
//                }

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

void FDYNSystemInspector::mousePressEvent(QMouseEvent *event)
{
    FDYNTreeWidget::mousePressEvent(event);
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

void FDYNSystemInspector::createContextMenu()
{
    // DELETING OLD ACTIONS FROM CONTEXT MENU
    QList<QAction*> currentActions = actions();
    QListIterator<QAction*> i(currentActions);

    while(i.hasNext())
    {
        removeAction(i.next());
    }

    // Get current (selected) item
    FDYNSystemInspectorItem* item = (FDYNSystemInspectorItem*) currentItem();
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

void FDYNSystemInspector::removeSelectedItem()
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
        FDYNGUI.EvalPyGUI(script);
        update();
        FDYNGUI.selectGUIObject(NULL);
    }
}


bool FDYNSystemInspector::isSelectedObect(QObject* object)
{
    const FDYNSystemInspectorItem* item = (FDYNSystemInspectorItem*) currentItem();
    if(item)
        return (object == item->Object());
    else
        return false;
}

void FDYNSystemInspector::selectItems()
{
    const QList<FDYNTreeItem *>& items = selectedItems();

    if(items.size() == 1)
    {
        FDYNSystemInspectorItem* sItem =  qobject_cast<FDYNSystemInspectorItem*>(items.first());
        if(sItem){
            QObject *object = sItem->Object();
            if(object)
                FDYNGUI.selectGUIObject(object);
        }

    }

    else if(items.size() > 0)
    {
        QList<NENode*> objects;
        foreach(FDYNTreeItem* item2, items)
        {
            FDYNSystemInspectorItem* sItem =  qobject_cast<FDYNSystemInspectorItem*>(item2);
            if(sItem){
                NENode* casted = qobject_cast<NENode*>(sItem->Object());
                if(casted)
                    objects.push_back(casted);
            }

        }
        FDYNGUI.selectObjects(objects);
    }
    else{
        FDYNGUI.selectObject(NULL);
    }
}



/*!
 If we would like to make the parent of the deleted instance the current object this method should be called.
 However, than we should also be able to keep the selected/current item when updating the qtreewidget.
 This is not possible yet.
*/
void FDYNSystemInspector::setParentCurrentItem()
{
    FDYNSystemInspectorItem* item = (FDYNSystemInspectorItem*) currentItem();
    FDYNSystemInspectorItem* parent = (FDYNSystemInspectorItem*)item->parent();
    setCurrentItem(parent);
    FDYNGUI.selectGUIObject(parent->Object());
}


void FDYNSystemInspector::setNewCurrentItem(QObject* object)
{
    if(object)
    {
        QList<FDYNTreeItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
        QListIterator<FDYNTreeItem*> i(items);

        if(i.hasNext())
        {
            FDYNSystemInspectorItem* item = (FDYNSystemInspectorItem*) (i.next());
            setCurrentItem(item);
            //selectItem(i.next(),0);
        }
    }
}

