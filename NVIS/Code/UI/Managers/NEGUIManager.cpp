#include "Base/NEGUICallInterface.h"
#include "Base/NEApplication.h"
#include "Spatial/NESpatialGroup.h"
#include "Manager/NECollisionManager.h"
#include "Base/NEBase.h"
#include "Utility/NEUserEvent.h"
#include "Dialogs/Process/NEProcessDialog.h"
#include "Windows/Main/NEMainWindow.h"
#include "Windows/SceneInspector/NESceneInspector.h"
#include "Windows/Message/NEMessageWindow.h"
#include "NEGLWidget.h"
#include "Manager/NESimulationManager.h"
#include "NENode.h"

#include "Rendering/Manager/NERenderNEOManager.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NEGLManager_p.h"
#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Spatial/NECamera.h"
#include "Rendering/Utility/RenderSettings/NERenderSetting.h"

#include "Windows/PropertyManager/NEObjectController.h"
#include "Base/NESceneManager.h"
#include "Dialogs/Collider/NEColliderDialog.h"
#include "Dialogs/Help/NEHelpEngine.h"
#include "Windows/ScriptEditor/NEConsoleWindow.h"
#include "Windows/GLMain/NEGLMainWindow.h"
#include "Utilities/NEGUIStyle.h"
#include "Widgets/MenuBar/NEMenuBar.h"
#include "Widgets/Dock/NEDockWidget.h"
#include "Widgets/GPUBufferVis/NEGPUBufferVis.h"
#include "Windows/PropertyManager/NEObjectController.h"
#include "Windows/ExternalRendererSettings/NEExternalRendererSettingsWindow.h"
#include "Windows/SceneInspector/NESceneInspectorWindow.h"

#include "Rendering/NEGLRenderer.h"

#include "Widgets/Timeline/NETimeline.h"
#include "Widgets/Timeline/NETimelineWidget.h"
#include "Windows/ConnectionInspector/NEConnectionInspector.h"
#include "Windows/ExpressionEditor/NEExpressionEditorWindow.h"
#include "Windows/PropertyManager/NEPropertyEditorWindow.h"
#include "Windows/NodeGraphEditor/NENodeGraphEditorWindow.h"
#include "Windows/SceneInspector/NESceneInspectorWindow.h"
#include "Dialogs/Help/NEHelpEngine.h"

#include "Rendering/Spatial/NECamera.h"
#include "Rendering/Spatial/Light/NEDirectionalLight.h"
#include "Rendering/Spatial/Light/NESpotLight.h"
#include "Rendering/Spatial/Light/NEPointLight.h"

#include "ui_NEGLMainWindow.h"
#include "Windows/SystemInspector/NESystemInspectorWindow.h"

#include "NEGUIManager.h"
#include "Widgets/Timeline/NETimeline.h"
#include "Windows/BufferVis/NEBufferVisWindow.h"
#include "Windows/SceneBuilder/NESceneBuilderWindow.h"
#include "Windows/ChartsVis/NEChartsVisWindow.h"
#include "Windows/ExpressionInspector/NEExpressionInspector.h"
#include "Windows/KeyframeInspector/NEKeyframeInspector.h"
#include "Windows/Message/NEMessageWindow.h"
#include "Dialogs/Preferences/NEPreferencesDialog.h"
#include "Utility/NEOsdMsg.h"
#include "Rendering/Utility/NEOsd.h"
#include "Widgets/Menu/NEMenu.h"

#include "Utilities/NELogBrowser.h"
#include "NEGUIManager_p.h"


NE_PROP_DEF(bool, NEGUIManagerUserProps, DarkTheme, true)
NE_PROP_DEF(NEGUIManager::NETimelineUnits, NEGUIManager, TimelineUnits, NEGUIManager::Frames)
NE_PROP_DEF(NEGUIManagerUserProps::NEGUIItemSize, NEGUIManagerUserProps, NormalFontSize, NEGUIManagerUserProps::Normal)
NE_PROP_DEF(NEGUIManagerUserProps::NEGUIItemSize, NEGUIManagerUserProps, SmallFontSize, NEGUIManagerUserProps::Small)
NE_PROP_DEF(NEGUIManagerUserProps::NEGUIItemSize, NEGUIManagerUserProps, LargeFontSize, NEGUIManagerUserProps::Large)
NE_PROP_DEF(NEGUIManagerUserProps::NEGUIItemSize, NEGUIManagerUserProps, OSDFontSize, NEGUIManagerUserProps::Small)
NE_PROP_DEF(bool, NEGUIManager, SaveGLFrames, false)
NE_PROP_DEF(NEGUIManagerUserProps::NEGUIItemSize, NEGUIManagerUserProps, ToolbarSize, NEGUIManagerUserProps::Normal)


QPointer<NELogBrowser> NELOGBROWSER;

void NEMessageOutput( QtMsgType type, const QMessageLogContext& mcontext, const QString& msg )
{
    if(NELOGBROWSER)
        NELOGBROWSER->outputMessage(type, msg);
}


class NEGUIManagerUserPropsPrivate
{
public:
    NEGUIManagerUserPropsPrivate(NEGUIManagerUserProps* s) : S(s)
    {
    }

    NEGUIManagerUserProps::NEGUIItemSize m_NormalFontSize = NE_DEFAULT(NormalFontSize);
    NEGUIManagerUserProps::NEGUIItemSize m_SmallFontSize = NE_DEFAULT(SmallFontSize);
    NEGUIManagerUserProps::NEGUIItemSize m_LargeFontSize = NE_DEFAULT(LargeFontSize);
    NEGUIManagerUserProps::NEGUIItemSize m_OSDFontSize = NE_DEFAULT(OSDFontSize);
    NEGUIManagerUserProps::NEGUIItemSize m_ToolbarSize = NE_DEFAULT(ToolbarSize);

private:
    NEGUIManagerUserProps* S;

};


static void initProperties()
{
    NE_INIT_PROP(NEGUIManagerUserProps, DarkTheme);
    NE_INIT_PROP(NEGUIManager, TimelineUnits);
    NE_INIT_PROP(NEGUIManagerUserProps, NormalFontSize);
    NE_INIT_PROP(NEGUIManagerUserProps, SmallFontSize);
    NE_INIT_PROP(NEGUIManagerUserProps, LargeFontSize);
    NE_INIT_PROP(NEGUIManagerUserProps, OSDFontSize);
    NE_INIT_PROP(NEGUIManager, SaveGLFrames);
}


NEGUIManager& NEGUIManager::Instance()
{
    static NEGUIManager instance;
    return instance;
}



NEGUIManager::NEGUIManager()
{
    // Initialize the private members
    P = new NEGUIManagerPrivate( this );

    // Initialize per-user properties
    setPropertyObject(new NEGUIManagerUserProps(this));

    QObject::connect( &NEBASE, SIGNAL(initUI()), this, SLOT(init()) );

    // Initialize the properties
    CALL_ONCE(initProperties);


    P->m_TimelineUnits = NE_DEFAULT(TimelineUnits);
    P->m_SaveGLFrames = NE_DEFAULT(SaveGLFrames);

}

NEGUIManager::~NEGUIManager()
{
    // Delete the private members
    delete P;
    P = NULL;
}


void NEGUIManager::init()
{
    P->init();

    NELOGBROWSER = new NELogBrowser( this );
    qInstallMessageHandler(NEMessageOutput);


    qRegisterMetaType<QTextBlock>();
    qRegisterMetaType<QTextCursor>();

    //QObject::connect( &NEBASE, SIGNAL(initUI()), this, SLOT(init()) );

    // Make basic GUI to Neutrino connections
    if( NEBASE.guiMode() )
        P->makeConnections();
    // Hide everything in the main window first
    if( NEBASE.guiMode() )
        mainWindow()->hideEverythingForStartup();

    getPyMain().addObject( objectName(), this );



}

void NEGUIManagerPrivate::init()
{
    if( !NEBASE.guiMode() )
        return;

    // Set the objectname
    S->setObjectName("GUI");

    // Initialize the GUI styles
    NEGUISTYLE.init();

    // Initialize member variables
    m_OpenGLUpdate = true;
    m_SaveGLFrames = NE_DEFAULT(SaveGLFrames);
    m_pMainGLWidget = 0;

    // Create the main application window
    m_pMainWindow = new NEMainWindow;

    // Initialize the script editor
    S->consoleWindow()->init();

    // Initialize the connection inspector
    S->mainWindow()->connectionInspector()->init();

    // Initialize the expression editor
    S->mainWindow()->expressionEditor()->init();

    // Initialize the property editor
    S->mainWindow()->propertyEditorWindow()->init();
    m_PropEdWins.append(S->mainWindow()->propertyEditorWindow());

    // Initialize the node graph editor
    S->mainWindow()->nodeGraphEditorWindow()->init();

    // Keep a private pointer to the timeline
    m_pTimeline = m_pMainWindow->Timeline();

    // Create the main OpenGL window
    {
        createGLWidget();
        // This is the main OpenGL widget that keeps the OpenGL context and resources
        m_pMainGLWidget = m_IDtoGLDockWidget.last()->glWidget;
    }

    // Initialize the scene inspector
    S->mainWindow()->sceneInspectorWindow()->setRoot(&NESCENE);

    // Initialize the system inspector
    S->mainWindow()->systemInspectorWindow()->addRoot(&NEBASE);

    // Make the GUI manager visible in the system inspector
    S->mainWindow()->systemInspectorWindow()->addRoot(S);

    // Split a previous dock widget to create one for the just created OpenGL widget
    S->mainWindow()->splitDockWidget( S->mainWindow()->sceneInspectorDock(),
                                      m_IDtoGLDockWidget.last()->dockWidget,  Qt::Horizontal);

    // Create the external rendering dialog
    m_pExtRenderWindow = new NEExternalRendererSettingsWindow(m_pMainWindow);

    // Initialize the help
    NEHELP.init("neutrino.qch");

    showMainWindow();
    // We have an OpenGL context now, check the OpenGL version
    printGLInfo();

    // Initializes some utility geometry for the OpenGL widgets
    NEGLWidget::initStaticData();
}


void NEGUIManagerPrivate::postInit()
{
    if( !NEBASE.guiMode() )
        return;

    // Build all shaders
    NERENDERMAN.GLManager()->updateAllMaterialShaders();
    NERENDERMAN.GLManager()->updateAllShaders();

    // Initialize GPU buffer visualization widgets
    S->GPUBufferVis()->initWidgets();

    //updatePropertyEditorsAtPostInit();

    // Sets the default camera of the scene as the default OpenGL widget's camera
    if( m_pMainGLWidget && !m_pMainGLWidget->camera() )
        setCameraForGLDock( m_GLtoGLDockWidget[m_pMainGLWidget], NERENDERMAN.defaultCamera() );

    // This is somehow required for spacebar to work correctly to maximize gl docks
    // The main windows also needs to have strong focus policy for this to work
    if( m_pMainGLWidget )
        m_pMainGLWidget->setFocus( Qt::OtherFocusReason );
    S->mainWindow()->consoleWindow()->reinitStyle( NEGUISTYLE.darkTheme() );

    {
        S->updateGLWindows();
        loadGUIState();
    }
    S->mainWindow()->hideSceneLoadingSplash();

    m_pTimeline->timelineWidget->changeUnits((NETimelineWidget::NETimelineUnits)(int)m_TimelineUnits);
    m_pTimeline->setValueWithTimelineUnits();
    emit S->TimelineUnitsChanged(m_TimelineUnits);
}

void NEGUIManagerPrivate::deInit()
{
    if( !NEBASE.guiMode() )
        return;

    // Save the GUI state first
    saveMainWindowGeometry();

    // Deinit the OpenGL manager
    //NERENDERMAN.GLManager()->deInit();

    // Clear the static data shared by all OpenGL widgets
    NEGLWidget::clearStaticData();

    // Save the contents of the messages window
    QDir directory(NEBASE.sceneDataPath());
    directory.mkpath("Log/Messages");
    S->mainWindow()->messageWindow()->saveToFile(NEBASE.sceneDataPath() + "/Log/Messages/");

    foreach(GLDockWidgetBundle* b, m_IDtoGLDockWidget)
        delete b;

}

