#include "Core/Base/NEBase.h"
#include "NEObject.h"
#include "NENode.h"
#include "Spatial/NESpatialGroup.h"
#include "Base/NESceneManager.h"
#include "Managers/NEGUIManager.h"
#include "Utilities/NEGUIStyle.h"
#include "FZoneInspector.h"
#include "FZoneInspectorWindow.h"

#include "Mesh/NEZone.h"
#include "Mesh/NEZoneItem.h"
#include "Mesh/NEScenario.h"

class FZoneInspectorItem : public QTreeWidgetItem
{
public:
    FZoneInspectorItem() : QTreeWidgetItem(), m_pObject(0) {}
    ~FZoneInspectorItem() {}

    QObject* Object() const { return m_pObject; }
    void setObject(QObject* val) { m_pObject = val; }

private:
    QObject* m_pObject;
};

// This class is used to make the second column non-editable
class EditDelegate: public QStyledItemDelegate
{
public:
    EditDelegate(QObject* parent=0, QRegExpValidator* validator=0): QStyledItemDelegate(parent), m_pValidator(validator) { }
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        if(index.column() == 0)
        {
            QWidget* w =  QStyledItemDelegate::createEditor(parent, option, index);
            if(QLineEdit* le = qobject_cast<QLineEdit*>(w))
            {
                le->setValidator(m_pValidator);
                return le;
            }
        }

        return 0;
    }
private:
    QRegExpValidator* m_pValidator;

};



FZoneInspector::FZoneInspector(QWidget* parent) : QTreeWidget(parent)
{
    clearState();
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_pRoot = 0;
    // Make necessary connections
    QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectItems()));

    // Create name validation text edit delagate
    validator = new QRegExpValidator(QRegExp("[A-Za-z_][A-Za-z_0-9]*"), this);
    editDelagate =  new EditDelegate(this, validator);

    // Make second columns non-editable
    setItemDelegateForColumn(0, editDelagate);
    setItemDelegateForColumn(1, editDelagate);

    QObject::connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(nodeRenamed(QTreeWidgetItem*, int)));

    m_IconActive = new QIcon(":/Neutrino/Resources/32/active.png");
    m_IconCache = new QIcon(":/Neutrino/Resources/32/cache.png");
    m_IconInactive = new QIcon(":/Neutrino/Resources/32/deactive.png");

    connect(this, &QTreeWidget::itemExpanded, this, &FZoneInspector::itemExpansion);
    connect(this, &QTreeWidget::itemCollapsed, this, &FZoneInspector::itemCollapse);
    connect(this, &QTreeWidget::itemClicked, this, &FZoneInspector::itemActivation);

    groupAction = new QAction("Group", 0);
    ungroupAction = new QAction("Ungroup", 0);
//    connect(groupAction, &QAction::triggered, this, &FZoneInspector::groupNodes);
//    connect(ungroupAction, &QAction::triggered, this, &FZoneInspector::ungroupNodes);

    connect(groupAction, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedGroup);
    connect(ungroupAction, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedUngroup);


    parentAction = new QAction("Parent", 0);
    unparentAction = new QAction("Unparent", 0);
    connect(parentAction, &QAction::triggered, this, &FZoneInspector::parentNodes);
    connect(unparentAction, &QAction::triggered, this, &FZoneInspector::unparentNodes);

    //m_pBrush
}

FZoneInspector::~FZoneInspector()
{
    while(topLevelItemCount())
        delete takeTopLevelItem(0);

    delete m_IconActive;
    delete m_IconCache;
    delete m_IconInactive;
    delete groupAction;
    delete ungroupAction;
    delete parentAction;
    delete unparentAction;
}

void FZoneInspector::setRoot( QObject* root)
{
    m_pRoot = root;
}

void FZoneInspector::update()
{
    if(!m_pRoot)
        return;

    this->clear();
    // Clear the pointers before rebuilding the contents
    clearState();
    Q_ASSERT_X(m_pRoot, " FZoneInspector::update", "root pointer is null");

    const QObject* child = m_pRoot;
    updateChildren( child );
}

