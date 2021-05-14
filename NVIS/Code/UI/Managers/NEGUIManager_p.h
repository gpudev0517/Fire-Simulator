#pragma once

#include <QMutex>

NE_FWD_DECL_6(class, NEGUIManager, NEManager, NETimeline, NEGLWidget, NENode, NEMainWindow)
NE_FWD_DECL_6(class, NERenderDialog, NECamera, NEDockWidget, NEGLMainWindow, NEPropertyEditorWindow, NEExternalRendererSettingsWindow)
NE_FWD_DECL_1(struct, GLDockWidgetBundle)

class NEGLWidget;
class NEGLMainWindow;

class NEPreferencesDialog;
/*!
 * \brief Keeps OpenGL viewport related information
 */
struct GLDockWidgetBundle
{
    GLDockWidgetBundle();
    ~GLDockWidgetBundle();
    int id;
    NEDockWidget* dockWidget;
    NEGLMainWindow* mainWindow;
    NEGLWidget* glWidget;
};



class NEGUIManagerPrivate
{
public:
    NEGUIManagerPrivate(NEGUIManager* s);
    //! Initializes the window manager
    void init();
    //! Initializes dependent window elements after NEBase and NEScene are initialized
    void postInit();
    //! De-initializes the window manager
    void deInit();
    //! Shows the main application window
    void showMainWindow();
    //! Saves the GUI state (dock and toolbar locations and sizes)
    void saveGUIState();
    //! Saves the GUI state (dock and toolbar locations and sizes)
    void loadGUIState();
    //! Saves the position and size of NEMainWindow
    void saveMainWindowGeometry();
    //! Loads the position and size of NEMainWindow
    void loadMainWindowGeometry();
    //! Used to populate scene builder and menubar for different types of objects
    void addCreateActionToGUI(const QString& type, const NEManager* manager, const uint nodeType);

    void removeCreateActionToGUI(const QString& type, const NEManager* manager, const uint nodeType);

    //! Maps the given action to the correct createObject function in Console
    void mapActionToCreateObject(QAction* action, const QString& type, const NEManager* manager);
    //! Makes Ui to Neutrino connections
    void makeConnections();
    //! Gives information about the active OpenGL driver
    bool printGLInfo();

    //! Writes all related GUI state into the scene file
    void saveGUIStateIntoScene(QXmlStreamWriter* writer);
    void restoreGUIStateFromScene(QXmlStreamReader* reader);

    //! Colorizes the range of the timeline which corresponds to the maximum number of frames simulated
    void setTimelineMaxSimulated(int start, int end);
    //! Colorizes the range of the timeline which corresponds to the minimum number of frames simulated
    void setTimelineMinSimulated(int start, int end);
    //! Sets marks for keyframes of currently selected NESpatial
    void setTimelineKeyframeMarks(QList<uint> marks);
    //! Sets the start frame of the timeline.
    void setTimelineMinFrame(int val);
    void setWindowTitle(QString scene);

    //! Split the given OpenGL dock horizontally
    void splitGLDockHorizontally(NEGLWidget* wid);
    //! Split the given OpenGL dock vertically
    void splitGLDockVertically(NEGLWidget *wid);
    //! Split the given OpenGL dock into four parts
    void splitGLDockToQuad(NEGLWidget* wid);
    //! Returns the main GL widget that keeps OpenGL resources
    NEGLWidget* mainGLWidget();

    //! Prepares the UI for object removal
    void prepareForObjectRemoval(NENode* object);
    //! Synronizes all widgets in case of deletion
    void updateWidgetsForDeletion(QObject* object);

    void updateWidgetsForCreation();

    //! Enables/Disables some specific widgets in the simulation mode
    void enableSimulationWidgets(bool b);
    //! Enables/Disables some specific widgets in the playback mode
    void enablePlaybackWidgets(bool b);

    //! Returns the timeline widget
    NETimeline* timeline() { return m_pTimeline; }
    //! For toggling the simulate button
    void setSimulateButton(bool val);
    //! For toggling the repeat button
    void setRepeatButton(bool val);
    //! Returns whether the simulate button is toggled
    bool simulateButtonState();
    //! For toggling the play button
    void setPlayButton(bool val);
    //! Returns whether the play button is toggled
    bool playButtonState();
    //! Repaints the timeline
    void repaintTimeline();
    //! The first playback frame (syncronized with the GUI)
    //int playbackStartFrameOrTime();
    void setPlaybackStartFrameOrTime(double val);
    //! The last playback frame (syncronized with the GUI)
    //int playbackEndFrameOrTime();
    void setPlaybackEndFrameOrTime(double val);
    //! The frame where the simulation should stop (syncronized with the GUI)
    //int simulationEndFrameOrTime();
    void setSimulationEndFrameOrTime(double val);

    //! Sets the given frame as the current frame in all related widgets
    void setCurrentFrame(double val);

    void closeAllGLDocks();
    //! Adds a camera into the context menu of all gl widgets
    void addCameraIntoContextMenus(NECamera* pCamera);
    //! Removes a camera from the context menu of all gl widgets
    void removeCameraFromContextMenus(NECamera* pCamera);

    //! Creates an OpenGL dock window, and returns its parts in a structure
    void createGLWindowOnLoad(QString name = QString());
    GLDockWidgetBundle* createGLWidget(QString name = QString());


    void updatePropertyEditorsAtPostInit();

    void closeGLWidget(NEGLWidget* val);
    void hideGLWidget(NEGLWidget* val);
    void showMainGLWidget();
    //! Sets a camera for an OpenGL dock window
    void setCameraForGLDock(GLDockWidgetBundle* bundle, NECamera *cam);

    void closePropertyEditor(NEPropertyEditorWindow* w);
    void triggerSavingGLData();

    //! Resizes the gl widget, e.g. for gl capturing
    bool resizeGLWidget(int width=0, int height=0, bool fixed=false);
    void resizeGLWidgetsForFrameSaving(bool val);

    void writeAllGUISubCreations( QXmlStreamWriter *xmlWriter );

    //! Pointers to ui elements
    NEMainWindow* m_pMainWindow;
    NETimeline* m_pTimeline;
    NERenderDialog* m_pRenderDialog;
//    NEPreferencesDialog* m_pPreferencesDialog = 0;
    NEExternalRendererSettingsWindow* m_pExtRenderWindow;

    //! Map that holds a pointer to all opengl dock widgets
    QMap<int, GLDockWidgetBundle*> m_IDtoGLDockWidget;
    QMap<NEGLWidget*, GLDockWidgetBundle*> m_GLtoGLDockWidget;
    //! List of open property editor windows
    QList<NEPropertyEditorWindow*> m_PropEdWins;
    //! Object names to load into the property editors. Filled when loading the scene.
    QList<QString> m_PropEdObjs;

    //! This is the main OpenGL widget that keeps the OpenGL context and resources
    NEGLWidget* m_pMainGLWidget;

    bool m_OpenGLUpdate;
    bool m_ShowGLBuffers;
    bool m_SaveGLFrames;

    NEGUIManager::NETimelineUnits m_TimelineUnits;

    QStack<NEUserEvent*> m_UndoStack;
    QStack<NEUserEvent*> m_RedoStack;

    // Below are used for resizing docked qdockwidgets
    mutable QSize oldMaxSize, oldMinSize;
    mutable QDockWidget* dockW;

    // Keeps the path of the GUI state file
    QString m_GUIStatePath;


    matrix44f m_gizmoTransform;

    Qt::WindowStates m_PreviousWindowState;

    QMutex m_locker;

private:
    friend class NESceneManager;
    NEGUIManager* S;
};
