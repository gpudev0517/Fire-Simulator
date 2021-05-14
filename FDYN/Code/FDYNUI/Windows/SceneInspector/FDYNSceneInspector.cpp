#include "Core/Base/NEBase.h"
#include "Core/NEObject.h"
#include "Core/NENode.h"
#include "Core/Spatial/NESpatialGroup.h"
#include "Core/Base/NESceneManager.h"
#include "Managers/FDYNGUIManager.h"
#include "Windows/Main/FDYNMainWindow.h"

#include "FDYNSceneInspector.h"
#include "FDYNSceneInspectorWindow.h"

#include "Geometry/Mesh/NEZone.h"
#include "Geometry/Mesh/NEZoneItem.h"
#include "Geometry/Mesh/NEScenario.h"
#include "Mesh/NESpatialEngine.h"

#include "Utilities/FDYNTreeModel.h"
#include "Utilities/FDYNTreeItem.h"

#include "FDYNSceneInspectorItem.h"

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



FDYNSceneInspector::FDYNSceneInspector(QWidget* parent) : FDYNTreeWidget(parent)
{
    clearState();
    //this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    // Make necessary connections
    QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectItems()));

    // Create name validation text edit delagate
    validator = new QRegExpValidator(QRegExp("[A-Za-z_][A-Za-z_0-9]*"), this);
    editDelagate =  new EditDelegate(this, validator);

    // Make second columns non-editable
    //setItemDelegateForColumn( 0, editDelagate );
    //setItemDelegateForColumn( 1, editDelagate );

    QObject::connect(this, SIGNAL(itemChanged(FDYNTreeItem*, int)), this, SLOT(nodeRenamed(FDYNTreeItem*, int)));

    m_IconActive = new QIcon("alert_active");
    m_IconCache = new QIcon("alert_cache.png");
    m_IconInactive = new QIcon("alert.png");

    /// TreeWidget Integration - start
    /// - need to configure the following signals/slots to the correct
    /// - implementation
    connect(this, &FDYNTreeWidget::itemExpanded, this, &FDYNSceneInspector::itemExpansion);
    connect(this, &FDYNTreeWidget::itemCollapsed, this, &FDYNSceneInspector::itemCollapse);
    //    connect(m_FDYNTreeModel, &FDYNTreeModel::itemClicked, this, &FDYNSceneInspector::itemActivation);
    /// TreeWidget Integration - end
    ///

    connect(treeview, SIGNAL(sigCreateContextMenu()), this, SLOT(createContextMenu()));


    groupAction = new QAction("Group", 0);
    ungroupAction = new QAction("Ungroup", 0);
    connect(groupAction, &QAction::triggered, this, &FDYNSceneInspector::groupNodes);
    connect(ungroupAction, &QAction::triggered, this, &FDYNSceneInspector::ungroupNodes);

    subItemAction = new QAction("Add Item", 0);
    connect(subItemAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addItemToSelected);

    connect(groupAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::setSelectedGroup);
    connect(ungroupAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::setSelectedUngroup);

    parentAction = new QAction("Parent", 0);
    unparentAction = new QAction("Unparent", 0);
    connect(parentAction, &QAction::triggered, this, &FDYNSceneInspector::parentNodes);
    connect(unparentAction, &QAction::triggered, this, &FDYNSceneInspector::unparentNodes);
}

FDYNSceneInspector::~FDYNSceneInspector()
{
    //    while(topLevelItemCount())
    //        delete takeTopLevelItem(0);

    delete m_IconActive;
    delete m_IconCache;
    delete m_IconInactive;
    delete groupAction;
    delete ungroupAction;
    delete parentAction;
    delete unparentAction;
}

void FDYNSceneInspector::setRoot( QObject* root)
{
    if(m_pRoot)
        clear();
    m_pRoot = root;
}

void FDYNSceneInspector::createCategoryTypes( QObject* rootscn, QStringList &ctypes )
{
    static QStringList cctypes = {
        "Forces",
        "Cameras",
        "Lights",
        "Materials",
        "Groups",
        "Data",
        "Visuals"
    };

    ctypes = cctypes;
    return;

#if 0
    for( QObject* item: rootscn->children() )
    {
        NENode* nodeitem = qobject_cast< NENode *>( item );
        QString ctype;

        if( nodeitem )
        {
            NENodeFactory& factory = nodeitem->factory();
            if( ctypes.contains( factory.nodeInherits() ) == false )
                ctype = factory.nodeInherits();

            if( item->children().size() > 0 )
                ctype += "/";
        }

        if( !ctype.isEmpty() )
            ctypes.append( ctype );
        createCategoryTypes( item, ctypes );
    }
#endif
}


