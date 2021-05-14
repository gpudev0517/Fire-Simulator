#pragma once


#include "NEObject.h"
#include "Utility/NEFrameInfo.h"

NE_FWD_DECL_5(class, QtVariantPrperty, NECurve, NE_PRIVATE(NENode), NEColorRamp, NENodeFactory)
NE_FWD_DECL_2(struct, NEUserEvent, NEOsdMsg)

/*! \brief NENode class is the base class for scene instantiateable nodes in Neutrino.
 *
    The class provides subsystems for:
    \list
        \li High level node-node connections.
        \li Scriptable property-property connections.
        \li Scriptable property callbacks.
        \li Scriptable per-frame property evaluation expressions.
        \li Internal connections not changable by the user.
        \li Keyframes and interpolations
        \li Per-node evaluation for DAG nodes
*/
class NECEXP_CORE NENode : public NEObject
{
    Q_OBJECT
    NE_CLASS("Node State")

public:
    //! Possible node states.
    enum NENodeState { Inactive = 1, Active = 2, Cache = 4 };
    enum SceneType
    {
        ALL = 0,
        Modeling = 1,
        Simulation = 2,
        Visualization = 3
    };

    NE_PROP_RW(NENodeState, Behavior, "Behavior", "Behavior of the node, cache means just load data, others should be self explanatory.", true, virtual)
    NE_PROP_RW(bool, CacheData, "Cache Data", "Behavior of the node, cache means just load data, others should be self explanatory.", true, )
    NE_PROP_RW(NEFrameInfo, FrameInfo, "FrameInfo", " ", false,)

    Q_ENUMS(NENodeState SceneType)



public:
    //! Structure for keeping Qt-based internal connections.
    struct NEIntConn
	{
        NEIntConn(QObject* Sender, QString Signal, QObject* Receiver, QString Slot, Qt::ConnectionType Type = Qt::AutoConnection):
			sender(Sender), signal(Signal), receiver(Receiver), slot(Slot), type(Type) { }

        QObject* sender;    /**< Signal sender */
        QString signal;     /**< Signal name */
        QObject* receiver;  /**< Signal receiver */
        QString slot;       /**< Slot name */
        Qt::ConnectionType type;    /**< Type of the connection */
	};

    //! Structure for keeping high level node-node connections.
    struct NEConn
	{
        NENode* source;     /**< The Source node */
        uint    sIndex;
        NENode* target;     /**< The target node */
        uint    tIndex;
        NE::IoTypes type;   /**< Type of the connection \sa NE::IoTypes */
        int order;          /**< The order of the connection */
	};

    //! Structure for keeping property to property connections based on expressions.
    struct NEPropConn
	{
        NENode* source;     /**< The source node */
        NENode* target;     /**< The target node */
        QString srcProp;    /**< The source property */
        QString trgProp;    /**< The target property */
        QString expr;       /**< Python expression of the connection */
        int order;
	};

    //! Structure for bookkeeping property to property connections based on expressions for target nodes.
    struct NESrcPropConn
    {
        NENode* target;     /**< The target node */
        QString srcProp;    /**< The source property */
        QString trgProp;    /**< The target property */
        QString* expr;      /**< Python expression of the connection */
        int order;
    };

    //! Constructor does member variable initialization
    NENode(NENodeFactory* factory);
    NENode* copy();

    //! Destructor emits deleteObject signal always
    virtual ~NENode();

    //! Node initialization function, should be called from all inheriting classes.
    virtual uint init(uint initMode = NE::kInitOnCreate);
    //! Node post-initialization function, should be called just after the init function.
    virtual uint postInit();

    //! Returns true if the node is initialized, false otherwise
    const bool& nodeInitialized() const;

    //! Evaluates the node. Reserved for the nodes that can work as part of a node graph.
    virtual uint eval();

    //! Updates (usually interpolates) properties depending on the elapsedTime
    void updateProperties(uint currentFrame, double elapsedTime);

    //! This is for updating a property before computing some other property
    bool updateProperty(QString propName);

