#include "Core/Base/NEBase.h"
#include "Core/NEObject.h"
#include "Core/NENode.h"
#include "Core/Spatial/NESpatialGroup.h"
#include "Core/Base/NESceneManager.h"
#include "Managers/FDYNGUIManager.h"
#include "FDYNFRANXInspector.h"
#include "FDYNFRANXInspectorWindow.h"
#include "FDYNFRANXInspectorItem.h"
#include "Windows/Main/FDYNMainWindow.h"

#include "Mesh/NEZone.h"
#include "Mesh/NEZoneItem.h"
#include "Mesh/NEScenario.h"
#include "Mesh/NESpatialEngine.h"



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



FDYNFRANXInspector::FDYNFRANXInspector(QWidget* parent) : FDYNTreeWidget(parent)
{
    clearState();

#if 0
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
#endif

    m_pRoot = 0;
    // Make necessary connections
    QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectItems()));

    // Create name validation text edit delagate
    validator = new QRegExpValidator(QRegExp("[A-Za-z_][A-Za-z_0-9]*"), this);
    editDelagate =  new EditDelegate(this, validator);

#if 0
    // Make second columns non-editable
    setItemDelegateForColumn(0, editDelagate);
    setItemDelegateForColumn(1, editDelagate);
#endif



    m_IconActive = new QIcon("alert_active.png");
    m_IconCache = new QIcon("alert_cache.png");
    m_IconInactive = new QIcon("alert.png");

#if 1
    connect(this, &FDYNTreeWidget::itemExpanded, this, &FDYNFRANXInspector::itemExpansion);
    connect(this, &FDYNTreeWidget::itemCollapsed, this, &FDYNFRANXInspector::itemCollapse);
    //connect(this, &QTreeWidget::itemClicked, this, &FDYNZoneInspector::itemActivation);
#endif




    //setSource(QUrl("qrc:/ScenarioManager.qml"));

    //context menu
    connect(treeview, SIGNAL(sigCreateContextMenu()), this, SLOT(createContextMenu()));

}




FDYNFRANXInspector::~FDYNFRANXInspector()
{
    delete m_IconActive;
    delete m_IconCache;
    delete m_IconInactive;

}



void FDYNFRANXInspector::setRoot( QObject* root)
{
    if(m_pRoot)
        clear();
    m_pRoot = root;
}



void FDYNFRANXInspector::update()
{
    if(!m_pRoot)
        return;

    QList<NEObject*> prevSelected =  NESCENE.selectedObjects()->values();
    this->clear();
    // Clear the pointers before rebuilding the contents
    clearState();
    Q_ASSERT_X(m_pRoot, " FDYNZoneInspector::update", "root pointer is null");

    m_searchMatchedObjects.clear();

    const QObject* child = m_pRoot;

    m_allScenarioItems.clear();
    NESpatialEngine *fEngine = qobject_cast<NESpatialEngine *>(m_pRoot->parent());
    if(fEngine){
        NEScenario *activeScn = qobject_cast<NEScenario *>(NESCENE.findChildNode(fEngine->activeScenarioName()));

        if(activeScn)
        {
            foreach (QString str, activeScn->itemNameChildren(activeScn->objectName()))
            {
                m_allScenarioItems.push_back(str);
                NENode *node = NESCENE.findChildNode(str);
                if(node)
                {
                    foreach (QObject *obj, node->children())
                    {
                        m_allScenarioItems.push_back(obj->objectName());
                    }
                }
            }
        }
    }

    updateChildren( child );
//    if(parentWidget()->isActiveWindow()){
//        NEScenario *scn = qobject_cast<NEScenario *>(NESCENE.selectedObject());
//        if(!scn)
//            setNewCurrentItem(NESCENE.selectedObject());
//    }

    QList<NENode*> selNodes;
    foreach (NEObject *obj, prevSelected) {
        selNodes.push_back(qobject_cast<NENode*>(obj));
    }
    setCurrentItems(selNodes);
}



