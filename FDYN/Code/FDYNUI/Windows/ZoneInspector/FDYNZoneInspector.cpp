#include "Core/Base/NEBase.h"
#include "Core/NEObject.h"
#include "Core/NENode.h"
#include "Core/Spatial/NESpatialGroup.h"
#include "Core/Base/NESceneManager.h"
#include "Manager/NEGeometryManager.h"
#include "Managers/FDYNGUIManager.h"
#include "FDYNZoneInspector.h"
#include "FDYNZoneInspectorWindow.h"
#include "FDYNZoneInspectorItem.h"
#include "Windows/Main/FDYNMainWindow.h"

#include "Geometry/FEngine/FZoneItem.h"

#include "Mesh/NEZone.h"
#include "Mesh/NEZoneItem.h"
#include "Mesh/NEScenario.h"
#include "Mesh/NESpatialEngine.h"
#include "Mesh/NEZoneCompartment.h"
#include "Resource/Mesh/NEMeshContainer.h"
#include "Core/Utility/mathUtil.h"

#include <QElapsedTimer>

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

FDYNZoneInspector::FDYNZoneInspector(QWidget* parent) : FDYNTreeWidget(parent)
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

    m_IconActive    = new QIcon("alert_active.png");
    m_IconCache     = new QIcon("alert_cache.png");
    m_IconInactive  = new QIcon("alert.png");

#if 1
    connect(this, &FDYNTreeWidget::itemExpanded, this, &FDYNZoneInspector::itemExpansion);
    connect(this, &FDYNTreeWidget::itemCollapsed, this, &FDYNZoneInspector::itemCollapse);
    //connect(this, &QTreeWidget::itemClicked, this, &FDYNZoneInspector::itemActivation);
#endif

    loadDatabaseAction       = new QAction("Load Database" , 0);
    importFRANXAction        = new QAction("Import FRANX", 0);

    subItemAction            = new QAction("Add Item", 0);
    addComponent             = new QAction("Add Component", 0);
    deleteItemAction         = new QAction("Delete", 0);

    duplicateAction          = new QAction("Duplicate", 0 );
    copyPropertiesAction     = new QAction("Copy", 0);
    pastePropertiesAction    = new QAction("Paste All", 0);
    pasteDataPropertiesAction = new QAction("Paste Data", 0);
    paste3DPropertiesAction   = new QAction("Paste 3D", 0);

    addToScnAction           = new QAction("Add To Scenario", 0);
    removeFromScnAction      = new QAction("Remove From Scenario", 0);
    removeItemAction         = new QAction("Remove From Zone", 0);

    simulateNewScns          = new QAction("Simulate (CFAST)");
    convertToSource          = new QAction("Convert To Source" );
    convertToItem            = new QAction("Convert To Component" );
    convertToPhyOnly         = new QAction("Convert To Physical" );

    addRaceway               = new QAction("Add Tray");
    addCable                 = new QAction("Add Cable");

    addFireDescAction        = new QAction("Add Source", 0 );

    addVentAction            = new QAction("Add Natural Vent", 0 );
    addMechVentAction        = new QAction("Add Mechanical Vent", 0 );
    turnIntoVentAction       = new QAction("Convert To Natural Vent", 0);
    turnIntoMechVentAction   = new QAction("Convert To Mechanical Vent", 0);

    addCompAction            = new QAction("Add Boundary", 0 );
    addFireDetectionAction   = new QAction("Add Detection", 0 );

    actionNewZone = new QAction("New Zone");
    connect( actionNewZone, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addZoneAndScenario );
    addAction(actionNewZone);

    actionNewScn = new QAction( "New Scenario" );
    connect( actionNewScn, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addScenario );
    addAction(actionNewScn);

    connect(addCompAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addCompartment );

    connect(addFireDescAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addFire );

    connect(addVentAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addVent );
    connect(addMechVentAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addMechVent );
    connect(turnIntoVentAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::turnIntoVent);
    connect(turnIntoMechVentAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::turnIntoMechVent);

    connect(addFireDetectionAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addFireDetection );

    connect(subItemAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addItemToSelected);
    connect(addComponent, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addComponent);
    connect(deleteItemAction, &QAction::triggered, this, &FDYNZoneInspector::deleteSelectedItem);
    connect(removeItemAction, &QAction::triggered, this, &FDYNZoneInspector::removeSelectedItem);

    connect(duplicateAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::duplicateObject);
    connect(copyPropertiesAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::copyProperties);
    connect(pastePropertiesAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::pasteProperties);
    connect(pasteDataPropertiesAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::pasteDataProperties);
    connect(paste3DPropertiesAction, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::paste3DProperties);


    connect(addToScnAction, &QAction::triggered, this, &FDYNZoneInspector::addItemToScenario);
    connect(removeFromScnAction, &QAction::triggered, this, &FDYNZoneInspector::removeItemFromScenario);

    connect(simulateNewScns, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::simulateNewScenario);
    connect(convertToSource, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::convertToSource);
    connect(convertToItem, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::convertToItem);
    connect(convertToPhyOnly, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::convertToItem);

    connect(addRaceway, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addTrayToSelected );
    connect(addCable, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::addCableToSelected );

    //setSource(QUrl("qrc:/ScenarioManager.qml"));

    //context menu
    connect(treeview, SIGNAL(sigCreateContextMenu()), this, SLOT(createContextMenu()));

    connect(&FDYNGUI, &FDYNGUIManager::dropCalled, this, &FDYNZoneInspector::runDrop);
}

FDYNZoneInspector::~FDYNZoneInspector()
{
//    while(topLevelItemCount())
//        delete takeTopLevelItem(0);

    delete duplicateAction;
    delete copyPropertiesAction;
    delete pastePropertiesAction;
    delete pasteDataPropertiesAction;
    delete paste3DPropertiesAction;

    delete addToScnAction;
    delete removeFromScnAction;

    delete simulateNewScns;
    delete convertToSource;
    delete convertToItem;
    delete convertToPhyOnly;

    delete addCompAction;

    delete addRaceway;
    delete addCable;

    delete addFireDetectionAction;

    delete addFireDescAction;

    delete addVentAction;
    delete addMechVentAction;
    delete turnIntoVentAction;
    delete turnIntoMechVentAction;

    delete subItemAction;
    delete addComponent;
    delete deleteItemAction;
    delete removeItemAction;

    delete actionNewZone;
    delete actionNewScn;

    delete loadDatabaseAction;
    delete importFRANXAction;

    delete m_IconActive;
    delete m_IconCache;
    delete m_IconInactive;
}

void FDYNZoneInspector::setRoot( QObject* root)
{
    if(m_pRoot)
        clear();
    m_pRoot = root;
}

void FDYNZoneInspector::update()
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
    //std::cout << "The update operation took " << timer.elapsed() << " milliseconds" << std::endl;

    QList<NENode*> selNodes;
    foreach (NEObject *obj, prevSelected)
        selNodes.push_back(qobject_cast<NENode*>(obj));
    setCurrentItems(selNodes);
}

