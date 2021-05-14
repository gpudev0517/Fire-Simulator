#include "Base/NEBase.h"
#include "NENodeGraphEditorScene.h"
#include "Managers/NEGUIManager.h"
#include "Base/NESceneManager.h"
#include "NEConnectionGEItem.h"
#include "NENode.h"
#include "NodesEditor.h"
#include "NEBlock.h"
#include "NEPort.h"
#include "Base/NENodeFactory.h"

#include <boost/graph/circle_layout.hpp>


NENodeGraphEditorScene::LayoutType NENodeGraphEditorScene::m_LayoutType = NENodeGraphEditorScene::LayoutRandom; //org value is LayoutCircle;
QList<int> NENodeGraphEditorScene::m_FilterPortsType = QList<int>() << NE::kIONone;
bool NENodeGraphEditorScene::m_FilterNodeByConnnection = false;
QList<int> NENodeGraphEditorScene::m_FilterNodeType = QList<int>() << -1;
NENodeGraphEditorScene::SceneType NENodeGraphEditorScene::m_SceneType = NENodeGraphEditorScene::ALL;

enum NEFilterType
{
    BY_NODE_FILTER_ALL = -1,
    BY_NODE_CONNECTION_ALL = 0,
};

NENodeGraphEditorScene::NENodeGraphEditorScene(QObject *parent) :
    QGraphicsScene(parent)
{
    m_PropertyMap = boost::get( &VertexProperties::index, m_NodeGraph );
    m_Initialized = false;
    setSelectionProcess(false);

    connect(this, &QGraphicsScene::selectionChanged, this, &NENodeGraphEditorScene::notifyOthersForUpdatedSelection);

    NodesEditor* nodesEditor = new NodesEditor(this);
    nodesEditor->install(this);

    m_bSnapToGrid = false;
}

void NENodeGraphEditorScene::update()
{
    // bcakup all old node's pos
    if (m_Initialized)
        BackupAllNodePos();

    // Clear the maps
    m_NodeGraph.clear();
    m_NodeNameToNodeMap.clear();
    m_NodeToItemMap.clear();
    m_ItemToNodeMap.clear();
    m_ConnToItemMap.clear();

    m_GraphItemMap.clear();
    m_ItemGraphMap.clear();

    this->clear();
    updateNodes( &NESCENE );
    updateConnections( &NESCENE );
    emit NESCENE.updateGLWindows();

    //    int nv = boost::num_vertices(  m_NodeGraph );
    //    if( nv > 0 )
    //    {
    //        layout();
    //    }
}

void NENodeGraphEditorScene::selectItem( NENode *node )
{
    if (IsSelectionProcess())
        return;
    setSelectionProcess(true);
    clearSelection();
    if (m_NodeToItemMap.contains(node))
        m_NodeToItemMap[node]->setSelected(true);
    setSelectionProcess(false);
    return;
}

void NENodeGraphEditorScene::selectItem(QList<NENode*>& nodes)
{
    if (IsSelectionProcess())
        return;
    setSelectionProcess(true);
    clearSelection();
    foreach(NENode* node, nodes)
    {
        if (m_NodeToItemMap.contains(node))
            m_NodeToItemMap[node]->setSelected(true);
    }

    setSelectionProcess(false);
    return;
}

void NENodeGraphEditorScene::notifyOthersForUpdatedSelection()
{
    if (IsSelectionProcess())
        return;
    QList<QGraphicsItem *>items = selectedItems();

    QList<NENode*> nodeObjects;
    foreach(QGraphicsItem* item, items)
    {
        if(item->type() != NEBlock::Type)
            continue;
        NEBlock *blockItem = (NEBlock *)item;
        if(blockItem && blockItem->getNode() &&
                m_ItemToNodeMap.contains(blockItem))
        {
            NENode* nodeItem = qobject_cast<NENode*>(m_ItemToNodeMap[blockItem]);

            if(nodeItem)
                nodeObjects.push_back(nodeItem);
        }
    }

    if(nodeObjects.size() == 0)
        NEGUI.selectObject(0);
    else if(nodeObjects.size() == 1)
        NEGUI.selectObject((QObject*)nodeObjects.first());
    else
        NEGUI.selectObjects(nodeObjects);
#if 0
    if(selectedItems().isEmpty())
    {
        //NEGUI.selectObject(0);
        return;
    }

    QObject* item = (QObject*)m_ItemToNodeMap[(NEBlock*)selectedItems().last()];

    NEGUI.selectObject(item);
#endif
}