NEGUIManagerPrivate::NEGUIManagerPrivate(NEGUIManager* s): S(s)
{
    m_pMainWindow = 0;
}

void NEGUIManagerPrivate::showMainWindow()
{
    S->mainWindow()->show();
    loadMainWindowGeometry();
}

const bool& NEGUIManagerUserProps::DarkTheme() const
{
    return NEGUISTYLE.DarkTheme();
}

uint NEGUIManagerUserProps::SetDarkTheme(const bool& val)
{
    if( NEGUISTYLE.darkTheme() == val )
        return NE::kReturnSuccess;;

    // Clear the log window first, as the colors of its elements cannot be changed
    S->mainWindow()->messageWindow()->clear();

    NEGUISTYLE.SetDarkTheme(val);
    S->mainWindow()->consoleWindow()->reinitStyle(val);
    NEObject* selectedObj = NESCENE.selectedObject();
    NEObjectController::clearPropertiesMap();
    emit DarkThemeChanged(val);
    S->selectObject(selectedObj);
    return NE::kReturnSuccess;
}

const bool& NEGUIManager::SaveGLFrames() const
{
    return P->m_SaveGLFrames;
}

uint NEGUIManager::SetSaveGLFrames(const bool& val)
{
    NE_PROP_SET_NOTIFY(SaveGLFrames, P, val);
    return NE::kReturnSuccess;
}

void NEGUIManager::pushUserEvent(NEUserEvent* event)
{
    P->m_UndoStack.push(event);
    P->m_RedoStack.clear();
}

void NEGUIManager::updateSceneInspector()
{
    if( mainWindow() )
        mainWindow()->sceneInspectorWindow()->update();
}

void NEGUIManager::updateSystemInspector()
{
    if( mainWindow() )
        mainWindow()->systemInspectorWindow()->update();
}

void NEGUIManager::updateExpressionInspector()
{
    if( mainWindow() )
        mainWindow()->expressionInspector()->update();
}

void NEGUIManager::updatePropertyConnectionsTable()
{
    if( mainWindow() )
        mainWindow()->connectionInspector()->updateAllPropertyConnectionsTable();
}

void NEGUIManager::updateNodeConnectionsTable()
{
    mainWindow()->connectionInspector()->updateAllNodeConnectionsTable();
}

void NEGUIManager::updatePropertyCallbacksTable()
{
    mainWindow()->connectionInspector()->updateAllPropertyCallbacksTable();
}

void NEGUIManager::updateInternalConnectionsTable()
{
    mainWindow()->connectionInspector()->updateAllInternalConnectionsTable();
}

void NEGUIManager::updatePropertyEditor()
{
    if( mainWindow()->propertyEditorWindow() )
        mainWindow()->propertyEditorWindow()->updateProperties();
}

void NEGUIManager::updatePropertyEditorsAtPostInit()
{
    P->updatePropertyEditorsAtPostInit();
}

void NEGUIManager::updateWidgetsForCreation()
{
    P->updateWidgetsForCreation();
}

void NEGUIManager::updateWidgetsForDeletion(QObject* object)
{
    P->updateWidgetsForDeletion( object );
}

void NEGUIManager::managersCreated( QList<std::shared_ptr<NEManager> > managers )
{
    QList<std::shared_ptr<NEManager> >::const_iterator ssItr = managers.begin();
    for( ; ssItr != managers.end(); ++ssItr )
    {
        QObject::connect( ssItr->get(), SIGNAL(addCreateActionToGUI( const QString&, const NEManager*, const uint)),
                          this, SLOT(addCreateActionToGUI( const QString&, const NEManager*, const uint)),
                          Qt::DirectConnection );
        QObject::connect( ssItr->get(), SIGNAL(removeCreateActionToGUI( const QString&, const NEManager*, const uint)),
                          this, SLOT(removeCreateActionToGUI( const QString&, const NEManager*, const uint)),
                          Qt::DirectConnection );

    }
    return;
}

void NEGUIManager::managerCreated(NEManager* pManager )
{
    if(!pManager)
        return;

    QObject::connect( pManager, SIGNAL(addCreateActionToGUI( const QString&, const NEManager*, const uint)),
                      this, SLOT(addCreateActionToGUI( const QString&, const NEManager*, const uint)),
                      Qt::DirectConnection );
    QObject::connect( pManager, SIGNAL(removeCreateActionToGUI( const QString&, const NEManager*, const uint)),
                      this, SLOT(removeCreateActionToGUI( const QString&, const NEManager*, const uint)),
                      Qt::DirectConnection );

    return;
}

void NEGUIManager::closeGLWidget(NEGLWidget *val)
{
    return P->closeGLWidget( val );
}

void NEGUIManager::hideGLWidget(NEGLWidget *val)
{
    return P->hideGLWidget( val );
}

void NEGUIManager::objectCreated( NENode *object )
{
    QObject::connect( object, SIGNAL(pushUserEvent(NEUserEvent*)),
                      this, SLOT(pushUserEvent(NEUserEvent*)) );

    QObject::connect( object, SIGNAL(updateNodeConnectionsTable()),
                      this, SLOT(updateNodeConnectionsTable()) );

    QObject::connect( object, SIGNAL(updatePropertyConnectionsTable()),
                      this, SLOT(updatePropertyConnectionsTable()) );

    QObject::connect( object, SIGNAL(updateNodeRename(NENode *)),
                      this, SLOT(updateNodeRenameToAllGUI(NENode *)) );

//     QObject::connect( object, SIGNAL(registerAction(QString &, QObject *)),
//                       this, SLOT(registerActionOnGUI(QString &, QObject *)) );
 	QObject::connect(object, &NENode::registerAction, this, &NEGUIManager::registerActionOnGUI, Qt::DirectConnection);
    //QObject::connect(object, &NENode::unregisterAction, this, &NEGUIManager::unregisterActionOnGUI, Qt::DirectConnection);

    QObject::connect( object, SIGNAL(updatePropertyCallbacksTable()),
                      this, SLOT(updatePropertyCallbacksTable()) );

    QObject::connect( object, SIGNAL(updateExpressionInspector()),
                      this, SLOT(updateExpressionInspector()) );

}


void NEGUIManager::objectRemoved(NENode* object)
{
    P->prepareForObjectRemoval( object );
}

void NEGUIManager::setCurrentFrame(int val)
{
    if(P->m_TimelineUnits == NEGUIManager::Seconds)
            P->setCurrentFrame( val / NESCENE.RecordFrameRate() );
        else
            P->setCurrentFrame( val );
}

void NEGUIManager::repaintTimeline()
{
    P->repaintTimeline();
}

void NEGUIManager::setPlay(bool val)
{
    P->setPlayButton( val );
}

void NEGUIManager::setTimelineKeyframeMarks(QList<uint> marks)
{
    P->setTimelineKeyframeMarks( marks );
}

void NEGUIManager::setSimulationStartFrame(int val)
{
	P->setSimulationEndFrameOrTime(val);
}
void NEGUIManager::setSimulationEndFrame(int val)
{
    if(P->m_TimelineUnits == NEGUIManager::Frames){
        P->setSimulationEndFrameOrTime( val );
    }

}

void NEGUIManager::setPlaybackStartFrame(int val)
{
    if(P->m_TimelineUnits == NEGUIManager::Frames){
        P->setPlaybackStartFrameOrTime( val );
    }

}

void NEGUIManager::setPlaybackEndFrame(int val)
{
    if(P->m_TimelineUnits == NEGUIManager::Frames){
        P->setPlaybackEndFrameOrTime( val );
    }
}

void NEGUIManager::setSimulationEndTime(double val)
{
    if(P->m_TimelineUnits == NEGUIManager::Seconds){
        P->setSimulationEndFrameOrTime(val);
    }

}

void NEGUIManager::setPlaybackStartTime(double val)
{
    if(P->m_TimelineUnits == NEGUIManager::Seconds){
        P->setPlaybackStartFrameOrTime(val);
    }

}

void NEGUIManager::setPlaybackEndTime(double val)
{
    if(P->m_TimelineUnits == NEGUIManager::Seconds){
        P->setPlaybackEndFrameOrTime(val);
    }
}

void NEGUIManager::setSimulate(bool val)
{
    if (!val)
        P->setPlayButton( false );
    P->setSimulateButton( val );
}

void NEGUIManager::setRepeat(bool val)
{
    P->setRepeatButton( val );
}

void NEGUIManager::triggerSavingData()
{
    P->triggerSavingGLData();
}

void NEGUIManager::registerActionOnGUI(QString actionName, QObject* &action)
{
	QAction* objectAction = new QAction(actionName, NULL);
	
    if(objectAction && objectAction->text() == "Delete")
        if( mainWindow() )
            mainWindow()->registerRemoveAction(objectAction);
	action = objectAction;
}

void NEGUIManager::unregisterActionOnGUI(QString actionName, QObject* &action)
{
    return;
}


void NEGUIManager::addCreateActionToGUI(const QString &type, const NEManager *manager, const uint nodeType)
{
    P->addCreateActionToGUI( type, manager, nodeType );
}

void NEGUIManager::removeCreateActionToGUI(const QString &type, const NEManager *manager, const uint nodeType)
{
    P->removeCreateActionToGUI( type, manager, nodeType );
}

void NEGUIManager::updatePropertyEditorTitle(QString val)
{
    mainWindow()->propertyEditorDock()->setTitleText("Property Editor - " + val);
}

void NEGUIManager::hideObjectFromOtherPropEditors(QObject* obj, NEPropertyEditorWindow* pe)
{
    foreach(NEPropertyEditorWindow* w, P->m_PropEdWins)
    {
        if(w->currentObject() == obj)
        {
            if(w != pe)
            {
                w->showPrevObjProperty();
                break;
            }
        }
    }
}

void NEGUIManager::updateKeyframeEditor()
{
    if(mainWindow())
        mainWindow()->keyframeInspector()->updateKeyframes();
}

void NEGUIManager::updateNodeGraphEditor()
{
    if(mainWindow())
        mainWindow()->nodeGraphEditorWindow()->update();
}

void NEGUIManager::updateNodeGraphEditorForDeletion()
{
    if(mainWindow())
        mainWindow()->nodeGraphEditorWindow()->updateForDeletion();
}


void NEGUIManagerPrivate::mapActionToCreateObject(QAction* action, const QString& type, const NEManager* manager)
{
    QSignalMapper* signalMapper = new QSignalMapper( action );
    QObject::connect( action, SIGNAL(triggered()), signalMapper, SLOT(map()) );
    if( manager != NULL )
    {
        signalMapper->setMapping( action, type );
        QObject::connect( signalMapper, SIGNAL(mapped(const QString&)), manager, SLOT(callPyScriptForObjectCreation(const QString&)) );
    }
}

void NEGUIManagerPrivate::addCreateActionToGUI(const QString& type, const NEManager* manager, const uint nodeType)
{
    QMenu* targetMenu = m_pMainWindow->creationMenu( nodeType );

    QAction* action = new QAction( type, m_pMainWindow );
    targetMenu->addAction( action );
    if( m_pMainWindow->sceneBuilder() )
        m_pMainWindow->sceneBuilder()->addCreateAction( action, nodeType );
    if( m_pMainWindow->nodeGraphEditorWindow() )
        m_pMainWindow->nodeGraphEditorWindow()->addCreateAction( action, nodeType );
    mapActionToCreateObject( action, type, manager );
}

void NEGUIManagerPrivate::removeCreateActionToGUI(const QString& type, const NEManager* manager, const uint nodeType)
{
    QMenu* targetMenu = m_pMainWindow->creationMenu(nodeType);
    QList<QAction*> la = targetMenu->actions();
    foreach(QAction* a, la)
        if(a->text() == type)
        {
            m_pMainWindow->sceneBuilder()->removeCreateAction(a, nodeType);
            m_pMainWindow->nodeGraphEditorWindow()->removeCreateAction( a, nodeType );
            a->disconnect();
            la.removeAll(a);
            delete a;
        }
}

QDebug& NEGUIManager::log()
{
    if (P && mainWindow())
        return mainWindow()->messageWindow()->getNewDebugStream();
}

