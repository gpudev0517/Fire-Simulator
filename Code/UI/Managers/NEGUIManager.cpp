#include "Base/NEGUICallInterface.h"
#include "Base/NEApplication.h"
#include "Spatial/NESpatialGroup.h"
#include "Base/NEBase.h"
#include "Utility/NEUserEvent.h"
#include "Dialogs/Process/NEProcessDialog.h"
#include "Windows/Main/NEMainWindow.h"
#include "Windows/SceneInspector/NESceneInspector.h"
#include "Windows/PropertyManager/NEPropertyEditor.h"
#include "Windows/Message/NEMessageWindow.h"
#include "NEGLWidget.h"
#include "Core/NENode.h"

#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Spatial/NECamera.h"

#include "Windows/PropertyManager/NEObjectController.h"
#include "Base/NESceneManager.h"
#include "Dialogs/Help/NEHelpEngine.h"
#include "Windows/GLMain/NEGLMainWindow.h"
#include "Utilities/NEGUIStyle.h"
#include "Widgets/MenuBar/NEMenuBar.h"
#include "Widgets/Dock/NEDockWidget.h"
#include "Windows/PropertyManager/NEObjectController.h"
#include "Windows/SceneInspector/NESceneInspectorWindow.h"
#include "Windows/FZoneInspector/FZoneInspectorWindow.h"

#include "Rendering/NEGLRenderer.h"

#include "Widgets/Timeline/NETimeline.h"
#include "Widgets/Timeline/NETimelineWidget.h"
#include "Windows/PropertyManager/NEPropertyEditorWindow.h"
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
#include "Windows/SceneBuilder/NESceneBuilderWindow.h"
#include "Windows/FZoneInspector/FZoneInspector.h"
#include "Windows/Message/NEMessageWindow.h"
#include "Utility/NEOsdMsg.h"
//#include "Rendering/Utility/NEOsd.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Widgets/Menu/NEMenu.h"

#include "Utilities/NELogBrowser.h"
#include "NEGUIManager_p.h"

#include "Dialogs/File/NEFileDialog.h"

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

    QObject::connect( &NEBASE, SIGNAL(initUI()), this, SLOT(init()), Qt::DirectConnection );

    // Initialize the properties
    CALL_ONCE(initProperties);


    P->m_TimelineUnits = NE_DEFAULT(TimelineUnits);
    P->m_SaveGLFrames = NE_DEFAULT(SaveGLFrames);

}

NEGUIManager::~NEGUIManager()
{
    // Delete the private members
    delete P;
}


void NEGUIManager::init()
{
    P->init();

    NELOGBROWSER = new NELogBrowser( this );
    qInstallMessageHandler(NEMessageOutput);

    //QObject::connect( &NEBASE, SIGNAL(initUI()), this, SLOT(init()), Qt::DirectConnection );

    // Make basic GUI  connections
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


    // Initialize the property editor
    S->mainWindow()->propertyEditorWindow()->init();
    m_PropEdWins.append(S->mainWindow()->propertyEditorWindow());



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



    //updatePropertyEditorsAtPostInit();

    // Sets the default camera of the scene as the default OpenGL widget's camera
    if( m_pMainGLWidget && !m_pMainGLWidget->m_pCamera)
        setCameraForGLDock( m_GLtoGLDockWidget[m_pMainGLWidget], NERENDERMAN.defaultCamera() );

    // This is somehow required for spacebar to work correctly to maximize gl docks
    // The main windows also needs to have strong focus policy for this to work
    if( m_pMainGLWidget )
        m_pMainGLWidget->setFocus( Qt::OtherFocusReason );

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
    if( mainWindow() ){
        mainWindow()->sceneInspectorWindow()->update();
        if(!mainWindow()->fZoneInspectorWindow()->isHidden())
            mainWindow()->fZoneInspectorWindow()->update();
    }
}

void NEGUIManager::updateSystemInspector()
{
    if( mainWindow() )
        mainWindow()->systemInspectorWindow()->update();
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
                      this, SLOT(pushUserEvent(NEUserEvent*)), Qt::DirectConnection );


    QObject::connect( object, SIGNAL(updateNodeRename(NENode *)),
                      this, SLOT(updateNodeRenameToAllGUI(NENode *)), Qt::DirectConnection );

//     QObject::connect( object, SIGNAL(registerAction(QString &, QObject *)),
//                       this, SLOT(registerActionOnGUI(QString &, QObject *)), Qt::DirectConnection );
 	QObject::connect(object, &NENode::registerAction, this, &NEGUIManager::registerActionOnGUI, Qt::DirectConnection);
    //QObject::connect(object, &NENode::unregisterAction, this, &NEGUIManager::unregisterActionOnGUI, Qt::DirectConnection);

    connect( object, SIGNAL(activeZone(QObject*&, QObject*&)), this, SLOT(activeZone(QObject*&, QObject*&)));

}