    //! Returns the derivative of property. Useful for keyed properties.
    float propertyDerivative(QString prop, uint currentFrame, double elapsedTime);
    //! Returns the derivative of vec3f property. Useful for keyed properties.
    vec3f propertyDerivativeVec3f(QString prop, uint currentFrame, double elapsedTime);
    //!  Returns the derivative for euler angle property. Useful for keyed properties.
    vec3f propertyDerivativeEulerf(QString prop, uint currentFrame, double elapsedTime);

    //! Removes all property based and node based connections from/to the node
    void removeAllConnections();

    //! Clones the properties and connections of the passed node to this node
    void cloneNode(NENode *clone);

    static QList< QString > g_NodeGraphSceneInherits[3];
    static bool addNodeTypeToNodeGraphSceneType(QString nodeInherit, SceneType sceneType);

public slots:
    //! Returns the input types flag of the node.
    uint inputs( uint inputNum = 0 ) const;

    //! Returns the number of inputs of the node
    uint numInputs() const;

    //! Set the number of inputs to the node
    void setNumInputs( uint numInputs );

    //! Sets the input types flag of the node
    void setInputs(uint val, uint inputNum = 0);

    //! Returns the output types flag of the node.
    uint outputs(uint outputNum = 0) const;

    //! Sets the output types flag of the node
    //! Returns the number of outputs of the node
    uint numOutputs() const;
    void setOutputs(uint val,  uint outputNum = 0);

    //! Set the number of outputs to the node
    void setNumOutputs( uint numOutputs );

    //! Returns a pointer to the factory class for this node type
    NENodeFactory& factory() const;

    //! Returns whether two nodes are connectable or not
    static bool areConnectable(NENode* from, NENode* to, uint type, uint sIndex, uint tIndex);

    //! Returns the node based connections map for the node
    QMap<uint,NEConn>& connections();

    //! Returns the property based connections map for the node
    QMap<QString, QMap<NENode*,NEPropConn>>& propertyConnections();

    //! Returns the source connections map for the node
    QMap<uint,NEConn>& sourceConnections();

    //! Returns the source property connections map for the node
    QMap<QString, NESrcPropConn>& sourcePropertyConnections();

    //! Connects this node to the target node if the target inputs the given type
    virtual bool connectTo(NENode* target, uint type, uint sIndex = 0, uint tIndex = 0, int order = 0);

    //! Connects this node's source property to the the target node's target property if property types types are compatible
    bool connectTo(NENode* target, QString sourceProperty, QString targetProperty, QString expr = QString(), int order = 0);

    //! Disconnects this node from the source node for a specified connection type
    virtual bool disconnectFrom(NENode* source, uint type, uint sIndex, uint tIndex);

    //! Disconnects this node's source property from the source node's target property
    bool disconnectFrom(NENode* target, QString sourceProperty, QString targetProperty);

    //! Shows a dialog which queries for caching mode
    virtual void editCachingMode();

    //! This is overridden to put some restrictions to possible object names
    virtual void setObjectName(const QString& name);

    //! Adds a keyframe at the currentFrame location for a property
    void addKeyframe(uint currentFrame, const QString& propertyName, QVariant v, NE::NECurveType type = NE::kLinear, NECurve* curve = nullptr);

    //! Adds a keyframe at the currentFrame location for a list of properties
    virtual void addKeyframe( uint currentFrame, const QStringList& propertyNames, NE::NECurveType type = NE::kLinear, NECurve* curve = nullptr);

    //! Removes the keyframe at the currentFrame location for a property
    void removeKeyframe(uint currentFrame, const QString& propertyName);

    //! Removes keyframes at the currentFrame location for a list of properties
    void removeKeyframe(uint currentFrame, const QStringList& propertyNames);

    //! Removes all keyframes for this NENode
    void removeAllKeyframes();

    //! Removes keyframes for a given property
    void removeKeyframesForProperty(QString& propertyName);

    //! Moves a keyframe to another frame
    void moveKeyframe(uint oldFrame, uint newFrame, QString propertyName);