void NEGUIManager::endLog(NE::NELogType type)
{

    if( NEBASE.guiMode() )
        if(mainWindow() && mainWindow()->isVisible())
            mainWindow()->messageWindow()->pushLogInformation(type);

}

void NEGUIManager::EvalPyGUI(const QString& script)
{
    consoleWindow()->executeCode(script);
}

void NEGUIManager::updateGLWindowsFromBase()
{
    updateGLWindows();
}

void NEGUIManager::addCameraIntoContextMenus(NENode *node)
{
    if(NECamera* tmpObject = qobject_cast<NECamera*> (node))
    {
        P->addCameraIntoContextMenus(tmpObject);
    }
}

void NEGUIManager::removeCameraFromContextMenus(NENode *node)
{
    if(NECamera* tmpObject = qobject_cast<NECamera*> (node))
    {
        P->removeCameraFromContextMenus(tmpObject);
    }
}

void NEGUIManager::removeChart(QString chartName)
{
    if(mainWindow()->chartsVisWindow())
        mainWindow()->chartsVisWindow()->removePlot(chartName);
}

void NEGUIManager::setupChart(QString chartName)
{
    if(mainWindow()->chartsVisWindow())
        mainWindow()->chartsVisWindow()->setupPlot(chartName);
}

void NEGUIManager::clearChart(QString chartName)
{
    if(mainWindow()->chartsVisWindow())
        mainWindow()->chartsVisWindow()->clearPlot(chartName);
}

void NEGUIManager::clearCharts()
{
    if(mainWindow()->chartsVisWindow())
        mainWindow()->chartsVisWindow()->clearPlots();
}

void NEGUIManager::updateCharts()
{
    if(mainWindow()->chartsVisWindow())
        mainWindow()->chartsVisWindow()->updatePlots();
}

void NEGUIManager::setChartType(QString chartType)
{
    if(mainWindow()->chartsVisWindow())
        mainWindow()->chartsVisWindow()->setPlotType(chartType);
}