void NEGUIManager::activeZone(QObject*& fengine, QObject *& zone)
{
    if(zone)
        mainWindow()->fZoneInspectorWindow()->setRoot(zone->parent());
    else if( fengine)
        mainWindow()->fZoneInspectorWindow()->setRoot(fengine);
    else
        mainWindow()->fZoneInspectorWindow()->setRoot(0);
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
    QAction* objectAction = new QAction(actionName, NULL);


    action = objectAction;
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
            a->disconnect();
            la.removeAll(a);
            delete a;
        }
}

QDebug& NEGUIManager::log()
{
    return mainWindow()->messageWindow()->getNewDebugStream();
}

void NEGUIManager::endLog(NE::NELogType type)
{
    /*
    if( NEBASE.guiMode() )
        if(mainWindow() && mainWindow()->isVisible())
            mainWindow()->messageWindow()->pushLogInformation(type);
            */
}

void NEGUIManager::EvalPyGUI(const QString& script)
{
    //consoleMessage(">> " + code);

    getPyMain().evalScript(script);

    if(UpdateUI())
    {
        //outputWindow->appendHtml("<font color=#003A00>" + code + "</font>");
        //flushStdOut();
        NEGUI.updateSceneInspector();
        NEGUI.updateSystemInspector();
        NEGUI.updatePropertyEditor();
        NEGUI.updateNodeGraphEditor();
        NEGUI.updateGLWindows();
    }

    NESCENE.cleanup();
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

}

void NEGUIManager::updateInitFlagForNodeGraph(bool aInitFlag)
{
;
}

void NEGUIManager::updateNodeRenameForNodeGraph(const QString &oldName, const QString &newName)
{

}

