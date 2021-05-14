#include "Core/Base/NEBase.h"
#include "Core/NEObject.h"
#include "Core/NENode.h"
#include "Core/Spatial/NESpatialGroup.h"
#include "Core/Base/NESceneManager.h"
#include "Managers/FDYNGUIManager.h"
#include "FDYNScenarioInspector.h"
#include "FDYNScenarioInspectorWindow.h"
#include "FDYNScenarioInspectorItem.h"

#include "Mesh/NEZone.h"
#include "Mesh/NEZoneItem.h"
#include "Mesh/NEScenarioItem.h"
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



FDYNScenarioInspector::FDYNScenarioInspector(QWidget* parent) : FDYNTreeWidget(parent)
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
     QObject::connect(this, SIGNAL(itemChanged(FDYNTreeItem*, int)), this, SLOT(nodeRenamed(FDYNTreeItem*, int)));


    m_IconActive = new QIcon("alert_active.png");
    m_IconCache = new QIcon("alert_cache.png");
    m_IconInactive = new QIcon("alert.png");

#if 1
    connect(this, &FDYNTreeWidget::itemExpanded, this, &FDYNScenarioInspector::itemExpansion);
    connect(this, &FDYNTreeWidget::itemCollapsed, this, &FDYNScenarioInspector::itemCollapse);
    //connect(this, &QTreeWidget::itemClicked, this, &FDYNZoneInspector::itemActivation);
#endif
    addScnAction    = new QAction("Add Scenario", 0 );
    connect(addScnAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addScenarioToSelected );

    deleteScnAction = new QAction("Delete Scenario", 0 );
    connect(deleteScnAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::deleteScenario );

    simulateAction = new QAction("Simulate (CFAST)");
    connect(simulateAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::simulateScenario );

//    connect(groupAction, &QAction::triggered, this, &FDYNZoneInspector::groupNodes);
//    connect(ungroupAction, &QAction::triggered, this, &FDYNZoneInspector::ungroupNodes);

    //connect(deleteScnAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::deleteScenario);

    removeItemAction = new QAction("Remove From Scenario", 0);
    connect(removeItemAction, &QAction::triggered, this, &FDYNScenarioInspector::removeSelectedItem);

    saveAsScnAction = new QAction("Save As", 0 );
    connect(saveAsScnAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::saveAsScenario);

    //setSource(QUrl("qrc:/ScenarioManager.qml"));

    //context menu
    connect(treeview, SIGNAL(sigCreateContextMenu()), this, SLOT(createContextMenu()));

    connect(&FDYNGUI, &FDYNGUIManager::dropCalled, this, &FDYNScenarioInspector::runDrop);
}

FDYNScenarioInspector::~FDYNScenarioInspector()
{
    delete addScnAction;

    delete simulateAction;
    delete deleteScnAction;
    delete m_IconActive;
    delete m_IconCache;
    delete m_IconInactive;
    delete removeItemAction;
    delete saveAsScnAction;

}

void FDYNScenarioInspector::setRoot( QObject* root)
{
    if(m_pRoot)
        clear();
    m_pRoot = root;
}

void FDYNScenarioInspector::update()
{
    if(!m_pRoot)
        return;

    QList<NEObject*> prevSelected =  NESCENE.selectedObjects()->values();

    clear();
    // Clear the pointers before rebuilding the contents
    clearState();
    Q_ASSERT_X(m_pRoot, " FDYNZoneInspector::update", "root pointer is null");

    m_searchMatchedObjects.clear();

    const QObject* child = m_pRoot;
    updateChildren( child );


    QList<NENode*> selNodes;
    foreach (NEObject *obj, prevSelected) {
        selNodes.push_back(qobject_cast<NENode*>(obj));
    }
    setCurrentItems(selNodes);

}