void NEGUIManager::createNewPropertyEditor()
{
    NEDockWidget* pDock = new NEDockWidget(mainWindow());
    QSizePolicy sp(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sp.setHorizontalStretch(2);
    sp.setVerticalStretch(2);
    sp.setHeightForWidth(pDock->sizePolicy().hasHeightForWidth());
    pDock->setSizePolicy(sp);
    pDock->setMinimumSize(QSize(250, 58));
    pDock->setMaximumSize(QSize(524287, 524287));
    NEPropertyEditorWindow* p = new NEPropertyEditorWindow();

    pDock->setWidget(p);
    mainWindow()->addDockWidget(static_cast<Qt::DockWidgetArea>(1), pDock);
    pDock->setContainedMainWindow(p);

    if( P->m_PropEdWins.last() )
        mainWindow()->tabifyDockWidget(static_cast<QDockWidget*>(P->m_PropEdWins.last()->parentWidget()), pDock);

    P->m_PropEdWins.append(p);
    p->setObjectName(QStringLiteral("propertyEditor") + QString::number(P->m_PropEdWins.size()));
    pDock->setObjectName(QStringLiteral("propertyEditorDock") + QString::number(P->m_PropEdWins.size()));

    p->init();
}

void NEGUIManager::showMainWindow()
{
    if( mainWindow() )
        P->showMainWindow();
}

void NEGUIManager::hideSceneLoadingSplash()
{
    if( mainWindow() )
        mainWindow()->hideSceneLoadingSplash();
}

void NEGUIManager::showSceneManagementDialog()
{
    if( mainWindow() )
        mainWindow()->showSceneManagementDialog();

}

void NEGUIManager::postInit()
{
    P->postInit();
}

void NEGUIManager::layout()
{
    if( mainWindow() && mainWindow()->nodeGraphEditorWindow() )
        mainWindow()->nodeGraphEditorWindow()->layout();
}

void NEGUIManager::updateInitFlagForNodeGraph(bool aInitFlag)
{
    if( mainWindow() && mainWindow()->nodeGraphEditorWindow() )
        mainWindow()->nodeGraphEditorWindow()->updateInitFlag(aInitFlag);
}

void NEGUIManager::updateNodeRenameForNodeGraph(const QString &oldName, const QString &newName)
{
    if( mainWindow() && mainWindow()->nodeGraphEditorWindow() )
        mainWindow()->nodeGraphEditorWindow()->updateNodeRenameForNodeGraph(oldName, newName);
}

void NEGUIManager::deInit()
{
    if(NELOGBROWSER){
        delete NELOGBROWSER;
        NELOGBROWSER = NULL;
    }
    P->deInit();
}

void NEGUIManager::hideSceneLoadingWidget()
{
    if( mainWindow() )
        mainWindow()->sceneLoadingWidget()->hide();
}

void NEGUIManager::showSceneLoadingWidget()
{
    if( mainWindow() )
        mainWindow()->showSceneLoadingWidget();
}

void NEGUIManager::importAssScene( const aiScene *scene)
{
    uint numCams      = scene->mNumCameras;
    uint numLights    = scene->mNumLights;
    //uint numMaterials = scene->mNumMaterials;

    for(uint i = 0; i < numCams; ++i)
    {
        aiCamera* cam = scene->mCameras[i];
        QString camName = QString(cam->mName.C_Str());
        NENode* newNode = NESCENE.createObject("Camera", camName);
        NECamera* newCamNode = qobject_cast< NECamera*>( newNode );
        newCamNode->SetFov( cam->mHorizontalFOV );
        newCamNode->SetNearClip( cam->mClipPlaneNear );
        newCamNode->SetFarClip( cam->mClipPlaneFar );
        newCamNode->setObjectName(QString(cam->mName.C_Str()));

        aiMatrix4x4t<float> transformNode;
        vec3f cposition;
        eulerf cor;
        vec3f cdirection;

        if( scene->mRootNode )
        {
            aiNode* camTransformNode = scene->mRootNode->FindNode( cam->mName );
            if( camTransformNode )
            {
                transformNode = camTransformNode->mTransformation;
                aiVector3t<float> scaling;
                aiQuaterniont<float> rotation;
                aiVector3t<float> position;
                transformNode.Decompose( scaling, rotation, position );
                cposition = vec3f( position.x, position.y, position.z );
                quatf r = quatf( rotation.x, rotation.y, rotation.z, rotation.w );
                cor = eulerAnglesFromQuaternion( r );
                //lor.phi() *= 180/M_PI;
                //lor.psi() *= 180/M_PI;
                //lor.theta() *= 180/M_PI;

                //ldirection = directionFromEulerAngles( lor );
            }
        }
        cposition += vec3f( cam->mPosition.x, cam->mPosition.y, cam->mPosition.z );
        newCamNode->SetPosition( cposition );
        newCamNode->SetOrientation( cor );
        //newCamNode->SetLookAt( vec3f( cam->mLookAt.x, cam->mLookAt.y, cam->mLookAt.z ) );

        for(unsigned i = 0; i < scene->mNumAnimations; ++i)
        {
            aiAnimation* a = scene->mAnimations[i];
            if(QString(a->mName.C_Str()) == camName)
            {
                for(unsigned j = 0; j  < a->mNumChannels; ++j)
                {
                    //aiNodeAnim* na = a->mChannels[j];

                }
            }
        }
    }
    //std::cout << "Num Lights: " << numLights << std::endl;
    for(uint i = 0; i < numLights; ++i)
    {
        aiLight* light = scene->mLights[i];
        QString lightName = QString( light->mName.C_Str() );
        NENode* newNode = NULL;
        //NELight* lightNode;

        aiMatrix4x4t<float> transformNode;
        vec3f lposition;
        eulerf lor;
        vec3f ldirection;
        if( scene->mRootNode )
        {
            aiNode* lightTransformNode = scene->mRootNode->FindNode( light->mName );
            if( lightTransformNode )
            {
                transformNode = lightTransformNode->mTransformation;
                aiVector3t<float> scaling;
                aiQuaterniont<float> rotation;
                aiVector3t<float> position;
                transformNode.Decompose( scaling, rotation, position );
                lposition = vec3f( position.x, position.y, position.z );
                quatf r = quatf( rotation.x, rotation.y, rotation.z, rotation.w );
                lor = eulerAnglesFromQuaternion( r );
                //lor.phi() *= 180/M_PI;
                //lor.psi() *= 180/M_PI;
                //lor.theta() *= 180/M_PI;

                //ldirection = directionFromEulerAngles( lor );
            }
        }

        if( light->mType == aiLightSourceType::aiLightSource_SPOT )
        {
            newNode = NESCENE.createObject( "Spot Light", lightName );
            NESpotLight* newLNode = qobject_cast< NESpotLight* >(newNode);
            newLNode->SetPosition( lposition );
            newLNode->SetOrientation( lor );
            //newLNode->setDirection( ldirection );
            newLNode->SetCutoff( light->mAngleInnerCone * 180/M_PI );
            newLNode->SetCutoff( light->mAngleOuterCone * 180/M_PI );
            newLNode->SetLinearAttenuation( light->mAttenuationLinear );
            newLNode->SetQuadraticAttenuation( light->mAttenuationQuadratic );
            newLNode->SetColor( QColor( light->mColorAmbient.r, light->mColorAmbient.g, light->mColorAmbient.b ) );
        }
        else if( light->mType == aiLightSourceType::aiLightSource_DIRECTIONAL )
        {
            newNode = NESCENE.createObject( "Directional Light", lightName );
            NEDirectionalLight* newLNode = qobject_cast< NEDirectionalLight* >(newNode);
            newLNode->SetPosition( lposition );
            newLNode->SetOrientation( lor );

            //newLNode->SetDirection( vec3f( light->mDirection.x, light->mDirection.y, light->mDirection.z ) );
            newLNode->SetColor( QColor( light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b ) );
        }
        else if( light->mType == aiLightSourceType::aiLightSource_POINT )
        {
            newNode = NESCENE.createObject( "Point Light", lightName );
            NEPointLight* newLNode = qobject_cast< NEPointLight* >(newNode);
            newLNode->SetPosition( lposition );
            newLNode->SetOrientation( lor );

            //newLNode->SetColor( QColor( light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b ) );
        }
        /*
        for(unsigned i = 0; i < scene->mNumAnimations; ++i)
        {
            aiAnimation* a = scene->mAnimations[i];
            if(QString(a->mName.C_Str()) == camName)
            {
                for(unsigned j = 0; j  < a->mNumChannels; ++j)
                {
                    //aiNodeAnim* na = a->mChannels[j];

                }
            }
        }
        */
    }


    if( NEBASE.guiMode() )
    {
        updateSceneInspector();
    }
}

void NEGUIManager::createGLWindow(QString name)
{
    if( P->m_pMainGLWidget->objectName() != name )
    {
        P->createGLWindowOnLoad( name );
    }
}

void NEGUIManager::createPropertyEditor(QString name)
{
    P->m_PropEdObjs.append( name );
    createNewPropertyEditor();
}

void NEGUIManager::restoreGUIState(QXmlStreamReader *xmlReader)
{
    P->restoreGUIStateFromScene( xmlReader );
}

void NEGUIManager::setCurrentOperation(QString msg)
{
    if( mainWindow() &&  mainWindow()->sceneLoadingWidget() )
        mainWindow()->sceneLoadingWidget()->setCurrentOperation( msg );
    return;
}


void NEGUIManager::setCurrentSubOperation(QString msg)
{
    if( mainWindow() && mainWindow()->sceneLoadingWidget() )
        mainWindow()->sceneLoadingWidget()->setCurrentSubOperation( msg );
    return;
}


void NEGUIManager::loadGUIState()
{
    if( !NEBASE.sceneDataPath().isEmpty() && NEBASE.guiMode() )
    {
        P->m_GUIStatePath = NEBASE.sceneDataPath() + "/neutrino.state";
        if( NEBASE.initialized() )
            P->loadGUIState();
    }
}

void NEGUIManager::deleteMainWindow()
{
    if( mainWindow() ){
        delete P->m_pMainWindow;
        P->m_pMainWindow = NULL;
    }
}

void NEGUIManager::setWindowTitle( QString title )
{
    if( mainWindow() )
        mainWindow()->setWindowTitle( title );
}

void NEGUIManager::splitGLDockHorizontally(NEGLWidget *wid)
{
    return P->splitGLDockHorizontally( wid );
}

void NEGUIManager::splitGLDockVertically(NEGLWidget *wid)
{
    return P->splitGLDockVertically( wid );
}

void NEGUIManager::splitGLDockToQuad(NEGLWidget *wid)
{
    return P->splitGLDockToQuad( wid );
}

void NEGUIManager::writeAllGUIState( QXmlStreamWriter *xmlWriter )
{
    if( !xmlWriter )
        return;

    xmlWriter->writeStartElement( "GUI_STATE" );
    P->saveGUIStateIntoScene( xmlWriter );
    xmlWriter->writeEndElement();
}


void NEGUIManager::saveGUIState()
{
    P->m_GUIStatePath = NEBASE.sceneDataPath() + "/neutrino.state";

    if( NEBASE.initialized() )
        P->saveGUIState();
}

void NEGUIManager::writeAllGUICreations( QXmlStreamWriter *xmlWriter )
{
    if( !xmlWriter )
        return;


    // Write all the objects in the scene
    xmlWriter->writeStartElement("guicreations");

    P->writeAllGUISubCreations( xmlWriter );

    // Writing objects ends here
    xmlWriter->writeEndElement();
}



void NEGUIManager::setParent(QObject *varRoot)
{
    NEObject::setParent( varRoot );
}


void NEGUIManager::updateChart(QString chartName, QVector<double>& x, QVector<double>& y)
{
    if( mainWindow() && mainWindow()->chartsVisWindow() )
        mainWindow()->chartsVisWindow()->updatePlot(chartName, x, y);
}


void NEGUIManager::updateGLWindows()
{
    foreach(const GLDockWidgetBundle* w, P->m_IDtoGLDockWidget)
        w->glWidget->update();
}

void NEGUIManager::updateOfflineRenderer()
{
    foreach (NEGLOfflineRenderer *renderer, NERENDERMAN.m_offlineRenderers)
    {
        renderer->update();
    }
    P->timeline()->m_recordingProgress->setValue(NESCENE.CurrentFrame()/(float)NESCENE.PlaybackEndFrame() * 100);
}

void NEGUIManager::finishedOfflineProcess()
{
    P->m_locker.lock();
    P->timeline()->setOfflineRecordingMode(false);
    P->m_locker.unlock();

/*
    QEvent::Type type = QEvent::Type(QEvent::User + 10);
    QEvent *customEvent = new QEvent( type );
    QApplication::postEvent(this, customEvent);
    */
}

void NEGUIManager::startedOfflineProcess()
{
    P->m_locker.lock();
    P->timeline()->setOfflineRecordingMode(true);
    P->m_locker.unlock();
}

void NEGUIManager::updateOfflineProgress(float val)
{
    P->m_locker.lock();
    P->timeline()->m_recordingProgress->setValue(val * 100);
    P->m_locker.unlock();
}


void NEGUIManager::finishedOfflineRendering()
{
    foreach (NEGLOfflineRenderer *renderer, NERENDERMAN.m_offlineRenderers) {
        delete renderer;
    }
    NERENDERMAN.m_offlineRenderers.clear();

    NESCENE.startUpdateTimer(false);

    QEvent::Type type = QEvent::Type(QEvent::User + 10);
    QEvent *customEvent = new QEvent( type );
    QApplication::postEvent(this, customEvent);
}

void NEGUIManager::startedOfflineRendering()
{
    P->timeline()->setOfflineRecordingMode(true);
}

void NEGUIManager::resizeGLWindows()
{
    foreach(const GLDockWidgetBundle* w, P->m_IDtoGLDockWidget){
        // If there is no any OpenGL context assigned for the widget, return immediately
        if(!w->glWidget->isVisible() || !w->glWidget->isValid() || !NESCENE.initialized() || !NEBASE.initialized())
            return;
        w->glWidget->resize(w->glWidget->width(), w->glWidget->height());
    }
}

bool NEGUIManager::event(QEvent *e)
{
    if(e->type() != QEvent::User + 10)
        return NEObject::event(e);
    else
    {
        P->timeline()->setOfflineRecordingMode(false);
        P->timeline()->setCurrentFrame(0);
        NESCENE.SetCurrentFrame(0);
        updatePropertyEditor();
        return true;
    }
}



void NEGUIManager::updateGLMotionMap()
{
    //((NEGLWidget*)m_pGLWidgetPersp)->renderMotionVectors();
}


void NEGUIManager::updateNodeRenameToAllGUI(NENode* node)
{
    if(NESCENE.selectedObject() == node)
    {
        updatePropertyEditorTitle(node->objectName());
    }
    // Update node graph editor
    updateNodeGraphEditor();
    // Update keyframe inspector
    if(mainWindow() && mainWindow()->keyframeInspector()->activeNode() == node)
        mainWindow()->keyframeInspector()->updateNodeName();
    // Update the connection inspector
    if(mainWindow())
        mainWindow()->connectionInspector()->updateNodeRenameToEditors(node);
}

void NEGUIManager::showMainGLWidget()
{
    return P->showMainGLWidget();
}

NEMainWindow* NEGUIManager::mainWindow()
{
    return P->m_pMainWindow;
}

NEConsoleWindow* NEGUIManager::consoleWindow()
{
    return P->m_pMainWindow->consoleWindow();
}


bool NEGUIManagerPrivate::resizeGLWidget(int width, int height, bool fixed)
{
    m_IDtoGLDockWidget.last()->glWidget->setMaximumSize(16777215, 16777215);
    m_IDtoGLDockWidget.last()->glWidget->setMinimumSize(0, 0);
    m_IDtoGLDockWidget.last()->glWidget->adjustSize();
    return true;
}

void NEGUIManagerPrivate::makeConnections()
{
    if( !NEBASE.guiMode() )
        return;

    // Initialize the timeline related connections
    timeline()->init();

    QObject::connect( &NEBASE, SIGNAL(showMainWindow()), S, SLOT(showMainWindow()) );
    QObject::connect( &NEBASE, SIGNAL(hideSceneLoadingSplash()), S, SLOT(hideSceneLoadingSplash()) );
    QObject::connect( &NEBASE, SIGNAL(showSceneLoadingWidget()), S, SLOT(showSceneLoadingWidget()) );
    QObject::connect( &NEBASE, SIGNAL(hideSceneLoadingWidget()), S, SLOT(hideSceneLoadingWidget()) );
    QObject::connect( &NEBASE, SIGNAL(showSceneManagementDialog()), S, SLOT(showSceneManagementDialog()) );
    QObject::connect( &NEBASE, SIGNAL(postInit()), S, SLOT(postInit()) );
    QObject::connect( &NEBASE, SIGNAL(layout()), S, SLOT(layout()) );
    //    QObject::connect( &NEBASE, SIGNAL(updateInitFlagForNodeGraph(bool)), S, SLOT(updateInitFlag(bool)), Qt::ConnectionType::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(deInitUI()), S, SLOT(deInit()) );
    QObject::connect( &NEBASE, SIGNAL(setCurrentOperation(QString)), S, SLOT(setCurrentOperation(QString)) );
    QObject::connect( &NEBASE, SIGNAL(setCurrentSubOperation(QString)), S, SLOT(setCurrentSubOperation(QString)) );

    QObject::connect( &NEBASE, SIGNAL(selectObject(QObject *)), S, SLOT(selectObject(QObject *)) );
    //QObject::connect( &NESCENE, SIGNAL(selectObject(QObject *)), S, SLOT(selectObject(QObject *)) );

    QObject::connect( &NEBASE, SIGNAL(updateSceneInspector()), S, SLOT(updateSceneInspector()) );
    QObject::connect( &NESCENE, SIGNAL(updateSceneInspector()), S, SLOT(updateSceneInspector()) );
    QObject::connect( &NECOLMGR, SIGNAL(updateSceneInspector()), S, SLOT(updateSceneInspector()) );


    QObject::connect( &NEBASE, SIGNAL(updateSystemInspector()), S, SLOT(updateSystemInspector()) );

    QObject::connect( &NEBASE, SIGNAL(updatePropertyEditor()), S, SLOT(updatePropertyEditor()) );
    QObject::connect( &NESCENE, SIGNAL(updatePropertyEditor()), S, SLOT(updatePropertyEditor()) );

    QObject::connect( &NEBASE, SIGNAL(updatePropertyEditorsAtPostInit()), S, SLOT(updatePropertyEditorsAtPostInit()) );

    QObject::connect( &NEBASE, SIGNAL(updateAllConnectionTables()), S, SLOT(updateAllConnectionTables()) );
    QObject::connect( &NEBASE, SIGNAL(updateExpressionInspector()), S, SLOT(updateExpressionInspector()) );

    QObject::connect( &NEBASE, SIGNAL(updateGLWindows()), S, SLOT(updateGLWindows()) );
    QObject::connect( &NESCENE, SIGNAL(updateGLWindows()), S, SLOT(updateGLWindows()) );
    QObject::connect( &NESCENE, SIGNAL(updateOfflineRenderer()), S, SLOT(updateOfflineRenderer()) );
    QObject::connect( &NESCENE, SIGNAL(updateOfflineProgress(float)), S, SLOT(updateOfflineProgress(float)) );
    QObject::connect( &NESCENE, SIGNAL(startedOfflineRendering()), S, SLOT(startedOfflineRendering()) );
    QObject::connect( &NESCENE, SIGNAL(finishedOfflineRendering()), S, SLOT(finishedOfflineRendering()) );

    QObject::connect( &NESCENE, SIGNAL(startedOfflineProcess()), S, SLOT(startedOfflineProcess()) );
    QObject::connect( &NESCENE, SIGNAL(finishedOfflineProcess()), S, SLOT(finishedOfflineProcess()) );


    QObject::connect( &NEBASE, SIGNAL(updateNodeGraphEditor()), S, SLOT(updateNodeGraphEditor()) );
    QObject::connect( &NESCENE, SIGNAL(updateNodeGraphEditor()), S, SLOT(updateNodeGraphEditor()) );

    QObject::connect( &NEBASE, SIGNAL(updateWindowVisibilityFlags()), S, SLOT(updateWindowVisibilityFlags()) );
    QObject::connect( &NEBASE, SIGNAL(deleteMainWindow()), S, SLOT(deleteMainWindow()) );
    QObject::connect( &NEBASE, SIGNAL(setWindowTitle(QString)), S, SLOT(setWindowTitle(QString)) );

    QObject::connect( &NEBASE, SIGNAL(loadGUIState()), S, SLOT(loadGUIState()) );
    QObject::connect( &NEBASE, SIGNAL(saveGUIState()), S, SLOT(saveGUIState()) );

    QObject::connect( &NEBASE, SIGNAL(writeAllGUICreations( QXmlStreamWriter *) ),
                      S, SLOT(writeAllGUICreations( QXmlStreamWriter *) ) );

    QObject::connect( &NEBASE, SIGNAL(writeAllGUIState( QXmlStreamWriter *)),
                      S, SLOT(writeAllGUIState( QXmlStreamWriter *)) );
    QObject::connect( &NEBASE, SIGNAL(restoreGUIState( QXmlStreamWriter *)),
                      S, SLOT(restoreGUIState( QXmlStreamWriter *)) );

    QObject::connect( &NEBASE, SIGNAL(setParentUI(QObject *)),
                      S, SLOT(setParent(QObject *)) );

    QObject::connect( &NEBASE, SIGNAL(closeAllPropertyEditors()),
                      S, SLOT(closeAllPropertyEditors()) );
    QObject::connect( &NEBASE, SIGNAL(closeAllGLDocks()),
                      S, SLOT(closeAllGLDocks()) );

    QObject::connect( &NEBASE, SIGNAL(importAssScene(aiScene*)), S, SLOT(importAssScene(aiScene*)) );
    QObject::connect( &NEBASE, SIGNAL(updateNodeRename(NENode*)),
                      S, SLOT(updateNodeRenameToAllGUI(NENode*)) );

    QObject::connect( &NEBASE, SIGNAL(createGLWindow(QString)),
                      S, SLOT(createGLWindow(QString)) );
    QObject::connect( &NEBASE, SIGNAL(createGLWindowOnLoad(QString)),
                      S, SLOT(createGLWindowOnLoad(QString)) );

    QObject::connect( &NEBASE, SIGNAL(createPropertyEditor(QString)),
                      S, SLOT(createPropertyEditor(QString)) );

    QObject::connect( &NESCENE, SIGNAL(updateWidgetsForCreation()),
                      S, SLOT(updateWidgetsForCreation()) );
    QObject::connect( &NESCENE, SIGNAL(updateWidgetsForDeletion(QObject *)),
                      S, SLOT(updateWidgetsForDeletion(QObject *)) );
    QObject::connect( &NESCENE, SIGNAL(pushUserEvent(NEUserEvent*)),
                      S, SLOT(pushUserEvent(NEUserEvent*)) );


    QObject::connect( &NESCENE, SIGNAL(resetWidgets()), S, SLOT( resetWidgets()) );
    QObject::connect( &NESCENE, SIGNAL(updatePropertyConnectionsTable()),S, SLOT(updateAllConnectionTables()) );

    QObject::connect( &NESCENE, SIGNAL(CurrentFrameChanged(int)), S, SLOT( setCurrentFrame(int)) );

    QObject::connect( &NESCENE, SIGNAL(repaintTimeline()), S, SLOT( repaintTimeline()) );
    QObject::connect( &NESCENE, SIGNAL(setPlay(bool)), S, SLOT( setPlay(bool)) );

    QObject::connect( &NESCENE, SIGNAL(selectObjectUI(QObject *)), S, SLOT( selectObject(QObject *)) );

    QObject::connect( &NESCENE, SIGNAL(setTimelineKeyframeMarks(QList<uint>)),
                      S, SLOT(setTimelineKeyframeMarks(QList<uint>))
                      );

    QObject::connect( &NESCENE, SIGNAL(updateKeyframeEditor()),
                      S, SLOT(updateKeyframeEditor()) );

	QObject::connect( &NESCENE, SIGNAL(SimulationEndFrameChanged( int) ),
                      S, SLOT(setSimulationEndFrame( int )) );
    QObject::connect( &NESCENE, SIGNAL(PlaybackStartFrameChanged( int) ),
                      S, SLOT(setPlaybackStartFrame( int )) );
    QObject::connect( &NESCENE, SIGNAL(PlaybackEndFrameChanged( int) ),
                      S, SLOT(setPlaybackEndFrame( int )) );

	QObject::connect(&NESCENE, SIGNAL(SimulationEndTimeChanged(double)),
                      S, SLOT(setSimulationEndTime(double)));
	QObject::connect(&NESCENE, SIGNAL(PlaybackStartTimeChanged(double)),
                      S, SLOT(setPlaybackStartTime(double)));
	QObject::connect(&NESCENE, SIGNAL(PlaybackEndTimeChanged(double)),
                      S, SLOT(setPlaybackEndTime(double)));

	QObject::connect(S, SIGNAL(TimelineUnitsChanged(NETimelineUnits)),
                        &NESCENE, SLOT(setTimeLineUnits(NETimelineUnits)));

    QObject::connect( &NESCENE, SIGNAL(setSimulate( bool) ), S, SLOT(setSimulate( bool )));
    QObject::connect( &NESCENE, SIGNAL(setRepeat( int) ), S, SLOT(setRepeat( int )) );

    QObject::connect( &NESCENE, SIGNAL(triggerSavingData( ) ), S, SLOT(triggerSavingData(  )) );

    QObject::connect( &NESCENE, SIGNAL(updateCharts( ) ), S, SLOT(updateCharts(  )) );

    QObject::connect( &NESIMMGR, SIGNAL(managersCreated(QList<std::shared_ptr<NEManager> >) ),
                      S, SLOT(managersCreated(QList<std::shared_ptr<NEManager> >)) );

    QObject::connect( &NESCENE, SIGNAL(managersCreated(QList<std::shared_ptr<NEManager> >) ),
                      S, SLOT(managersCreated(QList<std::shared_ptr<NEManager> >)));

    QObject::connect( &NERENDERMAN, SIGNAL(managerCreated(NEManager*) ),
                      S, SLOT(managerCreated(NEManager*)) );

    QObject::connect( &NERENDERMAN, SIGNAL(getActiveCameraAndResolution(QMap<QString,QSize>&)),
                      S, SLOT(getActiveCameraAndResolution(QMap<QString,QSize>&)) );

    QObject::connect( &NERENDERMAN, SIGNAL(updateGLWindows() ),
                      S, SLOT(updateGLWindows()) );

    QObject::connect( &NESIMMGR, SIGNAL(addCreateActionToGUI( const QString&, const NEManager*, const uint)),
                      S, SLOT(addCreateActionToGUI( const QString&, const NEManager*, const uint)));
    QObject::connect( &NESIMMGR, SIGNAL(removeCreateActionToGUI( const QString&, const NEManager*, const uint)),
                      S, SLOT(removeCreateActionToGUI( const QString&, const NEManager*, const uint)) );

    foreach(const GLDockWidgetBundle* w, m_IDtoGLDockWidget)
    {

        QObject::connect(w->glWidget, SIGNAL(popupDescriptionAndName(const QPoint&, QWidget *, NENode *, QPainter *)),
                         S, SLOT(popupDescriptionAndName(QPoint,QWidget*,NENode*,QPainter*)));

        QObject::connect(w->glWidget, SIGNAL(showPropertyShortCut(const QPoint,QWidget*,NENode*)),
                         S, SLOT(showPropertyShortCutDialog(const QPoint,QWidget*,NENode*)));
    }
}


void NEGUIManagerPrivate::setTimelineMinFrame(int val)
{
    timeline()->setPlaybackStartFrame(val);
}

void NEGUIManagerPrivate::setCurrentFrame(double val)
{
    timeline()->setCurrentFrame(val);
}

void NEGUIManagerPrivate::setTimelineMinSimulated(int start, int end)
{
    //timeline()->timelineWidget->setSimulatedMin(start,end);
}

void NEGUIManagerPrivate::setTimelineKeyframeMarks(QList<uint> marks)
{
    timeline()->timelineWidget->setKeyframeMarks(marks);
}

void NEGUIManagerPrivate::setTimelineMaxSimulated(int start, int end)
{
    //timeline()->setSimulatedMax(start,end);
}


void NEGUIManagerPrivate::prepareForObjectRemoval(NENode* object)
{
    if(S->mainWindow()->sceneInspectorWindow()->isSelectedObject(object))
        S->mainWindow()->sceneInspectorWindow()->setParentCurrentItem();

    if(NEPropertyEditorWindow::m_pPrevObject == object)
    {
        NEPropertyEditorWindow::m_pPrevObject = 0;
    }
}

void NEGUIManagerPrivate::writeAllGUISubCreations( QXmlStreamWriter *xmlWriter )
{
    if( !xmlWriter )
        return;

    // Write the opengl window creations
    foreach(const GLDockWidgetBundle* dock, m_IDtoGLDockWidget)
    {
        xmlWriter->writeStartElement("guiobject");
        xmlWriter->writeAttribute("type", "glwindow");
        xmlWriter->writeAttribute("name", dock->glWidget->objectName());
        xmlWriter->writeEndElement();
    }

    // Write the property editor window creations
    foreach(NEPropertyEditorWindow* w, m_PropEdWins)
    {
        xmlWriter->writeStartElement("guiobject");
        xmlWriter->writeAttribute("type", "proped");
        xmlWriter->writeAttribute("name", w->objectName());
        if(w->currentObject())
            xmlWriter->writeAttribute("curobj", w->currentObject()->objectName());
        xmlWriter->writeEndElement();
    }
}


void NEGUIManager::showPropertyShortCutDialog(const QPoint& pos, QWidget *w, NENode *selSpatial)
{
    if(selSpatial && NEGLWidget::m_ActiveManipulator != NEManipulator::kManipNone){
        mainWindow()->showPropertyShortCutDlg(w, selSpatial, NEGLWidget::m_ActiveManipulator);
    }
}

void NEGUIManager::popupDescriptionAndName(const QPoint& pos, QWidget *w, NENode *selSpatial, QPainter *painter)
{

//    if(selSpatial){

//        painter->setPen(Qt::NoPen);
//        painter->setBrush(QColor(0, 0, 0, 127));

//        QPainterPath path;
//        path.addRoundedRect(pos.x() - 120, pos.y() - 120, 180, 90, 8, 8);

//        path.moveTo(pos.x(), pos.y());
//        path.lineTo(pos.x() - 112, pos.y() - 30);
//        path.lineTo(pos.x() + 52, pos.y() - 30);
//        path.closeSubpath();

//        painter->drawPath(path);

//        QFont font(QString("Segoe UI"));
//        font.setPixelSize(12);
//        font.setBold(true);
//        painter->setFont(font);

//        QPen pen(QColor(255, 255, 255, 150));
//        pen.setStyle(Qt::SolidLine);
//        painter->setPen(pen);

////        NESpatialEngine *engine = qobject_cast<NESpatialEngine *>(selSpatial);
////        NEZone *zone = qobject_cast<NEZone *>(selSpatial);
////        NEScenario *scn = qobject_cast<NEScenario *>(selSpatial);
////        NEZoneItem *zitem = qobject_cast<NEZoneItem *>(selSpatial);
////        QString tooltipText;
////        if(zone)
////            tooltipText = zone->objectName() + "\n" + zone->nodeDescription();
////        else if(scn)
////            tooltipText = scn->objectName() + "\n" + scn->nodeDescription();
////        else if(zitem)
////            tooltipText = zitem->objectName() + "\n" + zitem->nodeDescription();
////        else if(engine)
////            tooltipText = engine->objectName() + "\n" + engine->nodeDescription();

////        painter->drawText( QRect(pos.x() - 120, pos.y() - 120, 180, 90), Qt::AlignHCenter | Qt::AlignVCenter, tooltipText);

//        QTimer::singleShot(2000, w, SLOT(hideDescription()));
//    }

}


void NEGUIManager::selectObject(QObject* object)
{
    if (isDuringSelectionProcess())
        return;
    if(object == NESCENE.selectedObject())
        return;
    NEObject* node = qobject_cast<NEObject*>(object);
    if(object != 0 && node == 0)
        return;
    SetSelectionProcess(true);
    NESCENE.selectObject(qobject_cast<NENode*>(object));
    if(NENode *nd = qobject_cast<NENode*>(object))
        NESCENE.selectAdditionalObject(nd);
    mainWindow()->sceneInspectorWindow()->setNewCurrentItem(object);
    mainWindow()->propertyEditorWindow()->showObjectProperty(object);
    hideObjectFromOtherPropEditors(object, mainWindow()->propertyEditorWindow());
    mainWindow()->nodeGraphEditorWindow()->selectItem(qobject_cast<NENode*>(object));

    NESpatial* sp = qobject_cast<NESpatial*>(object);
    if(sp != 0)
    {
        P->setTimelineMinSimulated(sp->FrameInfo().firstFrame(), sp->FrameInfo().lastFrame());
    }
    else
    {
        P->setTimelineMinSimulated(0, 0);
    }
    updateGLWindows();
    SetSelectionProcess(false);
}

void NEGUIManager::selectGUIObject(QObject* object)
{
    mainWindow()->systemInspectorWindow()->setNewCurrentItem(object);
    P->m_pMainWindow->propertyEditorWindow()->showObjectProperty(object);
}

void NEGUIManager::selectObjects(QList<NENode*>& objects)
{
    if (isDuringSelectionProcess())
        return;
    if(objects.size() == 0)
        return;
    SetSelectionProcess(true);
    foreach(NENode* np, objects)
        NESCENE.selectAdditionalObject(np);
    mainWindow()->nodeGraphEditorWindow()->selectItem(objects);
    P->m_pMainWindow->propertyEditorWindow()->showObjectProperty(objects.last());
    NESpatial* sp = qobject_cast<NESpatial*>(objects.last());
    if(sp != 0)
    {
        P->setTimelineMinSimulated(sp->FrameInfo().firstFrame(), sp->FrameInfo().lastFrame());
    }
    else
    {
        P->setTimelineMinSimulated(0, 0);
    }
    updateGLWindows();
    SetSelectionProcess(false);
}

QMap<QString, QSize> g_activeCameras;
void NEGUIManager::getActiveCameraAndResolution( QMap<QString, QSize> &activeCameras)
{
    g_activeCameras.clear();
    foreach(const GLDockWidgetBundle* w, P->m_IDtoGLDockWidget){
        g_activeCameras[w->glWidget->camera()->objectName()] = w->glWidget->P()->imageSizeForSaving();
    }
    activeCameras = g_activeCameras;
}

void NEGUIManagerPrivate::addCameraIntoContextMenus(NECamera* pCamera)
{
    foreach(const GLDockWidgetBundle* w, m_IDtoGLDockWidget)
        w->mainWindow->addCamera(pCamera);
}

void NEGUIManagerPrivate::removeCameraFromContextMenus(NECamera* pCamera)
{
    foreach(const GLDockWidgetBundle* w, m_IDtoGLDockWidget)
        w->mainWindow->removeCamera(pCamera);
}

void NEGUIManagerPrivate::createGLWindowOnLoad(QString name)
{
    GLDockWidgetBundle* bundle = createGLWidget(name);
    S->mainWindow()->splitDockWidget(m_GLtoGLDockWidget[m_pMainGLWidget]->dockWidget, bundle->dockWidget, Qt::Horizontal);
    m_GLtoGLDockWidget[bundle->glWidget]->mainWindow->addCameras( NERENDERMAN.cameras() );
}

GLDockWidgetBundle* NEGUIManagerPrivate::createGLWidget(QString name)
{
    int i = 0;
    if(name.isNull())
        name = QString("OpenGL%1").arg(i);
    foreach(QObject* child, S->children())
    {
        if(name == child->objectName())
        {
            ++i;
            name = QString("OpenGL%1").arg(i);
        }
    }

    // Create GL widget bookeeping object
    GLDockWidgetBundle* wBundle = new GLDockWidgetBundle;
    // Give id to the widget
    wBundle->id = m_IDtoGLDockWidget.size();
    // Create a dockwidget
    wBundle->dockWidget = new NEDockWidget(S->mainWindow());
    // Create a GL main window
    wBundle->mainWindow = new NEGLMainWindow(0);
    wBundle->mainWindow->m_pBundle = wBundle;
    wBundle->dockWidget->setContainedMainWindow(wBundle->mainWindow);
    // Set object and window name for the dock widget
    wBundle->dockWidget->setWindowTitle(name);
    wBundle->dockWidget->setObjectName(name);
    // Create the GL Widget
    wBundle->glWidget = new NEGLWidget(wBundle->mainWindow->centralWidget());

    QObject::connect( wBundle->glWidget, SIGNAL(selectObject(QObject *)), S, SLOT(selectObject(QObject *)) );
    QObject::connect( wBundle->glWidget, SIGNAL(selectObjects(QList<NENode*> &)), S, SLOT(selectObjects(QList<NENode*> &))  );

    QObject::connect( wBundle->glWidget, SIGNAL(updatePropertyEditorUI()), S, SLOT(updatePropertyEditor()) ) ;
    QObject::connect( wBundle->glWidget, SIGNAL(updateGLWindows()), S, SLOT(updateGLWindows()) );

    wBundle->glWidget->setObjectName(name);
    wBundle->glWidget->m_pBundle = wBundle;
    wBundle->glWidget->P()->setObjectName(name);
    wBundle->glWidget->installEventFilter( reinterpret_cast< QMainWindow* >( wBundle->mainWindow) );

    // Create a layout for the gl widget
    QHBoxLayout* layout = new QHBoxLayout(wBundle->mainWindow->centralWidget());
    layout->setContentsMargins(0, 0, 0, 0);
    // Insert the gl widget into the created layout
    layout->addWidget(wBundle->glWidget);
    // Connect actions of the Mainwindow of the gl widget to the appropriate slots
    wBundle->mainWindow->setupConnections();
    wBundle->dockWidget->setWidget(wBundle->mainWindow);
    m_IDtoGLDockWidget.insert(m_IDtoGLDockWidget.size(), wBundle);
    m_GLtoGLDockWidget.insert(wBundle->glWidget, wBundle);
    // Set the properties of the widget as the child of the GUI manager
    wBundle->glWidget->P()->setParent(S);
    getPyMain().addObject(wBundle->glWidget->P()->objectName(), wBundle->glWidget->P());
    return wBundle;
}

void NEGUIManagerPrivate::updatePropertyEditorsAtPostInit()
{
    if(m_PropEdObjs.isEmpty())
        return;

    QList<QObject*> objs;
    foreach(const QString& s, m_PropEdObjs)
    {
        foreach(QObject* o, NESCENE.children())
        {
            if(s == o->objectName())
                objs.push_back(o);

        }
    }

    if(objs.isEmpty())
        return;

    foreach(NEPropertyEditorWindow* p, m_PropEdWins)
    {
        p->showObjectProperty(objs.first());
        objs.pop_front();

        if(objs.isEmpty())
            break;
    }
}




void NEGUIManagerPrivate::closeGLWidget(NEGLWidget* val)
{
    val->setCamera(0);
    GLDockWidgetBundle* dock = m_GLtoGLDockWidget[val];
    if(!dock)
        return;
    // Don't delete the main OpenGL widget, just hide it
    if(val == m_pMainGLWidget)
    {
        hideGLWidget(val);
        return;
    }
    S->mainWindow()->propertyEditorWindow()->removeObject(val->P());
    m_IDtoGLDockWidget.remove(dock->id);
    m_GLtoGLDockWidget.remove(val);
    dock->dockWidget->hide();
    getPyMain().removeVariable(val->objectName());
    S->mainWindow()->removeDockWidget(dock->dockWidget);
    dock->dockWidget->close();
    dock->glWidget->P()->setParent(0);
    dock->dockWidget->setParent(0);
    //delete dock;
    S->selectGUIObject(0);
    S->updateSystemInspector();
}

void NEGUIManagerPrivate::hideGLWidget(NEGLWidget* val)
{
    GLDockWidgetBundle* dock = m_GLtoGLDockWidget[val];
    dock->dockWidget->hide();
}

void NEGUIManagerPrivate::showMainGLWidget()
{
    GLDockWidgetBundle* dock = m_GLtoGLDockWidget[m_pMainGLWidget];
    dock->dockWidget->show();
    m_pMainGLWidget->setCamera(NERENDERMAN.defaultCamera());
}

void NEGUIManagerPrivate::setCameraForGLDock(GLDockWidgetBundle* bundle, NECamera* cam)
{
    bundle->mainWindow->setCamera(cam);
}

void NEGUIManagerPrivate::closePropertyEditor(NEPropertyEditorWindow* w)
{
    if(w == S->mainWindow()->propertyEditorWindow())
    {
        w->parentWidget()->hide();
        return;
    }

    QList<NEPropertyEditorWindow*>::iterator i = m_PropEdWins.begin();
    for(; i != m_PropEdWins.end(); ++i)
        if(*i == w)
            break;

    m_PropEdWins.erase(i);

    NEDockWidget* dw = static_cast<NEDockWidget*>(w->parentWidget());
    S->mainWindow()->removeDockWidget(dw);
    dw->close();

}

void NEGUIManager::closeAllPropertyEditors()
{
    mainWindow()->propertyEditorWindow()->parentWidget()->hide();

    P->m_PropEdWins.clear();
    P->m_PropEdObjs.clear();

    NEObjectController::clearPropertiesMap();

    P->m_PropEdWins.append(mainWindow()->propertyEditorWindow());

    return;
}

void NEGUIManager::closeAllGLDocks()
{
    P->closeAllGLDocks();
}

NEConnectionInspector* NEGUIManager::connectionInspector()
{
    return mainWindow()->connectionInspector();
}

NEExpressionInspector* NEGUIManager::expressionInspector()
{
    return P->m_pMainWindow->expressionInspector();
}

void NEGUIManagerPrivate::enableSimulationWidgets(bool b)
{
    timeline()->enableSimulationUI(b);
}

void NEGUIManagerPrivate::enablePlaybackWidgets(bool b)
{
    timeline()->enablePlaybackUI(b);
}

void NEGUIManagerPrivate::setSimulateButton(bool val)
{
    timeline()->repeatButton->blockSignals(true);
    timeline()->simulateButton->setChecked(val);
    timeline()->repeatButton->blockSignals(false);
}

void NEGUIManagerPrivate::setRepeatButton(bool val)
{
    timeline()->repeatButton->blockSignals(true);
    timeline()->repeatButton->setChecked(val);
    timeline()->repeatButton->blockSignals(false);
}

bool NEGUIManagerPrivate::simulateButtonState()
{
    return timeline()->simulateButton->isChecked();
}

void NEGUIManagerPrivate::setPlayButton(bool val)
{
    timeline()->repeatButton->blockSignals(true);
    timeline()->playButton->setChecked(val);
    timeline()->repeatButton->blockSignals(false);
    resizeGLWidgetsForFrameSaving(val);
}

bool NEGUIManagerPrivate::playButtonState()
{
    return timeline()->playButton->isChecked();
}

void NEGUIManagerPrivate::repaintTimeline()
{
    if( timeline() )
        timeline()->repaint();
}

// int NEGUIManagerPrivate::playbackStartFrameOrTime()
// {
//     return timeline()->playbackStartSpinBox->value();
// }

void NEGUIManagerPrivate::setPlaybackStartFrameOrTime(double val)
{
	if(timeline()->playbackStartSpinBox->value() != val)
		timeline()->playbackStartSpinBox->setValue(val);
}

// int NEGUIManagerPrivate::playbackEndFrameOrTime()
// {
//     return timeline()->playbackEndSpinBox->value();
// }

void NEGUIManagerPrivate::setPlaybackEndFrameOrTime(double val)
{
	if( timeline()->playbackEndSpinBox->value() != val)
		timeline()->playbackEndSpinBox->setValue(val);
}

// int NEGUIManagerPrivate::simulationEndFrameOrTime()
// {
//     return timeline()->maxFramesSpinBox->value();
// }

void NEGUIManagerPrivate::setSimulationEndFrameOrTime(double val)
{
	if(timeline()->maxFramesSpinBox->value() != val)
		timeline()->maxFramesSpinBox->setValue(val);
}

void NEGUIManager::updateAllConnectionTables()
{
    connectionInspector()->updateAllPropertyConnectionsTable();
    connectionInspector()->updateAllInternalConnectionsTable();
    connectionInspector()->updateAllNodeConnectionsTable();
    connectionInspector()->updateAllPropertyCallbacksTable();
}

void NEGUIManagerPrivate::closeAllGLDocks()
{
    QList<NEGLWidget*> glDocksToDelete;
    // Collect the docks except the main dock widget
    foreach(GLDockWidgetBundle* w, m_IDtoGLDockWidget)
        if(w->glWidget != m_pMainGLWidget)
            glDocksToDelete.push_back(w->glWidget);
    // Close them
    foreach(NEGLWidget* w, glDocksToDelete)
    {
        closeGLWidget(w);
    }
}

void NEGUIManagerPrivate::triggerSavingGLData()
{
    if(!S->SaveGLFrames())
        return;
    foreach(const GLDockWidgetBundle* w, m_IDtoGLDockWidget)
    {
        if(w->glWidget->P()->saveGLFrames())
            w->glWidget->saveGLFrameData();
    }
}

void NEGUIManager::resetWidgets()
{
    P->timeline()->simulateButton->setChecked(false);
    mainWindow()->connectionInspector()->resetEditor();
    mainWindow()->keyframeInspector()->resetEditor();
}

NEGPUBufferVis* NEGUIManager::GPUBufferVis()
{
    return mainWindow()->gpuBuffersWindow()->bufferVis();
}

void NEGUIManager::undo()
{
    if(!P->m_UndoStack.size())
        return;
    // Get the topmost undo event
    NEUserEvent* event = P->m_UndoStack.top();

    NEUserEvent* redoevent = event->undo(event);
    P->m_RedoStack.push(redoevent);

    // Pop from the undo stack
    delete P->m_UndoStack.pop();
    NEPushAttrib(NE_UNDO_REDO_STATE);
    EnableUndoRedo(false);
    NESCENE.cleanup();
    updateSceneInspector();
    updateSystemInspector();
    updateGLWindows();
    updatePropertyEditor();
    if (redoevent->type == kUserEventObjectCreation ||
            redoevent->type == kUserEventObjectDeletion ||
            redoevent->type == kUserEventNodeConnection ||
            redoevent->type == kUserEventNodeDisconnection)
        updateNodeGraphEditor();
    NEPopAttrib(NE_UNDO_REDO_STATE);
}

void NEGUIManager::redo()
{
    if(!P->m_RedoStack.size())
        return;
    // Get the topmost redo event
    NEUserEvent* event = P->m_RedoStack.top();
    NEUserEvent* undoevent = event->redo(event);
    P->m_UndoStack.push(undoevent);

    // Pop from the redo stack
    delete P->m_RedoStack.pop();
    NEPushAttrib(NE_UNDO_REDO_STATE);
    EnableUndoRedo(false);
    NESCENE.cleanup();
    updateSceneInspector();
    updateSystemInspector();
    updateGLWindows();
    updatePropertyEditor();
    if (undoevent->type == kUserEventObjectCreation ||
            undoevent->type == kUserEventObjectDeletion ||
            undoevent->type == kUserEventNodeConnection ||
            undoevent->type == kUserEventNodeDisconnection)
        updateNodeGraphEditor();
    NEPopAttrib(NE_UNDO_REDO_STATE);
}

void NEGUIManager::showExternalRendererSettingsDialog()
{
    P->m_pExtRenderWindow->show();
}

void NEGUIManager::showNodePropertyExpression(const NENode* val, const QString P)
{
    // Remove the previous object
    getPyMain().removeVariable("this");
    // Add the object
    getPyMain().addObject("this", static_cast<QObject*>(const_cast<NENode*>(val)));
    mainWindow()->expressionEditor()->showNodePropertyExpression(val, P);
    mainWindow()->expressionEditorDock()->setTitleText("Expression Editor - [" + val->objectName() + "::" + P + "]");
}

bool NEGUIManager::OpenGLUpdate() const
{
    return P->m_OpenGLUpdate;
}

void NEGUIManager::showHelp()
{
    NEHELP.show();
}

void NEGUIManager::showColliderDialog()
{
    NEColliderDialog selectDialog(mainWindow());
    selectDialog.exec();
}

void NEGUIManagerPrivate::updateWidgetsForDeletion(QObject* object)
{
    S->mainWindow()->propertyEditorWindow()->removeObject(object);
    S->connectionInspector()->deselectObject(object);
    S->updatePropertyConnectionsTable();
    S->updateNodeConnectionsTable();
    S->updatePropertyCallbacksTable();
    S->updateInternalConnectionsTable();
    S->updateNodeGraphEditorForDeletion();
}

void NEGUIManagerPrivate::updateWidgetsForCreation()
{
    S->updatePropertyConnectionsTable();
    S->updateNodeConnectionsTable();
    S->updatePropertyCallbacksTable();
    S->updateInternalConnectionsTable();
    S->updateNodeGraphEditor();
}

bool NEGUIManagerPrivate::printGLInfo()
{
    int temp;
    printf("Neutrino: Info - OpenGL version: %s by %s\n", GL->glGetString(GL_VERSION), GL->glGetString(GL_VENDOR));
    printf("#---OpenGL Info---#\n\n");
    printf("OpenGL Version: %s\n", GL->glGetString(GL_VERSION));
    temp = atoi((const char*)GL->glGetString(GL_VERSION));
    if(temp==1)	return false;
    printf("GLSL Version: %s\n\n", GL->glGetString(GL_SHADING_LANGUAGE_VERSION));
    /*glGetIntegerv(GL_MAX_LIGHTS, &temp);
    printf("gl_MaxLights = %d\n", temp);
    glGetIntegerv(GL_MAX_CLIP_PLANES, &temp);
    printf("gl_MaxClipPlanes = %d\n", temp);
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &temp);
    printf("gl_MaxTextureUnits = %d\n", temp);*/

    GL->glGetIntegerv(GL_MAX_TEXTURE_COORDS, &temp);
    //printf("Max tex coords : %d\n", temp); //printf("gl_MaxTextureCoords = %d\n", temp);

    /*glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &temp);
    printf("gl_MaxVertexAttribs = %d\n", temp);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &temp);
    printf("gl_MaxVertexUniformComponents = %d\n", temp);
    glGetIntegerv(GL_MAX_VARYING_FLOATS, &temp);
    printf("gl_MaxVaryingFloats = %d\n", temp);
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &temp);
    printf("gl_MaxVertexTextureImageUnits = %d\n", temp);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &temp);
    printf("gl_MaxTextureImageUnits = %d\n", temp);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &temp);
    printf("gl_MaxFragmentUniformComponents = %d\n", temp);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &temp);
    printf("gl_MaxCombinedTextureImageUnits = %d\n", temp);
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &temp);
    printf("gl_MaxDrawBuffers = %d\n", temp);
    GLfloat fSizes[2];
    glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, fSizes);
    printf("Min and max aliased point sizes = %f, %f\n\n", fSizes[0], fSizes[1]);*/
    return true;
}

void NEGUIManagerPrivate::saveGUIStateIntoScene(QXmlStreamWriter* writer)
{
    //std::cout << "Write GUI State into scene" << std::endl;
    writer->writeStartElement("PropertyEditor");
    NEObjectController::saveUIStates(writer);
    writer->writeEndElement();

    S->mainWindow()->nodeGraphEditorWindow()->writeGUIState(writer);
    S->mainWindow()->sceneInspectorWindow()->writeGUIState(writer);
}

void NEGUIManagerPrivate::restoreGUIStateFromScene(QXmlStreamReader* reader)
{
    while(reader->readNextStartElement())
    {
        if(reader->name() == "PropertyEditor")
        {
            NEObjectController::loadUIStates(reader);
        }
        else if(reader->name() == "NodeGraphEditor")
        {
            S->mainWindow()->nodeGraphEditorWindow()->readGUIState(reader);
        }
        else if(reader->name() == "SceneInspector")
        {
            S->mainWindow()->sceneInspectorWindow()->readGUIState(reader);
        }
        else
        {
            reader->skipCurrentElement();
            Log() << "Unknown section " << reader->name() << " in the scene file.";
            EndLog(NE::kLogWarning);
        }
    }
}

void NEGUIManager::setOpenGLUpdate(bool val)
{
    P->m_OpenGLUpdate = val;
    if(P->m_OpenGLUpdate)
        updateGLWindows();
}

void NEGUIManagerPrivate::saveGUIState()
{
    QFile file(m_GUIStatePath);
    if(!file.open(QFile::WriteOnly | QFile::Truncate))
        return;
    QDataStream out(&file);
    out << S->mainWindow()->saveState();
    file.close();
    //qDebug() << "Program state saved to: "<< m_GUIStatePath;
}

void NEGUIManagerPrivate::loadGUIState()
{
    QFile file( m_GUIStatePath );

    if( !QFile::exists( m_GUIStatePath ) )
        file.setFileName( "neutrino.state" );

    if( !file.open(QFile::ReadOnly | QFile::Truncate) )
        return;

    QDataStream in(&file);
    QByteArray barr;
    in >> barr;
    S->mainWindow()->restoreState(barr);
    file.close();
}

void NEGUIManagerPrivate::saveMainWindowGeometry()
{
    QSettings settings("Neutrino Dynamics", "Neutrino");
    settings.setValue("geometry", S->mainWindow()->saveGeometry());
}

void NEGUIManagerPrivate::loadMainWindowGeometry()
{
    QSettings settings("Neutrino Dynamics", "Neutrino");
    S->mainWindow()->restoreGeometry(settings.value("geometry").toByteArray());
}

void NEGUIManager::selectCursorMode(bool val)
{
    if(!val)
        return;
    Log() << "Cursor mode";
    EndLog(NE::kLogInfo);
    NEGLWidget::m_ActiveManipulator = NEManipulator::kManipNone;
    updateGLWindows();
}

void NEGUIManager::selectTranslationMode(bool val)
{
    if(!val)
        return;
    EndLog(NE::kLogInfo);
    NEGLWidget::m_ActiveManipulator = NEManipulator::kManipTranslate;
    updateGLWindows();
}

void NEGUIManager::selectRotationMode(bool val)
{
    if(!val)
        return;
    EndLog(NE::kLogInfo);
    NEGLWidget::m_ActiveManipulator = NEManipulator::kManipRotate;
    updateGLWindows();
}

void NEGUIManager::selectScaleMode(bool val)
{
    if(!val)
        return;
    EndLog(NE::kLogInfo);
    NEGLWidget::m_ActiveManipulator = NEManipulator::kManipScale;
    updateGLWindows();
}

void NEGUIManager::setSelectedVisibile(bool val)
{
    if(!NESCENE.selectedNode())
        return;
    if(NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode()))
    {
        spatial->SetRenderMode(NESpatial::SmoothShaded);
        updateSceneInspector();
        updatePropertyEditor();
    }
}