FDYNSceneInspectorItem*  FDYNSceneInspector::createRootOrganizerItems( QObject *rootscn, FDYNSceneInspectorItem* rootItem )
{

    //QStringList items = FZoneItem::categoryTypes();
    QStringList ctypes;
    createCategoryTypes( rootscn, ctypes );


    //qDebug() << "Ctypes " << ctypes;

    for( auto item: ctypes )
    {
        //createOrganizerItem( item, childItem );
        createOrganizerItem( item, rootItem );

    }
    return rootItem;
}

#if 0
void FDYNSceneInspector::update()
{
    if(!m_pRoot)
        return;

    QList<NEObject*> prevSelected =  NESCENE.selectedObjects()->values();

    this->clear();
    // Clear the pointers before rebuilding the contents
    clearState();
    Q_ASSERT_X(m_pRoot, " FDYNSceneInspector::update", "root pointer is null");

    m_searchMatchedObjects.clear();

    const QObject* child = m_pRoot;
    FDYNSceneInspectorItem* tmpItem = new FDYNSceneInspectorItem();
#if 0
    tmpItem->setForeground(0, *NEGUISTYLE.NormalFontBrush());
    tmpItem->setFont(0, *NEGUISTYLE.BoldFont());
#endif

    tmpItem->setText(0, child->objectName());
    tmpItem->setText(1, "Scene Manager");
    //qDebug(child->objectName().toLatin1());

#if 0
    tmpItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
    tmpItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
#endif
    tmpItem->setObject(const_cast<QObject*>(child));
    addTopLevelItem( tmpItem );
    tmpItem->setExpanded(true);

    if(tmpItem->Object() == &NESCENE)
        SceneItem = tmpItem;

    if(child->children().size())
        traverseTree(child, tmpItem);

#if 0
    resizeColumnToContents(0);
#endif
    // QModelIndex idx = this->currentIndex().child(m_LastSelectedItemIndex, 0);

    //QItemSelection *selection = new QItemSelection(topLeft, topLeft);
    //QModelIndex mIndex =  QAbstractItemModel::createIndex((m_LastSelectedItemIndex, 0,
    //   setCurrentIndex(idx);
    //selectionModel()->select(*selection, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    FDYNTreeWidget::update();
    restoreExpandedState();

    //setNewCurrentItem(NESCENE.selectedObject());

    QList<NENode*> selNodes;
    foreach (NEObject *obj, prevSelected) {
        selNodes.push_back(qobject_cast<NENode*>(obj));
    }
    setCurrentItems(selNodes);
}
#else
void FDYNSceneInspector::update()
{
    if( !m_pRoot )
        return;

    QList<NEObject*> prevSelected = NESCENE.selectedObjects()->values();

    clear();
    // Clear the pointers before rebuilding the contents
    clearState();
    Q_ASSERT_X(m_pRoot, " FDYNZoneInspector::update", "root pointer is null");

    m_searchMatchedObjects.clear();

    const QObject* child = m_pRoot;

    //QElapsedTimer timer;
    //timer.start();

    updateChildren( child );
    restoreExpandedState();

    //std::cout << "The update operation took " << timer.elapsed() << " milliseconds" << std::endl;

    QList<NENode*> selNodes;
    foreach (NEObject *obj, prevSelected)
        selNodes.push_back(qobject_cast<NENode*>(obj));
    setCurrentItems(selNodes);
}