    //! Returns whether a property with the given name has ever keyed or not
    bool isKeyed(QString prop);

    //! Returns keyframes numbers
    QList<uint> getKeyframes();

    //! Returns the value of a keyframe for a keyed propery for a given frame
    QVariant getKeyframeValue(QString* prop, uint currentFrame);

    //! Returns the keyframes for a given property
    QList<uint> getKeyframesForProperty(QString* prop);

    //! Returns the keyframes where all spatial transform properties are keyed
    QList<uint> getTransformKeyframes();

    //! Returns a list of keyed properties
    QStringList getKeyedProperties();
    QMap<QString, QVariant> getKeyedProperties(uint keyframe);
    QMap<QString, QMap<uint, QVariant>>& getKeyData();
    QMap<uint, double>& getKeysAndTimes();

    //! Removes all expressions of the node
    void removeAllPropertyUpdateExpressions();

    //! Set the given python expression for the property
    void setPropertyUpdateExpression(QString prop, QString expr);

    //! Removes the expression of a property
    void removePropertyUpdateExpression(QString prop);

    //! Returns the python expression set for the property
    QString propertyUpdateExpression(QString prop);

    //! Checks whether the given property has any expression set or not
    bool hasUpdateExpression(QString prop);

    //! Returns the map of the property expressions
    const QMap<QString, QString>& propertyUpdateExpressionsMap();

    //! Removes all expressions of the node
    void removeAllPropertyCallbackExpressions();

    //! Set the given python expression for the property
    void setPropertyCallbackExpression(QString prop, QString expr);

    //! Removes the expression of a property
    void removePropertyCallbackExpression(QString prop);

    //! Returns the python expression set for the property
    QString propertyCallbackExpression(QString prop);

    //! Checks whether the given property has any expression set or not
    bool hasCallbackExpression(QString prop);

    //! Returns the map of the property expressions
    QMap<int, QString> &propertyCallbackExpressionsMap();

    //! Returns the map of incoming internal connections
    QMultiMap<QObject* , NEIntConn> &incomingInternalConnectionsMap();

    //! Returns the map of outgoing internal connections
    QMultiMap<QObject* , NEIntConn> &outgoingInternalConnectionsMap();

    //! Resets all properties of the node, should be called by inheriting members
    virtual void resetProperties();

    bool isShowFlag();
    void setShowFlag(bool);
    //! For generic editing of caching modes.
    virtual int primitiveGroupIndexOfEnum();
    virtual int primitiveDataTypesIndexOfEnum();

    //! Set the Information about cached frames for the spatial
    void SetFrameInfo(uint firstFrame, uint lastFrame, bool completelyCached);

    //! Cleans all cached data of this entity and clears the current data like physical particles
    virtual uint cleanData(QString path);
    virtual uint cleanData();
    virtual uint cropData(QString path, uint fromFrame, uint toFrame, bool moveToZeroFrame=false);

        //! Updates the cache information so that the given frame is also included
    void updateFrameCachingInformation(int frameNumber);

    //! Crops the frameCacheInformation in the range fromFrame toFrame (at the beginning or at the end)
    uint cropFrameCachingInformation(uint fromFrame, uint toFrame, bool moveToZeroFrame);

    //! Crops the cache files in the range fromFrame toFrame (at the beginning or at the end)
    uint cropCacheFiles(QString dirName, uint fromFrame, uint toFrame, bool moveToZeroFrame);

    //!< save properties to restore them later
    virtual void saveProperties();

    //!< restore the properties from saved values
    virtual void restoreProperties();

    //!< Called when archiving the entire scene is called
    virtual void archive();

    //! Imports from file if needed
    virtual void import(QString geoFileName);

    //! Saves the loading of data for the current simulation/animation frame.
    virtual uint saveFrameData(QString path, const int frameNumber);

    //! Calls the loading of data for the current simulation/animation frame.
    virtual uint loadFrameData(QString path, const int frameNumber);