void NENodeGraphEditorScene::saveNodeStates(QXmlStreamWriter *writer)
{
    writer->writeStartElement("nodepositions");

    foreach(NEBlock* item, m_NodeToItemMap)
    {
        if( m_ItemToNodeMap.contains( item ) )
        {
            writer->writeStartElement(m_ItemToNodeMap[item]->objectName());
            writer->writeAttribute("x", QString("%1").arg(item->pos().x()));
            writer->writeAttribute("y", QString("%1").arg(item->pos().y()));
        }
        writer->writeEndElement();
    }

    writer->writeEndElement();
}

void NENodeGraphEditorScene::loadNodeStates(QXmlStreamReader *reader)
{
    QString nodeName;
    float posx;
    float posy;

    update();

    while(reader->readNextStartElement())
    {
        if(reader->name() == "nodepositions")
        {
            // Read node names
            while(reader->readNext() != QXmlStreamReader::Invalid)
            {
                if(reader->tokenType() == QXmlStreamReader::EndElement && reader->name() == "nodepositions")
                    break;
                else if(reader->tokenType() != QXmlStreamReader::StartElement)
                    continue;

                nodeName = reader->name().toString();
                posx = reader->attributes().value("x").toFloat();
                posy = reader->attributes().value("y").toFloat();

                NEBlock* item = m_NodeNameToNodeMap[nodeName];
                if(item)
                    item->setPos(posx, posy);
                m_NodeNameToNodePosforBackup[nodeName] = QPointF(posx, posy);

            }
            if(reader->hasError())
            {
                Log()<< "Error parsing scene file: " << reader->errorString(); EndLog(NE::kLogWarning);
            }
            break;
        }
        else
        {
            reader->skipCurrentElement();
            Log()<< "Unknown section " << reader->name() << " in the scene file."; EndLog(NE::kLogWarning);
        }
    }

}


typedef boost::rectangle_topology<> topology_type;

class progress_cooling : public boost::linear_cooling<double>
{
    typedef boost::linear_cooling<double> inherited;

public:
    explicit progress_cooling(std::size_t iterations) : inherited(iterations)
    {
        //display.reset(new boost::progress_display(iterations + 1, std::cerr));
    }

    double operator()()
    {
        //++(*display);
        return inherited::operator()();
    }

private:
    std::shared_ptr< boost::progress_display > display;
};

