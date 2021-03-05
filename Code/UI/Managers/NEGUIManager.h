#pragma once

#include "NEObject.h"
#include "Utility/NEExternalProperties.h"
#include "PropertyManager/NEPropertyObject.h"
#include "Base/NEGUICallInterface.h"

NE_FWD_DECL_4(class, NEMainWindow, NENode, NERenderSetting, NEGPUBufferVis)
NE_FWD_DECL_5(class, NEExpressionInspector, NEPropertyEditorWindow, NE_PRIVATE(NEGUIManager),NE_PRIVATE(NEGUIManagerUserProps), NEGUIManager)
NE_FWD_DECL_1(struct, NEUserEvent)
NE_FWD_DECL_1(class, NENodeFactory)
NE_FWD_DECL_1(class, NEManager)

struct aiScene;
class NENode;

class NEGUIManagerUserProps : public NEPropertyObject
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
	NEGUIManagerUserProps(NEGUIManager* s);
	~NEGUIManagerUserProps();

private:
	NEGUIManager* S;
	NE_DECL_PRIVATE(NEGUIManagerUserProps)
};\


//!  Manages all Graphical User Interface related things
/*!
	Its responsibilities include managing all UI elements, populating menus
	and connecting signals and slots.
*/
class NECEXP_NEUI NEGUIManager final :
        public NEObject,
        public NEExternalProperties<NEGUIManagerUserProps>,
        public ICallInterfaceFromBaseToGUI
{
    Q_OBJECT
    NE_CLASS("GUI Manager")
public:
	enum NETimelineUnits { Frames, Seconds };

    NE_PROP_RW(NETimelineUnits, TimelineUnits, "Timeline Units", "Set Timeline Units", true,)
	NE_PROP_RW(bool, SaveGLFrames, "Save OpenGL Frames", "When enabled, OpenGL frames are saved for the OpenGL windows with frame saving set to true.", true,)

	NE_FWD_DECL_6(friend class, Singleton<NEGUIManager>, NEManager, NE_PRIVATE(NEBase), NE_PRIVATE(NESceneManager), NEDockWidget, NEGLWidget)
	NE_FWD_DECL_6(friend class, NEGLWidgetProperties, NEMainWindow, NETimelineWidget, QtTreePropertyBrowser, NEKeyframeEditor, NEKeyframeInspector)
    NE_FWD_DECL_5(friend class, NEGLMainWindow, NERenderManager, NESceneManager, NEBase, NEManagerPrivate)

	Q_ENUMS(NETimelineUnits)

public:

    static NEGUIManager& Instance();

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

    //! Resize all GL windows
    virtual void resizeGLWindows();


    //! file dialog
    virtual QStringList fileImportBrowser( FilePath& filePath, QString orgName, QString appName, QString settingsName );

public slots:
    //! Initialization
    void init();

	//! Updates only the title of the property editor
	void updatePropertyEditorTitle(QString val);
	//! Hides the object from the other property editors
	void hideObjectFromOtherPropEditors(QObject* obj, NEPropertyEditorWindow* pe);



	//! Updates all GL windows
	void updateGLWindows();
	//! Updates the screen space motion vectors
	void updateGLMotionMap();
	//! Updates all related GUI when a node is renamed
//	void updateNodeRenameToAllGUI(NENode* node);
	//! Returns a pointer to the main application window
	NEMainWindow* mainWindow();



	//! This resets all the widgets when the scene is reset
	void resetWidgets();
	//! Selects the given object
	void selectObject(QObject* object);
	//! Selects the given object
	void selectGUIObject(QObject* object);
	//! Select the given list of objects
    void selectObjects(QList<NENode*>& objects);
	//! Pushes the given event to the undo stack
	void pushUserEvent(NEUserEvent* event);

public slots:
	//! Performs an undo
	void undo();
	//! Performs a redo
	void redo();
	//! Shows the Qt Assistant based help engine (not working yet)
	void showHelp();


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
	//! Sets selected node visible
	void setSelectedVisibile(bool val);
	//! Sets selected node hidden
	void setSelectedHidden(bool val);
	//! Views the selected node as bounding box
	void setSelectedBoundingBox(bool val);
	//! Views the selected node as flat shaded
	void setSelectedFlatShaded(bool val);
	//! Views the selected node as smooth shaded
	void setSelectedSmoothShaded(bool val);
	//! Groupscthe selected nodes
	void setSelectedGroup();
	//! Un-group selected nodes
	void setSelectedUngroup();
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

	//! Properly maximizes neutrino main window
	void maximize();
	void playToggled(bool val);
	//! Set the selected node to be rendered as wireframe
	void setSelectedWireframe(bool val);
	//! Toggle the state of the play button in the timeline based on the value
    void playButtonToggled(bool val);

    void activeZone(QObject*& fengine, QObject *& zone);


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
public:
    NE_DECL_PRIVATE_SINGLETON(NEGUIManager)
};

#define NEGUI NEGUIManager::Instance()




