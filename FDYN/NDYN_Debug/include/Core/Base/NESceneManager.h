#pragma once

#include "Utility/NEFrameInfo.h"
#include "NEObject.h"


NE_FWD_DECL_6(class, NEBase, NENode, NECollisionManager, NEGeometryManager, NEDataFieldManager, NEPostProcessorManager)
NE_FWD_DECL_2(class, NEDataFieldManager, NEManager )


NE_FWD_DECL_3(class, NESpatial, NEManager, NEGravity)
NE_FWD_DECL_5(class, NESpatialLayer, NESpatialGroup, NESpatialLayerFactory, NESpatialGroupFactory, NE_PRIVATE(NESceneManager))

NE_FWD_DECL_2(struct, NEUserEvent, NEOsdMsg)

NE_FWD_DECL_1( class, NESimulationNodeGraph )

//! The scene manager class is the central class for controlling scene related things.
/*!
	It controls the pipeline of the whole scene update. Derefers tasks like the creation of
	objects to the corresponding managers.
*/
class NECEXP_CORE NESceneManager final : public NEObject
{
    Q_OBJECT
    NE_CLASS("Scene Manager")

public:

	enum NETimelineUnits { Frames, Seconds };

    NE_PROP_RW(double, ElapsedTime, "Elapsed Time", "The elapsed simulation time.", true,)
    NE_PROP_RW(double, RecordFrameRate, "Frame Rate", "Specifies the number of frames to be written/read per second.", true,)
    NE_PROP_RW(int, CurrentFrame, "Current Frame", "The current frame on the timeline.", true,)
    NE_PROP_RW(int, ResetFrame, "Reset Time", "The Reset time which clears all the nodes", true,)
    NE_PROP_GROUP_START(Timeline, "Timeline Properties", "Timeline Properties", true)
        NE_PROP_RW(int, PlaybackStartFrame, "Playback Start Frame", "The start frame where the playback starts.", timeLineUnitsIsFrame,)
        NE_PROP_RW(int, PlaybackEndFrame, "Playback End Frame", "The end frame where the playback stops.", timeLineUnitsIsFrame,)
        NE_PROP_RW(int, SimulationEndFrame, "Simulation End Frame", "The end frame where the simulation stops.", timeLineUnitsIsFrame,)
		NE_PROP_RW(double, PlaybackStartTime, "Playback Start Time", "The start time where the playback starts.", timeLineUnitsIsTime,)
		NE_PROP_RW(double, PlaybackEndTime, "Playback End Time", "The end time where the playback stops.", timeLineUnitsIsTime,)
		NE_PROP_RW(double, SimulationEndTime, "Simulation End Time", "The end time where the simulation stops.", timeLineUnitsIsTime,)
    NE_PROP_GROUP_END(Timeline)
    NE_PROP_RW(bool, CacheAllPhysicals, "Cache All Physicals Frame", "Switches all physics related nodes to cache mode.", true,)
    NE_PROP_RW(NEFrameInfo, FrameInfo, "", "", false,)
    NE_PROP_RW(bool, ForceGLUpdate, "Frequent Viewport Updates", "When set to true, the OpenGL viewports are updated in each simulation step instead of each frame.", false,)

    Q_ENUMS(NERBDSolver)


public slots:

    //! Returns whether the scene manager is initialized or not
    bool initialized() const;

    //! Returns whether the scene properties are initialized or not
    bool propertiesInitialized() const;
    //! Sets the boolean sorting whether the scene properties are initialized or not
    void setPropertiesInitialized(bool val);

    //! Returns a reference to the list of all spatial objects in the scene
    const QMap<uint, NESpatial*>& spatials();

    //! These functions set/remove key-frame for the selected spatial
    void setKeyFrameForSelectedNode(const QStringList& propertyNames, NE::NECurveType type);
    void removeKeyForSelectedNode(const QStringList& propertyNames);

    //! Returns the current frame set on the timeline
    int getCurrentFrame() const;

    //! Updates the frame caching information by including the supplied frame number
    void updateFrameCachingInformation(int CurrentFrame);

    //! Duplicates the clone node of type and gives the new Node the provided name.
    NENode* duplicateObject(NENode* clone, QString type, QString nameNewNode);