void NEGUIManager::setSelectedWireframe(bool val)
{
    if(!NESCENE.selectedNode())
        return;
    if(NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode()))
    {
        spatial->SetRenderMode(NESpatial::Wireframe);
        updateSceneInspector();
        updatePropertyEditor();
    }
}

void NEGUIManager::playButtonToggled(bool val)
{
    P->timeline()->play(val);
}


void NEGUIManager::setSelectedFlatShaded(bool val)
{
    if(!NESCENE.selectedNode())
        return;
    if(NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode()))
    {
        spatial->SetRenderMode(NESpatial::FlatShaded);
        updateSceneInspector();
        updatePropertyEditor();
    }
}

void NEGUIManager::setSelectedSmoothShaded(bool val)
{
    if(!NESCENE.selectedNode())
        return;
    if(NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode()))
    {
        spatial->SetRenderMode(NESpatial::SmoothShaded);
        updateSceneInspector();
        updatePropertyEditor();
    }
}

void NEGUIManager::setSelectedGroup()
{
    QList<NENode*> tmp;
    tmp.append(NESCENE.selectedNode());
    foreach(NEObject* obj, *NESCENE.selectedObjects())
        if(NENode* node = qobject_cast<NENode*>(obj))
            tmp.append(node);
    NESCENE.groupObjects(tmp);
}