void FDYNSceneInspector::updateChildren( const QObject* rootobj )
{
    FDYNSceneInspectorItem* rootitem = new FDYNSceneInspectorItem();
#if 0
    childItem->setForeground(0, *NEGUISTYLE.NormalFontBrush());
    childItem->setFont(0, *NEGUISTYLE.BoldFont());
#endif
    rootitem->setText(0, rootobj->objectName());
    rootitem->setText(1, "Scene Manager");

    rootitem->setToolTip( 0, rootobj->objectName() );
#if 0
    childItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
    childItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
#endif
    rootitem->setObject( const_cast<QObject*>(rootobj) );
    addTopLevelItem( rootitem );
    rootitem->setExpanded( true );

    if( rootitem->Object() == &NESCENE )
        SceneItem = rootitem;


    //
    // Then Create Categories under the root zone
    //
    FDYNSceneInspectorItem* childitem = createRootOrganizerItems( &NESCENE, rootitem );

    //
    // Then update the children - Dont create the zone element in the tree since createRootOrganizerItem
    // has already created that. So pass the activeZone and the zone inspector item to be the root and
    // create subtrees under that using traverseTree
    //

    if ( rootobj->children().size() > 0 )
        traverseTree( rootobj, childitem );

    FDYNTreeWidget::update();
    restoreExpandedState();
#if 0
    rsizeColumnToContents(0);
#endif
}
#endif