    //!This flag decides whether the primitives are internally stored in the file,or stored externally in a folder (not functional yet)
    static const uint kSectionExternal();

    //!This flag decides if all primitives are stored or not.
    static const uint kSectionAll();


    virtual QString toJson();
    virtual void fromJson( QString& obj );

    void setPropertiesFromJSON(QString model3DInfoStr);
    QJsonArray getJSONFromProperties();

signals:
    void deleteObject(const QString&);
    void connectionRemoved(NENode* source, NENode* target, uint type);
    void connectionsRemoved(NENode* node);
    void propertyConnectionRemoved(NENode* source, NENode* target, QString sourceProperty, QString targetProperty);
    void propertyConnectionsRemoved(NENode* node);
    void updateExpressionInspector();
    void updateNodeConnectionsTable();
    void updatePropertyConnectionsTable();
    void updatePropertyCallbacksTable();
    //    void updateNodeRename( NENode *node );
    void pushUserEvent( NEUserEvent *ue );


protected:
    //! Registers an NENode as a source of a connection, should be overridden by child classes that make connection and should be called from there
    virtual bool addSource(NENode* source, NEConn& conn);

    //! Unregisters an NENode as a source of a connection, should be overridden by child classes that make connection and should be called from there
    virtual void removeSource(NENode* source, uint type, uint sIndex, uint tIndex);

    //! Registers a property based connection
    void addSource(NENode* source, QString sourceProperty, QString targetProperty, QString *srcExpr);

    //! Unregisters a property based connection
    void removeSource(NENode* source, QString sourceProperty, QString targetProperty);

    //!	Initializes GUI actions of the node, should be called from inheriting classes
    virtual void initializeGUIActions();

    //! Returns true if the node connection of this type is unique, i.e. must not exist more than once.
    static bool uniqueConnection(NENode* oldTarget, NENode* newTarget, uint type);

    //! Disables keyframe interpolation for a specific property
    void disableInterpolation(QString propertyName);

    //! Re-enables interpolation for a specific property
    void enableInterpolation(QString propertyName);

    //! Evaluates the expression for the given property
    void evalPropExps(int index);
    void evalPropExps(QString propertyName);

    bool addSourceNodeGraph(NENode *source);
    bool removeSourceNodeGraph(NENode *source);

    //! Returns the path were the solver stores its content
    virtual QString savePath();

private:
    NE_DECL_PRIVATE(NENode)
};



Q_DECLARE_INTERFACE(NENode, "NENode")

//! Calls to Qt's QObject::connect function to keep track of internal connections
#define NE_CONNECT(_From_, _Type1_, _Signal_, _To_, _Type2_, _Slot_)\
{\
	QMetaObject::Connection _C_ = QObject::connect(_From_, &_Type1_::_Signal_, _To_, &_Type2_::_Slot_);\
	if(_C_)\
	{\
		_From_->outgoingInternalConnectionsMap().insertMulti(_To_, NENode::NEIntConn(_From_, #_Signal_, _To_, #_Slot_, Qt::AutoConnection));\
		if(_To_->inherits("NENode"))\
			((NENode*)_To_)->incomingInternalConnectionsMap().insertMulti(_From_, NENode::NEIntConn(_From_, #_Signal_, _To_, #_Slot_, Qt::AutoConnection));\
		if(UpdateUI())\
			updateInternalConnectionsTable();\
	}\
}

//! Call Qt's QObject::disconnect function to keep track of internal connections
#define NE_DISCONNECT(_From_, _Type1_, _Signal_, _To_, _Type2_, _Slot_)\
{\
	bool _B_= QObject::disconnect(_From_, &_Type1_::_Signal_, _To_, &_Type2_::_Slot_);\
	if(_B_)\
	{\
		_From_->outgoingInternalConnectionsMap().remove(_To_);\
		if(_To_->inherits("NENode"))\
			((NENode*)_To_)->incomingInternalConnectionsMap().remove(_From_);\
		if(UpdateUI())\
			updateInternalConnectionsTable();\
	}\
}