void NEGUIManager::setSelectedUngroup()
{
    if(NESpatialGroup* group = qobject_cast<NESpatialGroup*>(NESCENE.selectedNode())){
        NESCENE.ungroupNode(group);
        mainWindow()->sceneInspectorWindow()->removeSelectedItem();
    }
}

void NEGUIManager::setSelectedParent()
{
    QList<NENode*> tmp;
    foreach(NEObject* obj, *NESCENE.selectedObjects())
        if(NENode* node = qobject_cast<NENode*>(obj))
            tmp.append(node);
    foreach(NENode* node, tmp)
        if(NESpatialGroup* group = qobject_cast<NESpatialGroup*>(node)){
            tmp.removeAt(tmp.indexOf(node));
            NESCENE.parentObjects(tmp, group);
            return;
        }
}

void NEGUIManager::setSelectedUnparent()
{
    NENode* node = NESCENE.selectedNode();
    if(NESpatialGroup* group = qobject_cast<NESpatialGroup*>(node)){
        NESCENE.unparentObjects(node, group);
        return;
    }
}

void NEGUIManager::setSelectedHidden(bool val)
{
    if(!NESCENE.selectedNode())
        return;
    if(NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode()))
    {
        spatial->SetRenderMode(NESpatial::Invisible);
        updateSceneInspector();
        updatePropertyEditor();
    }
}