void NENodeGraphEditorScene::layout(bool bFullPerform)
{


    /*
    m_NodeGraph.clear();
    m_NodeNameToNodeMap.clear();
    m_NodeToItemMap.clear();
    m_ItemToNodeMap.clear();
    m_ConnToItemMap.clear();

    m_GraphItemMap.clear();
    m_ItemGraphMap.clear();

    this->clear();

    updateNodes( &NESCENE );
    updateConnections( &NESCENE );
*/

    //std::cout << "Vertices\n";
    //boost::print_vertices( m_NodeGraph, m_PropertyMap );

    //std::cout << "Edges\n";
    //boost::print_edges( m_NodeGraph, m_PropertyMap );
    if (!m_Initialized){
        m_NodeGraph.clear();
        //m_PropertyMap = boost::get( &VertexProperties::index, m_NodeGraph );
        m_Initialized = true;
    }

    m_PositionMap = boost::get( &VertexProperties::point, m_NodeGraph );
    WeightPropertyMap weightPropertyMap = boost::get( &EdgeProperty::weight, m_NodeGraph );
    // Circle layout gives a strange build error on linux and mac
    if( layoutType() == LayoutCircle  )
    {
        boost::circle_graph_layout( m_NodeGraph, m_PositionMap, 1000 );
    }
    boost::minstd_rand gen;
    topology_type topo( gen, -1000, -1000, 1000, 1000 );

    //std::cout << "Width " << width() << " " << height() << std::endl;

    if( layoutType() == LayoutRandom )
    {
        boost::random_graph_layout( m_NodeGraph, m_PositionMap, topo );
    }

    if( layoutType() == LayoutForceDirected )
    {
        boost::fruchterman_reingold_force_directed_layout( m_NodeGraph, m_PositionMap, topo,
                                                           boost::cooling( progress_cooling( 50000 ) ) ) ;
    }

    if( layoutType() == LayoutSpring )
    {
        //std::cout << "Spring " << std::endl;
        bool retval = boost::kamada_kawai_spring_layout( m_NodeGraph, m_PositionMap, weightPropertyMap, topo, boost::side_length<double>( 2800 ) );
        if( !retval )
            qWarning() << "NENodeGraphEditorScene: Coudn't do a Spring Layout - disconnected nodes could be present";
    }

    //std::cout << "Coordinates\n";
    boost::graph_traits< Graph >::vertex_iterator itr, end;
    for( boost::tie( itr, end ) = boost::vertices( m_NodeGraph ); itr != end; ++itr )
    {
        //std::cout << "ID " << *itr << std::endl;
        QPointF p = QPointF( ((m_PositionMap[*itr][0] ) ), ((m_PositionMap[*itr][1] ) ) );
#ifdef hanin
        m_GraphItemMap[ m_PropertyMap[*itr] ]->setPosition( p ) ;
#endif

        if (!bFullPerform && m_NodeNameToNodePosforBackup.contains(m_GraphItemMap[ m_PropertyMap[*itr] ]->getBlockName()))
        {
            QPointF newP = m_NodeNameToNodePosforBackup[m_GraphItemMap[ m_PropertyMap[*itr] ]->getBlockName()] ;
            if(m_bSnapToGrid)
            {
                if( ((int)newP.x()) % 25 >= 13 )
                    newP.setX( ((int)newP.x() - ((int)newP.x()) % 25) + 25);
                else
                    newP.setX( ((int)newP.x() - ((int)newP.x()) % 25));

                if( ((int)newP.y()) % 25 >= 13 )
                    newP.setY( ((int)newP.y() - ((int)newP.y()) % 25) + 25);
                else
                    newP.setY( ((int)newP.y() - ((int)newP.y()) % 25));

            }
            m_GraphItemMap[ m_PropertyMap[*itr] ]->setPos( newP );
        }
        else{
            QPointF newP = p;
            if(m_bSnapToGrid)
            {
                if( ((int)newP.x()) % 25 >= 13 )
                    newP.setX( ((int)newP.x() - ((int)newP.x()) % 25) + 25);
                else
                    newP.setX( ((int)newP.x() - ((int)newP.x()) % 25));

                if( ((int)newP.y()) % 25 >= 13 )
                    newP.setY( ((int)newP.y() - ((int)newP.y()) % 25) + 25);
                else
                    newP.setY( ((int)newP.y() - ((int)newP.y()) % 25));

            }
            m_GraphItemMap[ m_PropertyMap[*itr] ]->setPos( p );
        }

        //std::cout << "ID: (" << m_PropertyMap[*itr] << ") x: " << p.x() << " y: " << p.y()<< std::endl;

    }

    QMap<NENode::NEConn*, NEConnectionGEItem* >::iterator iter = m_ConnToItemMap.begin();
    for(; iter != m_ConnToItemMap.end(); ++iter)
    {
        iter.value()->adjust();
    }


    //update();
}
void NENodeGraphEditorScene::drawBackground( QPainter *p, const QRectF& crect )
{
    const int spacing = 25;
    const QRectF rect = crect.normalized();
    p->save();

    p->setPen( QPen( Qt::lightGray, 0.5 ) );
    int l = int(rect.left());
    l -= (l % spacing);

    int r = int(rect.right());
    r -= (r % spacing);
    if(r < int(rect.right()))
        r += spacing;

    int t = int(rect.top());
    t -= (t % spacing);

    int b = int(rect.bottom());
    b -= (b % spacing);
    if(b < int(rect.bottom()))
        b += spacing;

    for( int x = l; x <= r; x += spacing)
        for( int y = t; y <= b; y += spacing)
            p->drawPoint( x, y );

    p->restore();
}

void NENodeGraphEditorScene::BackupAllNodePos()
{
    QPointF     oldPos(0, 0);
    NEBlock*    pBlock;
    QString     nodeName;

    m_NodeNameToNodePosforBackup.clear();
    QMap<QString, NEBlock*>::iterator iter = m_NodeNameToNodeMap.begin();
    for(; iter != m_NodeNameToNodeMap.end(); ++iter)
    {
        pBlock = iter.value();
        nodeName = iter.key();
        if (pBlock)
        {
            oldPos = pBlock->pos();
            m_NodeNameToNodePosforBackup[nodeName] = oldPos;
        }
    }
}