    //! Find the child object with the given name and return. If child is not found, returns 0. Works recursively
    NENode* findChildNode(QString name);

    //! Deletes an object and removes all dependencies.
    void deleteObject(NENode* object, bool removeData);

    //! Deletes all objects and removes their dependencies
    void deleteAllObjects();

    //! How many OpenGL updates should be done per second
    double renderFrameRate() const;
    void   setRenderFrameRate(double fps);

    //! If true we are in simulation mode, if false we are either in playback or idle mode
    bool solveMode() const;

    //! Sets the simulation mode .
    void setSolveMode(bool b);

    //! Sets offline solve mode
    void setSolveModeOffline(bool b);

    //! Returns whether playback repeat mode is enabled
    bool repeatFlag();
    //! Sets playback repeat mode
    void setRepeatFlag(bool b);

    //! Starts the timer that periodically calls NEScene::update()
    void startUpdateTimer(bool b);
    //! This method is called whenever the number in currentFrameLineEdit is changed from outside of the program.
    void setCurrentFrame(int val);

    //! Create visualizations/renders
    void renderOffline();

    //! Resets values like elapsedTime. TODO: Also reset the nodes' properties
    uint resetScene();
    //! Deletes the frames earlier than firstPlaybackFrame, and shifts the timeline to 0
    void cropLeft(bool moveToZeroFrame = true);

    //! Deletes the frames later than lastPlaybackFrame
    void cropRight();

    //! Main update loop of the animation and simulation called during simulation run
    uint update();

    //! Duplicates all selected objects called with ctrl+d for example, see initializeGUIActions
    uint duplicateSelectedObjects();

    //! Resets all values and deletes all saved data - when user clicks reset button
    uint resetAndCleanScene();
    //! Cleans all the data for all spatials
    uint cleanData();

    //! Creates a new object.
    NENode* createObject(QString type, QString name, const NEObject* parent = 0);
    //! When a scene is loaded, this function is called for object creation
    bool createObjectOnLoad(QString type, QString name, const NEObject* parent = 0);

    //! Deletes an object and removes all dependencies.
    void deleteObject(QString type, const QString& name, bool removeData);

    //! Initializes the children when the scene is loaded
    bool initChildrenOnLoad();

    //! Object grouping - multiple spatials can be grouped into one group node
    NESpatialGroup *groupObjects(QList<NENode*> objs);
    void ungroupNode(NESpatialGroup* group);

    //! Object parent
    void parentObjects(QList<NENode*> objs, NESpatialGroup* group);
    void unparentObjects(NENode* node,NESpatialGroup* group);

    //! Starts/Stops simulation update in a separate thread
    void startSimulation();
    void stopSimulation();
    void exitSimulation();

public:
     static NESceneManager& Instance();
	 bool timeLineUnitsIsFrame() const;
	 bool timeLineUnitsIsTime() const;

     QMap<QString, NEManager*>& managers();

     uint preInit();

     //! Initialization of scene manager which initializes scene items
     uint init();

     //! Do some last initialization at a later time
     uint postInit();


     //! deinitialization
     uint deInit();

     void setExportOfflineRendering(bool b);
     bool exportOfflineRendering();
     

     //! Returns whether there is a simulation/playback in progress
     bool isUpdating();

     //! Computes the frame number from the elapsed time using the record frame rate
     int computeFrameNr(double elapsed_Time);

     //! When the user hits the next frame button, additional preparations have to be performed when in simulation mode.
     void incrementFrame();
     //! When the user hits the previous frame button, additional preparations have to be performed when in simulation mode.
     void decrementFrame();

     //! Returns the current selected object
     NEObject* selectedObject() const;

     //! Returns the current selected NENode. (Warning: Performs a dynamic cast, which has non-constant runtime).
     NENode* selectedNode() const;

     //! Selects the given object
     void selectObject(NEObject* val);
     QMap<NEObject*, NEObject*>* selectedObjects();

     //! Appends the given object to the already selected objects list
     void selectAdditionalObject(NENode* val);

     //! Returns the version of the scene file
     float sceneVersionNumber();