void FDYNZoneInspector::updateChildren( const QObject* rootobj )
{
    FDYNZoneInspectorItem* rootitem = new FDYNZoneInspectorItem();
#if 0
    childItem->setForeground(0, *NEGUISTYLE.NormalFontBrush());
    childItem->setFont(0, *NEGUISTYLE.BoldFont());
#endif
    rootitem->setText( 0, rootobj->objectName() );
    rootitem->setText( 1, "Engine" );
    rootitem->setIcon( 0, "ring.png" );
    if( NESpatialEngine *engine = (NESpatialEngine *)qobject_cast<const NESpatialEngine *>(rootitem) )
        rootitem->setToolTip( 0, engine->nodeDescription() );
    else
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

    if (NESpatialEngine* fEngine = qobject_cast<NESpatialEngine*>(m_pRoot))
    {
        if (NEZone* activeZone = qobject_cast<NEZone*>(NESCENE.findChildNode(fEngine->activeZoneName())))
        {
            if (NEScenario* activeScenario = qobject_cast<NEScenario*>(NESCENE.findChildNode(fEngine->activeScenarioName())))
            {
                //
                // Then Create Categories under the root zone
                //
                FDYNZoneInspectorItem* zoneiitem = createRootOrganizerItems( activeZone, rootitem );
                //
                // Then update the children - Dont create the zone element in the tree since createRootOrganizerItem
                // has already created that. So pass the activeZone and the zone inspector item to be the root and
                // create subtrees under that using traverseTree
                //
                if ( rootobj->children().size() > 0 )
                    traverseTree( activeZone, activeZone, activeScenario, zoneiitem );
            }
        }
    }

    FDYNTreeWidget::update();
    restoreExpandedState();
#if 0
    rsizeColumnToContents(0);
#endif
}


FDYNZoneInspectorItem*  FDYNZoneInspector::createRootOrganizerItems( NEZone *rootzone, FDYNZoneInspectorItem* rootItem )
{
    FDYNZoneInspectorItem* childItem = new FDYNZoneInspectorItem();
    childItem->setText( 0, rootzone->objectName() );
    childItem->setText( 1, rootzone->metaObject()->className() );
    if( NEZone* zone = qobject_cast<NEZone*>( rootzone ))
        childItem->setIcon( 0, zone->Type() );

    rootItem->addChild( childItem );
#if 0
    childItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
    childItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
#endif
    childItem->setObject( rootzone );
    childItem->setObjectName( rootzone->objectName() );
    childItem->setFlags( childItem->flags()^Qt::ItemIsEditable );

    //QStringList items = FZoneItem::categoryTypes();
    QStringList ctypes = {
                        "Boundaries",
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


bool containsStringinZone(QObject *obj, const NEZone* zone, QString searchText)
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
            else if( containsStringinZone(zitem, zone, searchText))
            {
                ret = true;
                break;
            }
        }
    }

    return ret;
}

void FDYNZoneInspector::traverseTree(const QObject* obj, const NEZone* zone,
                                     const NEScenario* scenario,
                                     FDYNZoneInspectorItem* objItem)
{
    const QObjectList& objList = obj->children();
    QObjectList cobjList;

    //
    // Search the scene based on name (if scenario or scenezoneitem) and add it to objList
    //
    if( qobject_cast< NESpatialEngine*>(const_cast<QObject*>(obj)) )
        cobjList = objList;
    else
    {
        for( auto& itemName : zone->itemNameChildren(obj->objectName()) )
        {
            QObject* cnode = NESCENE.findChildNode( itemName );

            if( cnode && !cobjList.contains( cnode) )
                cobjList.push_back( cnode );
        }
    }

    Q_FOREACH(const QObject* child, cobjList)
    {
        if( qobject_cast< NEScenario * >( const_cast<QObject*>( child ) ) )
            continue;

        FDYNZoneInspectorItem* childItem = new FDYNZoneInspectorItem();
        childItem->setText( 0, child->objectName() );
        childItem->setText( 1, child->metaObject()->className() );
        if( NEZone* zone = qobject_cast<NEZone*>(const_cast<QObject*>( child )))
            childItem->setIcon( 0, zone->Type() );
        else
            childItem->setIcon( 0, QString("ring.png") );
#if 0
        childItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
        childItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
#endif
        childItem->setObject( const_cast<QObject*>(child) );
        childItem->setObjectName( child->objectName() );
        childItem->setFlags( childItem->flags()^Qt::ItemIsEditable );

        if( NEObject* neObject = qobject_cast<NEObject*>(const_cast<QObject*>(child)))
        {
            NENode* node = qobject_cast<NENode*>(neObject);
            if(neObject->isHidden())
            {
                node = qobject_cast<NENode*> (const_cast<QObject*>(child));
                if( node == nullptr )
                    continue;

                FDYNZoneInspectorItem *categoryItem = nullptr;
                //if(node->factory().nodeInherits() != "Groups")
                {
                    if( NEZoneItem* zoneItem = qobject_cast< NEZoneItem* >(const_cast< QObject* >( child )))
                    {
                        QString keyString = zoneItem->categoryType();
                        FDYNZoneInspectorItem *pp = dynamic_cast<FDYNZoneInspectorItem *>( objItem );
                        while( pp )
                        {
                            keyString += "," + pp->text(0);
                            pp = dynamic_cast<FDYNZoneInspectorItem *>(pp->parent());
                        }

                        if(m_searchText.isEmpty() || containStringinChildren(childItem->Object(), m_searchText) ||
                                containsStringinZone(childItem->Object(), zone, m_searchText))
                        {
                            //std::cout << "Key String Find: " << keyString.toStdString() << std::endl;

                            QMap<QString, FDYNZoneInspectorItem*>::iterator itr = SceneInpectorItems.find(keyString);
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

                            categoryItem->setIcon( 0, QString("ring.png") );
                            categoryItem->addChild( childItem );

                            if(!m_searchText.isEmpty() &&
                                    childItem->Object()->objectName().contains(m_searchText, Qt::CaseInsensitive))
                                m_searchMatchedObjects.push_back(childItem->Object());
                        }
                    }
                }
                //else
                {
                    if(m_searchText.isEmpty() || containStringinChildren(childItem->Object(), m_searchText) || containsStringinZone(childItem->Object(), zone, m_searchText))
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
                    childItem->setText( 1, f->nodeName() );

                    //if(node->factory().nodeInherits() != "Groups")
                    {
                        NENodeIcon icon = *f->nodeIcon();

                        QIcon qicon;
                        for(unsigned i = 0; i < icon.size(); i++)
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
                                if(isWireframe)
                                    childItem->setIcon(1, QString("3d.png"));
                                else
                                    childItem->setIcon(1, QString("3dfill.png"));

                                //
                                // Dont have 3D create icon for cables
                                //
                                if( node->factory().nodeName() == "Cables" )
                                {
                                    childItem->setIcon(1, QString(""));
                                }
                            }
                            else
                            {
                                if( NEZoneItem* zoneItem = qobject_cast<NEZoneItem*>(const_cast<QObject*>(child)))
                                    childItem->setIcon(0, zoneItem->Type());
                                NESpatialMesh *spatial = qobject_cast<NESpatialMesh *>(const_cast<QObject*>(child));
                                if(spatial)
                                {
                                    if(spatial->SpatialMeshType() != NESpatialMesh::None)
                                        childItem->setIcon(1, QString("3dfill.png"));
                                    else
                                        childItem->setIcon(1, QString("3d.png"));
                                }

                                //
                                // Dont have 3D create icon for cables
                                //
                                if( node->factory().nodeName() == "Cables" )
                                {
                                    childItem->setIcon(1, QString(""));
                                }
                            }

                            if (qobject_cast<NEZoneCompartment*>(node))
                                node->SetBehavior(NENode::Active);
                            else if (qobject_cast<NEZone*>(const_cast<QObject*>(obj)) && scenario->itemNameTreeContains(node->objectName(), scenario->objectName()))
                                node->SetBehavior(NENode::Active);
                            else if (!qobject_cast<NEZone*>(const_cast<QObject*>(obj)) && scenario->itemNameTreeContains(node->objectName(), obj->objectName()))
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
                    {
                        if(zone->RenderMode() == NESpatial::SmoothShaded)
                            childItem->setIcon2(1, QString("3dfill.png"));
                        else
                            childItem->setIcon2(1, QString("3d.png"));
                        childItem->setToolTip(0, zone->nodeDescription());
                    }
                    else if(NEScenario *scn = qobject_cast<NEScenario *>(node))
                        childItem->setToolTip(0, scn->nodeDescription());
                    else if(NEZoneItem *zitem = qobject_cast<NEZoneItem *>(node))
                        childItem->setToolTip(0, zitem->nodeDescription());
                }

                // Recursion
                traverseTree(child, zone, scenario, childItem);
            }
        }
        else
            delete childItem;
    }
}