bool NENodeGraphEditorScene::getOldNodePos(NENode* pNode, QPointF& aOldPos)
{
    if (pNode)
    {
        QString nodeName = pNode->objectName();

        if (m_NodeNameToNodePosforBackup.contains(nodeName))
        {
            aOldPos = m_NodeNameToNodePosforBackup[nodeName];
            return true;
        }
    }
    return false;
}

void NENodeGraphEditorScene::updateNodes( NEObject* parent )
{
    //std::cout << "Updating Nodes " << std::endl;
    // Create all node items
    int nCount = 0;
    foreach( QObject* obj, parent->children() )
    {
        NENode* node = qobject_cast< NENode* >(obj);

        if( !node)
            continue;

        NEBlock *itemBlock = new NEBlock(node, 0);
        itemBlock->m_pNode->setShowFlag(checkDrawFlag(itemBlock));
        QPointF oldPos(0, 0);
        getOldNodePos(node, oldPos);
        itemBlock->setPos(oldPos);
        addItem(itemBlock);
        nCount++;
#ifdef hanin
        NENodeGraphEditorItem* item = new NENodeGraphEditorItem( node );
#endif
        m_NodeNameToNodeMap[node->objectName()] = itemBlock;
        m_NodeToItemMap[node] = itemBlock;
        m_ItemToNodeMap[itemBlock] = node;

        NENodeFactory& f = node->factory();
        switch( f.nodeType() )
        {
        case NE::kObjectFluid:
            itemBlock->setColor( Qt::red );
            break;
        case NE::kObjectChemistry:
            itemBlock->setColor( Qt::red );
            break;
        case NE::kObjectParticleFluidSolver:
            itemBlock->setColor( Qt::red );
            break;
        case NE::kObjectParticleRigid:
            itemBlock->setColor( Qt::red );
            break;
        case NE::kObjectRigidSolver:
            itemBlock->setColor( Qt::red );
            break;
        case NE::kObjectKiller:
            itemBlock->setColor( Qt::blue );
            break;
        case NE::kObjectParticleDeformable:
            itemBlock->setColor( Qt::green );
            break;
        case NE::kObjectGridFluidSolver:
            itemBlock->setColor( Qt::magenta );
            break;
        case NE::kObjectCollider:
            itemBlock->setColor( Qt::cyan );
            break;
        case NE::kObjectMeshing:
            itemBlock->setColor( Qt::yellow );
            break;
        case NE::kObjectMaterial:
            itemBlock->setColor( Qt::darkGray );
            break;
        case NE::kObjectParticleSource:
            itemBlock->setColor( Qt::darkBlue );
            break;
        case NE::kObjectCamera:
            itemBlock->setColor( Qt::darkYellow );
            break;
        case NE::kObjectLight:
            itemBlock->setColor( Qt::darkCyan );
            break;
        case NE::kObjectMeshPP:
            itemBlock->setColor( Qt::darkCyan );
            break;
        case NE::kObjectDataField:
            itemBlock->setColor( Qt::darkGreen );
            break;
        case NE::kObjectForceField:
            itemBlock->setColor( Qt::blue );
            break;
        case NE::kObjectParticleEmitter:
            itemBlock->setColor( Qt::blue );
            break;
        case NE::kObjectMeshVolumeOP:
            itemBlock->setColor( Qt::darkGreen );
            break;
        case NE::kObjectVolumeOP:
            itemBlock->setColor( Qt::darkYellow );
            break;
        case NE::kObjectMeasurementField:
            itemBlock->setColor( Qt::cyan );
            break;
        case NE::kObjectGroup:
            itemBlock->setColor( Qt::magenta );
            break;
        case NE::kObjectDataCollector:
            itemBlock->setColor( Qt::yellow );
            break;
        default:
            itemBlock->setColor( Qt::white );
            break;
        }

        itemBlock->updatePortsofBlock();

        //        QPointF     oldPos = itemBlock->pos();
        //        if( nodeConnections.size() == 0 && f.nodeType() != NE::kObjectMaterial && f.nodeType() != NE::kObjectParticleFluidSolver )
        //        {
        //            if ((int)(oldPos.x()) == 0 && (int)(oldPos.y()) == 0)
        //                itemBlock->setPos( -400, -400 );
        //            continue;
        //        }
        //        if( nodeConnections.size() == 0 && f.nodeType() == NE::kObjectKiller )
        //        {
        //            if ((int)(oldPos.x()) == 0 && (int)(oldPos.y()) == 0)
        //                itemBlock->setPos( -400, 400 );
        //            continue;
        //        }

        VirtexDescriptor vd = boost::add_vertex( m_NodeGraph );
        m_PropertyMap[vd] = vd;
        m_GraphItemMap[vd] = itemBlock;
        m_ItemGraphMap[itemBlock] = vd;

        if( obj->children().size() )
            updateNodes( node );
    }
}