void FDYNFRANXInspector::updateChildren(const QObject* child)
{

    FDYNFRANXInspectorItem* childItem = new FDYNFRANXInspectorItem();
#if 0
    childItem->setForeground(0, *NEGUISTYLE.NormalFontBrush());
    childItem->setFont(0, *NEGUISTYLE.BoldFont());
#endif
    childItem->setText(0, child->objectName());
    childItem->setText(1, "Engine");
    childItem->setIcon(0, "ring.png");
    childItem->setFlags(Qt::NoItemFlags);
    if(NEZone *zone = (NEZone *)qobject_cast<const NEZone *>(child))
        childItem->setToolTip(0, zone->nodeDescription());
    else
        childItem->setToolTip(0, child->objectName());
#if 0
    childItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
    childItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
#endif
    childItem->setObject(const_cast<QObject*>(child));
    this->addTopLevelItem(childItem);
    childItem->setExpanded(true);

    if(childItem->Object() == &NESCENE)
        SceneItem = childItem;

    if (NEZone* zone = qobject_cast<NEZone*>(m_pRoot))
        if(child->children().size())
            traverseTree(child, zone, childItem);

    FDYNTreeWidget::update();
    restoreExpandedState();
#if 0
    rsizeColumnToContents(0);
#endif
}

bool containsFRANXStringinZone(QObject *obj, const NEZone* zone, QString searchText)
{
    bool ret = false;

    foreach (QString str, zone->itemNameChildren(obj->objectName()))
    {
        if(NEZoneItem *zitem = qobject_cast<NEZoneItem *>(NESCENE.findChildNode(str)))
        {
            if(zitem->objectName().contains(searchText, Qt::CaseInsensitive))
            {
                ret = true;
                break;
            }
            else if( containsFRANXStringinZone(zitem, zone, searchText)){
                ret = true;
                break;
            }
        }
    }

    return ret;
}