void FDYNScenarioInspector::updateChildren(const QObject* rootobj)
{
    FDYNScenarioInspectorItem* rootitem = new FDYNScenarioInspectorItem();

#if 0
    childItem->setForeground(0, *NEGUISTYLE.NormalFontBrush());
    childItem->setFont(0, *NEGUISTYLE.BoldFont());
#endif
    //rootitem->setText(0, rootobj->objectName());
    rootitem->setText(0, "SCENARIOS");
    rootitem->setText(1, "Engine");
    rootitem->setIcon(0, "ring.png");
    if(NESpatialEngine *engine = (NESpatialEngine *)qobject_cast<const NESpatialEngine *>(rootitem))
        rootitem->setToolTip(0, engine->nodeDescription());
    else
        rootitem->setToolTip(0, rootobj->objectName());
#if 0
    childItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
    childItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
#endif
    rootitem->setObject(const_cast<QObject*>(rootobj));
    rootitem->setObjectName(rootobj->objectName());
    addTopLevelItem(rootitem);
    rootitem->setExpanded(true);

    if(rootitem->Object() == &NESCENE)
        SceneItem = rootitem;

    if (NESpatialEngine* fEngine = qobject_cast<NESpatialEngine*>(m_pRoot))
    {
        if (NEScenario* activeScn = qobject_cast<NEScenario*>(NESCENE.findChildNode(fEngine->activeScenarioName())))
        {
            //
            // Then Create Categories under the root zone
            //
            FDYNScenarioInspectorItem* scnitem = createRootOrganizerItems( activeScn, rootitem );
            //
            // Then update the children - Dont create the zone element in the tree since createRootOrganizerItem
            // has already created that. So pass the activeZone and the zone inspector item to be the root and
            // create subtrees under that using traverseTree
            //
            if( rootobj && rootobj->children().size() > 0 )
            {
                traverseTree( activeScn, activeScn, scnitem );
            }
        }
    }

    FDYNTreeWidget::update();
    restoreExpandedState();
#if 0
    rsizeColumnToContents(0);
#endif
}

FDYNScenarioInspectorItem*  FDYNScenarioInspector::createRootOrganizerItems( NEScenario *rootscn, FDYNScenarioInspectorItem* rootItem )
{
    FDYNScenarioInspectorItem* childItem = new FDYNScenarioInspectorItem();
    childItem->setText( 0, rootscn->objectName() );
    childItem->setText( 1, rootscn->metaObject()->className() );
    if( NEScenario* scn = qobject_cast<NEScenario*>( rootscn ))
        childItem->setIcon( 0, scn->Type() );

    rootItem->addChild( childItem );
#if 0
    childItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
    childItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
#endif
    childItem->setObject( rootscn );
    childItem->setObjectName( rootscn->objectName() );
    childItem->setFlags( childItem->flags()^Qt::ItemIsEditable );

    //QStringList items = FZoneItem::categoryTypes();
    QStringList ctypes = {
                        "Components",
                        "Other Raceways",
                        "Sources",
                        "Trays",
                        "Conduits",
                        "Equipments",
                        "Physical Only",
                        "Simulation Items/Vents",
                        "Simulation Items/Suppression"
                        };
    for( auto item: ctypes )
    {
        createOrganizerItem( item, childItem );

    }
    return childItem;
}

bool containsStringinScenario(QObject *obj, const NEScenario* scenario, QString searchText)
{
    bool ret = false;

    foreach (QString str, scenario->itemNameChildren(obj->objectName()))
    {
        if(NEZoneItem *zitem = qobject_cast<NEZoneItem *>(NESCENE.findChildNode(str)))
        {
            if(zitem->objectName().contains(searchText, Qt::CaseInsensitive))
            {
                ret = true;
                break;
            }
            else if( containsStringinScenario(zitem, scenario, searchText))
            {
                ret = true;
                break;
            }
        }
    }

    return ret;
}