void NEGUIManager::setSelectedBoundingBox(bool val)
{
    if(!NESCENE.selectedNode())
        return;
    if(NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode()))
    {
        spatial->SetRenderMode(NESpatial::Invisible);
        updateSceneInspector();
        updatePropertyEditor();
    }
}

void NEGUIManager::setSelectedActive()
{
    if(!NESCENE.selectedNode())
        return;
    NESCENE.selectedNode()->SetBehavior(NENode::Active);
    updateSceneInspector();
    updatePropertyEditor();
}

void NEGUIManager::setSelectedCache()
{
    if(!NESCENE.selectedNode())
        return;
    NESCENE.selectedNode()->SetBehavior(NENode::Cache);
    updateSceneInspector();
    updatePropertyEditor();
}

void NEGUIManager::setSelectedInactive()
{
    if(!NESCENE.selectedNode())
        return;
    NESCENE.selectedNode()->SetBehavior(NENode::Inactive);
    updateSceneInspector();
    updatePropertyEditor();
}

void NEGUIManager::fullscreenToggle(bool val)
{
    mainWindow()->lockDocks(Qt::LeftDockWidgetArea);
    mainWindow()->lockDocks(Qt::RightDockWidgetArea);
    qApp->processEvents();
    if(val)
    {
        P->m_PreviousWindowState = mainWindow()->windowState();
        mainWindow()->showFullScreen();
    }
    else
    {
        mainWindow()->setWindowState(P->m_PreviousWindowState);
    }

    mainWindow()->unlockDocks(Qt::LeftDockWidgetArea);
    mainWindow()->unlockDocks(Qt::RightDockWidgetArea);
    qApp->processEvents();
}