void FDYNFRANXInspector::traverseTree(const QObject* obj, const NEZone* zone, FDYNFRANXInspectorItem* parent)
{
    const QObjectList& objList = obj->children();
    QObjectList cobjList;

    //
    // Search the scene based on name (if scenario or scenezoneitem) and add it to objList
    if (qobject_cast< NESpatialEngine*>(const_cast<QObject*>(obj)))
        cobjList = objList;
    else if( NEZone* zn   = qobject_cast< NEZone * >( const_cast<QObject*>(obj) ) )
    {
        for( auto& franxName : zn->otherItems() )
        {
            QObject* cnode = NESCENE.findChildNode( franxName );
            if( cnode && !cobjList.contains(cnode) )
                cobjList.push_back( cnode );
        }
    }
    else if( NEZoneItem* zitem = qobject_cast< NEZoneItem * >( const_cast<QObject*>(obj) ) )
    {
        for( auto& franxName : zitem->otherItems() )
        {
            NEObject* cnode = NESCENE.findChildNode( franxName );
            if( cnode && !cobjList.contains(cnode) )
                cobjList.push_back( cnode );
        }
    }

    Q_FOREACH(const QObject* child, cobjList)
    {
        if( qobject_cast< NEScenario * >( const_cast<QObject*>(child))  )
            continue;

        FDYNFRANXInspectorItem* childItem = new FDYNFRANXInspectorItem();
        childItem->setText(0, child->objectName());
        childItem->setText(1, child->metaObject()->className());
        if( NEZone* zone = qobject_cast<NEZone*>(const_cast<QObject*>(child)))
            childItem->setIcon(0, zone->Type());
        else
            childItem->setIcon(0, QString("ring.png"));
#if 0
        childItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
        childItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
#endif
        childItem->setObject(const_cast<QObject*>(child));
        childItem->setObjectName(child->objectName());
        childItem->setFlags(Qt::NoItemFlags);

        if(NEObject* neObject = qobject_cast<NEObject*>(const_cast<QObject*>(child)))
        {
            NENode* node = qobject_cast<NENode*>(neObject);
            if(neObject->isHidden())
            {
                node = qobject_cast<NENode*> (const_cast<QObject*>(child));
                if( node == NULL )
                    continue;

                FDYNFRANXInspectorItem *categoryItem = NULL;
                if(node->factory().nodeInherits() != "Groups")
                {
                    QString keyString = parent->text(0);
                    FDYNFRANXInspectorItem *pp = dynamic_cast<FDYNFRANXInspectorItem *>(parent->parent());
                    while (pp)
                    {
                        keyString += "," + pp->text(0);
                        pp = dynamic_cast<FDYNFRANXInspectorItem *>(pp->parent());
                    }

                    if( NEZoneItem* zoneItem = qobject_cast<NEZoneItem*>(const_cast<QObject*>(child)))
                    {
                        keyString += "," + zoneItem->otherCategoryType();
                        if(m_searchText.isEmpty() || containStringinChildren(childItem->Object(), m_searchText) ||
                                containsFRANXStringinZone(childItem->Object(), zone, m_searchText))
                        {
                            if( SceneInpectorItems.find(keyString) == SceneInpectorItems.end())
                            {
                                categoryItem = createOrganizerItem(zoneItem->otherCategoryType(), parent);
                                SceneInpectorItems.insert(keyString, categoryItem);
                            }

                            categoryItem = SceneInpectorItems.value(keyString);
                            categoryItem->setIcon(0, QString("ring.png"));

                            categoryItem->addChild(childItem);

                            if(!m_searchText.isEmpty() &&
                                    childItem->Object()->objectName().contains(m_searchText, Qt::CaseInsensitive))
                                m_searchMatchedObjects.push_back(childItem->Object());
                        }
                    }
                }
                else
                {
                    if(m_searchText.isEmpty() ||
                            containStringinChildren(childItem->Object(), m_searchText) ||
                            containsFRANXStringinZone(childItem->Object(), zone, m_searchText))
                    {
#if 0
                        childItem->setForeground(0, *NEGUISTYLE.NormalFontBrush());
#endif
                        parent->addChild(childItem);

                        if(!m_searchText.isEmpty() &&
                                childItem->Object()->objectName().contains(m_searchText, Qt::CaseInsensitive))
                            m_searchMatchedObjects.push_back(childItem->Object());
                    }
                }

                // Init the item
                if(node)
                {
                    NENodeFactory* f = &(node->factory());
                    childItem->setText(1, f->nodeName());

                    if(node->factory().nodeInherits() != "Groups")
                    {
                        NENodeIcon icon = *f->nodeIcon();

                        QIcon qicon;
                        for(int i = 0; i < icon.size(); i++)
                        {
                            qicon.addFile(icon[i].iconPath, icon[i].iconSize);
                        }

                        childItem->setIcon(1, qicon);

                        // colorize item due to NENodeState
                        if(NENode* node = qobject_cast<NENode*>(const_cast<QObject*>(child)))
                        {
                            if(node->children().size())
                            {
                                if( NEZoneItem* zoneItem = qobject_cast<NEZoneItem*>(const_cast<QObject*>(child)))
                                    childItem->setIcon(0, zoneItem->Type());

                                bool isWireframe = true;
                                foreach (QObject *obj, node->children())
                                {
                                    NESpatialMesh *spatial = qobject_cast<NESpatialMesh *>(obj);
                                    if(spatial)
                                    {
                                        if(spatial->SpatialMeshType() != NESpatialMesh::None)
                                        {
                                            isWireframe = false;
                                            break;
                                        }
                                    }
                                }
                                if(NESpatialMesh *snode = qobject_cast<NESpatialMesh *>(node))
                                {
                                    if(snode->SpatialMeshType() != NESpatialMesh::None)
                                        isWireframe = false;
                                }
                            }
                            else
                            {
                                if( NEZoneItem* zoneItem = qobject_cast<NEZoneItem*>(const_cast<QObject*>(child)))
                                    childItem->setIcon(0, zoneItem->Type());
                            }

                            if(m_allScenarioItems.indexOf(node->objectName()) != -1)
                                node->SetBehavior(NENode::Active);
                            else
                                node->SetBehavior(NENode::Inactive);

                            switch(node->Behavior())
                            {
                            case NENode::NENodeState::Inactive:
                                childItem->setIcon2(1, QString("alert.png"));
                                break;
                            case NENode::NENodeState::Cache:
                                childItem->setIcon2(1, QString("alert_cache.png"));
                                break;
                            case NENode::NENodeState::Active:
                                childItem->setIcon2(1, QString("alert_active.png"));
                                break;
                            default:
                                break;
                            }
                        }
                    }

                    if(NEZone *zone = qobject_cast<NEZone *>(node))
                        childItem->setToolTip(0, zone->nodeDescription());
                    else if(NEScenario *scn = qobject_cast<NEScenario *>(node))
                        childItem->setToolTip(0, scn->nodeDescription());
                    else if(NEZoneItem *zitem = qobject_cast<NEZoneItem *>(node))
                        childItem->setToolTip(0, zitem->nodeDescription());
                }
                // Recursion
                traverseTree(child, zone, childItem);
            }
        }
        else
        {
            delete childItem;
        }
    }
}