void FDYNScenarioInspector::traverseTree(const QObject* obj,
                                         const NEScenario* scenario,
                                         FDYNScenarioInspectorItem* objItem)
{
    const QObjectList& objList = obj->children();
    QObjectList cobjList;

    //
    // Search the scene based on name (if scenario or scenezoneitem) and add it to objList
    if (qobject_cast<NESpatialEngine*>(const_cast<QObject*>(obj)))
        cobjList = objList;
    else
    {
        for (auto& itemName : scenario->itemNameChildren(obj->objectName()))
        {
            QObject* cnode = NESCENE.findChildNode(itemName);
            if (cnode && !cobjList.contains(cnode))
                cobjList.push_back(cnode);
        }
    }

    Q_FOREACH(const QObject* child, cobjList)
    {
        if( qobject_cast< NEZone * >( const_cast<QObject*>(child)) )
            continue;


        FDYNScenarioInspectorItem* childItem = new FDYNScenarioInspectorItem();
        childItem->setText( 0, child->objectName() );
        childItem->setText( 1, child->metaObject()->className() );
        if( NEScenario* scn = qobject_cast<NEScenario*>(const_cast<QObject*>(child)))
            childItem->setIcon(0, scn->Type() );
        else
            childItem->setIcon(0, QString("ring.png"));
#if 0
        childItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
        childItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
#endif
        childItem->setObject( const_cast<QObject*>(child) );
        childItem->setObjectName(child->objectName());
        childItem->setFlags( childItem->flags()^Qt::ItemIsEditable );

        if(NEObject* neObject = qobject_cast<NEObject*>(const_cast<QObject*>(child)))
        {
            NENode* node = qobject_cast<NENode*>(neObject);
            if(neObject->isHidden())
            {
                node = qobject_cast<NENode*> (const_cast<QObject*>(child));
                if( node == NULL )
                    continue;

                FDYNScenarioInspectorItem *categoryItem = NULL;
                //if(node->factory().nodeInherits() != "Groups")
                {
                    //
                    // Add all the zone specific items
                    //
                    if( NEZoneItem* zoneItem = qobject_cast<NEZoneItem*>(const_cast<QObject*>(child)))
                    {
                        QString keyString = zoneItem->categoryType();
                        FDYNScenarioInspectorItem *pp = dynamic_cast<FDYNScenarioInspectorItem *>(objItem);
                        while (pp)
                        {
                            keyString += "," + pp->text(0);
                            pp = dynamic_cast<FDYNScenarioInspectorItem *>(pp->parent());
                        }


                        if(m_searchText.isEmpty()
                                || containStringinChildren(childItem->Object(), m_searchText)
                                || containsStringinScenario(childItem->Object(), scenario, m_searchText))
                        {
                            QMap<QString, FDYNScenarioInspectorItem*>::iterator itr = SceneInpectorItems.find(keyString);
                            if(  itr == SceneInpectorItems.end())
                            {
                                //std::cout << "Key String Insert2: " << keyString.toStdString() << std::endl;

                                //
                                // Since categories are created already this should never be reached
                                //
                                categoryItem = createOrganizerItem(zoneItem->categoryType(), objItem);
                                SceneInpectorItems.insert(keyString, categoryItem);
                            }
                            else
                            {
                                categoryItem = itr.value();
                            }

                            //std::cout << "Category item " << categoryItem << std::endl;

                            categoryItem->setIcon(0, QString("ring.png"));
                            categoryItem->addChild(childItem);

                            if(!m_searchText.isEmpty() &&
                                    childItem->Object()->objectName().contains(m_searchText, Qt::CaseInsensitive))
                                m_searchMatchedObjects.push_back(childItem->Object());
                        }
                    }
                }
                //else
                {
                    if(m_searchText.isEmpty() || containStringinChildren(childItem->Object(), m_searchText) || containsStringinScenario(childItem->Object(), scenario, m_searchText))
                    {
#if 0
                        childItem->setForeground(0, *NEGUISTYLE.NormalFontBrush());
#endif
                        //objItem->addChild(childItem);

                        if(!m_searchText.isEmpty() && childItem->Object()->objectName().contains(m_searchText, Qt::CaseInsensitive))
                            m_searchMatchedObjects.push_back(childItem->Object());
                    }
                }

                // Init the item
                if(node)
                {
                    NENodeFactory* f = &(node->factory());
                    childItem->setText(1, f->nodeName());

                    //if(node->factory().nodeInherits() != "Groups")
                    {
                        NENodeIcon icon = *f->nodeIcon();

                        QIcon qicon;
                        for(unsigned int i = 0; i < icon.size(); i++)
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
//                                if(isWireframe)
//                                    childItem->setIcon(1, QString("3d.png"));
//                                else
//                                    childItem->setIcon(1, QString("3dfill.png"));

                                if( node->factory().nodeName() == "Cables" )
                                {
                                    childItem->setIcon(1, QString(""));
                                }

                            }
                            else
                            {
                                if( NEZoneItem* zoneItem = qobject_cast<NEZoneItem*>(const_cast<QObject*>(child)))
                                    childItem->setIcon(0, zoneItem->Type());
//                                if(NESpatialMesh *spatial = qobject_cast<NESpatialMesh *>(const_cast<QObject*>(child)))
//                                {
//                                    if(spatial->SpatialMeshType() != NESpatialMesh::None)
//                                        childItem->setIcon(1, QString("3dfill.png"));
//                                    else
//                                        childItem->setIcon(1, QString("3d.png"));
//                                }
                                //
                                // Dont have 3D create icon in scenario tree
                                //
                                if( node->factory().nodeName() == "Cables" )
                                {
                                    childItem->setIcon(1, QString(""));
                                }
                            }

                            if (scenario->itemNameTreeContains(node->objectName(), obj->objectName()))
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
                traverseTree(child, scenario, childItem);
            }
        }
        else
        {
            delete childItem;
        }
    }
}

void FDYNScenarioInspector::slotDropDownItemChanged(QString newScn)
{
    emit FDYNGUI.setActiveScenario( newScn );
    return;
}

void FDYNScenarioInspector::slotOtherItemChanged(QString newScn)
{
    emit FDYNGUI.setActiveScenario( newScn );
    m_otherComboObject->setProperty("typedString", QString(""));
    return;
}


FDYNScenarioInspectorItem* FDYNScenarioInspector::createOrganizerItem(QString name, FDYNScenarioInspectorItem *objItem)
{
    QRegExp rx("\\/");
    QStringList query = name.split(rx);

    FDYNScenarioInspectorItem* objOrgItem = objItem;
    FDYNScenarioInspectorItem* parentOrgItem = objOrgItem;

    //std::cout << "Name: " << name.toStdString() << std::endl;
    //
    // If there are sub category items separated by / then use the parent as the category name
    //
    QStringList parentquery = query;
    if( parentquery.size() > 1 )
        parentquery.removeLast();

    QString keyParentString = parentquery.last();
    FDYNScenarioInspectorItem *pp = dynamic_cast< FDYNScenarioInspectorItem *>( objItem );

    while( pp )
    {
        keyParentString += "," + pp->text(0);
        pp = dynamic_cast< FDYNScenarioInspectorItem * >( pp->parent() );
    }

    //std::cout << "Finding Parent Item " << keyParentString.toStdString() << std::endl;
    if( SceneInpectorItems.find(keyParentString) != SceneInpectorItems.end() )
    {
        parentOrgItem = SceneInpectorItems.value( keyParentString );
        //std::cout << " -- found " << std::endl;
    }

    QStringList keyStrings ;
    QString ks = query[0];
    FDYNScenarioInspectorItem* item = nullptr;
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
        FDYNScenarioInspectorItem *pp = dynamic_cast< FDYNScenarioInspectorItem *>( objItem );

        while( pp )
        {
            keyString += "," + pp->text(0);
            pp = dynamic_cast< FDYNScenarioInspectorItem * >( pp->parent() );
        }

        //std::cout << "  Key Parent Str " << keyParentString.toStdString() << std::endl;
        //std::cout << "  Key Str " << keyString.toStdString() << std::endl;

        if( SceneInpectorItems.find(keyString) != SceneInpectorItems.end() )
        {
            continue;
        }

        item = new FDYNScenarioInspectorItem();
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
        item->setToolTip(0, query[i]);
        item->setIcon(0, QString("ring.png"));
        item->setObjectName(name+"_"+objItem->objectName());

        parentOrgItem->addChild(item);
        parentOrgItem = item;
    }

    return item;
}

