#pragma once

#include "NENode.h"

#include <boost/property_map/property_map.hpp>
#include <boost/graph/topology.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/progress.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/kamada_kawai_spring_layout.hpp>


class NEConnectionGEItem;
class NEBlock;

class NENodeGraphEditorScene : public QGraphicsScene
{
    Q_OBJECT

public:

    enum LayoutType
    {
        LayoutCircle = 0,
        LayoutRandom = 1,
        LayoutForceDirected = 2,
        LayoutSpring = 3
    };

    enum SceneType
    {
        ALL = 0,
        Modeling = 1,
        Simulation = 2,
        Visualization = 3
    };

    typedef boost::square_topology<>::point_type Point;
    struct VertexProperties
    {
        std::size_t index;
        Point point;
    };

    struct EdgeProperty
    {
        EdgeProperty( const std::size_t &w ) : weight(w)
        {
        }
        double weight;
    };


    typedef boost::adjacency_list<boost::vecS,
                boost::vecS, boost::undirectedS,
                VertexProperties, EdgeProperty > Graph;

    typedef boost::property_map< Graph, std::size_t VertexProperties::* >::type VertexIndexPropertyMap;
    typedef boost::property_map< Graph, Point VertexProperties::* >::type PositionMap;
    typedef boost::property_map< Graph, double EdgeProperty::* >::type WeightPropertyMap;

    typedef boost::graph_traits< Graph >::vertex_descriptor VirtexDescriptor;



public:
    explicit NENodeGraphEditorScene(QObject *parent = 0);

    void update();
    void BackupAllNodePos();
    bool getOldNodePos(NENode* pNode, QPointF& aOldPos);
    void updateNodes(NEObject *parent);
    void updateConnections(NEObject *parent);
    void selectItem(NENode* node);
    void selectItem(QList<NENode*>& nodes);
    void notifyOthersForUpdatedSelection();
    void saveNodeStates(QXmlStreamWriter *writer);
    void loadNodeStates(QXmlStreamReader *reader);
    void updateInitFlag(bool aInitFlag);
    void updateNodeRenameForNodeGraph(const QString &oldName, const QString &newName);
    bool IsSelectionProcess() { return m_bIsSelectionProcess; }
    void setSelectionProcess(bool aFlag) { m_bIsSelectionProcess = aFlag; }

    QMap<NENode*, NEBlock*>* nodeToItemMap() {return &m_NodeToItemMap; }
    QMap<NENode::NEConn*, NEConnectionGEItem*>* connToItemMap() { return &m_ConnToItemMap; }


    void layout(bool bFullPerform = false);
    void setSnapToGrid(bool bChecked)
    {
        m_bSnapToGrid = bChecked;
    }

    bool snapToGrid()
    {
        return m_bSnapToGrid;
    }

    static LayoutType layoutType() { return m_LayoutType; }
    static void setLayoutType(LayoutType type) { m_LayoutType = type; }

    static SceneType sceneType() { return m_SceneType; }
    static void setSceneType(SceneType type) { m_SceneType = type; }

    static void setPortsFilterType(int type);
    static QList<int> portsFilterType() { return m_FilterPortsType; }
    static void setNodeFilterByConnection(bool type){ m_FilterNodeByConnnection = type; }
    static bool nodeFilterByConnection() { return m_FilterNodeByConnnection; }
    static void setNodeFilterType(int type);
    static QList<int> nodeFilterType() { return m_FilterNodeType; }

    bool checkDrawFlag(NEBlock* block);
    QRectF getFrameSelectionRect();
private:
    NEBlock* m_pSource;
    NEBlock* m_pTarget;
    QPointF m_SourcePoint;
    QPointF m_TargetPoint;
    bool    m_bIsSelectionProcess;
    bool    m_bSnapToGrid;

    static LayoutType m_LayoutType;
    static SceneType m_SceneType;

    static QList<int> m_FilterPortsType;
    static bool m_FilterNodeByConnnection;
    static QList<int> m_FilterNodeType; // {0} :ALL;

    PositionMap positionMap()
    {
        return m_PositionMap;
    }
protected:
    virtual void drawBackground(QPainter *painter, const QRectF &rect);

private:
    QMap<QString, NEBlock*> m_NodeNameToNodeMap;
    QMap<QString, QPointF> m_NodeNameToNodePosforBackup;
    QMap<NENode*, NEBlock*> m_NodeToItemMap;
    QMap<NEBlock*, NENode*> m_ItemToNodeMap;
    QMap<NENode::NEConn*, NEConnectionGEItem*> m_ConnToItemMap;

    Graph m_NodeGraph;
    VertexIndexPropertyMap m_PropertyMap;
    PositionMap m_PositionMap;
    bool m_Initialized;                     /* Set to true after scene file load is finished */

    QMap< VirtexDescriptor, NEBlock *> m_GraphItemMap;
    QMap< NEBlock *, VirtexDescriptor > m_ItemGraphMap;

};



