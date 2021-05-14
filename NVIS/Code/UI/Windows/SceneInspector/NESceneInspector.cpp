#include "Base/NEBase.h"
#include "NEObject.h"
#include "NENode.h"
#include "Spatial/NESpatialGroup.h"
#include "Base/NESceneManager.h"
#include "Managers/NEGUIManager.h"
#include "Utilities/NEGUIStyle.h"
#include "NESceneInspector.h"
#include "NESceneInspectorWindow.h"


class NESceneInspectorItem : public QTreeWidgetItem
{
public:
    NESceneInspectorItem() : QTreeWidgetItem(), m_pObject(0) {}
    ~NESceneInspectorItem() {}

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



NESceneInspector::NESceneInspector(QWidget* parent) : QTreeWidget(parent)
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

    connect(this, &QTreeWidget::itemExpanded, this, &NESceneInspector::itemExpansion);
    connect(this, &QTreeWidget::itemCollapsed, this, &NESceneInspector::itemCollapse);
    connect(this, &QTreeWidget::itemClicked, this, &NESceneInspector::itemActivation);

    groupAction = new QAction("Group", 0);
    ungroupAction = new QAction("Ungroup", 0);
//    connect(groupAction, &QAction::triggered, this, &NESceneInspector::groupNodes);
//    connect(ungroupAction, &QAction::triggered, this, &NESceneInspector::ungroupNodes);

    connect(groupAction, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedGroup);
    connect(ungroupAction, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedUngroup);


    parentAction = new QAction("Parent", 0);
    unparentAction = new QAction("Unparent", 0);
    connect(parentAction, &QAction::triggered, this, &NESceneInspector::parentNodes);
    connect(unparentAction, &QAction::triggered, this, &NESceneInspector::unparentNodes);

    //m_pBrush
}

NESceneInspector::~NESceneInspector()
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

void NESceneInspector::setRoot( QObject* root)
{
    m_pRoot = root;
}