void FDYNScenarioInspector::runDrop(matrix44f view, matrix44f proj, double x, double y, double w, double h, void *tItem)
{
    return;
    //qDebug() << x << y << tItem;
    if(!tItem )
        return;

    if(FDYNScenarioInspectorItem *ziItem = static_cast<FDYNScenarioInspectorItem *>(tItem))
    {
        if(!ziItem->Object())
            return;
        if(NEZoneItem *zItem = qobject_cast<NEZoneItem*>(ziItem->Object())){
            zItem->SetSpatialMeshType(NESpatialMesh::Cuboid);

            matrix44f finalMat = proj * view;
            bool inverted = false;
            finalMat.invert(&inverted);
            double px = x / w - 0.5;
            double py = y / h - 0.5;
            py = - py;

            px *= 2.0;
            py *= 2.0;

            vec3f p1(px, py, 0.0);
            p1 = finalMat * p1;
            vec3f p2(px, py, 1.0);
            p2 = finalMat * p2;

            vec3f dirLine = p2 - p1;
            dirLine.normalize();
            vec3f norPlane(0, 1, 0);

            vec3f interPoint = linePlaneInterPoint(p1, dirLine, vec3f(0.0, 0.0, 0.0), norPlane);

            if (!std::isnan(interPoint.x()) && !std::isnan(interPoint.y()) && !std::isnan(interPoint.z()))
                zItem->SetPosition(interPoint);

            FDYNGUI.updateInspectors();
            FDYNGUI.updateGLWindows();
        }
    }
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

void FDYNScenarioInspector::createContextMenu()
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
    FDYNScenarioInspectorItem* item = (FDYNScenarioInspectorItem*) currentItem();
    if(item != NULL)
    {
        QObject* object = item->Object();

        if(NEObject* tmpObject =  qobject_cast<NEObject*> (object))
        {
            //populate context menu with actions
            const QList<QObject*>& objectActions = *tmpObject->actions();
            Q_FOREACH(QObject* action, objectActions)
            {
                QAction *aaction = qobject_cast<QAction *>( action );
                if( aaction->text() == QString( "Delete" ) )
                {
                }
                else if( aaction->text() == QString("Edit Caching"))
                {
                    //removeAction( aaction );
                }
                else if( aaction->text().contains(QString("Reload 3D") ))
                {
                    //removeAction( aaction );
                }
                else if( aaction->text() == QString("Clean Data"))
                {

                }
                else
                {
                    addAction( aaction );
                }
            }
            if(qobject_cast<NESpatialGroup*>(object))
            {
                //if(!qobject_cast<NESpatialEngine*>(object) && !qobject_cast<NEZone*>(object) && !qobject_cast<NEScenario*>(object) && !qobject_cast<NEZoneItem*>(object))
                //    addAction( ungroupAction ) ;
            }
            if(NESpatialEngine* parent = qobject_cast<NESpatialEngine*>(tmpObject))
            {
                //addAction( addScnAction );
                //addAction( deleteScnAction );
            }

            if( qobject_cast<NEZone*> (tmpObject) ||  qobject_cast<NEZoneItem*> (tmpObject) )
            {
                addAction(removeItemAction);
                //addAction(subItemAction);
            }
            if( qobject_cast<NEScenario*> (tmpObject) )
            {
                addAction( simulateAction );
                addAction( deleteScnAction );
                addAction( saveAsScnAction );
                //addAction(subItemAction);
            }
        }
    }


    //if(items.size() > 1)
    //    addAction(groupAction);

    QList<NENode*> nodeList;

    Q_FOREACH(FDYNTreeItem* item , selectedItems())
    {
        FDYNScenarioInspectorItem* sItem = static_cast<FDYNScenarioInspectorItem*>(item);
        nodeList.append(static_cast<NENode*>(sItem->Object()));
    }
    if(items.size() > 1)
    {
        foreach(NENode* node, nodeList)
            if(NESpatialGroup* group = qobject_cast<NESpatialGroup*>(node))
            {
                //addAction(parentAction);
                break;
            }
    }
    //set context policy
    this->setContextMenuPolicy(Qt::ActionsContextMenu);
}

