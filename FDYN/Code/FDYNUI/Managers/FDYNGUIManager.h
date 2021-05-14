#pragma once

#include "NEObject.h"
#include "Utility/NEExternalProperties.h"
#include "PropertyManager/NEPropertyObject.h"
#include "Base/NEGUICallInterface.h"
#include "Utilities/FDYNTreeItem.h"

#include "Dialogs/MaterialSelector/FDYNFireMaterialDialog.h"
#include "Dialogs/MaterialSelector/FDYNSurfaceMaterialDialog.h"
#include "Dialogs/CriteriaSelector/FDYNFailureCriteriaDialog.h"
#include "Dialogs/VentCriteriaSelector/FDYNVentCriteriaDialog.h"
#include "Dialogs/CableSelector/FDYNCableDialog.h"

#include "Geometry/FEngine/FFRANXProperties.h"
#include "Geometry/FEngine/FZone.h"

#include "Utility/NESurfaceMaterial.h"
#include "Utility/NEFireMaterial.h"
#include "Utility/NEFailureCriteria.h"
#include "Utility/NEVentCriteria.h"
#include "Utility/NECableProperty.h"
#include "Utility/NECustomVariants.h"
#include <QList>

NE_FWD_DECL_3(class, FDYNMainWindow, NENode, NERenderSetting)
NE_FWD_DECL_3(class, NE_PRIVATE(FDYNGUIManager),NE_PRIVATE(FDYNGUIManagerUserProps), FDYNGUIManager)
NE_FWD_DECL_1(struct, NEUserEvent)
NE_FWD_DECL_1(class, NENodeFactory)
NE_FWD_DECL_1(class, NEManager)
NE_FWD_DECL_1(class, FDYNPropertyEditorWindow)
NE_FWD_DECL_1(struct, NEPrimitivePolygon)

struct aiScene;
class NENode;
class NEZone;
class NEScenario;
class NEZoneItem;
class NEZoneCompartment;

class FDYNGUIManagerUserProps : public NEPropertyObject
{
public:
    enum NEGUIItemSize { Smallest, Smaller, Small, Normal, Large, Larger, Largest };

    Q_OBJECT
    NE_CLASS("UI Properties")
    NE_PROP_RW(bool, DarkTheme, "Dark Theme", "Dark theme", true,)
    NE_PROP_GROUP_START(Fonts, "Fonts", "Fonts", true)
    NE_PROP_RW(NEGUIItemSize, NormalFontSize, "Normal Font Size", "Size of the normal font (unit: pt.)", true,)
    NE_PROP_RW(NEGUIItemSize, SmallFontSize, "Small Font Size", "Size of the small font (unit: pt.)", true,)
    NE_PROP_RW(NEGUIItemSize, LargeFontSize, "Large Font Size", "Size of the large font (unit: pt.)", true,)
    NE_PROP_RW(NEGUIItemSize, OSDFontSize, "OSD Font Size", "Size of the OSD font (unit: pt.)", true,)
    NE_PROP_GROUP_END(Fonts)
    NE_PROP_RW(NEGUIItemSize, ToolbarSize, "Toolbar Size", "Size of the toolbars", true,)

    Q_ENUMS(NEGUIItemSize)

    public:
        FDYNGUIManagerUserProps(FDYNGUIManager* s);
    ~FDYNGUIManagerUserProps();

private:
    FDYNGUIManager* S;
    NE_DECL_PRIVATE(FDYNGUIManagerUserProps)
};\