void FZoneInspector::updateChildren(const QObject* child)
{

    FZoneInspectorItem* tmpItem = new FZoneInspectorItem();
    tmpItem->setForeground(0, *NEGUISTYLE.NormalFontBrush());
    tmpItem->setFont(0, *NEGUISTYLE.BoldFont());
    tmpItem->setText(0, child->objectName());
    tmpItem->setText(1, "Engine");
    tmpItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
    tmpItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
    tmpItem->setObject(const_cast<QObject*>(child));
    this->addTopLevelItem(tmpItem);
    tmpItem->setExpanded(true);

    if(tmpItem->Object() == &NESCENE)
        SceneItem = tmpItem;

    if(child->children().size())
        traverseTree(child, tmpItem);

    restoreExpandedState();
    resizeColumnToContents(0);
}

void FZoneInspector::traverseTree(const QObject* obj, FZoneInspectorItem* parent)
{
    const QObjectList& objList = obj->children();
    Q_FOREACH(const QObject* child, objList)
    {

        if( qobject_cast< NEScenario * >( const_cast<QObject*>(child))  )
        {
            continue;
        }

        FZoneInspectorItem* tmpItem = new FZoneInspectorItem();

        tmpItem->setText(0, child->objectName());
        tmpItem->setText(1, child->metaObject()->className());
        tmpItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
        tmpItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
        tmpItem->setObject( const_cast<QObject*> (child) );
        tmpItem->setFlags(tmpItem->flags()^Qt::ItemIsEditable);
        //tmpItem->setExpanded(true);

        if(NEObject* neObject = qobject_cast<NEObject*>(const_cast<QObject*>(child)))
        {
            NENode* node = qobject_cast<NENode*>(neObject);
            //if(neObject->isHidden())
            {
                node = qobject_cast<NENode*> (const_cast<QObject*>(child));
                if( node == NULL )
                    continue;

                //std::cout << "node sub group is " << child->objectName().toStdString() << std::endl;
                FZoneInspectorItem *categoryItem = NULL;
                if(node->factory().nodeInherits() != "Groups")
                {
                    QString keyString = parent->text(0);
                    FZoneInspectorItem *pp = dynamic_cast<FZoneInspectorItem *>(parent->parent());
                    while (pp)
                    {
                        keyString += "," + pp->text(0);
                        pp = dynamic_cast<FZoneInspectorItem *>(pp->parent());
                    }

                    //std::cout << "Key String: " << keyString.toStdString() << std::endl;
                    if( NEZoneItem* zoneItem = qobject_cast<NEZoneItem*>(const_cast<QObject*>(child)))
                    {

                        //keyString += "," + node->factory().nodeInherits();
                        keyString += "," + zoneItem->categoryType();
                        if( SceneInpectorItems.find(keyString) == SceneInpectorItems.end()){

                            //categoryItem = createOrganizerItem(node->factory().nodeInherits(), parent);
                            categoryItem = createOrganizerItem(zoneItem->categoryType(), parent);
                            SceneInpectorItems.insert(keyString, categoryItem);
                        }

                        categoryItem = SceneInpectorItems.value(keyString);
                        categoryItem->addChild(tmpItem);
                    }

                }
                else
                {
                    tmpItem->setForeground(0, *NEGUISTYLE.NormalFontBrush());
                    //tmpItem->setFont(0, *NEGUISTYLE.BoldFont());
                    parent->addChild(tmpItem);
                }

                // Init the item
                if(node)
                {
                    NENodeFactory* f = &(node->factory());
                    tmpItem->setText(1, f->nodeName());

                    if(node->factory().nodeInherits() != "Groups"){
                        NENodeIcon icon = *f->nodeIcon();

                        QIcon qicon;
                        for(int i = 0; i < icon.size(); i++)
                        {
                            qicon.addFile(icon[i].iconPath, icon[i].iconSize);
                        }

                        tmpItem->setIcon(1, qicon);

                        // colorize item due to NENodeState
                        if(NENode* node = qobject_cast<NENode*>(const_cast<QObject*>(child)))
                        {
                            switch(node->Behavior())
                            {
                            case NENode::NENodeState::Inactive:
                                tmpItem->setIcon(0, *m_IconInactive);
                                break;
                            case NENode::NENodeState::Cache:
                                tmpItem->setIcon(0, *m_IconCache);
                                break;
                            case NENode::NENodeState::Active:
                                tmpItem->setIcon(0, *m_IconActive);
                                break;
                            default:
                                break;
                            }
                        }
                    }
                    tmpItem->setToolTip(1, f->nodeName());
                }
                // Recursion
                if(child->children().size())
                {
                    traverseTree(child, tmpItem);
                }
            }
        }
        else
        {
            delete tmpItem;
        }
    }

}