///
/// \brief FDYNScenarioInspector::removeSelected
///

void FDYNScenarioInspector::removeSelectedItem()
{
    FDYNScenarioInspectorItem* curItem = static_cast<FDYNScenarioInspectorItem*>(currentItem());
    if (curItem == NULL) return;

    FDYNScenarioInspectorItem* curParent = static_cast<FDYNScenarioInspectorItem*>(curItem->parent());

    QList<std::pair<QObject*, QObject*>> childParentPairs;

    // Ignore the category items
    while (curParent != nullptr && curParent->Object() == nullptr)
        curParent = static_cast<FDYNScenarioInspectorItem*>(curParent->parent());

    // Compute the child-parent pairs in the subtree without the category items
    QList<FDYNScenarioInspectorItem*> itemsToProcess;
    itemsToProcess.push_back(curItem);
    while (itemsToProcess.size() > 0)
    {
        FDYNScenarioInspectorItem* item = itemsToProcess.front();
        FDYNScenarioInspectorItem* parent = item;

        // Ignore the category items
        while (parent != nullptr && parent->Object() == nullptr)
            parent = static_cast<FDYNScenarioInspectorItem*>(parent->parent());

        for (FDYNTreeItem* child : item->takeChildren())
        {
            if (FDYNScenarioInspectorItem* scnChild = qobject_cast<FDYNScenarioInspectorItem*>(child))
            {
                if (scnChild->Object() != nullptr)
                    childParentPairs.push_back(std::pair<QObject*, QObject*>(scnChild->Object(), parent->Object()));

                itemsToProcess.push_back(scnChild);
            }
        }

        itemsToProcess.pop_front();
    }

    if (curParent != nullptr && curParent->Object() != nullptr)
        childParentPairs.push_back(std::pair<QObject*, QObject*>(curItem->Object(), curParent->Object()));

    FDYNGUI.removeItemsFromScenario(childParentPairs);

    update();
    FDYNGUI.selectObject( NULL );
    FDYNGUI.updateNodeConnectionsTable(); //Update node connection table of connection inspector
}