void NEGUIManager::showPreferencesDialog()
{
    mainWindow()->preferencesDialog()->show();
}

void NEGUIManager::maximize()
{
    mainWindow()->lockDocks(Qt::LeftDockWidgetArea);
    mainWindow()->lockDocks(Qt::RightDockWidgetArea);
    qApp->processEvents();
    //    fullscreenToggle(true);
    mainWindow()->showMaximized();
    mainWindow()->unlockDocks(Qt::LeftDockWidgetArea);
    mainWindow()->unlockDocks(Qt::RightDockWidgetArea);
    qApp->processEvents();
}

void NEGUIManager::playToggled(bool val)
{
    P->resizeGLWidgetsForFrameSaving(val);
}

NEGLWidget* NEGUIManager::mainWidget()
{
    return P->m_pMainGLWidget;
}

void NEGUIManagerPrivate::resizeGLWidgetsForFrameSaving(bool val)
{
    if(val && S->SaveGLFrames())
    {
        foreach(const GLDockWidgetBundle* w, m_IDtoGLDockWidget)
        {
            if(w->glWidget->P()->saveGLFrames())
            {
                float W = w->glWidget->P()->imageSizeForSaving().width();
                float H = w->glWidget->P()->imageSizeForSaving().height();
                w->glWidget->setMaximumSize(W, H);
                w->glWidget->setMinimumSize(W, H);
                w->glWidget->adjustSize();
            }
        }
    }
    else
    {
        foreach(const GLDockWidgetBundle* w, m_IDtoGLDockWidget)
        {
            w->glWidget->setMaximumSize(16777215, 16777215);
            w->glWidget->setMinimumSize(0, 0);
            w->glWidget->adjustSize();
        }
    }
}

const NEGUIManagerUserProps::NEGUIItemSize& NEGUIManagerUserProps::LargeFontSize() const
{
    return P->m_LargeFontSize;
}

uint NEGUIManagerUserProps::SetLargeFontSize(const NEGUIManagerUserProps::NEGUIItemSize& LargeFontSize)
{
    P->m_LargeFontSize = LargeFontSize;
    return NE::kReturnSuccess;
}

const NEGUIManagerUserProps::NEGUIItemSize& NEGUIManagerUserProps::SmallFontSize() const
{
    return P->m_SmallFontSize;

}

uint NEGUIManagerUserProps::SetSmallFontSize(const NEGUIManagerUserProps::NEGUIItemSize& SmallFontSize)
{
    P->m_SmallFontSize = SmallFontSize;
    return NE::kReturnSuccess;
}

const NEGUIManagerUserProps::NEGUIItemSize& NEGUIManagerUserProps::NormalFontSize() const
{
    return P->m_NormalFontSize;
}

uint NEGUIManagerUserProps::SetNormalFontSize(const NEGUIManagerUserProps::NEGUIItemSize& NormalFontSize)
{
    P->m_NormalFontSize = NormalFontSize;
    return NE::kReturnSuccess;
}

uint NEGUIManagerUserProps::SetOSDFontSize(const NEGUIManagerUserProps::NEGUIItemSize& val)
{
    P->m_OSDFontSize = val;
    NERENDERMAN.GLManager()->setOSDFontSize((NEGLManager::NEGUIItemSize)val);
    NEOSD.setOffsety(10 + 2 * val);

    return NE::kReturnSuccess;
}


const NEGUIManagerUserProps::NEGUIItemSize& NEGUIManagerUserProps::OSDFontSize() const
{
    return P->m_OSDFontSize;
}



void NEGUIManagerPrivate::setWindowTitle(QString scene)
{
    S->mainWindow()->setWindowTitle(scene);
}

void NEGUIManagerPrivate::splitGLDockHorizontally(NEGLWidget* wid)
{
    GLDockWidgetBundle* bundle = createGLWidget();
    QDockWidget* parDock = m_GLtoGLDockWidget[wid]->dockWidget;
    QSize pdSize = QSize(parDock->size().width() / 2, parDock->size().height());
    S->mainWindow()->splitDockWidget(parDock, bundle->dockWidget, Qt::Horizontal);

    m_GLtoGLDockWidget[ bundle->glWidget]->mainWindow->addCameras(NERENDERMAN.cameras() );
    setCameraForGLDock( bundle, NERENDERMAN.topCamera() );

    // Resize the docks appropriately
    parDock->setMaximumSize(pdSize);
    parDock->setMinimumSize(pdSize);
    bundle->dockWidget->setMaximumSize(pdSize);
    bundle->dockWidget->setMinimumSize(pdSize);
    qApp->processEvents();
    parDock->setMaximumSize(QSize(16384, 16384));
    parDock->setMinimumSize(QSize(20, 20));
    bundle->dockWidget->setMaximumSize(QSize(16384, 16384));
    bundle->dockWidget->setMinimumSize(QSize(20, 20));
    S->updateSystemInspector();
}

void NEGUIManagerPrivate::splitGLDockVertically(NEGLWidget* wid)
{
    GLDockWidgetBundle* bundle = createGLWidget();
    QDockWidget* parDock = m_GLtoGLDockWidget[wid]->dockWidget;
    QSize pdSize = QSize(parDock->size().width(), parDock->size().height() / 2);
    S->mainWindow()->splitDockWidget(parDock, bundle->dockWidget, Qt::Vertical);
    m_GLtoGLDockWidget[bundle->glWidget]->mainWindow->addCameras(NERENDERMAN.cameras());

    setCameraForGLDock( bundle, NERENDERMAN.topCamera() );
    // Resize the docks appropriately
    parDock->setMaximumSize(pdSize);
    parDock->setMinimumSize(pdSize);
    bundle->dockWidget->setMaximumSize(pdSize);
    bundle->dockWidget->setMinimumSize(pdSize);
    qApp->processEvents();
    parDock->setMaximumSize(QSize(16384, 16384));
    parDock->setMinimumSize(QSize(20, 20));
    bundle->dockWidget->setMaximumSize(QSize(16384, 16384));
    bundle->dockWidget->setMinimumSize(QSize(20, 20));
    S->updateSystemInspector();
}

void NEGUIManagerPrivate::splitGLDockToQuad(NEGLWidget* wid)
{
    QSize mainsize = S->mainWindow()->size();
    // Create upper-right part by diving the given window into two
    GLDockWidgetBundle* upperRight = createGLWidget();
    QDockWidget* parDock = m_GLtoGLDockWidget[wid]->dockWidget;
    QSize pdSize = QSize(parDock->size().width() / 2, parDock->size().height() / 2);
    S->mainWindow()->splitDockWidget(m_GLtoGLDockWidget[wid]->dockWidget, upperRight->dockWidget, Qt::Vertical);
    m_GLtoGLDockWidget[upperRight->glWidget]->mainWindow->addCameras(NERENDERMAN.cameras());
    setCameraForGLDock(upperRight, NERENDERMAN.leftCamera());
    // Create lower-left part by diving the upper-left window into two horizontally
    GLDockWidgetBundle* bottomLeft = createGLWidget();
    S->mainWindow()->splitDockWidget(m_GLtoGLDockWidget[wid]->dockWidget, bottomLeft->dockWidget, Qt::Horizontal);
    m_GLtoGLDockWidget[bottomLeft->glWidget]->mainWindow->addCameras(NERENDERMAN.cameras());
    setCameraForGLDock(bottomLeft, NERENDERMAN.topCamera());
    // Create lower-left part by diving the upper-right window into two horizontally
    GLDockWidgetBundle* bottomRight = createGLWidget();
    S->mainWindow()->splitDockWidget(upperRight->dockWidget, bottomRight->dockWidget, Qt::Horizontal);
    m_GLtoGLDockWidget[bottomRight->glWidget]->mainWindow->addCameras(NERENDERMAN.cameras());
    setCameraForGLDock(bottomRight, NERENDERMAN.frontCamera());
    // This is to prevent Qt from making the mainwindow larger
    S->mainWindow()->setMaximumSize(mainsize);
    S->mainWindow()->setMinimumSize(mainsize);
    // Resize the docks appropriately
    parDock->setMaximumSize(pdSize);
    parDock->setMinimumSize(pdSize);
    upperRight->dockWidget->setMaximumSize(pdSize);
    upperRight->dockWidget->setMinimumSize(pdSize);
    bottomLeft->dockWidget->setMaximumSize(pdSize);
    bottomLeft->dockWidget->setMinimumSize(pdSize);
    bottomRight->dockWidget->setMaximumSize(pdSize);
    bottomRight->dockWidget->setMinimumSize(pdSize);
    qApp->processEvents();
    parDock->setMaximumSize(QSize(16384, 16384));
    parDock->setMinimumSize(QSize(20, 20));
    upperRight->dockWidget->setMaximumSize(QSize(16384, 16384));
    upperRight->dockWidget->setMinimumSize(QSize(20, 20));
    bottomLeft->dockWidget->setMaximumSize(QSize(16384, 16384));
    bottomLeft->dockWidget->setMinimumSize(QSize(20, 20));
    bottomRight->dockWidget->setMaximumSize(QSize(16384, 16384));
    bottomRight->dockWidget->setMinimumSize(QSize(20, 20));
    S->updateSystemInspector();
    S->mainWindow()->setMaximumSize(QSize(16384, 16384));
    S->mainWindow()->setMinimumSize(QSize(1024, 768));
}

NEGLWidget* NEGUIManagerPrivate::mainGLWidget()
{
    return m_pMainGLWidget;
}




GLDockWidgetBundle::GLDockWidgetBundle()
{
}

GLDockWidgetBundle::~GLDockWidgetBundle()
{
    delete dockWidget;
}

NEGUIManagerUserProps::NEGUIManagerUserProps(NEGUIManager* s) : S(s)
{
    P = new NEGUIManagerUserPropsPrivate(this);
    setObjectName("UIProperties");
}

NEGUIManagerUserProps::~NEGUIManagerUserProps()
{
    delete P;
}

const NEGUIManagerUserProps::NEGUIItemSize& NEGUIManagerUserProps::ToolbarSize() const
{
    return P->m_ToolbarSize;
}

uint NEGUIManagerUserProps::SetToolbarSize(const NEGUIManagerUserProps::NEGUIItemSize& ToolbarSize)
{
    P->m_ToolbarSize = ToolbarSize;
    return NE::kReturnSuccess;
}

const NEGUIManager::NETimelineUnits& NEGUIManager::TimelineUnits() const
{
    return P->m_TimelineUnits;
}

uint NEGUIManager::SetTimelineUnits(const NETimelineUnits& val)
{
    NE_PROP_SET_AND_NOTIFY(TimelineUnits, P, val);
    P->m_pTimeline->timelineWidget->changeUnits((NETimelineWidget::NETimelineUnits)(int)val);
    P->m_pTimeline->setValueWithTimelineUnits();
    return NE::kReturnSuccess;
}