//!  Manages all Graphical User Interface related things
/*!
    Its responsibilities include managing all UI elements, populating menus
    and connecting signals and slots.
*/
class NECEXP_FDYNUI FDYNGUIManager final :
        public NEObject,
        public NEExternalProperties<FDYNGUIManagerUserProps>,
        public ICallInterfaceFromBaseToGUI
{
    Q_OBJECT
    NE_CLASS("GUI Manager")

    public:

    enum FDYNTimelineUnits { Frames, Seconds };

    NE_PROP_RW(FDYNTimelineUnits, TimelineUnits, "Timeline Units", "Set Timeline Units", true,)

    NE_FWD_DECL_6(friend class, Singleton<FDYNGUIManager>, NEManager, NE_PRIVATE(NEBase), NE_PRIVATE(NESceneManager), NEDockWidget, NEGLWidget)
    NE_FWD_DECL_6(friend class, NEGLWidgetProperties, FDYNMainWindow, NETimelineWidget, FDYNQtTreePropertyBrowser, NEKeyframeEditor, NEKeyframeInspector)
    NE_FWD_DECL_5(friend class, NEGLMainWindow, NERenderManager, NESceneManager, NEBase, NEManagerPrivate)

    Q_ENUMS(FDYNTimelineUnits)

    public:

    static FDYNGUIManager& Instance();

    // Interface functions for bridge between NEBASE and NEGUI
    virtual void updateInitFlagForNodeGraph(bool aInitFlag);
    virtual void restoreGUIState( QXmlStreamReader* xmlReader );
    virtual void updateNodeRenameForNodeGraph(const QString &oldName, const QString &newName);
    //! Updates all related GUI when a node is renamed
    virtual void updateNodeRenameToAllGUI(NENode* node);
    //! Returns a stream for logging
    virtual QDebug& log();
    //! Pushes the log message inside the stream into the messages window
    virtual void endLog(NE::NELogType type);
    //! Evaluates a python script for GUI->Python interaction
    virtual void EvalPyGUI(const QString& script);
    virtual void updateGLWindowsFromBase();
    virtual void addCameraIntoContextMenus(NENode *node);
    virtual void removeCameraFromContextMenus(NENode *node);

    virtual void removeItem( NEZoneItem *zoneItem, bool deleteitem );
    //! Resize all GL windows
    virtual void resizeGLWindows();

    virtual bool event(QEvent *e);

    //! file dialog
    virtual QStringList fileImportBrowser( FilePath& filePath, QString orgName, QString appName, QString settingsName );
signals:

    void sendImportFRANX( QString fileName );
    void sendImportPDMS( QString fileName );

    void setActiveZone( QString az );
    void setActiveScenario( QString as );

    void componentCreated( NEZoneItem *, NENode *);

    void itemCreated( NEZoneItem *, NENode *);
    void itemDeleted( NEZoneItem *, bool );

    void trayCreated(NEZoneItem *, NENode *);
    void cableCreated(NEZoneItem *, NENode *);

    void sendScenarioCreated( NEScenario *);
    void sendScenarioDeleted( NEScenario *);

    void zoneCreated( NEZone *);
    void zoneCompartmentCreated( NEZone*, NEZoneCompartment* );

    void sendDuplicateObject( NEZoneItem *);
    void sendCopyProperties( NEZoneItem * );
    void sendPasteProperties( QList< NEZoneItem *>, QString filter );

    void sendLoadDB(QString filename);
    void sendSaveDB(QString filename);
    void sendSaveAsDB(QString filename);
    void sendNewDB(QString filename);
    void sendImportDB( QString dbfile, QString franxfile, QString pdmsfile );
    void sendQuitDB();


    void sendUpdateWorkingFolder(QString settings);
    void sendUpdateUIFolder(QString settings);

    void sendSetRecoverData(bool recover);

    void saveAs( NENode *node, QString name );

    void sendAddItemToScenario( NEZoneItem* zone , NENode *parent );
    void sendRemoveItemFromScenario( NEZoneItem *zoneItem, NENode *parent = 0 );
    void sendRemoveItemFromScenarios( NEZoneItem *zoneItem, QList<NENode *>parents );
    void sendNodeRenamed( NENode* node, QString name );

    void sendComputeFranx();

    void dropCalled(matrix44f view, matrix44f proj, double, double, double, double, void *);

    void sendAddNewZoneAndScenario(NEZone* zone, NEScenario* scenario);
    void sendAddNewScenario(NEScenario* scenario);

    void sendAddFireDesc( NENode* node );

    void sendAddVent( NENode *node );
    void sendTurnIntoVent(NENode* prevNode, NENode* newNode);
    void sendTurnIntoMechVent(NENode* prevNode, NENode* newNode);

    void sendConvertToSource(NENode* prevNode, NENode* newNode);
    void sendConvertToItem(NENode* prevNode, NENode* newNode);

    void sendAddFireDetection( NENode* node );

    void sendAddCompartment( NENode *node, NENode *parent );

    void sendLoadDefaultSurfaceMaterials();
    void sendAddSurfaceMaterial( NESurfaceMaterial surfMat );
    void sendRemoveSurfaceMaterial( NESurfaceMaterial surfMat );
    void sendSurfaceMaterialChanged( NESurfaceMaterial surfMat );
    void sendAssignSurfaceMaterial( NENode *node, NESurfaceMaterial sm, QString materialKind );

    void sendAddFireMaterial( NEFireMaterial fireMat );
    void sendRemoveFireMaterial( NEFireMaterial fireMat );
    void sendAssignFireMaterial( NENode *node, NEFireMaterial fm );

    void sendAddFailureCriteria( NEFailureCriteria failure );
    void sendRemoveFailureCriteria( NEFailureCriteria failure );
    void sendAssignFailureCriteria( NENode *node, NEFailureCriteria failure );

    void sendAddVentCriteria( NEVentCriteria criterion );
    void sendRemoveVentCriteria( NEVentCriteria criterion );
    void sendAssignVentCriteria( NENode *node, NEVentCriteria criterion , QString criterionKind);

    void sendAddCableProperty( NECableProperty failure );
    void sendRemoveCableProperty( NECableProperty failure );
    void sendAssignCableProperty( NENode *node, NECableProperty failure );

    void sendSetDynamicCombo(NENode* node, NEDynamicCombo dynamicCombo, QString propName);

    void sendChangeColor(double elepsedTime);

    void sendSimulateScenario( QString name, QString desc, QString simEngineName );
    void sendUpdateComputeScenario( FRANXProperties fp );

    void sendLoadUIOption( bool loaduidata );
    void sendSaveUIOption( bool saveuidata );

    void sendLoadUIData( );
    void sendSaveUIData( QString uivalues );

public slots:
    //! Initialization
    void init();

    //! Updates only the title of the property editor
    void updatePropertyEditorTitle(QString val);
#ifdef OLD_PROPERTY
    //! Hides the object from the other property editors
    void hideObjectFromOtherPropEditors(QObject* obj, FDYNPropertyEditorWindow* pe);
#endif

    //void slotDropped(QVariant vx, QVariant vy, QVariant vArea);

    //! Updates all GL windows
    void updateGLWindows();

    void updateOfflineRenderer();
    void updateOfflineProgress(float val);
    void finishedOfflineProcess();
    void startedOfflineProcess();

    void startedOfflineRendering();
    void finishedOfflineRendering();


    void repaintGLWindows();
    //! Updates the screen space motion vectors
    void updateGLMotionMap();
    //! Updates all related GUI when a node is renamed
    //	void updateNodeRenameToAllGUI(NENode* node);
    //! Returns a pointer to the main application window
    FDYNMainWindow* mainWindow();

    //! This resets all the widgets when the scene is reset
    void resetWidgets();
    //! Selects the given object
    void selectObject(QObject *object);
    //! Selects the given object
    void selectGUIObject(QObject* object);
    //! Select the given list of objects
    void selectObjects(QList<NENode*>& objects);
    //! Pushes the given event to the undo stack
    void pushUserEvent(NEUserEvent* event);

    void popupDescriptionAndName(const QPoint& pos, QWidget *w, NENode *selSpatial, QPainter *painter);
    void showPropertyShortCutDialog(const QPoint& pos, QWidget *w, NENode *selSpatial);

    void showZoneSizeTool();
    void showZoneLocTool();
    void showSnapUnitProperty();

    void setSnapUnit(bool bSnapUnit);
    void setSnapToBoundary(bool bSnap);
    void setSnapToObj(bool bSnap);
    void setSnapToEdge(bool bSnap);
    void setMeasure(bool bMeasure);
    void setMeasureEdge(bool bMeasureEdge);

    void setTimeLineUnitsToScene(FDYNTimelineUnits unit);

    void setPropertyVisibleAsActiveDock();

public slots:

    void updateSurfaceMaterials(QList<NESurfaceMaterial> mlist);
    void updateFireMaterials(QList<NEFireMaterial> flist );
    void updateFailureCriterias(QList<NEFailureCriteria> flist);
    void updateVentCriterias(QList<NEVentCriteria> criteria);
    void updateCableProperties(QList<NECableProperty> props, std::vector<QList<QString>> options);
    void updateComputeScenarios(QList<FRANXProperties> fplist);

    void    processAboutFailedItems( QList<int> failedItemID,
                                     QList<double> failedTime,
                                     QList<int> scenarioID,
                                     QList<QString> failOutput );

    void    newProject();
    void    newDB();
    QString loadDB( QString fileName );
    void    saveDB();
    QString saveAsDB();

    void    loadUI( QString filename );
    void    saveUI( QString filename );

    void importDrawing();
    void importFRANX();
    void importPDMS();

    void quitProject( int errcode );

    void openRecoverWarning( QString msg = "", bool recoverypresent = true );
    void editPreferences();

    //! Performs an undo
    void undo();
    //! Performs a redo
    void redo();
    //! Shows the Qt Assistant based help engine (not working yet)
    void showHelp();

    //! This is called when render settings needs to be edited
    void showOfflineRendererSettingsDialog();

    //! Shows the selected property's expression
    void showNodePropertyExpression(const NENode* val, const QString Prop);
    //! Returns whether OpenGL update is enabled
    bool OpenGLUpdate() const;
    //! Enables/Disables OpenGL update
    void setOpenGLUpdate(bool val);
    //! Selects selection mode
    void selectCursorMode(bool val);
    //! Selects translation mode
    void selectTranslationMode(bool val);
    //! Selects rotation mode
    void selectRotationMode(bool val);
    //! Selects scale mode
    void selectScaleMode(bool val);
    //! Selects scale mode for only zone compartment
    void selectSizeMode(bool val);
    //! Selects scale mode for only zone compartment
    void selectLocationMode(bool val);
    //! Sets selected node visible
    void setSelectedVisibile(bool val);
    //! Sets selected node hidden
    void setSelectedHidden(bool val);
    //! Views the selected node as bounding box
    void setSelectedBoundingBox(bool val);
    //! Views the selected node as flat shaded
    void setSelectedFlatShaded(bool val);
    //! Views the selected node as smooth shaded
    void setSelectedSmoothShaded();
    //! Groupscthe selected nodes
    void setSelectedGroup();
    //! Un-group selected nodes
    void setSelectedUngroup();

    //! Delete the scenario
    void deleteScenario();

    //! add scenario to  selected Item
    void addScenarioToSelected();
    //! add zone to selected Item
    void addZoneToSelected();

    //!
    //! \brief addComponent
    //!
    void addComponent();

    //! add zone item to selected zone/scenario/item
    void addItemToSelected();

    ///
    /// \brief duplicateObject
    ///
    void duplicateObject();

    //!
    //! \brief copyProperties
    void copyProperties();

    ///
    /// \brief pasteProperties
    void pasteProperties();

    ///
    /// \brief pasteDataProperties
    ///
    void pasteDataProperties();

    ///
    /// \brief paste3DProperties
    ///
    void paste3DProperties();

    //! \brief renameNode
    //! Rename node with newName
    //!
    void renameNode(NENode* node, QString newName);

    //! \brief addZone
    //! add a zone to the engine
    //!
    void addZoneAndScenario();
    //! \brief addScenario
    //! add a scenario to the engine
    //!
    void addScenario();

    //!
    //! \brief addFire
    //! add a fire description
    //!
    void addFire();

    ///
    /// \brief addVent
    ///
    void addVent();
    ///
    /// \brief addMechVent
    ///
    void addMechVent();
    void turnIntoVent();
    void turnIntoMechVent();

    void loadDefaultSurfaceMaterials();
    void addSurfaceMaterial(SurfaceMaterial s);
    void removeSurfaceMaterial(SurfaceMaterial s);
    void changeSurfaceMaterial(SurfaceMaterial s);

    void addFireMaterial(FireMaterial f);
    void removeFireMaterial(FireMaterial f);

    void assignSurfaceMaterial(NENode* node, SurfaceMaterial s, QString matKind);
    void assignFireMaterial(NENode *node, FireMaterial f);

    void updateSurfaceMaterial( NESurfaceMaterial mat );
    void updateFireMaterial( NEFireMaterial fmat );

    void addFailureCriteria(FailureCriteria s);
    void removeFailureCriteria(FailureCriteria s);
    void assignFailureCriteria(NENode* node, FailureCriteria s);
    void updateFailureCriteria( NEFailureCriteria mat );

    void addVentCriteria(VentCriteria s);
    void removeVentCriteria(VentCriteria s);
    void assignVentCriteria(NENode* node, VentCriteria s, QString criterionKind);
    void updateVentCriteria( NEVentCriteria mat );

    void addCableProperty(CableProperty p);
    void removeCableProperty(CableProperty p);
    void assignCableProperty(NENode* node, CableProperty p);
    void updateCableProperty( NECableProperty prop );

    void updateTempFolder(QString folderName);
    void updateLoadUIOption(bool val);
    void updateSaveUIOption(bool val);

    void updateComputeScenario(FRANXProperties fp);

    ///
    /// \brief addCompartment
    ///
    NENode* addCompartment();

    //!
    //! \brief addFireDetection
    //! add alarm or detection unit to the system
    //!
    void addFireDetection();

    //! add items to active scenario
    void addItemsToScenario(QList<std::pair<QObject*, QObject*>>& childParentPairs);
    //! remove items from active scenario
    void removeItemsFromScenario(QList<std::pair<QObject*, QObject*>>& childParentPairs);
    //! remove item from all scenarios
    void removeItemFromAllScenarios();

    //! simulate to new scenario
    void simulateNewScenario();

    ///
    /// \brief convertToSource
    ///
    void convertToSource();

    ///
    /// \brief convertToItem
    ///
    void convertToItem();

    //! simulate replace existing scenario
    void simulateScenario();

    //! save the current scenario as
    void saveAsScenario();

    //! add item to active zone
    void addItemToZone();
    //! remove item from active zone
    void removeItemFromZone();

    //! Compute FRANX
    void computeFRANX();
    void computeFRANXOptions();

    //! Parent selected nodes
    void setSelectedParent();
    //! Un-Parent selected nodes
    void setSelectedUnparent();
    //! Sets the selected node active
    void setSelectedActive();
    //! Sets the selected node cache
    void setSelectedCache();
    //! Sets the selected node inactive
    void setSelectedInactive();
    //! Toggles fullscreen
    void fullscreenToggle(bool val);

    //! Properly maximizes  main window
    void maximize();
    void playToggled(bool val);
    //! Set the selected node to be rendered as wireframe
    void setSelectedWireframe();
    //! Toggle the state of the play button in the timeline based on the value
    void playButtonToggled(bool val);

    //!
    //! Slots to load all Zones/Scenarios in the appropriate inspector
    //!
    void allZones( QList< QString > &allZones );
    void allScenarios( QList< QString > &allScenarios );

    //
    // Zones related to the currently active scenario are sent here to load an apporopriate inspector
    void relatedZones(QList< QString > &zonelist );
    // Scenarios related to the currently active zone are sent here to load an appropriate inspector
    void relatedScenarios(QList< QString > &scnlist );

    void activeZone(QObject*& fengine, QObject *& zone);
    void activeScenario(QObject*& fengine, QObject *& scenario);

    void showWarningMessage(QString message);

    //! UI Window related slots/methods

    //! These are called when needed by NEBASE

    //! Creates a new property editor and shows the properties of passed object
    void createNewPropertyEditor();
    void createPropertyEditor( QString name );
    void closeAllPropertyEditors();

    //! Shows the main UI
    void showMainWindow();
    //! Delete
    void deleteMainWindow();
    //! Sets the title to indicated
    void setWindowTitle(QString title);

    //! Split the given OpenGL dock horizontally
    void splitGLDockHorizontally(NEGLWidget* wid);
    //! Split the given OpenGL dock vertically
    void splitGLDockVertically(NEGLWidget *wid);
    //! Split the given OpenGL dock into four parts
    void splitGLDockToQuad(NEGLWidget* wid);

    //! Progress related methods
    void hideSceneLoadingSplash();
    void showSceneManagementDialog();
    void setCurrentOperation( QString msg );
    void setCurrentSubOperation( QString msg );
    void hideSceneLoadingWidget();
    void showSceneLoadingWidget();

    //! Creates GL/Rendering related widgets
    void createGLWindow( QString name );

    //! Closes the GL Rendering related widgets
    void closeAllGLDocks();

    //! Loads the saved UI State
    void loadGUIState();

    //! Saves UI
    void writeAllGUIState( QXmlStreamWriter* xmlWriter );
    void saveGUIState();
    void writeAllGUICreations( QXmlStreamWriter* xmlWriter );

    //! Initialization related slots - these are called automatically by NEBASE
    void postInit();

    //! Layout widgets
    void layout();

    void deInit();
    void setParent( QObject* varRoot );

    //! Updates the scene inspector contents
    void updateSceneInspector();
    //! Update Zone Inspector contents
    void updateZoneInspector();
    //! Update Scenario Inspector contents
    void updateScenarioInspector();
    //! Update all Inspector contents
    void updateInspectors();
    //! Updates the system inspector contents
    void updateSystemInspector();

    //! Updates the connection table of the connection editor
    void updateAllConnectionTables();
    //! Updates the property editor contents
    void updatePropertyEditor();
    //! Updates the property editors contents
    void updatePropertyEditorsAtPostInit();
    //! Updates UI in case of creation
    void updateWidgetsForCreation();
    //! Updates UI in case of deletion
    void updateWidgetsForDeletion(QObject *object);

    //! Slot to be called for object created
    void objectCreated( NENode* object );

    //! Slot to be called for object removal
    void objectRemoved(NENode *object );

    //! Slot for setting current time/frame
    void setCurrentFrame( int val );

    //! Refresh timeline slot
    void repaintTimeline();

    //! Play/Sim manipulation
    void setPlay( bool val );

    //! Slot for highlighing keyframes in timeline
    void setTimelineKeyframeMarks( QList<uint> marks );

    //! Slot for updating the value of UI Simulation End Time
    void updateSimulationEndTime(double val );


    //! Slot for setting sim  start frame
    void setSimulationStartFrame( int val );
    //! Slot for setting sim end frame
    void setSimulationEndFrame( int val );

    //! Slot for setting playback start frame
    void setPlaybackStartFrame( int val );
    //! Slot for setting playbackend frame
    void setPlaybackEndFrame( int val );

    //! Slot for setting sim end time
    void setSimulationEndTime(double val);

    //! Slot for setting playback start time
    void setPlaybackStartTime(double val);
    //! Slot for setting playbackend time
    void setPlaybackEndTime(double val);

    //! Slot for simulate flag
    void setSimulate( bool val );

    //! Slot for repeat flag
    void setRepeat( bool val );

    //! Slot for saving gui related data
    void triggerSavingData();

    //! Slot for actions invoked
    void registerActionOnGUI(QString actionName, QObject* &action);
    void unregisterActionOnGUI(QString actionName, QObject *&action);

    //! Add the creation items to GUI
    void addCreateActionToGUI( const QString& type, const NEManager* manager, const uint nodeType );

    //! Remove the appropriate items from GUI
    void removeCreateActionToGUI( const QString& type, const NEManager* manager, const uint nodeType );

    //! When the managers are created by various super managers from core
    void managersCreated( QList< std::shared_ptr< NEManager > > managers );

    //! When the manager is created -- only used by NERENDERMAN
    void managerCreated( NEManager* pManager);

    //! Gl Widget/Window related.
    void closeGLWidget(NEGLWidget* val);
    void hideGLWidget(NEGLWidget* val);
    void showMainGLWidget();

    //! Returns a pointer to the main GLWidget
    NEGLWidget* mainWidget();

    void addCableToSelected();
    void addTrayToSelected();

    void addItemToZoneTree(NENode* node);
    void addItemToScenarioTree(NENode* node, NENode* parentNode);

    void updateUIFolder(QString folderName);

    void fromJson( QString jsonStr );
    QString toJson();


public:
    NE_DECL_PRIVATE_SINGLETON(FDYNGUIManager)
};

#define FDYNGUI FDYNGUIManager::Instance()