FZoneInspectorItem* FZoneInspector::createOrganizerItem(QString name, FZoneInspectorItem *parentItem)
{
    FZoneInspectorItem* item = new FZoneInspectorItem();
    item->setForeground(0, *NEGUISTYLE.GrayFontBrush());
    item->setFont(0, *NEGUISTYLE.BoldFont());
    item->setText(0, name);
    item->setObject(0);
    parentItem->addChild(item);
    item->setExpanded(true);
    item->setFlags(item->flags()^Qt::ItemIsSelectable);
    return item;
}

void FZoneInspector::mousePressEvent(QMouseEvent *event)
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
}

void FZoneInspector::createContextMenu()
{
    const QList<QTreeWidgetItem *>& items = selectedItems();

    // DELETING OLD ACTIONS FROM CONTEXT MENU
    QList<QAction*> currentActions = actions();
    QListIterator<QAction*> i(currentActions);

    while(i.hasNext())
    {
        removeAction(i.next());
    }


    // Get current (selected) item
    FZoneInspectorItem* item = (FZoneInspectorItem*) currentItem();
    if(item != NULL)
    {
        QObject* object = item->Object();

        if(NEObject* tmpObject =  qobject_cast<NEObject*> (object))
        {
            //populate context menu with actions
            const QList<QObject*>& objectActions = *tmpObject->actions();
            Q_FOREACH(QObject* action, objectActions)
            {
                addAction(qobject_cast<QAction *>(action));
            }
            if(qobject_cast<NESpatialGroup*>(object))
                addAction(ungroupAction);
            if(NESpatialGroup* parent = qobject_cast<NESpatialGroup*>(tmpObject->parent()))
                addAction(unparentAction);
        }
    }

    if(items.size() > 1)
        addAction(groupAction);

    QList<NENode*> nodeList;

    Q_FOREACH(QTreeWidgetItem* item , selectedItems())
    {
        FZoneInspectorItem* sItem = static_cast<FZoneInspectorItem*>(item);
        nodeList.append(static_cast<NENode*>(sItem->Object()));
    }
    if(items.size() > 1)
    {
        foreach(NENode* node, nodeList)
            if(NESpatialGroup* group = qobject_cast<NESpatialGroup*>(node)){
                addAction(parentAction);
                break;
            }
    }
    //set context policy
    this->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void FZoneInspector::removeSelectedItem()
{
    QMap<NEObject*,NEObject*> selObjs = *NESCENE.selectedObjects();
    selObjs[ NESCENE.selectedObject() ] = NESCENE.selectedObject();

    QString script;

    Q_FOREACH( NEObject* o, selObjs )
    {
        if( o == NULL )	// Sometimes null objects leaks into this map, this is a hack to omit it
        {

            //selObjs.remove(o);
            continue;
        }

        if(NENode* node = qobject_cast<NENode*>(o))
        {
            QString name = node->objectName();
            QString type = node->factory().nodeName();//tmpObject->metaObject()->className();
            //assemble the python script
            script += NESCENE.objectName()+".deleteObject(\"" + type + "\", \""+name+"\", True)\n";
        }
    }

    if( selObjs.size() )
    {
        NEGUI.EvalPyGUI(script);
        NESCENE.cleanup();
        update();
        setCurrentItem( NULL );
        NEGUI.selectObject( NULL );
    }
    NEGUI.updateNodeConnectionsTable(); //Update node connection table of connection inspector
}


bool FZoneInspector::isSelectedObect(QObject* object)
{
    const FZoneInspectorItem* item = (FZoneInspectorItem*) currentItem();
    if(item)
        return (object == item->Object());
    else
        return false;
}

void FZoneInspector::selectItems()
{
    const QList<QTreeWidgetItem *>& items = selectedItems();

    if(items.size() == 1)
    {
        FZoneInspectorItem* sItem =  (FZoneInspectorItem*)items.first();
        QObject* object = sItem->Object();//((FZoneInspectorItem*)item)->Object();
        NEGUI.selectObject(object);
    }

    else
    {
        QList<NENode*> objects;
        Q_FOREACH(QTreeWidgetItem* item2, items)
        {
            FZoneInspectorItem* sItem =  (FZoneInspectorItem*)item2;
            NENode* casted = qobject_cast<NENode*>(sItem->Object());

            if(casted)
                objects.push_back(casted);
        }
        NEGUI.selectObjects(objects);
    }
}

void FZoneInspector::groupNodes()
{
    QList<NENode*> toGroup;

    Q_FOREACH(QTreeWidgetItem* item , selectedItems())
    {
        FZoneInspectorItem* sItem = static_cast<FZoneInspectorItem*>(item);
        toGroup.append(static_cast<NENode*>(sItem->Object()));
    }

    NESCENE.groupObjects(toGroup);
}

void FZoneInspector::ungroupNodes()
{
    const QList<QTreeWidgetItem *>& items = selectedItems();
    FZoneInspectorItem* sItem =  (FZoneInspectorItem*)items.first();
    QObject* object = sItem->Object();//((FZoneInspectorItem*)item)->Object();
    NESCENE.ungroupNode(static_cast<NESpatialGroup*>(object));
    FZoneInspectorWindow *inspectorWindow = qobject_cast<FZoneInspectorWindow *>(parentWidget());
    inspectorWindow->removeSelectedItem();

}

void FZoneInspector::parentNodes()
{
    QList<NENode*> nodeList;

    Q_FOREACH(QTreeWidgetItem* item , selectedItems())
    {
        FZoneInspectorItem* sItem = static_cast<FZoneInspectorItem*>(item);
        nodeList.append(static_cast<NENode*>(sItem->Object()));
    }
    foreach(NENode* node, nodeList)
        if(NESpatialGroup* group = qobject_cast<NESpatialGroup*>(node)){
            nodeList.removeAt(nodeList.indexOf(node));
            NESCENE.parentObjects(nodeList, group);
            return;
        }
}

void FZoneInspector::unparentNodes()
{
    FZoneInspectorItem* item = (FZoneInspectorItem*) currentItem();
    if(item != NULL)
    {
        QObject* object = item->Object();

        if(NENode* tmpObject =  qobject_cast<NENode*> (object))
        {
            if(NESpatialGroup* parent = qobject_cast<NESpatialGroup*>(tmpObject->parent()))
                NESCENE.unparentObjects(tmpObject, parent);
        }
    }
}

void FZoneInspector::nodeRenamed(QTreeWidgetItem *item, int column)
{
    FZoneInspectorItem* sItem = static_cast<FZoneInspectorItem*>(item);

    if(!sItem->Object() || sItem->Object()->objectName() == sItem->text(0))
        return;

    NENode* node = qobject_cast<NENode*>(sItem->Object());

    node->setObjectName(sItem->text(0));
}

void FZoneInspector::itemExpansion(QTreeWidgetItem *item)
{
    FZoneInspectorItem* sItem = static_cast<FZoneInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = true;
}

void FZoneInspector::itemCollapse(QTreeWidgetItem *item)
{
    FZoneInspectorItem* sItem = static_cast<FZoneInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = false;
}

void FZoneInspector::itemActivation(QTreeWidgetItem *item, int column)
{
    topLeft = this->indexFromItem(item);
    //    m_LastSelectedItem = selectionModel()->selection();
    //    m_LastSelectedItem.select(topLeft, topLeft);

}

void FZoneInspector::restoreExpandedState()
{
    if(topLevelItemCount())
    {
        QTreeWidgetItemIterator it(this);
        while(*it)
        {
            FZoneInspectorItem* item = static_cast<FZoneInspectorItem*>(*it);
            if(item->Object())
            {
                int expState = m_ExpNodes[item->Object()->objectName()];
                item->setExpanded(expState);
            }

            ++it;
        }
    }
}

void FZoneInspector::clearState()
{
#if 1
    SceneInpectorItems.clear();
    SceneItem=0;
#endif
}

/*!
 If we would like to make the parent of the deleted instance the current object this method should be called.
 However, than we should also be able to keep the selected/current item when updating the qtreewidget.
 This is not possible yet.
*/
void FZoneInspector::setParentCurrentItem()
{	
    FZoneInspectorItem* item = (FZoneInspectorItem*) currentItem();
    FZoneInspectorItem* parent = (FZoneInspectorItem*)item->parent();
    setCurrentItem(parent);
    NEGUI.selectObject(parent->Object());
}


void FZoneInspector::setNewCurrentItem(QObject* object)
{
    if(object)
    {
        QList<QTreeWidgetItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
        QListIterator<QTreeWidgetItem*> i(items);

        if(i.hasNext())
        {
            FZoneInspectorItem* item = (FZoneInspectorItem*) (i.next());
            setCurrentItem(item);
        }
    }
    else{
        update();
    }
}

void FZoneInspector::setCurrentItems(QList<NENode*> objList)
{
    foreach(NENode* object, objList)
    {
        if(object)
        {
            QList<QTreeWidgetItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
            QListIterator<QTreeWidgetItem*> i(items);

            if(i.hasNext())
            {
                FZoneInspectorItem* item = (FZoneInspectorItem*) (i.next());
                item->setSelected(true);
            }
        }
    }
}

void FZoneInspector::saveGUIStates(QXmlStreamWriter *writer)
{
    writer->writeStartElement("expandednodes");

    if(topLevelItemCount())
    {
        QTreeWidgetItemIterator it(this);
        while(*it)
        {
            FZoneInspectorItem* item = static_cast<FZoneInspectorItem*>(*it);
            if(item->Object())
            {
                writer->writeStartElement(item->Object()->objectName());
                writer->writeAttribute("expanded", QString("%1").arg(item->isExpanded()));
                writer->writeEndElement();
            }

            ++it;
        }
    }

    writer->writeEndElement();
}

void FZoneInspector::readGUIStates(QXmlStreamReader *reader)
{
    QString nodeName;
    m_ExpNodes.clear();

    while(reader->readNextStartElement())
    {
        // Read the expanded states of the properties
        if(reader->name() == "expandednodes")
        {
            // Read node names
            while(reader->readNext() != QXmlStreamReader::Invalid)
            {
                if(reader->tokenType() == QXmlStreamReader::EndElement && reader->name() == "expandednodes")
                    break;
                else if(reader->tokenType() != QXmlStreamReader::StartElement)
                    continue;

                nodeName = reader->name().toString();
                int nodeState = reader->attributes().value("expanded").toInt();
                m_ExpNodes[nodeName] = nodeState;
            }
            if(reader->hasError())
            {
                Log()<< "Error parsing scene file: " << reader->errorString(); EndLog(NE::kLogWarning);
            }
        }
        else
        {
            reader->skipCurrentElement();
            Log()<< "Unknown section " << reader->name() << " in the scene file."; EndLog(NE::kLogWarning);
        }
    }
}