void NEGUIManager::deInit()
{
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
    if( mainWindow() )
        delete mainWindow();
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



void NEGUIManager::updateGLWindows()
{
    foreach(const GLDockWidgetBundle* w, P->m_IDtoGLDockWidget)
        w->glWidget->update();
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

QStringList NEGUIManager::fileImportBrowser(FilePath &filePath, QString orgName, QString appName, QString settingsName)
{
    QStringList fileNames;

    QSettings settings( orgName, appName );

    NEFileDialog dlg(mainWindow(), settings.value(settingsName).toByteArray(),
                     tr(""), filePath.filter(), NEFileDialog::IMPORT,
                     QFileDialog::ExistingFiles);
    if( dlg.exec() == QDialog::Accepted )
        fileNames = dlg.selectedFiles();


    if( fileNames.size() > 0 )
        settings.setValue( settingsName, QFileInfo(fileNames[0]).path());


    return fileNames;
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
}

void NEGUIManager::showMainGLWidget()
{
    return P->showMainGLWidget();
}

NEMainWindow* NEGUIManager::mainWindow()
{
    return P->m_pMainWindow;
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

    QObject::connect( &NEBASE, SIGNAL(showMainWindow()), S, SLOT(showMainWindow()), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(hideSceneLoadingSplash()), S, SLOT(hideSceneLoadingSplash()), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(showSceneLoadingWidget()), S, SLOT(showSceneLoadingWidget()), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(hideSceneLoadingWidget()), S, SLOT(hideSceneLoadingWidget()), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(showSceneManagementDialog()), S, SLOT(showSceneManagementDialog()), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(postInit()), S, SLOT(postInit()), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(layout()), S, SLOT(layout()), Qt::DirectConnection );
    //    QObject::connect( &NEBASE, SIGNAL(updateInitFlagForNodeGraph(bool)), S, SLOT(updateInitFlag(bool)), Qt::ConnectionType::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(deInitUI()), S, SLOT(deInit()), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(setCurrentOperation(QString)), S, SLOT(setCurrentOperation(QString)), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(setCurrentSubOperation(QString)), S, SLOT(setCurrentSubOperation(QString)), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(selectObject(QObject *)), S, SLOT(selectObject(QObject *)), Qt::DirectConnection );
    //QObject::connect( &NESCENE, SIGNAL(selectObject(QObject *)), S, SLOT(selectObject(QObject *)), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(updateSceneInspector()), S, SLOT(updateSceneInspector()), Qt::DirectConnection );
    QObject::connect( &NESCENE, SIGNAL(updateSceneInspector()), S, SLOT(updateSceneInspector()), Qt::DirectConnection );


    QObject::connect( &NEBASE, SIGNAL(updateSystemInspector()), S, SLOT(updateSystemInspector()), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(updatePropertyEditor()), S, SLOT(updatePropertyEditor()), Qt::DirectConnection );
    QObject::connect( &NESCENE, SIGNAL(updatePropertyEditor()), S, SLOT(updatePropertyEditor()), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(updatePropertyEditorsAtPostInit()), S, SLOT(updatePropertyEditorsAtPostInit()), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(updateAllConnectionTables()), S, SLOT(updateAllConnectionTables()), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(updateExpressionInspector()), S, SLOT(updateExpressionInspector()), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(updateGLWindows()), S, SLOT(updateGLWindows()), Qt::DirectConnection );
    QObject::connect( &NESCENE, SIGNAL(updateGLWindows()), S, SLOT(updateGLWindows()), Qt::DirectConnection );


    QObject::connect( &NEBASE, SIGNAL(updateWindowVisibilityFlags()), S, SLOT(updateWindowVisibilityFlags()), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(deleteMainWindow()), S, SLOT(deleteMainWindow()), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(setWindowTitle(QString)), S, SLOT(setWindowTitle(QString)), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(loadGUIState()), S, SLOT(loadGUIState()), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(saveGUIState()), S, SLOT(saveGUIState()), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(writeAllGUICreations( QXmlStreamWriter *) ),
                      S, SLOT(writeAllGUICreations( QXmlStreamWriter *) ), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(writeAllGUIState( QXmlStreamWriter *)),
                      S, SLOT(writeAllGUIState( QXmlStreamWriter *)), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(restoreGUIState( QXmlStreamWriter *)),
                      S, SLOT(restoreGUIState( QXmlStreamWriter *)), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(setParentUI(QObject *)),
                      S, SLOT(setParent(QObject *)), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(closeAllPropertyEditors()),
                      S, SLOT(closeAllPropertyEditors()), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(closeAllGLDocks()),
                      S, SLOT(closeAllGLDocks()), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(updateNodeRename(NENode*)),
                      S, SLOT(updateNodeRenameToAllGUI(NENode*)), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(createGLWindow(QString)),
                      S, SLOT(createGLWindow(QString)), Qt::DirectConnection );
    QObject::connect( &NEBASE, SIGNAL(createGLWindowOnLoad(QString)),
                      S, SLOT(createGLWindowOnLoad(QString)), Qt::DirectConnection );

    QObject::connect( &NEBASE, SIGNAL(createPropertyEditor(QString)),
                      S, SLOT(createPropertyEditor(QString)), Qt::DirectConnection );

    QObject::connect( &NESCENE, SIGNAL(updateWidgetsForCreation()),
                      S, SLOT(updateWidgetsForCreation()), Qt::DirectConnection );
    QObject::connect( &NESCENE, SIGNAL(updateWidgetsForDeletion(QObject *)),
                      S, SLOT(updateWidgetsForDeletion(QObject *)), Qt::DirectConnection );
    QObject::connect( &NESCENE, SIGNAL(pushUserEvent(NEUserEvent*)),
                      S, SLOT(pushUserEvent(NEUserEvent*)), Qt::DirectConnection );


    QObject::connect( &NESCENE, SIGNAL(resetWidgets()), S, SLOT( resetWidgets()), Qt::DirectConnection );

    QObject::connect( &NESCENE, SIGNAL(CurrentFrameChanged(int)), S, SLOT( setCurrentFrame(int)), Qt::DirectConnection );

    QObject::connect( &NESCENE, SIGNAL(repaintTimeline()), S, SLOT( repaintTimeline()), Qt::DirectConnection );
    QObject::connect( &NESCENE, SIGNAL(setPlay(bool)), S, SLOT( setPlay(bool)), Qt::DirectConnection );

    QObject::connect( &NESCENE, SIGNAL(selectObjectUI(QObject *)), S, SLOT( selectObject(QObject *)), Qt::DirectConnection );

    QObject::connect( &NESCENE, SIGNAL(setTimelineKeyframeMarks(QList<uint>)),
                      S, SLOT(setTimelineKeyframeMarks(QList<uint>)),
                      Qt::DirectConnection );

	QObject::connect( &NESCENE, SIGNAL(SimulationEndFrameChanged( int) ),
                      S, SLOT(setSimulationEndFrame( int )),
                      Qt::DirectConnection );
    QObject::connect( &NESCENE, SIGNAL(PlaybackStartFrameChanged( int) ),
                      S, SLOT(setPlaybackStartFrame( int )),
                      Qt::DirectConnection );
    QObject::connect( &NESCENE, SIGNAL(PlaybackEndFrameChanged( int) ),
                      S, SLOT(setPlaybackEndFrame( int )),
                      Qt::DirectConnection );

	QObject::connect(&NESCENE, SIGNAL(SimulationEndTimeChanged(double)),
					  S, SLOT(setSimulationEndTime(double)),
					  Qt::DirectConnection);
	QObject::connect(&NESCENE, SIGNAL(PlaybackStartTimeChanged(double)),
					  S, SLOT(setPlaybackStartTime(double)),
					  Qt::DirectConnection);
	QObject::connect(&NESCENE, SIGNAL(PlaybackEndTimeChanged(double)),
					  S, SLOT(setPlaybackEndTime(double)),
					  Qt::DirectConnection);

	QObject::connect(S, SIGNAL(TimelineUnitsChanged(NETimelineUnits)),
						&NESCENE, SLOT(setTimeLineUnits(NETimelineUnits)),
						Qt::DirectConnection);

    QObject::connect( &NESCENE, SIGNAL(setSimulate( bool) ), S, SLOT(setSimulate( bool )),
                      Qt::DirectConnection );
    QObject::connect( &NESCENE, SIGNAL(setRepeat( int) ), S, SLOT(setRepeat( int )),
                      Qt::DirectConnection );

    QObject::connect( &NESCENE, SIGNAL(triggerSavingData( ) ), S, SLOT(triggerSavingData(  )),
                      Qt::DirectConnection );

    QObject::connect( &NESCENE, SIGNAL(updateCharts( ) ), S, SLOT(updateCharts(  )),
                      Qt::DirectConnection );


    QObject::connect( &NESCENE, SIGNAL(managersCreated(QList<std::shared_ptr<NEManager> >) ),
                      S, SLOT(managersCreated(QList<std::shared_ptr<NEManager> >)),
                      Qt::DirectConnection );

    QObject::connect( &NERENDERMAN, SIGNAL(managerCreated(NEManager*) ),
                      S, SLOT(managerCreated(NEManager*)),
                      Qt::DirectConnection );

    QObject::connect( &NERENDERMAN, SIGNAL(updateGLWindows() ),
                      S, SLOT(updateGLWindows()),
                      Qt::DirectConnection );


    foreach(const GLDockWidgetBundle* w, m_IDtoGLDockWidget)
    {
        QObject::connect( w->glWidget, SIGNAL(selectObject(QObject *)), S, SLOT(selectObject(QObject *)) );
        QObject::connect( w->glWidget, SIGNAL(selectObjects(QList<NENode*> &)), S, SLOT(selectObjects(QList<NENode*> &))  );

        QObject::connect( w->glWidget, SIGNAL(updatePropertyEditorUI()), S, SLOT(updatePropertyEditor()) ) ;
        QObject::connect( w->glWidget, SIGNAL(updateGLWindows()), S, SLOT(updateGLWindows()) );
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
    mainWindow()->sceneInspectorWindow()->setNewCurrentItem(object);
    mainWindow()->propertyEditorWindow()->showObjectProperty(object);
    hideObjectFromOtherPropEditors(object, mainWindow()->propertyEditorWindow());

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

    return;
}

void NEGUIManager::closeAllGLDocks()
{
    P->closeAllGLDocks();
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
    NEPopAttrib(NE_UNDO_REDO_STATE);
}


void NEGUIManager::showNodePropertyExpression(const NENode* val, const QString P)
{
    // Remove the previous object
    getPyMain().removeVariable("this");
    // Add the object
    getPyMain().addObject("this", static_cast<QObject*>(const_cast<NENode*>(val)));
}

bool NEGUIManager::OpenGLUpdate() const
{
    return P->m_OpenGLUpdate;
}

void NEGUIManager::showHelp()
{
    NEHELP.show();
}



void NEGUIManagerPrivate::updateWidgetsForDeletion(QObject* object)
{
    S->mainWindow()->propertyEditorWindow()->removeObject(object);
}

void NEGUIManagerPrivate::updateWidgetsForCreation()
{

}

bool NEGUIManagerPrivate::printGLInfo()
{
    int temp;
    printf("FDYN: Info - OpenGL version: %s by %s\n", GL->glGetString(GL_VERSION), GL->glGetString(GL_VENDOR));
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
    QSettings settings("F Dynamics", "FDYN");
    settings.setValue("geometry", S->mainWindow()->saveGeometry());
}

void NEGUIManagerPrivate::loadMainWindowGeometry()
{
    QSettings settings("F Dynamics", "FDYN");
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
    //NEOSD.setOffsety(10 + 2 * val);

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