FDYNZoneInspectorItem* FDYNZoneInspector::createOrganizerItem( QString categoryName, FDYNZoneInspectorItem *objItem )
{
    QRegExp rx("\\/");
    QStringList query = categoryName.split(rx);

    FDYNZoneInspectorItem* objOrgItem = objItem;
    FDYNZoneInspectorItem* parentOrgItem = objOrgItem;

    //std::cout << "Name: " << categoryName.toStdString() << std::endl;
    //
    // If there are sub category items separated by / then use the parent as the category name
    //
    QStringList parentquery = query;
    if( parentquery.size() > 1 )
        parentquery.removeLast();

    QString keyParentString = parentquery.last();
    FDYNZoneInspectorItem *pp = dynamic_cast< FDYNZoneInspectorItem *>( objItem );

    while( pp )
    {
        keyParentString += "," + pp->text(0);
        pp = dynamic_cast< FDYNZoneInspectorItem * >( pp->parent() );
    }

    //std::cout << "Finding Parent Item " << keyParentString.toStdString() << std::endl;
    if( SceneInpectorItems.find(keyParentString) != SceneInpectorItems.end() )
    {
        parentOrgItem = SceneInpectorItems.value( keyParentString );
        //std::cout << " -- found " << std::endl;
    }

    QStringList keyStrings ;
    QString ks = query[0];
    FDYNZoneInspectorItem* item = nullptr;
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
        FDYNZoneInspectorItem *pp = dynamic_cast< FDYNZoneInspectorItem *>( objItem );

        while( pp )
        {
            keyString += "," + pp->text(0);
            pp = dynamic_cast< FDYNZoneInspectorItem * >( pp->parent() );
        }

        //std::cout << "  Key Parent Str " << keyParentString.toStdString() << std::endl;
        //std::cout << "  Key Str " << keyString.toStdString() << std::endl;

        if( SceneInpectorItems.find(keyString) != SceneInpectorItems.end() )
        {
            continue;
        }

        item = new FDYNZoneInspectorItem();
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



void FDYNZoneInspector::createContextMenu()
{
    const QList<FDYNTreeItem *>& items = selectedItems();

    //std::cout << "Items " << items[0]->objectName().toStdString() << std::endl;
    // DELETING OLD ACTIONS FROM CONTEXT MENU
    QList<QAction*> currentActions = actions();
    QListIterator<QAction*> i(currentActions);

    while( i.hasNext() )
        removeAction( i.next() );

    //addAction(loadDatabaseAction);
    //addAction(importFRANXAction);

    // Get current (selected) item
    FDYNZoneInspectorItem* item = (FDYNZoneInspectorItem*) currentItem();
    if( items[0] == nullptr )
        return;

    QString sel = items[0]->objectName();

    //std::cout << "Sel " << sel.toStdString() << std::endl;
    if( sel.contains("Trays_", Qt::CaseInsensitive) ||
            sel.contains("Other Raceways_", Qt::CaseInsensitive) ||
            sel.contains("Conduits_", Qt::CaseInsensitive) )
    {
        addAction(addRaceway);
    }
    else if( sel.contains( "Equipments_", Qt::CaseInsensitive) ||
             sel.contains( "Components_", Qt::CaseInsensitive ) )
    {
        addAction(addComponent);
    }
    else if( sel.contains( "Boundaries_", Qt::CaseInsensitive) )
    {
        addAction(addCompAction);
    }
    else if( sel.contains( "Sources_", Qt::CaseInsensitive) )
    {
        addAction(addFireDescAction);
    }
    else if( sel.contains( "Simulation Items", Qt::CaseInsensitive) )
    {
        addAction(addVentAction);
        addAction(addMechVentAction);
    }
    else if( sel.contains( "Physical Only_", Qt::CaseInsensitive) )
    {
        addAction(subItemAction);
    }
    else if( item != nullptr )
    {
        QObject* object = item->Object();

        //std::cout << "Sel2 " << item->objectName().toStdString() << std::endl;

        if(NEObject* neObject =  qobject_cast<NEObject*> (object))
        {
            //populate context menu with actions
            const QList<QObject*>& objectActions = *neObject->actions();
            Q_FOREACH(QObject* action, objectActions)
            {
                QAction *aaction = qobject_cast<QAction *>(action);
                if(aaction->text() == QString("Delete"))
                {
                    //aaction->setText( "Remove" );
                    //addAction(aaction);
                    //removeAction( aaction );
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
                    addAction(aaction);
                }
            }
            if(qobject_cast<NESpatialGroup*>(object))
            {
                //if(!qobject_cast<NESpatialEngine*>(object) &&
                // !qobject_cast<NEZone*>(object) &&
                // !qobject_cast<NEScenario*>(object) &&
                // !qobject_cast<NEZoneItem*>(object))
                //addAction(ungroupAction);
            }

            if(qobject_cast<NESpatialEngine*>(neObject))
            {
                addAction( actionNewZone );
                addAction( actionNewScn );
            }

            if( qobject_cast<NEZone*> (neObject)  )
            {
                addAction(subItemAction);
                addAction(addComponent);
                addAction(addFireDescAction );
                addAction(addCompAction );
                addAction(addVentAction );
                addAction(addMechVentAction );
                addAction(addRaceway );

                addAction(addFireDetectionAction );
            }
            if( NEZoneItem *zitem = qobject_cast<NEZoneItem*> (neObject) )
            {
                if( zitem->factory().nodeName() != "Fire Zone Boundary" )
                {
                    NESpatialEngine* fEngine = qobject_cast<NESpatialEngine*>(m_pRoot);
                    if (fEngine)
                    {
                        NEScenario* activeScenario = qobject_cast<NEScenario*>(NESCENE.findChildNode(fEngine->activeScenarioName()));
                        if( activeScenario )
                        {
                            FDYNZoneInspectorItem* parent = static_cast<FDYNZoneInspectorItem*>(item->parent());
                            // Ignore the category items
                            while (parent != nullptr && parent->Object() == nullptr)
                                parent = static_cast<FDYNZoneInspectorItem*>(parent->parent());

                            if (qobject_cast<NEZone*>(parent->Object()))
                            {
                                if (!activeScenario->itemNameTreeContains(zitem->objectName(), activeScenario->objectName()))
                                    addAction(addToScnAction);
                                else if (activeScenario->itemNameTreeContains(zitem->objectName(), activeScenario->objectName()))
                                    addAction(removeFromScnAction);
                            }
                            else
                            {
                                if (!activeScenario->itemNameTreeContains(zitem->objectName(), parent->Object()->objectName()))
                                    addAction(addToScnAction);
                                else if (activeScenario->itemNameTreeContains(zitem->objectName(), parent->Object()->objectName()))
                                    addAction(removeFromScnAction);
                            }
                        }
                    }
                }

                if( zitem->factory().nodeName() == "Fire Definition" )
                {
                    addAction(simulateNewScns);
                    addAction(convertToItem);
                    addAction(convertToPhyOnly);
                }
                if( zitem->factory().nodeName() == "Fire Plant Item" )
                {
                    if( zitem->categoryType() == "Conduits" ||
                            zitem->categoryType() == "Trays" ||
                            zitem->categoryType() == "OtherRaceways" )
                    {
                        addAction(addCable);
                    }
                    else if( zitem->categoryType() == "Equipments" )
                    {
                        addAction(convertToItem);
                    }
                    else if( zitem->categoryType() == "Physical Only" )
                    {
                        addAction(convertToItem);
                    }
                    else if( zitem->categoryType() == "Components" )
                    {
                        addAction(convertToPhyOnly);
                    }
                    addAction(convertToSource);
                }

                if (zitem->factory().nodeName() == "Fire Mech Vent")
                    addAction(turnIntoVentAction);
                else if (zitem->factory().nodeName() == "Fire Vent")
                    addAction(turnIntoMechVentAction);


                addAction(removeItemAction);

                addAction(duplicateAction);
                addAction(copyPropertiesAction);
                addAction(pastePropertiesAction);
                addAction(pasteDataPropertiesAction);
                addAction(paste3DPropertiesAction);
                addAction(deleteItemAction);
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

///
/// \brief FDYNZoneInspector::deleteSelectedItem
///

void FDYNZoneInspector::deleteSelectedItem()
{
    QMap<NEObject*, NEObject*> selObjs = *NESCENE.selectedObjects();
    //selObjs[ NESCENE.selectedObject() ] = NESCENE.selectedObject();

    Q_FOREACH( NEObject* o, selObjs )
    {
        if( o == NULL )	// Sometimes null objects leaks into this map, this is a hack to omit it
        {

            //selObjs.remove(o);
            continue;
        }

        if( qobject_cast< NEZoneItem *>( o ))
        {
            FDYNGUI.removeItem( qobject_cast< NEZoneItem *>( o ), true );

            //emit FDYNGUI.itemDeleted( qobject_cast< NEZoneItem *>( o ) );
        }
    }
}

void FDYNZoneInspector::removeSelectedItem()
{
    QMap<NEObject*, NEObject*> selObjs = *NESCENE.selectedObjects();
    //selObjs[ NESCENE.selectedObject() ] = NESCENE.selectedObject();

    Q_FOREACH( NEObject* o, selObjs )
    {
        if( o == NULL )	// Sometimes null objects leaks into this map, this is a hack to omit it
        {

            //selObjs.remove(o);
            continue;
        }

        if( NEZoneItem*item = qobject_cast< NEZoneItem *>( o ))
        {
            // Just remove the 3D Reference of this object

            item->SetSpatialMeshType( NESpatialMesh::None );
            item->SetDrawSolidWireframe( false );
            FDYNGUI.removeItem( qobject_cast< NEZoneItem *>( o ), false );
            ////emit FDYNGUI.itemDeleted( qobject_cast< NEZoneItem *>( o ) );

            update();
        }
    }
}


void FDYNZoneInspector::removeSelected3D()
{
    QMap<NEObject*, NEObject*> selObjs = *NESCENE.selectedObjects();

    Q_FOREACH( NEObject* o, selObjs )
    {
        if( o == NULL )	// Sometimes null objects leaks into this map, this is a hack to omit it
        {
            //selObjs.remove(o);
            continue;
        }

        if( NEZoneItem*item = qobject_cast< NEZoneItem *>( o ))
        {
            // Just remove the 3D Reference of this object

            item->SetSpatialMeshType( NESpatialMesh::None );
            item->SetDrawSolidWireframe( false );
            //
            // Send a signal following this to save model item removing the 3D info
            //

            //
            // Update the window
            //
            update();

        }
    }
}

void FDYNZoneInspector::addItemToScenario()
{
    if (NESpatialEngine* fEngine = qobject_cast<NESpatialEngine*>(m_pRoot))
    {
        if (NEScenario* activeScenario = qobject_cast<NEScenario*>(NESCENE.findChildNode(fEngine->activeScenarioName())))
        {
            FDYNZoneInspectorItem* curItem = static_cast<FDYNZoneInspectorItem*>(currentItem());
            FDYNZoneInspectorItem* curParent = static_cast<FDYNZoneInspectorItem*>(curItem->parent());

            QList<std::pair<QObject*, QObject*>> childParentPairs;

            // Ignore the category items
            while (curParent != nullptr && curParent->Object() == nullptr)
                curParent = static_cast<FDYNZoneInspectorItem*>(curParent->parent());

            // Compute the child-parent pairs along the path to the tree root without the category items
            if (curParent != nullptr)
            {
                FDYNZoneInspectorItem* item = curParent;
                FDYNZoneInspectorItem* parent = static_cast<FDYNZoneInspectorItem*>(item->parent());

                while (parent != nullptr)
                {
                    // Ignore the category items
                    while (parent != nullptr && parent->Object() == nullptr)
                        parent = static_cast<FDYNZoneInspectorItem*>(parent->parent());

                    if (parent == nullptr || parent->Object() == nullptr || parent->Object() == m_pRoot)
                        break;

                    if (qobject_cast<NEZone*>(parent->Object()))
                    {
                        if (!activeScenario->itemNameTreeContains(item->Object()->objectName(), activeScenario->objectName()))
                            childParentPairs.push_back(std::pair<QObject*, QObject*>(item->Object(), parent->Object()));
                    }
                    else
                    {
                        if (!activeScenario->itemNameTreeContains(item->Object()->objectName(), parent->Object()->objectName()))
                            childParentPairs.push_back(std::pair<QObject*, QObject*>(item->Object(), parent->Object()));
                    }

                    item = parent;
                    parent = static_cast<FDYNZoneInspectorItem*>(item->parent());
                }
            }

            if (curParent != nullptr && curParent->Object() != nullptr)
            {
                if (qobject_cast<NEZone*>(curParent->Object()))
                {
                    if (!activeScenario->itemNameTreeContains(curItem->Object()->objectName(), activeScenario->objectName()))
                        childParentPairs.push_back(std::pair<QObject*, QObject*>(curItem->Object(), curParent->Object()));
                }
                else
                {
                    if (!activeScenario->itemNameTreeContains(curItem->Object()->objectName(), curParent->Object()->objectName()))
                        childParentPairs.push_back(std::pair<QObject*, QObject*>(curItem->Object(), curParent->Object()));
                }
            }

            // Compute the child-parent pairs in the subtree without the category items
            QList<FDYNZoneInspectorItem*> itemsToProcess;
            itemsToProcess.push_back(curItem);
            while (itemsToProcess.size() > 0)
            {
                FDYNZoneInspectorItem* item = itemsToProcess.front();
                FDYNZoneInspectorItem* parent = item;

                // Ignore the category items
                while (parent != nullptr && parent->Object() == nullptr)
                    parent = static_cast<FDYNZoneInspectorItem*>(parent->parent());

                for (FDYNTreeItem* child : item->takeChildren())
                {
                    if (FDYNZoneInspectorItem* zoneChild = qobject_cast<FDYNZoneInspectorItem*>(child))
                    {
                        if (zoneChild->Object() != nullptr)
                        {
                            if (qobject_cast<NEZone*>(parent->Object()))
                            {
                                if (!activeScenario->itemNameTreeContains(zoneChild->Object()->objectName(), activeScenario->objectName()))
                                    childParentPairs.push_back(std::pair<QObject*, QObject*>(zoneChild->Object(), parent->Object()));
                            }
                            else
                            {
                                if (!activeScenario->itemNameTreeContains(zoneChild->Object()->objectName(), parent->Object()->objectName()))
                                    childParentPairs.push_back(std::pair<QObject*, QObject*>(zoneChild->Object(), parent->Object()));
                            }
                        }
                        itemsToProcess.push_back(zoneChild);
                    }
                    //child->setIcon(0, QString("active.png"));
                }

                itemsToProcess.pop_front();
            }

            FDYNGUI.addItemsToScenario(childParentPairs);

            //
            // Just need to refresh icon state and object state to active
            //

            //update();
            //FDYNGUI.updateNodeConnectionsTable(); //Update node connection table of connection inspector
        }
    }
}

void FDYNZoneInspector::removeItemFromScenario()
{
    if (NESpatialEngine* fEngine = qobject_cast<NESpatialEngine*>(m_pRoot))
        if (NEScenario* activeScenario = qobject_cast<NEScenario*>(NESCENE.findChildNode(fEngine->activeScenarioName())))
        {
            FDYNZoneInspectorItem* curItem = static_cast<FDYNZoneInspectorItem*>(currentItem());
            FDYNZoneInspectorItem* curParent = static_cast<FDYNZoneInspectorItem*>(curItem->parent());

            QList<std::pair<QObject*, QObject*>> childParentPairs;

            // Ignore the category items
            while (curParent != nullptr && curParent->Object() == nullptr)
                curParent = static_cast<FDYNZoneInspectorItem*>(curParent->parent());

            if (curParent != nullptr && curParent->Object() != nullptr)
            {
                if (qobject_cast<NEZone*>(curParent->Object()))
                {
                    if (activeScenario->itemNameTreeContains(curItem->Object()->objectName(), activeScenario->objectName()))
                        childParentPairs.push_back(std::pair<QObject*, QObject*>(curItem->Object(), curParent->Object()));
                }
                else
                {
                    if (activeScenario->itemNameTreeContains(curItem->Object()->objectName(), curParent->Object()->objectName()))
                        childParentPairs.push_back(std::pair<QObject*, QObject*>(curItem->Object(), curParent->Object()));
                }
            }

            // Compute the child-parent pairs in the subtree without the category items
            QList<FDYNZoneInspectorItem*> itemsToProcess;
            itemsToProcess.push_back(curItem);
            while (itemsToProcess.size() > 0)
            {
                FDYNZoneInspectorItem* item = itemsToProcess.front();
                FDYNZoneInspectorItem* parent = item;

                // Ignore the category items
                while (parent != nullptr && parent->Object() == nullptr)
                    parent = static_cast<FDYNZoneInspectorItem*>(parent->parent());

                for (FDYNTreeItem* child : item->takeChildren())
                {
                    if (FDYNZoneInspectorItem* zoneChild = qobject_cast<FDYNZoneInspectorItem*>(child))
                    {
                        if (zoneChild->Object() != nullptr)
                        {
                            if (qobject_cast<NEZone*>(parent->Object()))
                            {
                                if (activeScenario->itemNameTreeContains(zoneChild->Object()->objectName(), activeScenario->objectName()))
                                    childParentPairs.push_back(std::pair<QObject*, QObject*>(zoneChild->Object(), parent->Object()));
                            }
                            else
                            {
                                if (activeScenario->itemNameTreeContains(zoneChild->Object()->objectName(), parent->Object()->objectName()))
                                    childParentPairs.push_back(std::pair<QObject*, QObject*>(zoneChild->Object(), parent->Object()));
                            }
                        }
                        itemsToProcess.push_back(zoneChild);
                    }
                }

                itemsToProcess.pop_front();
            }

            FDYNGUI.removeItemsFromScenario(childParentPairs);

            //
            // Just need to refresh icon state and object state to inactive
            //
            //update();
            //FDYNGUI.updateNodeConnectionsTable(); //Update node connection table of connection inspector
        }
}

void FDYNZoneInspector::addItemToTree(const QObject* objItem)
{
    const QList<FDYNTreeItem *>& items = selectedItems();
    FDYNZoneInspectorItem* selectedItem = static_cast<FDYNZoneInspectorItem*>( items[0] );

    if( !selectedItem )
        return;

    FDYNZoneInspectorItem* item = new FDYNZoneInspectorItem();
    item->setText( 0, objItem->objectName() );
    item->setText( 1, objItem->metaObject()->className() );
    if (NEZone* zone = qobject_cast<NEZone*>(const_cast<QObject*>(objItem)))
        item->setIcon( 0, zone->Type() );
    else
        item->setIcon( 0, QString("ring.png") );

    item->setObject( const_cast<QObject*>(objItem) );
    item->setObjectName(objItem->objectName());
    item->setFlags(item->flags()^Qt::ItemIsEditable);

    FDYNZoneInspectorItem* categoryItem = nullptr;

    if (NENode* node = qobject_cast<NENode*>(const_cast<QObject*>(objItem)))
    {
        //if(node->factory().nodeInherits() != "Groups")
        {
            if (NEZoneItem* zoneItem = qobject_cast<NEZoneItem*>(node))
            {
                QString keyString = zoneItem->categoryType();
                FDYNZoneInspectorItem *parentItem = dynamic_cast<FDYNZoneInspectorItem *>( selectedItem );
                //
                // The following check is to see if the item selected is a category or a regular item
                // If Category - we need to start with parent as the key string
                //
                if (parentItem != nullptr && parentItem->Object() == nullptr && parentItem->parent() )
                    parentItem = dynamic_cast<FDYNZoneInspectorItem *>( parentItem->parent() );

                while( parentItem )
                {
                    keyString += "," + parentItem->text(0);
                    parentItem = dynamic_cast<FDYNZoneInspectorItem *>(parentItem->parent());
                }

                QMap<QString, FDYNZoneInspectorItem*>::iterator itr = SceneInpectorItems.find(keyString) ;
                if (itr == SceneInpectorItems.end())
                {
                    categoryItem = createOrganizerItem( zoneItem->categoryType(), selectedItem );
                    SceneInpectorItems.insert( keyString, categoryItem );
                }
                else
                    categoryItem = itr.value();

                //std::cout << "key string " << keyString.toStdString() << std::endl;

                categoryItem->setIcon(0, QString("ring.png"));
                categoryItem->addChild(item);
            }
        }
        //else
        //    selectedItem->addChild(item);

        NENodeFactory* f = &(node->factory());
        item->setText( 1, f->nodeName() );

        //if (node->factory().nodeInherits() != "Groups")
        {
            QIcon qicon;
            for (NEIcon& icon : *f->nodeIcon())
                qicon.addFile(icon.iconPath, icon.iconSize);
            item->setIcon( 1, qicon );

            if (NEZoneItem* zoneItem = qobject_cast<NEZoneItem*>(node))
                item->setIcon(0, zoneItem->Type());

            if (node->factory().nodeName() == "Cables")
                item->setIcon(1, QString(""));
            else
                item->setIcon(1, QString("3d.png"));

            if (qobject_cast<NEZoneCompartment*>(node))
            {
                node->SetBehavior(NENode::Active);
                item->setIcon2(1, QString("alert_active.png"));
            }
            else
            {
                node->SetBehavior(NENode::Inactive);
                item->setIcon2(1, QString("alert.png"));
            }
        }

        if (NEZone* zone = qobject_cast<NEZone*>(node))
        {
            if(zone->RenderMode() == NESpatial::SmoothShaded)
                item->setIcon2(1, QString("3dfill.png"));
            else
                item->setIcon2(1, QString("3d.png"));
            item->setToolTip(0, zone->nodeDescription());
        }
        else if (NEScenario* scn = qobject_cast<NEScenario*>(node))
            item->setToolTip(0, scn->nodeDescription());
        else if (NEZoneItem* zitem = qobject_cast<NEZoneItem*>(node))
            item->setToolTip(0, zitem->nodeDescription());
    }
    else
        delete item;

    QModelIndex parentindex = model()->indexFromItem( categoryItem, 0);

    QModelIndex first = model()->index( 0, 0, parentindex );
    QModelIndex last = model()->index( model()->rowCount( parentindex ), model()->columnCount( parentindex ), parentindex );

    std::cout << "Updating " << first.row() << " " << first.column() << " to " << last.row() << " " << last.column() << std::endl;
    emit model()->dataChanged( first, last );
//    FDYNTreeWidget::update();
//    restoreExpandedState();
}

void FDYNZoneInspector::displayItemAsActive(const QObject* objItem)
{
    FDYNZoneInspectorItem* selectedItem = static_cast<FDYNZoneInspectorItem*>(currentItem());

    if (NENode* node = qobject_cast<NENode*>(const_cast<QObject*>(objItem)))
    {
        node->SetBehavior(NENode::Active);
        selectedItem->setIcon2(1, QString("alert_active.png"));
    }
}

bool FDYNZoneInspector::isSelectedObect(QObject* object)
{
    const FDYNZoneInspectorItem* item = (FDYNZoneInspectorItem*) currentItem();
    if(item)
        return (object == item->Object());
    else
        return false;
}

void FDYNZoneInspector::selectItems()
{
    const QList<FDYNTreeItem *>& items = selectedItems();

    if(items.size() == 1)
    {
        if(FDYNZoneInspectorItem* sItem =  qobject_cast<FDYNZoneInspectorItem*>(items.first()))
        {
           if(NENode* casted = qobject_cast<NENode*>(sItem->Object()))
           {
               FDYNGUI.selectObject(casted);
           }
        }
    }
    else if(items.size() > 1)
    {
        QList<NENode*> objects;
        Q_FOREACH(FDYNTreeItem* item2, items)
        {
            if(FDYNZoneInspectorItem* sItem =  qobject_cast<FDYNZoneInspectorItem*>(item2))
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

void FDYNZoneInspector::renameSelected()
{
    QVariant returnValue;
    QMetaObject::invokeMethod( currentTextInput, "setActiveFocusFromCpp", Q_RETURN_ARG(QVariant, returnValue));
}

void FDYNZoneInspector::slotDropDownItemChanged(QString newZone)
{
    emit FDYNGUI.setActiveZone( newZone );
    return;
}

void FDYNZoneInspector::slotOtherItemChanged(QString newZone)
{
    emit FDYNGUI.setActiveZone( newZone );
    m_otherComboObject->setProperty("typedString", QString(""));
    return;
}

bool isSameSide(vec3f p1, vec3f p2, vec3f a, vec3f b){
    vec3f t1 = vec3f::crossProduct(b-a, p1-a);
    vec3f t2 = vec3f::crossProduct(b-a, p2-a);

    if (vec3f::dotProduct(t1, t2) >= 0)
        return true;
    else
        return false;
}

bool pointInsideTriangle(vec3f p, vec3f x1, vec3f x2, vec3f x3)
{
    if( isSameSide(p, x1, x2, x3) && isSameSide(p, x2, x1, x3) && isSameSide(p, x3, x1, x2))
        return true;
    else
        return false;

}

void FDYNZoneInspector::runDrop(matrix44f view, matrix44f proj, double x, double y, double w, double h, void *tItem)
{
    //qDebug() << x << y << tItem;
    if(!tItem )
        return;

    if(FDYNZoneInspectorItem *ziItem = static_cast<FDYNZoneInspectorItem *>(tItem))
    {
        if(!ziItem->Object())
            return;

        if(NEZoneItem *zItem = qobject_cast<NEZoneItem*>(ziItem->Object()))
        {
            if( zItem->factory().nodeName() == "Fire Vent" || zItem->factory().nodeName() == "Fire Mech Vent" )
            {
                NEZoneItem *vent = zItem;

                matrix44f parentTransform;
                parentTransform.setToIdentity();
                if(NESpatial *parent = qobject_cast<NESpatial*>(zItem->parent()))
                    parentTransform = parent->transform();

                matrix44f finalMat = proj * view * parentTransform;
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

                NEZone *zone = NULL;

                QObject *parent = vent->parent();
                while( parent )
                {
                    if( NEZone *z = qobject_cast<NEZone *>(parent) )
                    {
                        zone = z;
                        break;
                    }
                    parent = parent->parent();
                }


                QMap< NEZoneCompartment*, QMap<int, vec3f> > intersects;

                QList<NEZoneCompartment *> compartments = zone->findChildren<NEZoneCompartment *>();
                foreach (NEZoneCompartment *compartment, compartments)
                {
                    NEIndexedTriMesh &triMesh = compartment->triangleMesh();
                    std::vector<NEPrimitivePolygon> &faces = compartment->quadFaces();

                    QMap<int, vec3f> intersectsInZoneCompartment;

                    for( unsigned int i = 0; i < faces.size(); i++)
                    {
                        NEPrimitivePolygon face = faces[i];

                        for( unsigned int f = 0; f < face.triangles.size(); f++)
                        {
                            vec3f tp1 = compartment->transform() * triMesh.vertices()[face.triangles[f].p1].position;
                            vec3f tp2 = compartment->transform() * triMesh.vertices()[face.triangles[f].p2].position;
                            vec3f tp3 = compartment->transform() * triMesh.vertices()[face.triangles[f].p3].position;

                            vec3f norPlane = vec3f::normal(tp1, tp2, tp3);

                            vec3f interPoint = linePlaneInterPoint(p1, dirLine, tp1, norPlane);

                            if (!std::isnan(interPoint.x()) && !std::isnan(interPoint.y()) && !std::isnan(interPoint.z()))
                                if( pointInsideTriangle(interPoint, tp1, tp2, tp3))
                                {
                                    intersectsInZoneCompartment[i] = interPoint;
                                    break;
                                }
                        }
                    }
                    intersects[compartment] = intersectsInZoneCompartment;
                }

                float distance = FLT_MAX;
                vec3f minPoint;
                NEZoneCompartment *curCompart = NULL;
                int curFaceId = -1;

                foreach (NEZoneCompartment *compartment, intersects.keys())
                {

                    foreach (vec3f interPoint, intersects[compartment].values())
                    {
                        vec3f viewInterPos = view * interPoint;
                        if( vec3f::dotProduct(viewInterPos, vec3f(0, 0, -1)) > 0)
                        {
                            if(distance > viewInterPos.length())
                            {
                                minPoint = interPoint;
                                distance = viewInterPos.length();
                                curCompart = compartment;
                                curFaceId = intersects[compartment].key(interPoint);
                            }
                        }
                    }
                }

                if(distance < FLT_MAX && curFaceId != -1)
                {
                    zItem->SetSpatialMeshType(NESpatialMesh::Cuboid);
                    zItem->SetPosition(minPoint);

                    std::vector<NEPrimitivePolygon> &faces = curCompart->quadFaces();
                    NEIndexedTriMesh &triMesh = curCompart->triangleMesh();

                    vec3f tp1 = curCompart->transform() * triMesh.vertices()[faces[curFaceId].triangles[0].p1].position;
                    vec3f tp2 = curCompart->transform() * triMesh.vertices()[faces[curFaceId].triangles[0].p2].position;
                    vec3f tp3 = curCompart->transform() * triMesh.vertices()[faces[curFaceId].triangles[0].p3].position;
                    vec3d planeNormal = vec3d::normal(vec3d(tp1), vec3d(tp2), vec3d(tp3));

                    eulerf oldOrientation = vent->Orientation();

                    matrix44d ventTransform = matrix44d(vent->transform());
                    vec3d ventDirVec = vec3d(ventTransform*vec4d(0.0, 1.0, 0.0, 0.0));
                    vec3d rotAxis = vec3d::crossProduct(ventDirVec, planeNormal);
                    if (std::abs(rotAxis.x()) <= FEps)
                        rotAxis.x() = 0.0;
                    if (std::abs(rotAxis.y()) <= FEps)
                        rotAxis.y() = 0.0;
                    if (std::abs(rotAxis.z()) <= FEps)
                        rotAxis.z() = 0.0;

                    // Rotate the vent if the vent thickness direction and plane normal are not colinear
                    if (std::abs(rotAxis.x()) > FEps || std::abs(rotAxis.y()) > FEps || std::abs(rotAxis.z()) > FEps)
                    {
                        rotAxis.normalize();
                        double angle = std::acos(ventDirVec.normalized()*planeNormal);
                        constexpr double radToDeg = 180.0/M_PI;
                        vent->SetOrientation(oldOrientation+eulerAnglesFromQuaternion(quatf::fromAxisAndAngle(vec3f(rotAxis), radToDeg*angle)));

                        // Test rotating along one of the basis axis if the rotational axis is somehow not right
                        ventTransform = matrix44d(vent->transform());
                        ventDirVec = vec3d(ventTransform*vec4d(0.0, 1.0, 0.0, 0.0));
                        rotAxis = vec3d::crossProduct(ventDirVec, planeNormal);
                        if (std::abs(rotAxis.x()) <= FEps)
                            rotAxis.x() = 0.0;
                        if (std::abs(rotAxis.y()) <= FEps)
                            rotAxis.y() = 0.0;
                        if (std::abs(rotAxis.z()) <= FEps)
                            rotAxis.z() = 0.0;

                        // Test rotating along the x-axis
                        if (std::abs(rotAxis.x()) > FEps || std::abs(rotAxis.y()) > FEps || std::abs(rotAxis.z()) > FEps)
                        {
                            vec3f axisX(1.0, 0.0, 0.0);
                            vent->SetOrientation(oldOrientation+eulerAnglesFromQuaternion(quatf::fromAxisAndAngle(axisX, radToDeg*angle)));
                        }

                        ventTransform = matrix44d(vent->transform());
                        ventDirVec = vec3d(ventTransform*vec4d(0.0, 1.0, 0.0, 0.0));
                        rotAxis = vec3d::crossProduct(ventDirVec, planeNormal);
                        if (std::abs(rotAxis.x()) <= FEps)
                            rotAxis.x() = 0.0;
                        if (std::abs(rotAxis.y()) <= FEps)
                            rotAxis.y() = 0.0;
                        if (std::abs(rotAxis.z()) <= FEps)
                            rotAxis.z() = 0.0;

                        // Test rotating along the y-axis
                        if (std::abs(rotAxis.x()) > FEps || std::abs(rotAxis.y()) > FEps || std::abs(rotAxis.z()) > FEps)
                        {
                            vec3f axisY(0.0, 1.0, 0.0);
                            vent->SetOrientation(oldOrientation+eulerAnglesFromQuaternion(quatf::fromAxisAndAngle(axisY, radToDeg*angle)));
                        }

                        ventTransform = matrix44d(vent->transform());
                        ventDirVec = vec3d(ventTransform*vec4d(0.0, 1.0, 0.0, 0.0));
                        rotAxis = vec3d::crossProduct(ventDirVec, planeNormal);
                        if (std::abs(rotAxis.x()) <= FEps)
                            rotAxis.x() = 0.0;
                        if (std::abs(rotAxis.y()) <= FEps)
                            rotAxis.y() = 0.0;
                        if (std::abs(rotAxis.z()) <= FEps)
                            rotAxis.z() = 0.0;

                        // Test rotating along the z-axis
                        if (std::abs(rotAxis.x()) > FEps || std::abs(rotAxis.y()) > FEps || std::abs(rotAxis.z()) > FEps)
                        {
                            vec3f axisZ(0.0, 0.0, 1.0);
                            vent->SetOrientation(oldOrientation+eulerAnglesFromQuaternion(quatf::fromAxisAndAngle(axisZ, radToDeg*angle)));
                        }
                    }
                }
            }
            else
            {
                zItem->SetSpatialMeshType(NESpatialMesh::Cuboid);
                zItem->SetDrawSolidWireframe( true );

                matrix44f parentTransform;
                parentTransform.setToIdentity();
                if(NESpatial *parent = qobject_cast<NESpatial*>(zItem->parent()))
                    parentTransform = parent->transform();

                matrix44f finalMat = proj * view * parentTransform;
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

                // Make that the item bottom is at zero-elevation
                if (!std::isnan(interPoint.x()) && !std::isnan(interPoint.y()) && !std::isnan(interPoint.z()))
                    zItem->SetPosition(interPoint+vec3f(0.0, 0.5*zItem->Scale().y(), 0.0));
            }


            ziItem->setIcon(1, QString("3dfill.png"));

            FDYNGUI.updateGLWindows();
        }
        else if(NEZone *zone = qobject_cast<NEZone*>(ziItem->Object()))
        {
            FDYNGUI.selectObject( zone );
            if(NEZoneCompartment* zc = qobject_cast< NEZoneCompartment *>( FDYNGUI.addCompartment() ))
            {
                matrix44f parentTransform;
                parentTransform.setToIdentity();
                if(NESpatial *parent = qobject_cast<NESpatial*>(zc->parent()))
                    parentTransform = parent->transform();

                matrix44f finalMat = proj * view * parentTransform;
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
                    zc->SetPosition(interPoint+vec3f(0.0, 0.5*zc->Scale().y(), 0.0));

                emit FDYNGUI.itemCreated( zc, zone );
                //FDYNGUI.updateSceneInspector();
                ziItem->setIcon(1, QString("3dfill.png"));
                FDYNGUI.updateGLWindows();
                //FDYNGUI.updatePropertyEditor();

                QList<NEZoneCompartment *> zcList;

                foreach (QObject *obj, zone->children())
                {
                    if(NEZoneCompartment *zc = qobject_cast<NEZoneCompartment *>(obj) )
                        zcList.append(zc);
                }
            }
        }
    }
}

void FDYNZoneInspector::nodeRenamed(FDYNTreeItem *item, int column)
{
    FDYNZoneInspectorItem* sItem = qobject_cast<FDYNZoneInspectorItem*>(item);

    if(!sItem || !sItem->Object() || sItem->Object()->objectName() == sItem->text(0))
        return;

    //qDebug() << "object name is " << sItem->Object()->objectName() << ", item text is " <<  sItem->text(0);

    NENode* node = qobject_cast<NENode*>(sItem->Object());

    FDYNGUI.renameNode( node, sItem->text(0) );
    FDYNGUI.updateInspectors();
}

void FDYNZoneInspector::itemExpansion(FDYNTreeItem *item)
{
    FDYNZoneInspectorItem* sItem = static_cast<FDYNZoneInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = true;
    else
        m_ExpNodes[sItem->text(0)] = true;
}

void FDYNZoneInspector::itemCollapse(FDYNTreeItem *item)
{
    FDYNZoneInspectorItem* sItem = static_cast<FDYNZoneInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = false;
    else
        m_ExpNodes[sItem->text(0)] = false;
}

void FDYNZoneInspector::itemActivation(FDYNTreeItem *item, int column)
{
    //topLeft = this->indexFromItem(item);
    //    m_LastSelectedItem = selectionModel()->selection();
    //    m_LastSelectedItem.select(topLeft, topLeft);

}

void FDYNZoneInspector::restoreExpandedState()
{
    if(topLevelItemCount())
    {
        Q_FOREACH(FDYNTreeItem* it , items())
        {
            FDYNZoneInspectorItem* item = qobject_cast<FDYNZoneInspectorItem*>(it);
            if(item->Object())
            {
                bool expState = true;
                if(m_ExpNodes.find(item->Object()->objectName()) != m_ExpNodes.end())
                {
                    expState = m_ExpNodes[item->Object()->objectName()];
                }
                if(expState)
                    this->expandItem(item);
                else
                    this->collapseItem(item);
            }
            else
            {
                bool expState = false;
                if(m_ExpNodes.find(item->text(0)) != m_ExpNodes.end())
                {
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

void FDYNZoneInspector::clearState()
{
    SceneInpectorItems.clear();
    SceneItem=0;
}


/*!
 If we would like to make the parent of the deleted instance the current object this method should be called.
 However, than we should also be able to keep the selected/current item when updating the qtreewidget.
 This is not possible yet.
*/
void FDYNZoneInspector::setParentCurrentItem()
{	
    FDYNZoneInspectorItem* item = (FDYNZoneInspectorItem*) currentItem();
    FDYNZoneInspectorItem* parent = (FDYNZoneInspectorItem*)item->parent();
    setCurrentItem(parent);
    FDYNGUI.selectObject(parent->Object());
}

void FDYNZoneInspector::setNewCurrentItem(QObject* object)
{
    if(object)
    {
        QList<FDYNTreeItem *> items = findItems(object->objectName(), Qt::MatchRecursive);
        if( items.size() >= 2)
        {
            foreach (FDYNTreeItem *item, items)
            {
                if( -1 != m_selectedItems.indexOf(item))
                {
                    FDYNZoneInspectorItem* zitem = (FDYNZoneInspectorItem*) (item);
                    setCurrentItem(zitem);
                    break;
                }
            }
        }
        else
        {
            QListIterator<FDYNTreeItem *> i(items);
            if( i.hasNext())
            {
                if( FDYNZoneInspectorItem *zitem = qobject_cast<FDYNZoneInspectorItem *>(i.next()))
                    setCurrentItem(zitem);
            }
        }
    }
    else
    {
        //update();
        setCurrentItem(NULL);
    }
}

void FDYNZoneInspector::setCurrentItems(QList<NENode*> objList)
{
    foreach( NENode* object, objList )
    {
        if(object)
        {
            QList<FDYNTreeItem *> items = findItems(object->objectName(), Qt::MatchRecursive);
            if( items.size() >= 2)
            {
                foreach (FDYNTreeItem *item, items)
                {
                    if( -1 != m_selectedItems.indexOf(item))
                    {
                        FDYNZoneInspectorItem* zitem = (FDYNZoneInspectorItem*) (item);
                        zitem->setSelected(true);
                        break;
                    }
                }
            }
            else
            {
                QListIterator<FDYNTreeItem *> i(items);
                if( i.hasNext())
                {
                    if( FDYNZoneInspectorItem *zitem = qobject_cast<FDYNZoneInspectorItem *>(i.next()))
                        zitem->setSelected(true);
                }
            }
        }
    }
}

void FDYNZoneInspector::saveGUIStates(QXmlStreamWriter *writer)
{

    writer->writeStartElement("expandednodes");

    if(topLevelItemCount())
    {
        Q_FOREACH(FDYNTreeItem* it , items())
        {
            FDYNZoneInspectorItem* item = qobject_cast<FDYNZoneInspectorItem*>(it);
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

void FDYNZoneInspector::readGUIStates(QXmlStreamReader *reader)
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

void FDYNZoneInspector::updateOtherZones( QList< QString >& otherzones)
{
    NESpatialEngine *fEngine = qobject_cast<NESpatialEngine *>(m_pRoot);
    if(!fEngine)
        return;
    QString curZone = fEngine->activeZoneName();

    clearDropDownItems();

    for( int i = 0; i < otherzones.size(); ++i )
    {
        addDropDownItem( otherzones[i] );

        if(curZone == otherzones[i])
            m_dropDownObject->setProperty("currentIndex", i);
    }
}

void FDYNZoneInspector::updateAllZones(QList<QString> allZones)
{
    NESpatialEngine *fEngine = qobject_cast<NESpatialEngine *>(m_pRoot);
    if(!fEngine)
        return;
    QString curZone = fEngine->activeZoneName();

    clearOtherListItems();
    for( int i = 0; i < allZones.size(); ++i )
    {
        addOtherListItem( allZones[i] );
    }

    for( int i = 0; i < m_dropDownList.size(); i++){
        if(curZone == m_dropDownList[i])
            m_dropDownObject->setProperty("currentIndex", i);
    }
}

void FDYNZoneInspector::positionToSelected()
{
    //expandItem( m_currentItem->parent() );
    QVariant returnValue;
    QMetaObject::invokeMethod(treeview, "itemAdded", Q_RETURN_ARG(QVariant, returnValue));
}
