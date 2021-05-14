#include "Core/Base/NEBase.h"
#include "Core/NEObject.h"
#include "Core/NENode.h"
#include "Core/Spatial/NESpatialGroup.h"
#include "Core/Base/NESceneManager.h"
#include "Managers/NEGUIManager.h"
#include "Utilities/NEGUIStyle.h"
#include "NETreeInspector.h"


class NETreeInspectorItem : public QTreeWidgetItem
{
public:
    NETreeInspectorItem() : QTreeWidgetItem(), m_pObject(0) {}
    ~NETreeInspectorItem() {}

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



NETreeInspector::NETreeInspector(QWidget* parent) : QTreeWidget(parent)
{
    clearState();
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
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

    connect(this, &QTreeWidget::itemExpanded, this, &NETreeInspector::itemExpansion);
    connect(this, &QTreeWidget::itemCollapsed, this, &NETreeInspector::itemCollapse);
    connect(this, &QTreeWidget::itemClicked, this, &NETreeInspector::itemActivation);

    groupAction = new QAction("Group", 0);
    ungroupAction = new QAction("Ungroup", 0);
    connect(groupAction, &QAction::triggered, this, &NETreeInspector::groupNodes);
    connect(ungroupAction, &QAction::triggered, this, &NETreeInspector::ungroupNodes);

    parentAction = new QAction("Parent", 0);
    unparentAction = new QAction("Unparent", 0);
    connect(parentAction, &QAction::triggered, this, &NETreeInspector::parentNodes);
    connect(unparentAction, &QAction::triggered, this, &NETreeInspector::unparentNodes);

    //m_pBrush
}

NETreeInspector::~NETreeInspector()
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

void NETreeInspector::setRoot( QObject* root)
{
    m_pRoot = root;
}

void NETreeInspector::update()
{
    this->clear();
    // Clear the pointers before rebuilding the contents
    clearState();
    Q_ASSERT_X(m_pRoot, " NETreeInspector::update", "root pointer is null");

    const QObject* child = m_pRoot;
    NETreeInspectorItem* tmpItem = new NETreeInspectorItem();
    tmpItem->setForeground(0, *NEGUISTYLE.NormalFontBrush());
    tmpItem->setFont(0, *NEGUISTYLE.BoldFont());
    tmpItem->setText(0, child->objectName());
    tmpItem->setText(1, "Scene Manager");
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
    // QModelIndex idx = this->currentIndex().child(m_LastSelectedItemIndex, 0);

    //QItemSelection *selection = new QItemSelection(topLeft, topLeft);
    //QModelIndex mIndex =  QAbstractItemModel::createIndex((m_LastSelectedItemIndex, 0,
    //   setCurrentIndex(idx);
    //selectionModel()->select(*selection, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void NETreeInspector::traverseTree(const QObject* obj, NETreeInspectorItem* parent)
{
    const QObjectList& objList = obj->children();
    Q_FOREACH(const QObject* child, objList)
    {
        NETreeInspectorItem* tmpItem = new NETreeInspectorItem();
        tmpItem->setText(0, child->objectName());
        tmpItem->setText(1, child->metaObject()->className());
        tmpItem->setFont(1, *NEGUISTYLE.NormalSmallFont());
        tmpItem->setTextColor(1, NEGUISTYLE.GrayFontBrush()->color());
        tmpItem->setObject(const_cast<QObject*>(child));
        tmpItem->setFlags(tmpItem->flags()^Qt::ItemIsEditable);
        //tmpItem->setExpanded(true);



        if(NEObject* neObject = qobject_cast<NEObject*>(const_cast<QObject*>(child)))
        {
            NENode* node = qobject_cast<NENode*>(neObject);
            if(neObject->isHidden())
            {	//Skip grouping if the object has a parent except NESceneManager
                if(neObject->parent() != &NESCENE && neObject->parent() != 0)
                {
                    parent->addChild(tmpItem);
                    //tmpItem->setExpanded(true);
                    if(node)
                    {
                        tmpItem->setText(1, node->factory().nodeName());
                    }
                }
                else
                {
                    node = qobject_cast<NENode*> (const_cast<QObject*>(child));
                    if( node == NULL )
                        continue;
                    if( node->factory().objectName() == "Camera" )
                    {
                        if(!Cameras)
                            Cameras = createOrganizerItem("Cameras");

                        Cameras->addChild(tmpItem);
                    }
                    else if( node->factory().nodeInherits() == "NELight" )
                    {
                        if(!Lights)
                            Lights = createOrganizerItem("Lights");

                        Lights->addChild(tmpItem);
                    }
                    else if(node->factory().nodeInherits() == "NEMaterial" )
                    {
                        if(!Materials)
                            Materials = createOrganizerItem("Materials");

                        Materials->addChild(tmpItem);
                    }
                    else if(node->factory().nodeInherits() == "NEOSDBase" ||
                            node->factory().objectName() == "ClipPlane" )
                    {
                        if(!OSDs)
                            OSDs = createOrganizerItem("OSD");

                        OSDs->addChild(tmpItem);
                    }
                    else if(node->factory().nodeInherits() == "NESphLiquidSolver" )
                    {
                        if(!FluidSolvers)
                            FluidSolvers = createOrganizerItem("Particle Fluid Solvers");

                        FluidSolvers->addChild(tmpItem);
                    }
                    else if(node->factory().nodeName() == "Fluid" )
                    {
                        if(!Fluids)
                            Fluids = createOrganizerItem("Fluids");

                        Fluids->addChild(tmpItem);
                    }
                    else if(node->factory().nodeInherits() == "NEParticleSource" )
                    {
                        if(!ParticleSources)
                            ParticleSources = createOrganizerItem("Particle Sources");

                        ParticleSources->addChild(tmpItem);
                    }
                    else if(node->factory().nodeInherits() == "NEParticleKiller" )
                    {
                        if(!KillerFields)
                            KillerFields = createOrganizerItem("Particle Killers");

                        KillerFields->addChild(tmpItem);
                    }
                    else if(node->factory().nodeInherits() == "NECoupler" ||
                            node->factory().nodeInherits() == "NEDualBoundary" )
                    {

                        if(!DualBoundaries)
                            DualBoundaries = createOrganizerItem("Dual Boundaries");

                        DualBoundaries->addChild(tmpItem);
                    }
                    else if(node->factory().nodeInherits() == "NEForceField" )
                    {

                        if(!ForceFields)
                            ForceFields = createOrganizerItem("Force Fields");

                        ForceFields->addChild(tmpItem);
                    }
                    else if(node->factory().nodeInherits() == "NESpatialMesh" )
                    {
                        if(!Meshes)
                            Meshes = createOrganizerItem("Spatial Mesh");

                        Meshes->addChild(tmpItem);
                    }
                    else if(node->factory().nodeInherits() == "NESphRigid" )
                    {
                        if(!Rigids)
                            Rigids = createOrganizerItem("Particle Rigid Objects");

                        Rigids->addChild(tmpItem);
                    }
                    else if(node->factory().nodeInherits() == "NERigidSolver" )

                    {
                        if(!Rigids)
                            Rigids = createOrganizerItem("Rigid Solvers");

                        Rigids->addChild(tmpItem);
                    }
                    else if(node->factory().nodeInherits() == "NEMeshPostProcessor"  ||
                            node->factory().nodeInherits() == "NEMeshConstructor" ||
                            node->factory().nodeInherits() == "NEParticleInputMC" )
                    {
                        if(!PostProcessors)
                            PostProcessors = createOrganizerItem("Post Processors");

                        PostProcessors->addChild(tmpItem);
                    }
#if NE_DEFORMABLE
                    else if(node->factory().nodeInherits() == "NEDeformable" )
                    {
                        if(!Deformables)
                            Deformables = createOrganizerItem("Deformable Objects");

                        Deformables->addChild(tmpItem);
                    }
#endif
                    else if(node->factory().nodeInherits() == "NECollider" )
                    {
                        if(!CollisionDetection)
                            CollisionDetection = createOrganizerItem("Colliders");

                        tmpItem->setFlags(tmpItem->flags()^Qt::ItemIsEditable);

                        CollisionDetection->addChild(tmpItem);
                    }
#if NE_VOLUME
                    else if(node->factory().nodeInherits() == "NEVolumeNode" )
                    {
                        if(!VolumeOps)
                            VolumeOps = createOrganizerItem("Volumes");

                        VolumeOps->addChild(tmpItem);
                    }
#endif
                    else if(node->factory().nodeInherits() == "NEDataCollector" )
                    {
                        if(!MeasurementFields)
                            MeasurementFields = createOrganizerItem("Data Collectors");

                        MeasurementFields->addChild(tmpItem);
                    }
                    else if(node->factory().nodeInherits() == "NESpatialGroup" )
                    {
                        if(!Groups)
                            Groups = createOrganizerItem( "Groups" );
                        Groups->addChild(tmpItem);
                        //tmpItem->setExpanded(true);
                    }
                    else
                    {
                        parent->addChild(tmpItem);
                        //tmpItem->setExpanded(true);
                    }
                    // Init the item
                    if(node)
                    {
                        NENodeFactory* f = &(node->factory());
                        tmpItem->setText(1, f->nodeName());

                        NENodeIcon icon = *f->nodeIcon();

                        QIcon qicon;
                        for(int i = 0; i < icon.size(); i++)
                        {
                            qicon.addFile(icon[i].iconPath, icon[i].iconSize);
                        }

                        tmpItem->setIcon(1, qicon);
                        tmpItem->setToolTip(1, f->nodeName());
                    }
                }
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

NETreeInspectorItem* NETreeInspector::createOrganizerItem(const char* name)
{
    NETreeInspectorItem* item = new NETreeInspectorItem();
    item->setForeground(0, *NEGUISTYLE.GrayFontBrush());
    item->setFont(0, *NEGUISTYLE.BoldFont());
    item->setText(0, name);
    item->setObject(0);
    SceneItem->addChild(item);
    item->setExpanded(true);
    item->setFlags(item->flags()^Qt::ItemIsSelectable);
    return item;
}

void NETreeInspector::mousePressEvent(QMouseEvent *event)
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

void NETreeInspector::createContextMenu()
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
    NETreeInspectorItem* item = (NETreeInspectorItem*) currentItem();
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
        NETreeInspectorItem* sItem = static_cast<NETreeInspectorItem*>(item);
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

void NETreeInspector::removeSelectedItem()
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
    NEGUI.updateNodeGraphEditorForDeletion();//Update NodeGraphEditor
    NEGUI.updateNodeConnectionsTable(); //Update node connection table of connection inspector
}


bool NETreeInspector::isSelectedObect(QObject* object)
{
    const NETreeInspectorItem* item = (NETreeInspectorItem*) currentItem();
    if(item)
        return (object == item->Object());
    else
        return false;
}

void NETreeInspector::selectItems()
{
    const QList<QTreeWidgetItem *>& items = selectedItems();

    if(items.size() == 1)
    {
        NETreeInspectorItem* sItem =  (NETreeInspectorItem*)items.first();
        QObject* object = sItem->Object();//((NETreeInspectorItem*)item)->Object();
        NEGUI.selectObject(object);
    }

    else
    {
        QList<NENode*> objects;
        Q_FOREACH(QTreeWidgetItem* item2, items)
        {
            NETreeInspectorItem* sItem =  (NETreeInspectorItem*)item2;
            NENode* casted = qobject_cast<NENode*>(sItem->Object());

            if(casted)
                objects.push_back(casted);
        }
        NEGUI.selectObjects(objects);
    }
}

void NETreeInspector::groupNodes()
{
    QList<NENode*> toGroup;

    Q_FOREACH(QTreeWidgetItem* item , selectedItems())
    {
        NETreeInspectorItem* sItem = static_cast<NETreeInspectorItem*>(item);
        toGroup.append(static_cast<NENode*>(sItem->Object()));
    }

    NESCENE.groupObjects(toGroup);
}

void NETreeInspector::ungroupNodes()
{
    const QList<QTreeWidgetItem *>& items = selectedItems();
    NETreeInspectorItem* sItem =  (NETreeInspectorItem*)items.first();
    QObject* object = sItem->Object();//((NETreeInspectorItem*)item)->Object();
    NESCENE.ungroupNode(static_cast<NESpatialGroup*>(object));

}

void NETreeInspector::parentNodes()
{
    QList<NENode*> nodeList;

    Q_FOREACH(QTreeWidgetItem* item , selectedItems())
    {
        NETreeInspectorItem* sItem = static_cast<NETreeInspectorItem*>(item);
        nodeList.append(static_cast<NENode*>(sItem->Object()));
    }
    foreach(NENode* node, nodeList)
        if(NESpatialGroup* group = qobject_cast<NESpatialGroup*>(node)){
            nodeList.removeAt(nodeList.indexOf(node));
            NESCENE.parentObjects(nodeList, group);
            return;
        }
}

void NETreeInspector::unparentNodes()
{
    NETreeInspectorItem* item = (NETreeInspectorItem*) currentItem();
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

void NETreeInspector::nodeRenamed(QTreeWidgetItem *item, int column)
{
    NETreeInspectorItem* sItem = static_cast<NETreeInspectorItem*>(item);

    if(!sItem->Object() || sItem->Object()->objectName() == sItem->text(0))
        return;

    NENode* node = qobject_cast<NENode*>(sItem->Object());

    node->setObjectName(sItem->text(0));
}

void NETreeInspector::itemExpansion(QTreeWidgetItem *item)
{
    NETreeInspectorItem* sItem = static_cast<NETreeInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = true;
}

void NETreeInspector::itemCollapse(QTreeWidgetItem *item)
{
    NETreeInspectorItem* sItem = static_cast<NETreeInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = false;
}

void NETreeInspector::itemActivation(QTreeWidgetItem *item, int column)
{
    topLeft = this->indexFromItem(item);
    //    m_LastSelectedItem = selectionModel()->selection();
    //    m_LastSelectedItem.select(topLeft, topLeft);

}

void NETreeInspector::restoreExpandedState()
{
    if(topLevelItemCount())
    {
        QTreeWidgetItemIterator it(this);
        while(*it)
        {
            NETreeInspectorItem* item = static_cast<NETreeInspectorItem*>(*it);
            if(item->Object())
            {
                int expState = m_ExpNodes[item->Object()->objectName()];
                item->setExpanded(expState);
            }

            ++it;
        }
    }
}

void NETreeInspector::clearState()
{
    Cameras=0;
    Lights=0;
    Materials=0;
    Textures=0;
    FluidSolvers=0;
    Fluids = 0;
    ParticleSources=0;
    KillerFields=0;
    DualBoundaries=0;
    ForceFields=0;
    Meshes = 0;
    DataFields=0;
    Rigids=0;
    Deformables=0;
    MSSs=0;
    Meshing=0;
    PostProcessors=0;
    OSDs=0;
    CollisionDetection=0;
    SceneItem=0;
    VolumeOps=0;
    MeasurementFields = 0;
    Groups = 0;
}

/*!
 If we would like to make the parent of the deleted instance the current object this method should be called.
 However, than we should also be able to keep the selected/current item when updating the qtreewidget.
 This is not possible yet.
*/
void NETreeInspector::setParentCurrentItem()
{	
    NETreeInspectorItem* item = (NETreeInspectorItem*) currentItem();
    NETreeInspectorItem* parent = (NETreeInspectorItem*)item->parent();
    setCurrentItem(parent);
    NEGUI.selectObject(parent->Object());
}


void NETreeInspector::setNewCurrentItem(QObject* object)
{
    if(object)
    {
        QList<QTreeWidgetItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
        QListIterator<QTreeWidgetItem*> i(items);

        if(i.hasNext())
        {
            NETreeInspectorItem* item = (NETreeInspectorItem*) (i.next());
            setCurrentItem(item);
        }
    }
    else{
        update();
    }
}

void NETreeInspector::setCurrentItems(QList<NENode*> objList)
{
    foreach(NENode* object, objList)
    {
        if(object)
        {
            QList<QTreeWidgetItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
            QListIterator<QTreeWidgetItem*> i(items);

            if(i.hasNext())
            {
                NETreeInspectorItem* item = (NETreeInspectorItem*) (i.next());
                item->setSelected(true);
            }
        }
    }
}

void NETreeInspector::saveGUIStates(QXmlStreamWriter *writer)
{
    writer->writeStartElement("expandednodes");

    if(topLevelItemCount())
    {
        QTreeWidgetItemIterator it(this);
        while(*it)
        {
            NETreeInspectorItem* item = static_cast<NETreeInspectorItem*>(*it);
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

void NETreeInspector::readGUIStates(QXmlStreamReader *reader)
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