void FDYNSceneInspector::traverseTree(const QObject* obj, FDYNSceneInspectorItem* parent)
{
    const QObjectList& objList = obj->children();
    Q_FOREACH(const QObject* child, objList)
    {
        FDYNSceneInspectorItem* tmpItem = new FDYNSceneInspectorItem();
        tmpItem->setText(0, child->objectName());
        tmpItem->setText(1, child->metaObject()->className());
        //qDebug() << child->objectName() << child->metaObject()->className();
        /*
        tmpItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
        tmpItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
        */
        tmpItem->setObject(const_cast<QObject*>(child));
        tmpItem->setObjectName( child->objectName() );
        tmpItem->setFlags(tmpItem->flags()^Qt::ItemIsEditable);
        //tmpItem->setExpanded(true);

        if(NEObject* neObject = qobject_cast<NEObject*>(const_cast<QObject*>(child)))
        {
            NENode* node = qobject_cast<NENode*>(neObject);
            if(neObject->isHidden())
            {
                node = qobject_cast<NENode*> (const_cast<QObject*>(child));
                if( node == nullptr )
                    continue;

                // Temporary Fix to avoid crash
                //if (&(node->factory()) == nullptr)
                //{
                //    qDebug() << "Node with non-initialized factory when traversing scene-inspector tree: " << node->objectName();
                //    continue;
                //}

                FDYNSceneInspectorItem *categoryItem = NULL;

                QString keyString = node->factory().nodeInherits();
                FDYNSceneInspectorItem *pp = dynamic_cast<FDYNSceneInspectorItem *>( parent );
                while( pp )
                {
                    keyString += "," + pp->text(0);
                    pp = dynamic_cast<FDYNSceneInspectorItem *>(pp->parent());
                }

                //std::cout << "Key String Find: " << keyString.toStdString() << std::endl;

                if(m_searchText.isEmpty() || containStringinChildren(tmpItem->Object(), m_searchText) )
                {

                    //qDebug() << "Keys " << SceneInpectorItems.keys();
                    QMap<QString, FDYNSceneInspectorItem*>::iterator itr = SceneInpectorItems.find(keyString);
                    if(itr == SceneInpectorItems.end())
                    {
                        //qDebug() << "Key String Insert2: " << keyString;

                        //
                        // Since categories are created already this should never be reached
                        //
                        categoryItem = createOrganizerItem(node->factory().nodeInherits(), parent);
                        SceneInpectorItems.insert(keyString, categoryItem);
                    }
                    else
                    {
                        categoryItem = itr.value();
                    }

                    categoryItem->addChild( tmpItem );

                    if(!m_searchText.isEmpty() &&
                            tmpItem->Object()->objectName().contains(m_searchText, Qt::CaseInsensitive))
                        m_searchMatchedObjects.push_back(tmpItem->Object());
                }


                // Init the item
                if(node)
                {
                    NENodeFactory* f = &(node->factory());
                    tmpItem->setText(1, f->nodeName());

                    //if(node->factory().nodeInherits() != "Groups"){
                    NENodeIcon icon = *f->nodeIcon();

                    //QIcon qicon;
                    QString iconUrl;

                    //                        for(int i = 0; i < icon.size(); i++)
                    //                        {
                    //                            qicon.addFile(icon[i].iconPath, icon[i].iconSize);
                    //                        }
                    if(icon.size() > 0)
                    {
                        iconUrl =icon[icon.size() -1].iconPath;
                        //qDebug() << iconUrl;
                        //qDebug() << iconUrl.lastIndexOf("/");
                        iconUrl = iconUrl.right(iconUrl.length() - iconUrl.lastIndexOf("/") - 1);
                        //qDebug() << iconUrl;
                    }


                    //tmpItem->setIcon(1, iconUrl);

                    // colorize item due to NENodeState
                    if(NENode* node = qobject_cast<NENode*>(const_cast<QObject*>(child)))
                    {
                        switch(node->Behavior())
                        {
                        case NENode::NENodeState::Inactive:
                            //tmpItem->setIcon(0, *m_IconInactive);
                            tmpItem->setIcon(0, QString("alert.png"));
                            break;
                        case NENode::NENodeState::Cache:
                            //tmpItem->setIcon(0, *m_IconCache);
                            tmpItem->setIcon(0, QString("alert_cache.png"));
                            break;
                        case NENode::NENodeState::Active:
                            //tmpItem->setIcon(0, *m_IconActive);
                            tmpItem->setIcon(0, QString("alert_active.png"));
                            break;
                        default:
                            break;
                        }
                    }
                    //}
                    tmpItem->setToolTip(0, f->nodeName());
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

#if 1
FDYNSceneInspectorItem* FDYNSceneInspector::createOrganizerItem( QString categoryName, FDYNSceneInspectorItem *objItem )
{
    QRegExp rx("\\/");
    QStringList query = categoryName.split(rx);

    FDYNSceneInspectorItem* objOrgItem = objItem;
    FDYNSceneInspectorItem* parentOrgItem = objOrgItem;

    //std::cout << "Name: " << categoryName.toStdString() << std::endl;
    //
    // If there are sub category items separated by / then use the parent as the category name
    //
    QStringList parentquery = query;
    if( parentquery.size() > 1 )
        parentquery.removeLast();

    QString keyParentString = parentquery.last();
    FDYNSceneInspectorItem *pp = dynamic_cast< FDYNSceneInspectorItem *>( objItem );

    while( pp )
    {
        keyParentString += "," + pp->text(0);
        pp = dynamic_cast< FDYNSceneInspectorItem * >( pp->parent() );
    }

    //std::cout << "Finding Parent Item " << keyParentString.toStdString() << std::endl;
    if( SceneInpectorItems.find(keyParentString) != SceneInpectorItems.end() )
    {
        parentOrgItem = SceneInpectorItems.value( keyParentString );
        //std::cout << " -- found " << std::endl;
    }

    QStringList keyStrings ;
    QString ks = query[0];
    FDYNSceneInspectorItem* item = nullptr;
    for( auto i = 0 ; i < query.size(); ++i )
    {
        keyStrings.append( ks );
        if( i < query.size()-1 )
            ks += "/" + query[i+1];
    }
    for( auto i = 0 ; i < query.size(); ++i )
    {

        //std::cout << "---- Processing for " << query[i].toStdString() << std::endl;

        QString keyString = keyStrings[i];
        FDYNSceneInspectorItem *pp = dynamic_cast< FDYNSceneInspectorItem *>( objItem );

        while( pp )
        {
            keyString += "," + pp->text(0);
            pp = dynamic_cast< FDYNSceneInspectorItem * >( pp->parent() );
        }

        //std::cout << "  Key Parent Str " << keyParentString.toStdString() << std::endl;
        //std::cout << "  Key Str " << keyString.toStdString() << std::endl;

        if( SceneInpectorItems.find(keyString) != SceneInpectorItems.end() )
        {
            continue;
        }

        item = new FDYNSceneInspectorItem();
        SceneInpectorItems.insert( keyString, item );

        //std::cout << "  Inserting : " << keyString.toStdString() << std::endl;
        //std::cout << "  Adding Sub Item: " << query[i].toStdString() << " To " << parentOrgItem->text(0).toStdString() << std::endl;
#if 0
        item->setForeground(0, *NEGUISTYLE.GrayFontBrush());
        item->setFont(0, *NEGUISTYLE.BoldFont());
#endif
        item->setText( 0, query[i] );
        item->setObject( nullptr );
        item->setExpanded( true );
        item->setFlags( item->flags()^Qt::ItemIsSelectable );
        item->setToolTip( 0, query[i] );
        item->setIcon( 0, QString("ring.png") );
        item->setObjectName( categoryName+"_"+objItem->objectName() );

        parentOrgItem->addChild( item );
        parentOrgItem = item;
    }

    return item;
}
#else

FDYNSceneInspectorItem* FDYNSceneInspector::createOrganizerItem(QString name, FDYNSceneInspectorItem *parentItem)
{
    FDYNSceneInspectorItem* item = new FDYNSceneInspectorItem();
#if 0
    item->setForeground(0, *NEGUISTYLE.GrayFontBrush());
    item->setFont(0, *NEGUISTYLE.BoldFont());
#endif
    //qDebug() << name;
    item->setText(0, name);
    item->setObject(0);
    parentItem->addChild(item);
    item->setExpanded(true);
    item->setFlags(item->flags()^Qt::ItemIsSelectable);
    return item;
}

#endif

//void FDYNSceneInspector::mousePressEvent(QMouseEvent *event)
//{
//    /// TreeWidget Integration - start
//    /// - need to configure the following signals/slots to the correct
//    /// - implementation
//    ///
//    ///QTreeWidget::mousePressEvent(event);
//    /// TreeWidget Integration - end
//    //right clicks should open a pop-up


//    FDYNTreeWidget::mousePressEvent(event);

//    switch(event->button())
//    {
//    case Qt::RightButton:
//        createContextMenu();
//        break;
//    default:break;
//        //default:
//        //	QTreeWidget::mousePressEvent(event);
//    }
//}

void FDYNSceneInspector::createContextMenu()
{
    const QList<FDYNTreeItem *>& items = selectedItems();

    // DELETING OLD ACTIONS FROM CONTEXT MENU
    QList<QAction*> currentActions = actions();
    QListIterator<QAction*> i(currentActions);

    while(i.hasNext())
    {
        removeAction(i.next());
    }


    // Get current (selected) item
    FDYNSceneInspectorItem* item = qobject_cast<FDYNSceneInspectorItem*>( currentItem() );
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

                if( aaction->text() == QString("Delete"))
                    addAction(qobject_cast<QAction *>(action));
            }
//            if(qobject_cast<NESpatialGroup*>(object))
//            {
//                if(!qobject_cast<NESpatialEngine*>(object) &&
//                        !qobject_cast<NEZone*>(object) &&
//                        !qobject_cast<NEScenario*>(object) && !qobject_cast<NEZoneItem*>(object))
//                    addAction(ungroupAction);
//            }
//            if(NESpatialGroup* parent = qobject_cast<NESpatialGroup*>(tmpObject->parent()))
//            {
//                if(!qobject_cast<NESpatialEngine*>(object) && !qobject_cast<NEZone*>(object) && !qobject_cast<NEScenario*>(object) && !qobject_cast<NEZoneItem*>(object))
//                    addAction(unparentAction);
//            }
//            if( qobject_cast<NEScenario*> (tmpObject) || qobject_cast<NEZone*> (tmpObject) ||  qobject_cast<NEZoneItem*> (tmpObject) )
//            {
//                addAction(subItemAction);
//            }
        }
    }

    if(items.size() > 1)
        addAction(groupAction);

    QList<NENode*> nodeList;

    Q_FOREACH(FDYNTreeItem* item , selectedItems())
    {
        FDYNSceneInspectorItem* sItem = qobject_cast<FDYNSceneInspectorItem*>(item);
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

void FDYNSceneInspector::removeSelectedItem()
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
            NESpatialMesh *smesh = qobject_cast<NESpatialMesh *>(node);
            if(smesh && smesh->SpatialMeshType() != NESpatialMesh::None)
            {
                smesh->SetSpatialMeshType(NESpatialMesh::None);
                Q_FOREACH(int n, smesh->getMeasureObjects())
                    script += NESCENE.objectName()+".deleteObject(\"Measurement Item\", \""+QString::number(n)+"\", True)\n";
            }
            else
            {
                QString name = node->objectName();
                QString type = node->factory().nodeName();//tmpObject->metaObject()->className();
                //assemble the python script
                script += NESCENE.objectName()+".deleteObject(\"" + type + "\", \""+name+"\", True)\n";
            }
        }
    }

    if( selObjs.size() )
    {
        FDYNGUI.EvalPyGUI(script);
    }
    NESCENE.cleanup();
    update();
    FDYNGUI.selectObject( NULL );

    FDYNGUI.updateNodeConnectionsTable(); //Update node connection table of connection inspector

}


bool FDYNSceneInspector::isSelectedObect(QObject* object)
{
    FDYNSceneInspectorItem* item = qobject_cast<FDYNSceneInspectorItem*> (currentItem());
    if(item)
        return (object == item->Object());
    else
        return false;
}

void FDYNSceneInspector::selectItems()
{
    const QList<FDYNTreeItem *>& items = selectedItems();

    if(items.size() == 1)
    {
        FDYNSceneInspectorItem* sItem =  qobject_cast<FDYNSceneInspectorItem*>(items.first());
        NENode* casted = qobject_cast<NENode*>(sItem->Object());
        FDYNGUI.selectObject(casted);
    }

    else if(items.size() > 0)
    {
        QList<NENode*> objects;
        Q_FOREACH(FDYNTreeItem* item2, items)
        {
            FDYNSceneInspectorItem* sItem =  qobject_cast<FDYNSceneInspectorItem*>(item2);
            NENode* casted = qobject_cast<NENode*>(sItem->Object());

            if(casted)
                objects.push_back(casted);
        }
        FDYNGUI.selectObjects(objects);
    }
    else{
        FDYNGUI.selectObject(NULL);
    }
}

void FDYNSceneInspector::renameSelected()
{
    QVariant returnValue;
    QMetaObject::invokeMethod( currentTextInput, "setActiveFocusFromCpp", Q_RETURN_ARG(QVariant, returnValue));
}

void FDYNSceneInspector::groupNodes()
{
    QList<NENode*> toGroup;

    Q_FOREACH(FDYNTreeItem* item , selectedItems())
    {
        FDYNSceneInspectorItem* sItem = qobject_cast<FDYNSceneInspectorItem*>(item);
        toGroup.append(static_cast<NENode*>(sItem->Object()));
    }

    NESCENE.groupObjects(toGroup);
}

void FDYNSceneInspector::ungroupNodes()
{
    const QList<FDYNTreeItem *>& items = selectedItems();
    FDYNSceneInspectorItem* sItem =  qobject_cast<FDYNSceneInspectorItem*>(items.first());
    QObject* object = sItem->Object();//((FDYNSceneInspectorItem*)item)->Object();
    NESCENE.ungroupNode(static_cast<NESpatialGroup*>(object));
    FDYNGUI.mainWindow()->removeSelectedItem();
    FDYNSceneInspectorWindow *inspectorWindow = qobject_cast<FDYNSceneInspectorWindow *>(parentWidget());
    inspectorWindow->removeSelectedItem();

}

void FDYNSceneInspector::parentNodes()
{
    QList<NENode*> nodeList;

    Q_FOREACH(FDYNTreeItem* item , selectedItems())
    {
        FDYNSceneInspectorItem* sItem = static_cast<FDYNSceneInspectorItem*>(item);
        nodeList.append(static_cast<NENode*>(sItem->Object()));
    }
    foreach(NENode* node, nodeList)
        if(NESpatialGroup* group = qobject_cast<NESpatialGroup*>(node))
        {
            nodeList.removeAt(nodeList.indexOf(node));
            NESCENE.parentObjects(nodeList, group);
            return;
        }
}

void FDYNSceneInspector::unparentNodes()
{
    FDYNSceneInspectorItem* item = qobject_cast< FDYNSceneInspectorItem*>( currentItem() );
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

//void FDYNSceneInspector::clear()
//{

//}

void FDYNSceneInspector::expandAll()
{

}

void FDYNSceneInspector::nodeRenamed(FDYNTreeItem *item, int column)
{
    FDYNSceneInspectorItem* sItem = qobject_cast<FDYNSceneInspectorItem*>(item);

    if(!sItem || !sItem->Object() || sItem->Object()->objectName() == sItem->text(0))
        return;

    NENode* node = qobject_cast<NENode*>(sItem->Object());
    node->setObjectName(sItem->text(0));

    emit FDYNGUI.sendNodeRenamed( node, sItem->text(0) );
    FDYNGUI.updateInspectors();
}


void FDYNSceneInspector::itemExpansion(FDYNTreeItem *item)
{
    FDYNSceneInspectorItem* sItem = qobject_cast<FDYNSceneInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = true;
    else
        m_ExpNodes[sItem->text(0)] = true;
}


void FDYNSceneInspector::itemCollapse(FDYNTreeItem *item)
{
    FDYNSceneInspectorItem* sItem = qobject_cast<FDYNSceneInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = false;
    else
        m_ExpNodes[sItem->text(0)] = false;
}

void FDYNSceneInspector::itemActivation(FDYNTreeItem *item, int column)
{
    //    topLeft = m_FDYNTreeModel->indexFromItem( item );
    //    m_LastSelectedItem = selectionModel()->selection();
    //    m_LastSelectedItem.select(topLeft, topLeft);

}

void FDYNSceneInspector::restoreExpandedState()
{
    if(topLevelItemCount())
    {
        //int n = items().size();

        Q_FOREACH(FDYNTreeItem* it , items())
        {

            FDYNSceneInspectorItem* item = qobject_cast<FDYNSceneInspectorItem*>(it);

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
}

void FDYNSceneInspector::clearState()
{
#if 1
    //    Cameras=0;
    //    Lights=0;
    //    Materials=0;
    //    Textures=0;
    //    FluidSolvers=0;
    //    Fluids = 0;
    //    Daemons=0;
    //    DualBoundaries=0;
    //    ForceFields=0;
    //    Meshes = 0;
    //    DataFields=0;
    //    Rigids=0;
    //    RigidSolvers=0;
    //    Deformables=0;
    //    Meshing=0;
    //    PostProcessors=0;
    //    OSDs=0;
    //    Visuals = 0;
    //    CollisionDetection=0;
    //    SceneItem=0;
    //    VolumeOps=0;
    //    MeasurementFields = 0;
    //    Groups = 0;
    SceneInpectorItems.clear();
    SceneItem=0;
#endif
}

/*!
 If we would like to make the parent of the deleted instance the current object this method should be called.
 However, than we should also be able to keep the selected/current item when updating the qtreewidget.
 This is not possible yet.
*/
void FDYNSceneInspector::setParentCurrentItem()
{	
    FDYNSceneInspectorItem* item = qobject_cast< FDYNSceneInspectorItem *>( currentItem() );
    FDYNSceneInspectorItem* parent = (FDYNSceneInspectorItem*)item->parent();
    setCurrentItem(parent);
    FDYNGUI.selectObject(parent->Object());
}


void FDYNSceneInspector::setNewCurrentItem(QObject* object)
{
    if(object)
    {
        QList<FDYNTreeItem *> items = findItems( object->objectName(), Qt::MatchRecursive );
        if( items.size() >= 2){
            foreach (FDYNTreeItem *item, items) {
                if( -1 != m_selectedItems.indexOf(item)){
                    FDYNSceneInspectorItem* zitem = (FDYNSceneInspectorItem*) (item);
                    setCurrentItem(zitem);
                    break;
                }
            }
        }
        else{
            QListIterator<FDYNTreeItem*> i(items);

            if(i.hasNext())
            {
                FDYNSceneInspectorItem* item = qobject_cast<FDYNSceneInspectorItem*> (i.next());
                setCurrentItem(item);
            }
        }

    }
    else{
        //update();
        setCurrentItem(NULL);
    }
}

void FDYNSceneInspector::setCurrentItems(QList<NENode*> objList)
{
    foreach(NENode* object, objList)
    {
        if(object)
        {
            QList<FDYNTreeItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
            if( items.size() >= 2){
                foreach (FDYNTreeItem *item, items) {
                    if( -1 != m_selectedItems.indexOf(item)){
                        FDYNSceneInspectorItem* zitem = (FDYNSceneInspectorItem*) (item);
                        zitem->setSelected(true);
                        break;
                    }
                }
            }
            else {
                QListIterator<FDYNTreeItem*> i(items);
                if(i.hasNext())
                {
                    FDYNSceneInspectorItem* item = qobject_cast<FDYNSceneInspectorItem*> (i.next());
                    item->setSelected(true);
                }
            }

        }
    }
}

void FDYNSceneInspector::saveGUIStates(QXmlStreamWriter *writer)
{
    writer->writeStartElement("expandednodes");

    if(topLevelItemCount())
    {
        Q_FOREACH(FDYNTreeItem* it , items())
        {
            FDYNSceneInspectorItem* item = qobject_cast<FDYNSceneInspectorItem*>(it);
            if(item && item->Object())
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

void FDYNSceneInspector::readGUIStates(QXmlStreamReader *reader)
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
                //Log()<< "Error parsing scene file: " << reader->errorString(); EndLog(NE::kLogWarning);
            }
        }
        else
        {
            reader->skipCurrentElement();
            Log()<< "Unknown section " << reader->name() << " in the scene file."; EndLog(NE::kLogWarning);
        }
    }
}