void NESceneInspector::update()
{
    this->clear();
    // Clear the pointers before rebuilding the contents
    clearState();
    Q_ASSERT_X(m_pRoot, " NESceneInspector::update", "root pointer is null");

    const QObject* child = m_pRoot;
    NESceneInspectorItem* tmpItem = new NESceneInspectorItem();
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

void NESceneInspector::traverseTree(const QObject* obj, NESceneInspectorItem* parent)
{
    const QObjectList& objList = obj->children();
    Q_FOREACH(const QObject* child, objList)
    {
        NESceneInspectorItem* tmpItem = new NESceneInspectorItem();
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
            {
                node = qobject_cast<NENode*> (const_cast<QObject*>(child));
                if( node == NULL )
                    continue;

                //std::cout << "node sub group is " << child->objectName().toStdString() << std::endl;
                NESceneInspectorItem *categoryItem = NULL;
                if(node->factory().nodeInherits() != "Groups")
                {
                    QString keyString = parent->text(0);
                    NESceneInspectorItem *pp = dynamic_cast<NESceneInspectorItem *>(parent->parent());
                    while (pp) {
                        keyString += "," + pp->text(0);
                        pp = dynamic_cast<NESceneInspectorItem *>(pp->parent());
                    }
                    keyString += "," + node->factory().nodeInherits();
                    if( SceneInpectorItems.find(keyString) == SceneInpectorItems.end())
                    {

                        categoryItem = createOrganizerItem(node->factory().nodeInherits(), parent);
                        SceneInpectorItems.insert(keyString, categoryItem);
                    }

                    categoryItem = SceneInpectorItems.value(keyString);
                    categoryItem->addChild(tmpItem);
                }
                else
                {
                    tmpItem->setForeground(0, *NEGUISTYLE.NormalFontBrush());
                    tmpItem->setFont(0, *NEGUISTYLE.BoldFont());
                    parent->addChild(tmpItem);
                }

                // Init the item
                if(node)
                {
                    NENodeFactory* f = &(node->factory());
                    tmpItem->setText(1, f->nodeName());

                    if(node->factory().nodeInherits() != "Groups")
                    {
                        NENodeIcon icon = *f->nodeIcon();

                        QIcon qicon;
                        for(unsigned i = 0; i < icon.size(); i++)
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


NESceneInspectorItem* NESceneInspector::createOrganizerItem(QString name, NESceneInspectorItem *parentItem)
{
    NESceneInspectorItem* item = new NESceneInspectorItem();
    item->setForeground(0, *NEGUISTYLE.GrayFontBrush());
    item->setFont(0, *NEGUISTYLE.BoldFont());
    item->setText(0, name);
    item->setObject(0);
    parentItem->addChild(item);
    item->setExpanded(true);
    item->setFlags(item->flags()^Qt::ItemIsSelectable);
    return item;
}

void NESceneInspector::mousePressEvent(QMouseEvent *event)
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

void NESceneInspector::createContextMenu()
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
    NESceneInspectorItem* item = (NESceneInspectorItem*) currentItem();
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
        NESceneInspectorItem* sItem = static_cast<NESceneInspectorItem*>(item);
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

void NESceneInspector::removeSelectedItem()
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


bool NESceneInspector::isSelectedObect(QObject* object)
{
    const NESceneInspectorItem* item = (NESceneInspectorItem*) currentItem();
    if(item)
        return (object == item->Object());
    else
        return false;
}

void NESceneInspector::selectItems()
{
    const QList<QTreeWidgetItem *>& items = selectedItems();

    if(items.size() == 1)
    {
        NESceneInspectorItem* sItem =  (NESceneInspectorItem*)items.first();
        QObject* object = sItem->Object();//((NESceneInspectorItem*)item)->Object();
        NEGUI.selectObject(object);
    }

    else
    {
        QList<NENode*> objects;
        Q_FOREACH(QTreeWidgetItem* item2, items)
        {
            NESceneInspectorItem* sItem =  (NESceneInspectorItem*)item2;
            NENode* casted = qobject_cast<NENode*>(sItem->Object());

            if(casted)
                objects.push_back(casted);
        }
        NEGUI.selectObjects(objects);
    }
}

void NESceneInspector::groupNodes()
{
    QList<NENode*> toGroup;

    Q_FOREACH(QTreeWidgetItem* item , selectedItems())
    {
        NESceneInspectorItem* sItem = static_cast<NESceneInspectorItem*>(item);
        toGroup.append(static_cast<NENode*>(sItem->Object()));
    }

    NESCENE.groupObjects(toGroup);
}

void NESceneInspector::ungroupNodes()
{
    const QList<QTreeWidgetItem *>& items = selectedItems();
    NESceneInspectorItem* sItem =  (NESceneInspectorItem*)items.first();
    QObject* object = sItem->Object();//((NESceneInspectorItem*)item)->Object();
    NESCENE.ungroupNode(static_cast<NESpatialGroup*>(object));
    NESceneInspectorWindow *inspectorWindow = qobject_cast<NESceneInspectorWindow *>(parentWidget());
    inspectorWindow->removeSelectedItem();

}

void NESceneInspector::parentNodes()
{
    QList<NENode*> nodeList;

    Q_FOREACH(QTreeWidgetItem* item , selectedItems())
    {
        NESceneInspectorItem* sItem = static_cast<NESceneInspectorItem*>(item);
        nodeList.append(static_cast<NENode*>(sItem->Object()));
    }
    foreach(NENode* node, nodeList)
        if(NESpatialGroup* group = qobject_cast<NESpatialGroup*>(node)){
            nodeList.removeAt(nodeList.indexOf(node));
            NESCENE.parentObjects(nodeList, group);
            return;
        }
}

void NESceneInspector::unparentNodes()
{
    NESceneInspectorItem* item = (NESceneInspectorItem*) currentItem();
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

void NESceneInspector::nodeRenamed(QTreeWidgetItem *item, int column)
{
    NESceneInspectorItem* sItem = static_cast<NESceneInspectorItem*>(item);

    if(!sItem->Object() || sItem->Object()->objectName() == sItem->text(0))
        return;

    NENode* node = qobject_cast<NENode*>(sItem->Object());

    node->setObjectName(sItem->text(0));
}

void NESceneInspector::itemExpansion(QTreeWidgetItem *item)
{
    NESceneInspectorItem* sItem = static_cast<NESceneInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = true;
}

void NESceneInspector::itemCollapse(QTreeWidgetItem *item)
{
    NESceneInspectorItem* sItem = static_cast<NESceneInspectorItem*>(item);
    if(sItem->Object())
        m_ExpNodes[sItem->Object()->objectName()] = false;
}

void NESceneInspector::itemActivation(QTreeWidgetItem *item, int column)
{
    topLeft = this->indexFromItem(item);
    //    m_LastSelectedItem = selectionModel()->selection();
    //    m_LastSelectedItem.select(topLeft, topLeft);

}

void NESceneInspector::restoreExpandedState()
{
    if(topLevelItemCount())
    {
        QTreeWidgetItemIterator it(this);
        while(*it)
        {
            NESceneInspectorItem* item = static_cast<NESceneInspectorItem*>(*it);
            if(item->Object())
            {
                int expState = m_ExpNodes[item->Object()->objectName()];
                item->setExpanded(expState);
            }

            ++it;
        }
    }
}

void NESceneInspector::clearState()
{
#if 1
//    Cameras=0;
//    Lights=0;
//    Materials=0;
//    Textures=0;
//    FluidSolvers=0;
//    Fluids = 0;
//    Chemistry = 0;
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
void NESceneInspector::setParentCurrentItem()
{	
    NESceneInspectorItem* item = (NESceneInspectorItem*) currentItem();
    NESceneInspectorItem* parent = (NESceneInspectorItem*)item->parent();
    setCurrentItem(parent);
    NEGUI.selectObject(parent->Object());
}


void NESceneInspector::setNewCurrentItem(QObject* object)
{
    if(object)
    {
        QList<QTreeWidgetItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
        QListIterator<QTreeWidgetItem*> i(items);

        if(i.hasNext())
        {
            NESceneInspectorItem* item = (NESceneInspectorItem*) (i.next());
            setCurrentItem(item);
        }
    }
    else{
        update();
    }
}

void NESceneInspector::setCurrentItems(QList<NENode*> objList)
{
    foreach(NENode* object, objList)
    {
        if(object)
        {
            QList<QTreeWidgetItem *> items = findItems(object->objectName(),Qt::MatchRecursive);
            QListIterator<QTreeWidgetItem*> i(items);

            if(i.hasNext())
            {
                NESceneInspectorItem* item = (NESceneInspectorItem*) (i.next());
                item->setSelected(true);
            }
        }
    }
}

void NESceneInspector::saveGUIStates(QXmlStreamWriter *writer)
{
    writer->writeStartElement("expandednodes");

    if(topLevelItemCount())
    {
        QTreeWidgetItemIterator it(this);
        while(*it)
        {
            NESceneInspectorItem* item = static_cast<NESceneInspectorItem*>(*it);
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

void NESceneInspector::readGUIStates(QXmlStreamReader *reader)
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