void NENodeGraphEditorScene::updateConnections( NEObject *parent )
{
    //std::cout <<"Updating Connections " << std::endl;
    // Create all connection items
    foreach( QObject* obj, parent->children() )
    {
        NENode* node = qobject_cast<NENode*>( obj );
        if( !node )
            continue;
        NEBlock* curBlock = m_NodeToItemMap[ node ];
        if(curBlock->m_pNode->isShowFlag())
            curBlock->updateConnectionWithPorts(portsFilterType(), nodeFilterType());
        //        curBlock->updateSourceConnectionWithPorts();


        if(obj->children().size())
            updateConnections(node);
    }
}

void NENodeGraphEditorScene::updateInitFlag(bool aInitFlag)
{
    m_Initialized = aInitFlag;
}

void NENodeGraphEditorScene::updateNodeRenameForNodeGraph(const QString &oldName, const QString &newName)
{
    if (!oldName.isEmpty())
        m_NodeNameToNodePosforBackup.remove(oldName);
    if (oldName.isEmpty() || newName.isEmpty())
        return;
    NEBlock*    pBlock = m_NodeNameToNodeMap[oldName];
    m_NodeNameToNodeMap.remove(oldName);
    m_NodeNameToNodeMap[newName] = pBlock;
}

void NENodeGraphEditorScene::setPortsFilterType(int type)
{
    if(type == BY_NODE_CONNECTION_ALL)
    {
        m_FilterPortsType.clear();
        m_FilterPortsType.append(type);
        return;
    }
    else if(type == -1){        // check "Any" in "By connect Type"
        m_FilterPortsType = QList<int>() << NE::kIOParticles << NE::kIOTriangleMesh << NE::kIOForceField
                     << NE::kIOExtent << NE::kIOVolume << NE::kIOFlowRate << NE::kIOFluidHeight
                     << NE::kIONumParticles << NE::kIONumParticlesDrain << NE::kIOHydrostaticPressure
                     << NE::kIOFluid << NE::kIORigid << NE::kIOMeasurement << NE::kIOGizmo << NE::kIOGeoConversion;
        return;
    }
    else if(type == -2){        // uncheck "Any" in "By connect Type"
        m_FilterPortsType.clear();
        return;
    }
    else if((m_FilterPortsType.count() != 0) && (m_FilterPortsType.at(0) == BY_NODE_CONNECTION_ALL)){
        m_FilterPortsType.removeAt(0);
    }
    int i = m_FilterPortsType.indexOf(type);
    if (i != -1){
        m_FilterPortsType.removeAt(i);
    }
    else{
        m_FilterPortsType.append(type);
    }
}

void NENodeGraphEditorScene::setNodeFilterType(int type)
{
    if(type == BY_NODE_FILTER_ALL)
    {
        m_FilterNodeType.clear();
        m_FilterNodeType.append(type);
        return;
    }
    else if((m_FilterNodeType.count() != 0) && (m_FilterNodeType.at(0) == BY_NODE_FILTER_ALL)){
        m_FilterNodeType.removeAt(0);
    }
    int i = m_FilterNodeType.indexOf(type);
    if (i != -1){
        m_FilterNodeType.removeAt(i);
    }
    else{
        m_FilterNodeType.append(type);
    }
}