bool FDYNScenarioInspector::isSelectedObect(QObject* object)
{
    const FDYNScenarioInspectorItem* item = (FDYNScenarioInspectorItem*) currentItem();
    if(item)
        return (object == item->Object());
    else
        return false;
}

void FDYNScenarioInspector::selectItems()
{
    const QList<FDYNTreeItem *>& items = selectedItems();

    if(items.size() == 1)
    {
        if(FDYNScenarioInspectorItem* sItem =  qobject_cast<FDYNScenarioInspectorItem*>(items.first()))
            if (NENode* casted = qobject_cast<NENode*>(sItem->Object()))
            {
                FDYNGUI.selectObject(casted);
            }
    }
    else if(items.size() > 1)
    {
        QList<NENode*> objects;
        Q_FOREACH(FDYNTreeItem* item2, items)
        {
            if (FDYNScenarioInspectorItem* sItem =  qobject_cast<FDYNScenarioInspectorItem*>(item2))
                if(NENode* casted = qobject_cast<NENode*>(sItem->Object()))
                    objects.push_back(casted);
        }
        FDYNGUI.selectObjects(objects);
    }
    else
    {
        FDYNGUI.selectObject(NULL);
    }
}

void FDYNScenarioInspector::addItemToTree(const QObject* itemObj, const QObject* parentObj)
{

    FDYNScenarioInspectorItem* item = new FDYNScenarioInspectorItem();

    item->setText(0, itemObj->objectName());
    item->setText(1, itemObj->metaObject()->className());
    if (NEScenario* scn = qobject_cast<NEScenario*>(const_cast<QObject*>(itemObj)))
        item->setIcon(0, scn->Type());
    else
        item->setIcon(0, QString("ring.png"));
    item->setObject(const_cast<QObject*>(itemObj));
    item->setObjectName(itemObj->objectName());
    item->setFlags(item->flags()^Qt::ItemIsEditable);

    if(NENode* node = qobject_cast<NENode*>(const_cast<QObject*>(itemObj)))
    {
        //if(node->factory().nodeInherits() != "Groups")
        {
            if (NEZoneItem* zoneItem = qobject_cast<NEZoneItem*>(node))
            {
                QString keyString = zoneItem->categoryType();
                const FDYNScenarioInspectorItem *parentItem = qobject_cast<const FDYNScenarioInspectorItem *>( parentObj );
                //
                // The following check is to see if the item selected is a category or a regular item
                // If Category - we need to start with parent as the key string
                //
                if( parentItem != nullptr && parentItem->Object() == nullptr && parentItem->parent() )
                    parentItem = dynamic_cast<FDYNScenarioInspectorItem *>( parentItem->parent() );

                while( parentItem )
                {
                    keyString += "," + parentItem->text(0);
                    parentItem = dynamic_cast<FDYNScenarioInspectorItem *>(parentItem->parent());
                }

                QMap<QString, FDYNScenarioInspectorItem*>::iterator itr = SceneInpectorItems.find(keyString) ;
                FDYNScenarioInspectorItem* categoryItem = nullptr;
                if( itr == SceneInpectorItems.end() )
                {
                }
                else
                    categoryItem = itr.value();

                //std::cout << "key string " << keyString.toStdString() << std::endl;

                if( categoryItem )
                {
                    categoryItem->setIcon(0, QString("ring.png"));
                    categoryItem->addChild(item);
                }
            }
        }
        //else
        //    selectedItem->addChild(item);


        NENodeFactory* f = &(node->factory());
        item->setText(1, f->nodeName());

        if (node->factory().nodeInherits() != "Groups")
        {
            QIcon qicon;
            for (NEIcon& icon : *f->nodeIcon())
                qicon.addFile(icon.iconPath, icon.iconSize);
            item->setIcon(1, qicon);

            if (NEZoneItem* zoneItem = qobject_cast<NEZoneItem*>(node))
                item->setIcon(0, zoneItem->Type());

            if (node->factory().nodeName() == "Cables")
                item->setIcon(1, QString(""));

            node->SetBehavior(NENode::Active);
            item->setIcon2(1, QString("alert_active.png"));
        }

        if (NEZone* zone = qobject_cast<NEZone*>(node))
            item->setToolTip(0, zone->nodeDescription());
        else if (NEScenario* scn = qobject_cast<NEScenario*>(node))
            item->setToolTip(0, scn->nodeDescription());
        else if (NEZoneItem* zitem = qobject_cast<NEZoneItem*>(node))
            item->setToolTip(0, zitem->nodeDescription());
    }
    else
        delete item;
}