     //! Paths to the output directories
     QString simDataSaveDirectory(QString parent) const;
     QString simDataSaveDirectory() const;
     QString renderDataSaveDirectory(QString parent) const;
     QString renderDataSaveDirectory() const;
     QString visualizationSaveDirectory(QString parent) const;
     QString visualizationSaveDirectory() const;
     QString glFramesSaveDirectory(QString parent) const;
     QString glFramesSaveDirectory() const;
     QString measurementsDirectory(QString parent) const;
     QString measurementsDirectory() const;
     QString sceneGeometryDirectory(QString parent) const;
     QString sceneGeometryDirectory() const;

     std::shared_ptr< NEDataFieldManager> dataFieldManager() const;
     void setDataFieldManager( std::shared_ptr< NEDataFieldManager> pManager );

     std::shared_ptr<NEPostProcessorManager>  postProcessorManager() const;
     void setPostProcessorManager(std::shared_ptr<NEPostProcessorManager> pManager );

     std::shared_ptr<NEGeometryManager> geometryManager();
     void setGeometryManager(std::shared_ptr<NEGeometryManager> pManager );


     AABB getSceneAABB(); //!< Returns scene size
     AABB getSceneAABBForShadow();



     void prepareSceneForSimulation();
     NESimulationNodeGraph* NodeGraph() const;

     //! clear the simulation node graph
     void clearNodeGraph();

     void cleanup();
     void updateKeyFramedProperties();

     double loadFrameData(bool newFrame = true);
     double saveFrameData(bool newFrame = true);

protected:
     //! Initializes right-click actions for the scene manager
     virtual void initializeGUIActions();


signals:

    void postSimInit();

    void createRenderer( NESpatial* spatial );

    void updateKeyframeEditor();
    void setTimelineKeyframeMarks(QList<uint> list);
    void updatePropertyConnectionsTable();
    void updateSceneInspector();
    void updateNodeGraphEditor();
    void updatePropertyEditor();
    void updateGLWindows();
    void startedOfflineRendering();
    void updateOfflineRenderer();
    void finishedOfflineRendering();

    void updateOfflineRender();
    void updateOfflineProgress(float);
    void startedOfflineProcess();
    void finishedOfflineProcess();

    void repaintTimeline();
    void resetWidgets();

    void clearBuffers();
    void triggerSavingData();
    void updateCharts();
    void pushUserEvent(NEUserEvent* e);

    void selectObjectUI( QObject* obj );

    void updateWidgetsForCreation();
    void updateWidgetsForDeletion( QObject* object);
    void objectCreated( NENode* object );
    void objectRemoved( NENode* object );

    void setPlay( bool val );
    void increment();

    void updateCollider();
    void resetCollider();
    void clearCollider();

    void updateDataCollector();
    int simulateStartFrame();
    int simulateEndFrame();
    int playbackStartFrame();
    int playbackEndFrame();

    void setRepeat( bool val );
    void setSimulate( bool val );

    uint initSimManager();
    void deInitSimManager();

    void initSimSceneElements();

    void initRenderSceneElements();
    void initRenderSettings(QObject *parent);
    void initRenderManager();
    void initRenderer();

    void deInitRenderer();
    void deInitRenderSettings();
    void deInitRenderManager();
    void deInitMeshVolGeometryManager();

    void updateRenderManager();

    void setOutputRenderSettings( QString dir );

    void managersCreated( QList<std::shared_ptr< NEManager > > managers ) ;

    void makePhysicalDefaultConnection( NENode *node );
    void setPhysicalDefaultConnections( bool connections );
    void preparePhysicalsForSimulation( bool m_TimeLineEditedByUser );
    void simulate();

    void updateSolidForces();

    void resetValues();
    void writeFrameInfo( QString dataPath, uint frame );
    void loadFrameSimInfo( QString dataPath );
    void loadFrameSimData( QString dataPath, uint frame );
    void saveFrameSimData( QString dataPath, uint frame );
    void cropSavedSimData( QString dataPath, uint fromFrame, uint toFrame, bool moveToZeroFrame );

    void cleanSavedData( QString dataPath );


private:
    NE_DECL_PRIVATE_SINGLETON(NESceneManager)
    FRIEND_NE_CLASS_PRIVATE(NEBase)

    NE_FRIENDS_2(class, NEManager, NEManagerPrivate)

};



#define NESCENE NESceneManager::Instance()