FDYNFRANXInspectorItem* FDYNFRANXInspector::createOrganizerItem(QString name, FDYNFRANXInspectorItem *objItem)
{
    FDYNFRANXInspectorItem* item = new FDYNFRANXInspectorItem();
#if 0
    item->setForeground(0, *NEGUISTYLE.GrayFontBrush());
    item->setFont(0, *NEGUISTYLE.BoldFont());
#endif
    item->setText(0, name);
    item->setObject(nullptr);
    item->setExpanded(true);
    item->setFlags(item->flags()^Qt::ItemIsSelectable);
    item->setToolTip(0, name);
    item->setObjectName(name+"_"+objItem->objectName());

    objItem->addChild(item);

    return item;
}

#if 0
void FDYNZoneInspector::mousePressEvent(QMouseEvent *event)
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

#endif

void FDYNFRANXInspector::createContextMenu()
{
    return;
    const QList<FDYNTreeItem *>& items = selectedItems();

    // DELETING OLD ACTIONS FROM CONTEXT MENU
    QList<QAction*> currentActions = actions();
    QListIterator<QAction*> i(currentActions);

    while(i.hasNext())
    {
        removeAction(i.next());
    }
    //addAction(loadDatabaseAction);
    //addAction(importFRANXAction);

    // Get current (selected) item
    FDYNFRANXInspectorItem* item = (FDYNFRANXInspectorItem*) currentItem();
    if(item != NULL)
    {
        QObject* object = item->Object();

        if(NEObject* tmpObject =  qobject_cast<NEObject*> (object))
        {
            //populate context menu with actions
            const QList<QObject*>& objectActions = *tmpObject->actions();
            Q_FOREACH(QObject* action, objectActions)
            {
                QAction *aaction = qobject_cast<QAction *>(action);
                if(aaction->text() == QString("Delete"))
                {
                    //aaction->setText( "Remove" );
                    //removeAction( aaction );
                }
                else if( aaction->text() == QString("Edit Caching"))
                {
                    //removeAction( aaction );
                }
                else if( aaction->text() == QString("Clean Data"))
                {

                }
                else
                {
                    addAction(aaction);
                }
            }
            if(qobject_cast<NESpatialGroup*>(object))
            {
                //if(!qobject_cast<NESpatialEngine*>(object) && !qobject_cast<NEZone*>(object) && !qobject_cast<NEScenario*>(object) && !qobject_cast<NEZoneItem*>(object))
                    //addAction(ungroupAction);
            }
            if(NESpatialGroup* parent = qobject_cast<NESpatialGroup*>(tmpObject->parent()))
            {
                //if(!qobject_cast<NESpatialEngine*>(object) && !qobject_cast<NEZone*>(object) && !qobject_cast<NEScenario*>(object) && !qobject_cast<NEZoneItem*>(object))
                  //  addAction(unparentAction);
            }

        }
    }



//    QList<NENode*> nodeList;

//    Q_FOREACH(FDYNTreeItem* item , selectedItems())
//    {
//        FDYNZoneInspectorItem* sItem = static_cast<FDYNZoneInspectorItem*>(item);
//        nodeList.append(static_cast<NENode*>(sItem->Object()));
//    }

    //set context policy
    this->setContextMenuPolicy(Qt::ActionsContextMenu);
}


bool FDYNFRANXInspector::isSelectedObect(QObject* object)
{
    const FDYNFRANXInspectorItem* item = (FDYNFRANXInspectorItem*) currentItem();
    if(item)
        return (object == item->Object());
    else
        return false;
}