void FDYNScenarioInspector::renameSelected()
{
    QVariant returnValue;
    QMetaObject::invokeMethod( currentTextInput, "setActiveFocusFromCpp", Q_RETURN_ARG(QVariant, returnValue));
}

void FDYNScenarioInspector::nodeRenamed(FDYNTreeItem *item, int column)
{
    FDYNScenarioInspectorItem* sItem = qobject_cast<FDYNScenarioInspectorItem*>(item);

    //qDebug() << "object name is " << sItem->Object()->objectName() << ", item text is " <<  sItem->text(0);
    if(!sItem || !sItem->Object() || sItem->Object()->objectName() == sItem->text(0))
        return;

    //qDebug() << "object name is " << sItem->Object()->objectName() << ", item text is " <<  sItem->text(0);

    NENode* node = qobject_cast<NENode*>(sItem->Object());

    FDYNGUI.renameNode( node, sItem->text(0) );
    FDYNGUI.updateInspectors();
}


void FDYNScenarioInspector::itemExpansion(FDYNTreeItem *item)
{
    FDYNScenarioInspectorItem* sItem = static_cast<FDYNScenarioInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = true;
    else
        m_ExpNodes[sItem->text(0)] = true;
}

void FDYNScenarioInspector::itemCollapse(FDYNTreeItem *item)
{
    FDYNScenarioInspectorItem* sItem = static_cast<FDYNScenarioInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = false;
    else
        m_ExpNodes[sItem->text(0)] = false;
}

void FDYNScenarioInspector::itemActivation(FDYNTreeItem *item, int column)
{
    //topLeft = this->indexFromItem(item);
    //    m_LastSelectedItem = selectionModel()->selection();
    //    m_LastSelectedItem.select(topLeft, topLeft);

}