bool NENodeGraphEditorScene::checkDrawFlag(NEBlock *block){

    NENode *node = block->m_pNode;
    if( node == NULL )
        return false;

    QString nodeInherit = node->factory().nodeInherits();
    SceneType sType = sceneType();

    bool bVisible = false;

    for(int i =0; i < NENode::g_NodeGraphSceneInherits[sType-1].size(); i++)
    {
        if(nodeInherit == NENode::g_NodeGraphSceneInherits[sType-1][i])
        {
            bVisible = true;
            break;
        }

    }
    if(!bVisible)
        return false;


    QList<int> portType = portsFilterType();
    bool checkGetConnection = nodeFilterByConnection();
    QList<int> nodeType = nodeFilterType();
    if (nodeType.count() == 0)          // select all in "By Node Type"
    {
        return false;
    }
    else{
        int i = nodeType.indexOf(block->m_pNode->factory().nodeType());
        if(i == -1 && nodeType.at(0) != BY_NODE_FILTER_ALL)              //   none select in in "By Node Type"
        {
            return false;
        }
    }

    if (block->m_pNode->connections().size() == 0 && checkGetConnection == true
            && block->m_pNode->sourceConnections().size() == 0){    //  "By node without connection"
        return false;
    }

    if(portType.count() == 0){              //  none select in in "By Connection Type"
        return false;
    }

    if(portType.at(0) == BY_NODE_CONNECTION_ALL){      //  select all in "By Connection Type"
        return true;
    }

    // check has port

    uint outputCount = node->numOutputs();
    int enumOutputIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
    QMetaEnum outputEnum = NE::Instance().metaObject()->enumerator(enumOutputIndex);
    for (int i = 0; i < outputEnum.keyCount(); ++i)
    {
        // add an entry if the node accepts the according input
        const char* name = outputEnum.key(i);
        uint ioType = (uint)outputEnum.keyToValue(name);
        for (uint j = 0; j < outputCount; j++)
        {
            uint outputs = node->outputs(j);
            if (outputs & ioType){
                int i = portType.indexOf(ioType);
                if(i != -1){
                    return true;
                }
            }
        }
    }

    uint inputCount = node->numInputs();
    int enumInputIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
    QMetaEnum inputEnum = NE::Instance().metaObject()->enumerator(enumInputIndex);
    for(int i = 0; i < inputEnum.keyCount(); ++i)
    {
        // add an entry if the node accepts the according input
        const char* name = inputEnum.key(i);
        uint ioType = (uint)inputEnum.keyToValue(name);
        for (uint j = 0; j < inputCount; j++)
        {
            uint inputs = node->inputs(j);
            if (inputs & ioType){
                int i = portType.indexOf(ioType);
                if(i != -1){
                    return true;
                }
            }
        }
    }
    return false;
}

QRectF NENodeGraphEditorScene::getFrameSelectionRect()
{
    QPointF     minPos = QPointF(INT_MAX, INT_MAX);
    QPointF     maxPos = QPointF(INT_MIN, INT_MIN);
    NEBlock*    pBlock;

    QList<QGraphicsItem *>items = selectedItems();
    if(items.count() > 0)
    {
        pBlock = (NEBlock*)selectedItems().last();
        if (pBlock->m_pNode->isShowFlag())
        {
            QPointF p = pBlock->pos();
            minPos.setX( qMin(minPos.x(), p.x()) );
            maxPos.setX( qMax(maxPos.x(), p.x()) );
            minPos.setY( qMin(minPos.y(), p.y()) );
            maxPos.setY( qMax(maxPos.y(), p.y()) );
        }
        return QRectF(minPos - QPointF(200, 200), maxPos + QPointF(200, 200));
    }
    else{
        QMap<QString, NEBlock*>::iterator iter = m_NodeNameToNodeMap.begin();
        for(; iter != m_NodeNameToNodeMap.end(); ++iter)
        {
            pBlock = iter.value();
            if (pBlock->m_pNode->isShowFlag())
            {
                QPointF p = pBlock->pos();
                minPos.setX( qMin(minPos.x(), p.x()) );
                maxPos.setX( qMax(maxPos.x(), p.x()) );
                minPos.setY( qMin(minPos.y(), p.y()) );
                maxPos.setY( qMax(maxPos.y(), p.y()) );
            }
        }
        return QRectF(minPos - QPointF(200, 200), maxPos + QPointF(200, 200));
    }


}


