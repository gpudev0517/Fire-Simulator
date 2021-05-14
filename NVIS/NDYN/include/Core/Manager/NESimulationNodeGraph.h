#ifndef NESIMULATIONNODEGRAPH_H
#define NESIMULATIONNODEGRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/multi_array.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/lookup_edge.hpp>



NE_FWD_DECL_4(class, NENode, NESimulationNode, NESimulationNodeConnection, NESimulationNodePtr)

//
// The DAG for the simulation system evaluation
//

class NECEXP_CORE NESimulationNodeGraph
{
public:
    typedef
    boost::adjacency_list< boost::vecS, boost::vecS, boost::bidirectionalS, NESimulationNode, NESimulationNodeConnection > DAGType;

    typedef boost::graph_traits<DAGType>::vertex_descriptor Vertex;
    typedef boost::graph_traits<DAGType>::edge_descriptor Edge;

public:
    ///
    /// \brief NESimulationNodeGraph
    ///
    NESimulationNodeGraph();
    ~NESimulationNodeGraph();


public:

    ///
    /// \brief addNode
    /// \param n
    /// \param rootCandidate
    /// \return
    ///
    NESimulationNodeGraph::Vertex addNode( NESimulationNode& n , bool rootCandidate );

    ///
    /// \brief removeNode
    /// \param n
    /// \return
    ///
    bool removeNode( NESimulationNode& n );

    ///
    /// \brief addConnection
    /// \param n
    /// \return
    ///
    bool addConnection( NESimulationNodeConnection& n );
    ///
    /// \brief removeConnection
    /// \param n
    /// \return
    ///
    bool removeConnection( NESimulationNodeConnection& n );
    ///
    /// \brief removeConnections
    /// \param n
    /// \return
    ///
    bool removeConnections( NESimulationNode &n );
    ///
    /// \brief removeConnectionsFrom
    /// \param n
    /// \return
    ///
    bool removeConnectionsFrom( NESimulationNode &n );
    ///
    /// \brief removeConnectionsTo
    /// \param n
    /// \return
    ///
    bool removeConnectionsTo( NESimulationNode &n );

    ///
    /// \brief evaluate from internally stored head - top of the node graph tree
    /// \return
    ///
    bool evaluate();

    ///
    /// \brief evaluate downtree from a specific node pointer
    /// \param head
    /// \return
    ///
    bool evaluate( NENode* head );

    ///
    /// \brief evaluate downtree from a specific vertex id in the tree
    /// \param head
    /// \return
    ///
    bool evaluate( Vertex head );

    ///
    /// \brief findNode
    /// \param node pointer to find the vertex associated with it in the node graph
    /// \return
    ///
    NESimulationNodeGraph::Vertex findNode( NESimulationNode& n );
    ///
    /// \brief endEdge
    /// \param n
    /// \return the end of all edges for comparison
    ///
    NESimulationNodeGraph::Edge endEdge();


    ///
    /// \brief findConnection
    /// \param n
    /// \return
    ///
    NESimulationNodeGraph::Edge findConnection( NESimulationNodeConnection &n );
    ///
    /// \brief endNode
    /// \return
    ///
    NESimulationNodeGraph::Vertex endNode();

    ///
    /// \brief head
    /// \return A unique set of all the vertices which constitute as head of tree
    ///
    std::set<Vertex> &head();

    ///
    /// \brief addRoot
    /// \param root
    /// Add the specified Vertex as one of the heads of tree
    ///
    void addRoot(const Vertex &root);

    ///
    /// \brief clearRoot
    /// Clear all the roots
    ///
    void clearRoot();

    void replaceRoot( const NESimulationNodeGraph::Vertex origRoot, const Vertex newRoot );


    bool removeRoot(const NESimulationNodeGraph::Vertex &root);
    bool clear();

    //
    // Traverse the graph connections for nid and add it to the head of the graph if
    // there are no input connections
    //
    bool adjustHead(NESimulationNodeGraph::Vertex nid);

    bool findRoot(const NESimulationNodeGraph::Vertex &vid);

    void printRoots();
protected:
    //int     m_Nodes;

    //std::vector<Vertex>  m_heads;
    std::set<Vertex> m_heads;
    DAGType m_SimDAG;

    std::map< NESimulationNode, NESimulationNodeGraph::Vertex > m_NodeIDMap;

    //tree<NESimulationNode> m_tree;
    bool removeEdge( Edge ei );
};

//
//
// NESimulation Node
//

class  NECEXP_CORE NESimulationNode
{
public:

    typedef NENode* NESimulationNodePtr;

    NESimulationNode( );

    NESimulationNode( NESimulationNodePtr node );

    NESimulationNode( NESimulationNodePtr node, NESimulationNodeGraph::Vertex nodeID );

    NESimulationNode( const NESimulationNode& other );

    ~NESimulationNode();

    bool operator < ( const NESimulationNode& rhs ) const;
    NESimulationNode& operator = ( const NESimulationNode& rhs );

    NESimulationNodeGraph::Vertex NodeID() const;
    void setNodeID( NESimulationNodeGraph::Vertex NodeID );

    NESimulationNodePtr NodePtr() const;
    void setNodePtr(const NESimulationNodePtr nodePtr);

protected:

    NESimulationNodeGraph::Vertex m_NodeID;
    NESimulationNodePtr m_NodePtr;
};

//
// Simulation Connection between nodes
//

class NESimulationNodeConnection
{
public:
    NESimulationNodeConnection( );

    ~NESimulationNodeConnection();

    NESimulationNodeGraph::Vertex& sourceNodeID();

    void setSourceNodeID( NESimulationNodeGraph::Vertex sourceNodeID );

    NESimulationNodeGraph::Vertex& destNodeID();

    void setDestNodeID( const NESimulationNodeGraph::Vertex destNodeID);

    NESimulationNodeGraph::Edge connectionID() const;
    void setConnectionID(const NESimulationNodeGraph::Edge &connectionID);

protected:

    NESimulationNodeGraph::Edge   m_connectionID;
    NESimulationNodeGraph::Vertex m_sourceNodeID;
    NESimulationNodeGraph::Vertex m_destNodeID;
};

#endif // NESIMULATIONNODEGRAPH_H