void FDYNScenarioInspector::restoreExpandedState()
{

    if(topLevelItemCount())
    {
        Q_FOREACH(FDYNTreeItem* it , items())
        {
            FDYNScenarioInspectorItem* item = qobject_cast<FDYNScenarioInspectorItem*>(it);

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

void FDYNScenarioInspector::clearState()
{
    SceneInpectorItems.clear();
    SceneItem=0;
}

/*!
 If we would like to make the parent of the deleted instance the current object this method should be called.
 However, than we should also be able to keep the selected/current item when updating the qtreewidget.
 This is not possible yet.
*/
void FDYNScenarioInspector::setParentCurrentItem()
{	
    FDYNScenarioInspectorItem* item = (FDYNScenarioInspectorItem*) currentItem();
    FDYNScenarioInspectorItem* parent = (FDYNScenarioInspectorItem*)item->parent();
    setCurrentItem(parent);
    FDYNGUI.selectObject(parent->Object());
}

void FDYNScenarioInspector::setNewCurrentItem(QObject* object)
{
    if(object)
    {
        QList<FDYNTreeItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
        if(items.size() >= 2){
            foreach (FDYNTreeItem *item, items) {
                if( -1 != m_selectedItems.indexOf(item)){
                    FDYNScenarioInspectorItem* zitem = (FDYNScenarioInspectorItem*) (item);
                    setCurrentItem(zitem);
                    break;
                }
            }
        }
        else{
            QListIterator<FDYNTreeItem *> i(items);
            if( i.hasNext()){
                if( FDYNScenarioInspectorItem *sitem = qobject_cast<FDYNScenarioInspectorItem *>(i.next()))
                    setCurrentItem(sitem);
            }

        }

    }
    else{
        //update();
        setCurrentItem(NULL);
    }
}

void FDYNScenarioInspector::setCurrentItems(QList<NENode*> objList)
{
    foreach(NENode* object, objList)
    {
        if(object)
        {
            QList<FDYNTreeItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
            if(items.size() >= 2){
                foreach (FDYNTreeItem *item, items) {
                    if( -1 != m_selectedItems.indexOf(item)){
                        FDYNScenarioInspectorItem* zitem = (FDYNScenarioInspectorItem*) (item);
                        zitem->setSelected(true);
                        break;
                    }
                }
            }
            else{
                QListIterator<FDYNTreeItem *> i(items);
                if( i.hasNext()){
                    if( FDYNScenarioInspectorItem *sitem = qobject_cast<FDYNScenarioInspectorItem *>(i.next()))
                        sitem->setSelected(true);
                }

            }

        }
    }
}

void FDYNScenarioInspector::saveGUIStates(QXmlStreamWriter *writer)
{

    writer->writeStartElement("expandednodes");

    if(topLevelItemCount())
    {
        Q_FOREACH(FDYNTreeItem* it , items())
        {
            FDYNScenarioInspectorItem* item = qobject_cast<FDYNScenarioInspectorItem*>(it);
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



//    writer->writeStartElement("expandednodes");

//    if(topLevelItemCount())
//    {
//        QTreeWidgetItemIterator it(this);
//        while(*it)
//        {
//            FDYNZoneInspectorItem* item = static_cast<FDYNZoneInspectorItem*>(*it);
//            if(item->Object())
//            {
//                writer->writeStartElement(item->Object()->objectName());
//                writer->writeAttribute("expanded", QString("%1").arg(item->isExpanded()));
//                writer->writeEndElement();
//            }

//            ++it;
//        }
//    }

//    writer->writeEndElement();
}

void FDYNScenarioInspector::readGUIStates(QXmlStreamReader *reader)
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

void FDYNScenarioInspector::updateOtherScenarios(QList< QString >& otherscns)
{

    NESpatialEngine *fEngine = qobject_cast<NESpatialEngine *>(m_pRoot);
    if(!fEngine)
        return;
    QString curScenario = fEngine->activeScenarioName();

    clearDropDownItems();

    for( unsigned i = 0; i < otherscns.size(); ++i )
    {
        addDropDownItem( otherscns[i] );
        if(curScenario == otherscns[i])
            m_dropDownObject->setProperty("currentIndex", i);
    }

}

void FDYNScenarioInspector::updateAllScenarios( QList<QString> allScenarios)
{
    NESpatialEngine *fEngine = qobject_cast<NESpatialEngine *>(m_pRoot);
    if(!fEngine)
        return;
    QString curScenario = fEngine->activeScenarioName();

    clearOtherListItems();

    for( unsigned i = 0; i < allScenarios.size(); ++i )
    {
        addOtherListItem( allScenarios[i] );
    }
    for( unsigned i = 0; i < m_dropDownList.size(); i++){
        if(curScenario == m_dropDownList[i])
            m_dropDownObject->setProperty("currentIndex", i);
    }
}