void FDYNFRANXInspector::selectItems()
{
    const QList<FDYNTreeItem *>& items = selectedItems();

    if(items.size() == 1)
    {
        FDYNFRANXInspectorItem* sItem =  qobject_cast<FDYNFRANXInspectorItem*>(items.first());
        if(sItem)
        {
            NENode* casted = qobject_cast<NENode*>(sItem->Object());
            FDYNGUI.selectObject(casted);
        }
    }
    else if(items.size() > 0)
    {
        QList<NENode*> objects;
        Q_FOREACH(FDYNTreeItem* item2, items)
        {
            FDYNFRANXInspectorItem* sItem =  qobject_cast<FDYNFRANXInspectorItem*>(item2);
            if(sItem)
            {
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

void FDYNFRANXInspector::renameSelected()
{
    QVariant returnValue;
    QMetaObject::invokeMethod( currentTextInput, "setActiveFocusFromCpp", Q_RETURN_ARG(QVariant, returnValue));
}

void FDYNFRANXInspector::slotDropDownItemChanged(QString newZone)
{
    emit FDYNGUI.setActiveZone( newZone );
    return;
}

void FDYNFRANXInspector::slotOtherItemChanged(QString newZone)
{
    emit FDYNGUI.setActiveZone( newZone );
    return;
}




void FDYNFRANXInspector::itemExpansion(FDYNTreeItem *item)
{
    FDYNFRANXInspectorItem* sItem = static_cast<FDYNFRANXInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = true;
    else
        m_ExpNodes[sItem->text(0)] = true;
}

void FDYNFRANXInspector::itemCollapse(FDYNTreeItem *item)
{
    FDYNFRANXInspectorItem* sItem = static_cast<FDYNFRANXInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = false;
    else
        m_ExpNodes[sItem->text(0)] = false;
}

void FDYNFRANXInspector::itemActivation(FDYNTreeItem *item, int column)
{
    //topLeft = this->indexFromItem(item);
    //    m_LastSelectedItem = selectionModel()->selection();
    //    m_LastSelectedItem.select(topLeft, topLeft);

}

void FDYNFRANXInspector::restoreExpandedState()
{

    if(topLevelItemCount())
    {
        Q_FOREACH(FDYNTreeItem* it , items())
        {
            FDYNFRANXInspectorItem* item = qobject_cast<FDYNFRANXInspectorItem*>(it);
            if(item->Object())
            {
                bool expState = true;
                if(m_ExpNodes.find(item->Object()->objectName()) != m_ExpNodes.end()){
                    expState = m_ExpNodes[item->Object()->objectName()];
                }
                if(expState)
                    this->expandItem(item);
                else
                    this->collapseItem(item);
            }
            else{
                bool expState = false;
                if(m_ExpNodes.find(item->text(0)) != m_ExpNodes.end()){
                    expState = m_ExpNodes[item->text(0)];
                }
                if(expState)
                    this->expandItem(item);
                else
                    this->collapseItem(item);
            }
            ++it;
        }
    }

#if 0
    if(topLevelItemCount())
    {
        QTreeWidgetItemIterator it(this);
        while(*it)
        {
            FDYNZoneInspectorItem* item = static_cast<FDYNZoneInspectorItem*>(*it);
            if(item->Object())
            {
                int expState = m_ExpNodes[item->Object()->objectName()];
                item->setExpanded(expState);
            }

            ++it;
        }
    }
#endif
}

void FDYNFRANXInspector::clearState()
{
    SceneInpectorItems.clear();
    SceneItem=0;
}

/*!
 If we would like to make the parent of the deleted instance the current object this method should be called.
 However, than we should also be able to keep the selected/current item when updating the qtreewidget.
 This is not possible yet.
*/
void FDYNFRANXInspector::setParentCurrentItem()
{	
    FDYNFRANXInspectorItem* item = (FDYNFRANXInspectorItem*) currentItem();
    FDYNFRANXInspectorItem* parent = (FDYNFRANXInspectorItem*)item->parent();
    setCurrentItem(parent);
    FDYNGUI.selectObject(parent->Object());
}


void FDYNFRANXInspector::setNewCurrentItem(QObject* object)
{
    if(object)
    {
        QList<FDYNTreeItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
        QListIterator<FDYNTreeItem*> i(items);

        if(i.hasNext())
        {
            FDYNFRANXInspectorItem* item = (FDYNFRANXInspectorItem*) (i.next());
            setCurrentItem(item);
        }
    }
    else{
        //update();
        setCurrentItem(NULL);
    }
}



void FDYNFRANXInspector::setCurrentItems(QList<NENode*> objList)
{
    foreach(NENode* object, objList)
    {
        if(object)
        {
            QList<FDYNTreeItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
            QListIterator<FDYNTreeItem*> i(items);

            if(i.hasNext())
            {
                FDYNFRANXInspectorItem* item = (FDYNFRANXInspectorItem*) (i.next());
                item->setSelected(true);
            }
        }
    }
}


void FDYNFRANXInspector::saveGUIStates(QXmlStreamWriter *writer)
{

    writer->writeStartElement("expandednodes");

    if(topLevelItemCount())
    {
        Q_FOREACH(FDYNTreeItem* it , items())
        {
            FDYNFRANXInspectorItem* item = qobject_cast<FDYNFRANXInspectorItem*>(it);
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



void FDYNFRANXInspector::readGUIStates(QXmlStreamReader *reader)
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


void FDYNFRANXInspector::positionToSelected()
{
    expandItem( m_currentItem->parent() );
    QVariant returnValue;
    QMetaObject::invokeMethod(treeview, "itemAdded", Q_RETURN_ARG(QVariant, returnValue));
}
