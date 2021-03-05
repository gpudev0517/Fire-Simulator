#include <QStandardPaths>
#include <QList>
#include <QMessageBox>
#include <QQuickStyle>

#include "Base/NEGUICallInterface.h"
#include "Base/NEApplication.h"
#include "Spatial/NESpatialGroup.h"
#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include "Utility/NEOsdMsg.h"
#include "Utility/NEUserEvent.h"
#include "Core/Manager/NEGeometryManager.h"
#include "Geometry/Mesh/NEZone.h"
#include "Geometry/Mesh/NEZoneCompartment.h"
#include "Geometry/Mesh/NEZoneItem.h"
#include "Geometry/Resource/Mesh/NEMeshContainer.h"
#include "Geometry/Utility/NECustomVariants.h"
#include "Geometry/Utility/NESurfaceMaterial.h"
#include "Geometry/Utility/NEFireMaterial.h"
#include "Geometry/Utility/NEFailureCriteria.h"
#include "Geometry/Utility/NEVentCriteria.h"
#include "Geometry/Utility/NECableProperty.h"
#include "Mesh/NESpatialEngine.h"
#include "Mesh/NEScenario.h"
#include "NENode.h"
#include "Geometry/FEngine/FEngine.h"

#include "Rendering/NEGLRenderer.h"
//#include "Rendering/Utility/NEOsd.h"
#include "Rendering/Spatial/NECamera.h"
#include "Rendering/Spatial/Light/NEDirectionalLight.h"
#include "Rendering/Spatial/Light/NESpotLight.h"
#include "Rendering/Spatial/Light/NEPointLight.h"
#include "Rendering/Manager/NERenderManager.h"


#include "Managers/FDYNGUIManager.h"
#include "Windows/Main/FDYNMainWindow.h"
#include "Windows/SceneInspector/FDYNSceneInspector.h"

#include "NEGLWidget.h"

#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Spatial/NECamera.h"

#include "Windows/PropertyManager/FDYNObjectController.h"

#include "Windows/SceneInspector/FDYNSceneInspectorWindow.h"
#include "Windows/ZoneInspector/FDYNZoneInspectorWindow.h"
#include "Windows/ScenarioInspector/FDYNScenarioInspectorWindow.h"
#include "Windows/FRANXInspector/FDYNFRANXInspectorWindow.h"


#include "Widgets/Dock/FDYNDockWidget.h"
#include "Widgets/Timeline/FDYNTimeline.h"
#include "Windows/PropertyManager/FDYNPropertyEditorWindow.h"

#include "Windows/ZoneInspector/FDYNZoneInspector.h"
#include "Windows/ScenarioInspector/FDYNScenarioInspector.h"
#include "Windows/FRANXInspector/FDYNFRANXInspector.h"
#include "Windows/Message/FDYNMessageWindow.h"
#include "Windows/GLMain/FDYNGLMainWindow.h"

#include "Dialogs/Process/FDYNProcessDialog.h"
#include "Widgets/CADModeling/FDYNCADModelingTool.h"

#include "Physics/globals_Physics.h"

#include "Dialogs/SimulateScenario/FDYNSimulateScenarioDialog.h"

#include "Dialogs/Project/FDYNCreateProjectDialog.h"
#include "Dialogs/Settings/FDYNSettingsDialog.h"
#include "Dialogs/FRANXCalc/FDYNFRANXCalcDialog.h"


#include "Dialogs/File/FDYNFileDialog.h"

#include "Windows/SystemInspector/FDYNSystemInspectorWindow.h"

#include "FDYNGUIManager.h"
#include "FDYNGUIManager.h"
#include "FDYNGUIManager_p.h"

#include <chrono>

NE_PROP_DEF(bool, FDYNGUIManagerUserProps, DarkTheme, true)
NE_PROP_DEF(FDYNGUIManager::FDYNTimelineUnits, FDYNGUIManager, TimelineUnits, FDYNGUIManager::Seconds)
NE_PROP_DEF(FDYNGUIManagerUserProps::NEGUIItemSize, FDYNGUIManagerUserProps, NormalFontSize, FDYNGUIManagerUserProps::Normal)
NE_PROP_DEF(FDYNGUIManagerUserProps::NEGUIItemSize, FDYNGUIManagerUserProps, SmallFontSize, FDYNGUIManagerUserProps::Small)
NE_PROP_DEF(FDYNGUIManagerUserProps::NEGUIItemSize, FDYNGUIManagerUserProps, LargeFontSize, FDYNGUIManagerUserProps::Large)
NE_PROP_DEF(FDYNGUIManagerUserProps::NEGUIItemSize, FDYNGUIManagerUserProps, OSDFontSize, FDYNGUIManagerUserProps::Small)
NE_PROP_DEF(FDYNGUIManagerUserProps::NEGUIItemSize, FDYNGUIManagerUserProps, ToolbarSize, FDYNGUIManagerUserProps::Normal)

#if 0
QPointer<NELogBrowser> NELOGBROWSER;
#endif

void NEMessageOutput( QtMsgType type, const QMessageLogContext& mcontext, const QString& msg )
{
#if 0
    if(NELOGBROWSER)
        NELOGBROWSER->outputMessage(type, msg);
#endif
}

class FDYNGUIManagerUserPropsPrivate
{
public:
    FDYNGUIManagerUserPropsPrivate(FDYNGUIManagerUserProps* s) : S(s)
    {
        m_DarkTheme = true;
    }

    FDYNGUIManagerUserProps::NEGUIItemSize m_NormalFontSize = NE_DEFAULT(NormalFontSize);
    FDYNGUIManagerUserProps::NEGUIItemSize m_SmallFontSize = NE_DEFAULT(SmallFontSize);
    FDYNGUIManagerUserProps::NEGUIItemSize m_LargeFontSize = NE_DEFAULT(LargeFontSize);
    FDYNGUIManagerUserProps::NEGUIItemSize m_OSDFontSize = NE_DEFAULT(OSDFontSize);
    FDYNGUIManagerUserProps::NEGUIItemSize m_ToolbarSize = NE_DEFAULT(ToolbarSize);

    bool m_DarkTheme;
private:
    FDYNGUIManagerUserProps* S;

};

static void initProperties()
{
    NE_INIT_PROP(FDYNGUIManagerUserProps, DarkTheme);
    NE_INIT_PROP(FDYNGUIManager, TimelineUnits);
    NE_INIT_PROP(FDYNGUIManagerUserProps, NormalFontSize);
    NE_INIT_PROP(FDYNGUIManagerUserProps, SmallFontSize);
    NE_INIT_PROP(FDYNGUIManagerUserProps, LargeFontSize);
    NE_INIT_PROP(FDYNGUIManagerUserProps, OSDFontSize);
}

FDYNGUIManager& FDYNGUIManager::Instance()
{
    static FDYNGUIManager instance;
    return instance;
}

FDYNGUIManager::FDYNGUIManager()
{
    // Initialize the private members
    P = new FDYNGUIManagerPrivate( this );

    // Initialize per-user properties
    setPropertyObject(new FDYNGUIManagerUserProps(this));

    QObject::connect( &NEBASE, SIGNAL(initUI()), this, SLOT(init()) );

    // Initialize the properties
    CALL_ONCE(initProperties);

    P->m_TimelineUnits = NE_DEFAULT(TimelineUnits);
}

FDYNGUIManager::~FDYNGUIManager()
{
    // Delete the private members
    delete P;
}

void FDYNGUIManager::init()
{
    P->init();

#if 0
    NELOGBROWSER = new NELogBrowser( this );
    qInstallMessageHandler(NEMessageOutput);
#endif

    // Make basic GUI  connections
    if( NEBASE.guiMode() )
        P->makeConnections();

#if 1
    // Hide everything in the main window first
    if( NEBASE.guiMode() )
        mainWindow()->hideEverythingForStartup();
#endif

    getPyMain().addObject( objectName(), this );
    QApplication::processEvents();

    mainWindow()->setCADModelingMode();
}

void FDYNGUIManagerPrivate::init()
{
    if( !NEBASE.guiMode() )
        return;


    getFDYNVarIds().surfaceMaterialId = qRegisterMetaType<NESurfaceMaterial>("NESurfaceMaterial");
    getFDYNVarIds().fireMaterialId = qRegisterMetaType<NEFireMaterial>("NEFireMaterial");
    getFDYNVarIds().failureCriteriaId = qRegisterMetaType<NEFailureCriteria>("NEFailureCriteria");
    getFDYNVarIds().ventCriteriaId = qRegisterMetaType<NEVentCriteria>("NEVentCriteria");
    getFDYNVarIds().dynamicComboId = qRegisterMetaType<NEDynamicCombo>("NEDynamicCombo");
    getFDYNVarIds().cableMaterialId = qRegisterMetaType<NECableProperty>("NECableProperty");

    log = new QDebug( (QString *) &logStr );

    // Set the objectname
    S->setObjectName("GUI");

#if 0
    // Initialize the GUI styles
    FDYNGUISTYLE.init();
#endif

    // Initialize member variables
    m_OpenGLUpdate = true;
    m_pMainGLWidget = 0;
    m_spatialEngine = nullptr;

    // Create the main application window
    m_pMainWindow = new FDYNMainWindow;

    // Initialize the property editor
    S->mainWindow()->propertyEditorWindow()->init();
    m_PropEdWins.append(S->mainWindow()->propertyEditorWindow());

#if 1
    // Keep a private pointer to the timeline
    m_pTimeline = m_pMainWindow->Timeline();
#endif

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
    S->mainWindow()->splitDockWidget( S->mainWindow()->zoneInspectorDock(),
                                      m_IDtoGLDockWidget.last()->dockWidget,  Qt::Horizontal);
#if 0
    // Initialize the help
    NEHELP.init("neutrino.qch");
#endif

    showMainWindow();

    // We have an OpenGL context now, check the OpenGL version
    printGLInfo();

    // Initializes some utility geometry for the OpenGL widgets
    NEGLWidget::initStaticData();
}


void FDYNGUIManagerPrivate::postInit()
{
    //
    // Create a default FEngine
    //
    NESpatialEngine* en = qobject_cast< NESpatialEngine *>( NESCENE.createObject("Fire Engine", "COMPARTMENTS", 0) );
    if( en )
    {
        QObject::connect( en, SIGNAL(dataChanged(QString)), S, SLOT(setWindowTitle(QString)) );
        en->setStatic( true );
    }

    //
    // Check for defaults for the database and for the Scene file also
    //
    QStringList spaths = QStandardPaths::standardLocations( QStandardPaths::AppLocalDataLocation );
    QString defaultFolder = spaths[0];


    if( m_tempFolder != "" )
        defaultFolder = m_tempFolder;

    QString defaultDBName = "DefaultDB";
    QString defaultDBFile = defaultFolder + "/" + defaultDBName + ".sqlite";


    QFile dbfile( defaultDBFile );
    if( dbfile.exists() )
    {
        dbfile.remove();
    }
    if( en )
        en->newData( defaultDBFile );
    if( en )
        en->load( defaultDBFile );

    QObject::connect( &NEBASE, SIGNAL(quitApp(int)), S, SLOT(quitProject(int)) );


    //
    // Create a CFAST Node
    //

    NENode* cfnode = qobject_cast< NENode *>( NESCENE.createObject("CFAST", "CFAST", 0) );
    if( cfnode )
    {
        cfnode->setStatic(  true );
        m_cfastEngine = cfnode;
    }

    m_spatialEngine = en;
    S->updateInspectors();

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
        S->mainWindow()->updateWindowVisibilityFlags();
    }

    S->SetTimelineUnits(FDYNGUIManager::Seconds);
    m_pTimeline->changeUnits((int)m_TimelineUnits);
    m_pTimeline->setValueWithTimelineUnits();
    emit S->TimelineUnitsChanged(m_TimelineUnits);


}

void FDYNGUIManagerPrivate::deInit()
{
    if( !NEBASE.guiMode() )
        return;

    if( log )
        delete log;

    // Save the GUI state first
    saveMainWindowGeometry();


    // Clear the static data shared by all OpenGL widgets
    NEGLWidget::clearStaticData();
#if 0
    // Save the contents of the messages window
    QDir directory(NEBASE.sceneDataPath());
    directory.mkpath("Log/Messages");
    S->mainWindow()->messageWindow()->saveToFile(NEBASE.sceneDataPath() + "/Log/Messages/");
#endif
    foreach(GLDockWidgetBundle* b, m_IDtoGLDockWidget)
        delete b;
}

FDYNGUIManagerPrivate::FDYNGUIManagerPrivate(FDYNGUIManager* s): S(s)
{
    m_pMainWindow = 0;
    m_tempFolder = "";
}

void FDYNGUIManagerPrivate::showMainWindow()
{
    S->mainWindow()->show();
    loadMainWindowGeometry();
}

const bool& FDYNGUIManagerUserProps::DarkTheme() const
{
#if 0
    return NEGUISTYLE.DarkTheme();
#else
    return P->m_DarkTheme;
#endif
}

uint FDYNGUIManagerUserProps::SetDarkTheme(const bool& val)
{
    return NE::kReturnSuccess;
}

void FDYNGUIManager::pushUserEvent(NEUserEvent* event)
{
    P->m_UndoStack.push(event);
    P->m_RedoStack.clear();
}

///
/// \brief FDYNGUIManager::newDB
///
///
///

void FDYNGUIManager::newProject()
{
    FDYNCreateProjectDialog dlg(mainWindow());
    QString newProjectFileName;
    if( dlg.exec() == QDialog::Accepted)
    {

        QFileInfo file(dlg.selectedProjectFile());
        if( file.dir().exists())
        {
            newProjectFileName = dlg.selectedProjectFile();
            emit sendNewDB( newProjectFileName );
        }
        QString fileName1 = dlg.selectedFRANXFile();
        QString fileName2 = dlg.selectedPDMSFile();

        // To do loading PDMS and FRANX file
        emit sendImportDB( newProjectFileName, fileName1, fileName2 );
    }
}

void FDYNGUIManager::newDB()
{

    QStringList fileNames;

    QSettings settings( "Fire Dynamics", "FRI3D" );

    FDYNFileDialog dlg(mainWindow(), settings.value("LastLoadDB").toByteArray(),
                     tr("New Database Location"), "*.sqlite", FDYNFileDialog::SAVE,
                     QFileDialog::AnyFile);
    if( dlg.exec() == QDialog::Accepted )
        fileNames = dlg.selectedFiles();

    if( fileNames.size() > 0 )
    {
        QString fileName = fileNames[0];

        emit sendNewDB( fileName );
    }
}

///
/// \brief FDYNGUIManager::loadDB
///

QString FDYNGUIManager::loadDB(QString fileName)
{


    emit sendLoadDB( fileName );

    return fileName;
}


void FDYNGUIManager::openRecoverWarning( QString msg, bool recoverypresent )
{
    if( recoverypresent == true )
    {
        QMessageBox msgWarning ;
        msgWarning.setText( msg );
        msgWarning.setIcon( QMessageBox::Warning );
        msgWarning.setWindowTitle(tr("FRI3D"));
        msgWarning.addButton( QMessageBox::Yes );
        msgWarning.addButton( QMessageBox::No );
        msgWarning.setStyle( QStyleFactory::create ("Fusion" ) );

        int ret = msgWarning.exec();
//        int ret = QMessageBox::warning(mainWindow(), tr("FRI3D"), msg,
//                                   QMessageBox::Yes | QMessageBox::No );

        if( ret == QMessageBox::Yes )
        {
            emit sendSetRecoverData( true );
        }
        else
        {
            emit sendSetRecoverData( false );
        }
    }
    else
    {
        emit sendSetRecoverData( false );
    }
}

///
/// \brief FDYNGUIManager::saveDB
///

void FDYNGUIManager::saveDB()
{
    emit sendSaveDB(QString(""));
}


QString FDYNGUIManager::saveAsDB()
{
    QSettings settings( "Fire Dynamics", "FRI3D" );
    FDYNFileDialog dlg(mainWindow(), settings.value("LastDir/SaveDB").toByteArray(),
                     tr("Save As Database"), "*.sqlite", FDYNFileDialog::SAVE,
                     QFileDialog::AnyFile);

    QStringList fileNames;
    if( dlg.exec() == QDialog::Accepted )
    {
        fileNames = dlg.selectedFiles();
        settings.setValue("LastDir/SaveDB", dlg.directory());
    }

    if( fileNames.size() > 0 )
    {
        QString fileName = fileNames[0];

        emit sendSaveDB( fileName );
        setWindowTitle( fileName );
    }

    return fileNames[0];
}


void FDYNGUIManager::loadUI(QString filename)
{
    QFile sceneFilefri3d( filename );

}

void FDYNGUIManager::saveUI(QString filename)
{
    QFile sceneFilefri3d( filename );

    //NEBASE.save( sceneFilefri3d.fileName(), scenefolder, true );
    QObjectList objChildren; // Get a reference to the list of all children of the scene
    objChildren.append( this );
    QJsonArray guiitems;
    for(const QObject* child : objChildren)
    {
        QJsonObject   jsonObject ;

        if(NENode* neChild = qobject_cast<NENode*>(child))
        {
            if( neChild->factory().nodeInherits() != "Groups" ||
                neChild->factory().nodeInherits() != "Forces "
               )
            {
                //QJsonArray propList = neChild->getJSONFromProperties( );
                QJsonArray propList = getJSONFromProperties( qobject_cast< QObject *>( this ) );
                QString json = QString( QJsonDocument(propList).toJson() );

                jsonObject["name"] = neChild->objectName();
                jsonObject["properties" ] = json;
                jsonObject["factoryNodeName"] = neChild->factory().nodeName();
                jsonObject["factoryNodeType"] = (int) neChild->factory().nodeType();
                jsonObject["factoryNodeInherits"] = neChild->factory().nodeInherits();
                jsonObject["factoryVersion"] = neChild->factory().nodeVersion();
                jsonObject["factoryNodeObjectName"] = neChild->factory().objectName();
                jsonObject["factorynodeVendor"] = neChild->factory().nodeVendor();
                jsonObject["factorynodeHelp"] = neChild->factory().nodeHelp();

                guiitems.append( jsonObject );
            }
        }
        else if( const FDYNGUIManager* fgui = qobject_cast<const FDYNGUIManager*>(child))
        {
            QJsonObject propObj;
            jsonObject["name"] = fgui->objectName();
            jsonObject["properties" ] = getJSONFromProperties( qobject_cast< QObject *>( this ) );
            guiitems.append( jsonObject );

            //qDebug() << jsonObject ;
        }
    }
    QFileInfo cachedir( NEBASE.CacheDir().path() );
    if( !cachedir.exists() )
        NEBASE.create( filename );

    if ( sceneFilefri3d.open(QIODevice::WriteOnly) )
    {
        QJsonObject allobjects = QJsonDocument(guiitems).object();
        QJsonDocument saveDoc(  guiitems );
        sceneFilefri3d.write( saveDoc.toJson() );
    }
    //NEBASE.save( sceneFilefri3d.fileName()+"xx", NEBASE.SaveDir(), true );
    //NEBASE.saveGUIState();
}



QJsonArray FDYNGUIManager::getJSONFromProperties( const QObject *obj )
{
    const QObjectList& objList = obj->children();
    QJsonArray retval;


    for( int i = 0; i < obj->metaObject()->propertyCount(); i++ )
    {
        QMetaProperty metaProp = obj->metaObject()->property(i);
        QString metaPropName = metaProp.name();

        // if this property is an keyed property, continue with next property
       // if( !metaProp.isValid() || !metaProp.isWritable() || !metaProp.isStored() )
       //     continue;

        QJsonObject propValObj;
        QVariant v = metaProp.read( obj );
        propValObj[ "name" ]        = metaPropName;
        propValObj[ "type" ]        = metaProp.type();
        propValObj[ "typeName" ]    = metaProp.typeName();

        if( v.canConvert<FilePath>() )
        {
            FilePath sv = v.value<FilePath>();
            propValObj[ "value" ] = QJsonValue::fromVariant(sv);
        }
        else if (v.canConvert<vec3f>())
        {
            vec3f vec = v.value<vec3f>();
            QJsonObject vecObj;
            vecObj["x"] = vec.x();
            vecObj["y"] = vec.y();
            vecObj["z"] = vec.z();
            propValObj["value"] = vecObj;
        }
        else if (v.canConvert<vec2f>())
        {
            vec2f vec = v.value<vec2f>();
            QJsonObject vecObj;
            vecObj["x"] = vec.x();
            vecObj["y"] = vec.y();
            propValObj["value"] = vecObj;
        }
        else if( v.canConvert<eulerf>() )
        {
            eulerf sv = v.value<eulerf>();
            QJsonObject vv;
            vv ["phi"] = sv.phi();
            vv ["theta"] = sv.theta();
            vv ["psi"] = sv.psi();
            propValObj[ "value" ] = vv;
        }
        else if( static_cast<QMetaType::Type>(v.type())  == QMetaType::QColor )
        {
            QColor sv = v.value<QColor>();
            propValObj[ "value" ] = QJsonValue::fromVariant(sv);
        }
        else if( static_cast<QMetaType::Type>(v.type())  == QMetaType::QString )
        {
            QString sv = v.value<QString>();
            propValObj[ "value" ] = sv;
        }
        else if( static_cast<QMetaType::Type>(v.type()) == QMetaType::Double )
        {
            double sv = v.value<double>();
            propValObj[ "value" ] = sv;
        }
        else if( static_cast<QMetaType::Type>(v.type()) == QMetaType::Float )
        {
            float sv = v.value<float>();
            propValObj[ "value" ] = sv;
        }
        else if( static_cast<QMetaType::Type>(v.type()) == QMetaType::Int )
        {
            double sv = v.value<double>();
            propValObj[ "value" ] = sv;
        }
        else if( static_cast<QMetaType::Type>(v.type()) == QMetaType::Bool )
        {
            bool sv = v.value<bool>();
            propValObj[ "value" ] = sv;
        }

        retval.append( propValObj );
    }

    for( const QObject* child : objList )
        retval.append( getJSONFromProperties( child ) );

    return retval;
}



void FDYNGUIManager::importDrawing()
{
    QSettings settings("Fire Dynamics", "FRI3D");
    FDYNFileDialog dlg(mainWindow(), settings.value("LastDir/ImportDrawing").toString(),
                       tr("Import Drawing"), "Drawing files (*.pdf *.jpg *.tif *.png)", FDYNFileDialog::OPEN,
                     QFileDialog::ExistingFile);

    QString fileName;
    if( dlg.exec() == QDialog::Accepted )
    {
        fileName = dlg.selectedFile();
        settings.setValue("LastDir/ImportDrawing", dlg.directory());
    }

    if( !fileName.isEmpty() )
    {
        if(mainWindow()->zoneInspectorWindow()->root())
        {
            NESpatialEngine *fEngine = qobject_cast<NESpatialEngine *>(mainWindow()->zoneInspectorWindow()->root());
            if(!fEngine)
                return;
            QString zoneName = fEngine->activeZoneName();
            NEZone * activeZone = qobject_cast<NEZone *>(NESCENE.findChildNode(zoneName));
            if(activeZone)
            {
                ((FZone*)activeZone)->SetFloorPlan(FilePath(fileName));
            }
        }
    }
}



void FDYNGUIManager::importFRANX()
{
    QSettings settings("Fire Dynamics", "FRI3D");
    FDYNFileDialog dlg(mainWindow(), settings.value("LastDir/ImportFRANX").toString(),
                     tr("Import FRANX"), "FRANX Fire Data (*.franx)", FDYNFileDialog::OPEN,
                     QFileDialog::ExistingFile);

    QString fileName;
    if( dlg.exec() == QDialog::Accepted )
    {
        fileName = dlg.selectedFile();
        settings.setValue("LastDir/ImportFRANX", dlg.directory());
    }

    if( !fileName.isEmpty() )
    {
        emit sendImportFRANX( fileName );
    }
}


void FDYNGUIManager::importPDMS()
{
    QSettings settings("Fire Dynamics", "FRI3D");
    FDYNFileDialog dlg(mainWindow(), settings.value("LastDir/ImportFRANX").toString(),
                     tr("Import PDMS"), "Plant Database(*.*)", FDYNFileDialog::OPEN,
                     QFileDialog::ExistingFile);

    QString fileName;
    if( dlg.exec() == QDialog::Accepted )
    {
        fileName = dlg.selectedFile();
        settings.setValue("LastDir/ImportFRANX", dlg.directory());
    }

    if( !fileName.isEmpty() )
    {
        emit sendImportPDMS( fileName );
    }
}


void FDYNGUIManager::quitProject(int errcode)
{
    emit sendQuitDB();
}


void FDYNGUIManager::editPreferences()
{
    FDYNSettingsDialog* dlg = mainWindow()->settingsDialog();
    if( !dlg )
        return;

    dlg->show();
    dlg->setTempDataDir( P->m_tempFolder );
    QString newProjectFileName;
    if( dlg->exec() == QDialog::Accepted)
    {
        QFileInfo file( dlg->selectedTempDataDir()) ;
        if( file.dir().exists() )
        {
            newProjectFileName = dlg->selectedTempDataDir();
            emit sendUpdateWorkingFolder( newProjectFileName );
        }
        QFileInfo uifile( dlg->selectedUIDataDir()) ;
        if( uifile.dir().exists() )
        {
            newProjectFileName = dlg->selectedUIDataDir();
            emit sendUpdateUIFolder( newProjectFileName );
        }
        emit sendLoadUIData( dlg->loadUIData() );
        emit sendSaveUIData( dlg->saveUIData() );
    }
}

void FDYNGUIManager::updateSceneInspector()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    if( mainWindow() )
    {
        //if( mainWindow()->sceneInspectorWindow()->isVisible() )
            mainWindow()->sceneInspectorWindow()->update();
    }
}

void FDYNGUIManager::updateInspectors()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    if( mainWindow() )
    {
        //mainWindow()->sceneInspectorWindow()->update();
        auto endSceneTime = std::chrono::high_resolution_clock::now();
        qDebug() << "mainWindow()->sceneInspectorWindow()->update():" << std::chrono::duration_cast<std::chrono::milliseconds>(endSceneTime-startTime).count() << "msec.";
        //if( mainWindow()->zoneInspectorWindow()->isVisible() )
            mainWindow()->zoneInspectorWindow()->update();
        auto endZoneTime = std::chrono::high_resolution_clock::now();
        qDebug() << "mainWindow()->zoneInspectorWindow()->update():" << std::chrono::duration_cast<std::chrono::milliseconds>(endZoneTime-endSceneTime).count() << "msec.";
        //if( mainWindow()->scenarioInspectorWindow()->isVisible() )
            mainWindow()->scenarioInspectorWindow()->update();
        auto endScenarioTime = std::chrono::high_resolution_clock::now();
        qDebug() << "mainWindow()->scenarioInspectorWindow()->update():" << std::chrono::duration_cast<std::chrono::milliseconds>(endScenarioTime-endZoneTime).count() << "msec.";
        if( mainWindow()->franxInspectorWindow()->isVisible() )
            mainWindow()->franxInspectorWindow()->update();
        auto endFranxTime = std::chrono::high_resolution_clock::now();
        qDebug() << "mainWindow()->franxInspectorWindow()->update():" << std::chrono::duration_cast<std::chrono::milliseconds>(endFranxTime-endScenarioTime).count() << "msec.";
    }
}

void FDYNGUIManager::updateZoneInspector()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    if( mainWindow() )
    {
        //if( mainWindow()->zoneInspectorWindow()->isVisible() )
            mainWindow()->zoneInspectorWindow()->update();
        auto endZoneTime = std::chrono::high_resolution_clock::now();
        qDebug() << "mainWindow()->zoneInspectorWindow()->update():" << std::chrono::duration_cast<std::chrono::milliseconds>(endZoneTime-startTime).count() << "msec.";
    }
}

void FDYNGUIManager::updateScenarioInspector()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    if( mainWindow() )
    {
        //if( mainWindow()->scenarioInspectorWindow()->isVisible() )
        mainWindow()->scenarioInspectorWindow()->update();
        auto endScenarioTime = std::chrono::high_resolution_clock::now();
        qDebug() << "mainWindow()->scenarioInspectorWindow()->update():" << std::chrono::duration_cast<std::chrono::milliseconds>(endScenarioTime-startTime).count() << "msec.";
    }
}


void FDYNGUIManager::updateSystemInspector()
{
    if( mainWindow() )
        mainWindow()->systemInspectorWindow()->update();
}

void FDYNGUIManager::updatePropertyEditor()
{
    if( mainWindow()->propertyEditorWindow() )
        mainWindow()->propertyEditorWindow()->updateProperties();
}

void FDYNGUIManager::updatePropertyEditorsAtPostInit()
{
    P->updatePropertyEditorsAtPostInit();
}

void FDYNGUIManager::updateWidgetsForCreation()
{
    P->updateWidgetsForCreation();
}

void FDYNGUIManager::updateWidgetsForDeletion(QObject* object)
{
    P->updateWidgetsForDeletion( object );
}

void FDYNGUIManager::managersCreated( QList<std::shared_ptr<NEManager> > managers )
{
    QList<std::shared_ptr<NEManager> >::const_iterator ssItr = managers.begin();
    for( ; ssItr != managers.end(); ++ssItr )
    {
//        QObject::connect( ssItr->get(), SIGNAL(addCreateActionToGUI( const QString&, const NEManager*, const uint)),
//                          this, SLOT(addCreateActionToGUI( const QString&, const NEManager*, const uint)) );
//        QObject::connect( ssItr->get(), SIGNAL(removeCreateActionToGUI( const QString&, const NEManager*, const uint)),
//                          this, SLOT(removeCreateActionToGUI( const QString&, const NEManager*, const uint))  );
    }
    return;
}

void FDYNGUIManager::managerCreated(NEManager* pManager )
{
    if(!pManager)
        return;

//    QObject::connect( pManager, SIGNAL(addCreateActionToGUI( const QString&, const NEManager*, const uint)),
//                      this, SLOT(addCreateActionToGUI( const QString&, const NEManager*, const uint)) );
//    QObject::connect( pManager, SIGNAL(removeCreateActionToGUI( const QString&, const NEManager*, const uint)),
//                      this, SLOT(removeCreateActionToGUI( const QString&, const NEManager*, const uint)));

    return;
}

void FDYNGUIManager::closeGLWidget(NEGLWidget *val)
{
    return P->closeGLWidget( val );
}

void FDYNGUIManager::hideGLWidget(NEGLWidget *val)
{
    return P->hideGLWidget( val );
}

void FDYNGUIManager::objectCreated( NENode *object )
{
    QObject::connect( object, SIGNAL(pushUserEvent(NEUserEvent*)),
                      this, SLOT(pushUserEvent(NEUserEvent*)) );

    QObject::connect(object, &NENode::registerAction, this, &FDYNGUIManager::registerActionOnGUI);
    //QObject::connect(object, &NENode::unregisterAction, this, &FDYNGUIManager::unregisterActionOnGUI );

    if( NESpatialEngine* spatialEngine = qobject_cast< NESpatialEngine *>( object ))
    {
        connect( object, SIGNAL(allZones(QList<QString> &)), this, SLOT(allZones(QList<QString> &)));
        connect( object, SIGNAL(allScenarios(QList<QString> &)), this, SLOT(allScenarios(QList<QString> &)));

        connect( object, SIGNAL(relatedZones(QList<QString> &)), this, SLOT(relatedZones(QList<QString> &)));
        connect( object, SIGNAL(relatedScenarios(QList<QString> &)), this, SLOT(relatedScenarios(QList<QString> &)));

        connect( object, SIGNAL(activeZone(QObject*&, QObject*&)), this, SLOT(activeZone(QObject*&, QObject*&)));
        connect( object, SIGNAL(activeScenario(QObject*&, QObject*&)), this, SLOT(activeScenario(QObject*&, QObject*&)));

        connect( object, SIGNAL(sendShowWarningMessage(QString)), this, SLOT(showWarningMessage(QString)));

        connect( this, SIGNAL(sendImportFRANX(QString)), spatialEngine, SLOT(importFRANX(QString)));
        connect( this, SIGNAL(sendImportPDMS(QString)), spatialEngine, SLOT(importPDMS(QString)));

        connect( this, SIGNAL(setActiveZone(QString)), object, SLOT(setActiveZone(QString)));
        connect( this, SIGNAL(setActiveScenario(QString)), object, SLOT(setActiveScenario(QString)));

        connect( this, SIGNAL(zoneCreated(NEZone*)), spatialEngine, SLOT(zoneCreated(NEZone*)));

        connect( this, SIGNAL(sendAddNewZoneAndScenario(NEZone*, NEScenario*)), spatialEngine, SLOT(addNewZoneAndScenario(NEZone*, NEScenario*)) );
        connect( this, SIGNAL(sendAddNewScenario(NEScenario *)), spatialEngine, SLOT(addNewScenario(NEScenario*)) );

        connect( this, SIGNAL(sendScenarioCreated(NEScenario*)), spatialEngine, SLOT(scenarioCreated(NEScenario*)));
        connect( this, SIGNAL(sendScenarioDeleted(NEScenario*)), spatialEngine, SLOT(scenarioDeleted(NEScenario*)));

        connect( this, SIGNAL(componentCreated(NEZoneItem*, NENode*)),
                 spatialEngine, SLOT(componentCreated(NEZoneItem*, NENode*)));

        connect( this, SIGNAL(itemCreated(NEZoneItem*, NENode*)),
                 spatialEngine, SLOT(itemCreated(NEZoneItem*, NENode*)));
        connect( this, SIGNAL(itemDeleted(NEZoneItem*, bool)),
                 spatialEngine, SLOT(itemDeleted(NEZoneItem*, bool)));

        connect( this, SIGNAL(sendDuplicateObject(NEZoneItem*)),
                 spatialEngine, SLOT(duplicateObject(NEZoneItem*)));

        connect( this, SIGNAL(sendCopyProperties(NEZoneItem*)),
                 spatialEngine, SLOT(copyProperties(NEZoneItem*)));
        connect( this, SIGNAL(sendPasteProperties(QList<NEZoneItem*>, QString ) ),
                 spatialEngine, SLOT(pasteProperties(QList<NEZoneItem*>, QString )));

        connect( this, SIGNAL(trayCreated(NEZoneItem*, NENode*)),
                 spatialEngine, SLOT(trayCreated(NEZoneItem*, NENode*)));
        connect( this, SIGNAL(cableCreated(NEZoneItem*, NENode*)),
                 spatialEngine, SLOT(cableCreated(NEZoneItem*, NENode*)));

        connect( this, SIGNAL(sendAddItemToScenario(NEZoneItem*, NENode*)),
                 spatialEngine, SLOT(addItemToScenario(NEZoneItem*, NENode*)));
        connect( this, SIGNAL(sendRemoveItemFromScenario(NEZoneItem*, NENode*)),
                 spatialEngine, SLOT(removeItemFromScenario(NEZoneItem*, NENode*)));

        connect( this, SIGNAL(sendRemoveItemFromScenarios(NEZoneItem*, QList<NENode *>)),
                 spatialEngine, SLOT(removeItemFromScenarios(NEZoneItem*, QList<NENode *>)));

        connect( this, SIGNAL(saveAs(NENode*, QString)), spatialEngine, SLOT(saveAs(NENode*, QString)));

        connect( this, SIGNAL(sendNodeRenamed(NENode *, QString )),
                 spatialEngine, SLOT(nodeRenamed(NENode *, QString )) );

        connect( this, SIGNAL(sendComputeFranx()), spatialEngine, SLOT(compute()) );

        connect( this, SIGNAL(sendLoadDB(QString)), spatialEngine, SLOT(load(QString)) );
        connect( this, SIGNAL(sendSaveDB(QString)), spatialEngine, SLOT(save(QString)) );
        connect( this, SIGNAL(sendNewDB(QString)), spatialEngine, SLOT(newData(QString)) );
        connect( this, SIGNAL(sendImportDB(QString,QString,QString)),
                 spatialEngine, SLOT(importPlantData(QString, QString, QString)) );
        QObject::connect( this, SIGNAL(sendQuitDB()), spatialEngine, SLOT(quit()) );

        connect( this, SIGNAL(sendSetRecoverData(bool)), spatialEngine, SLOT(setRecoverData(bool)));
        connect( spatialEngine, SIGNAL(sendProjectRecovery(QString, bool)), this,  SLOT(openRecoverWarning(QString, bool)));

        connect( this, SIGNAL(sendAddFireDesc(NENode*)), spatialEngine, SLOT(addFire(NENode*)) );

        connect( this, SIGNAL(sendAddVent(NENode*)), spatialEngine, SLOT(addVent(NENode*)) );
        connect( this, SIGNAL(sendTurnIntoVent(NENode*, NENode*)), spatialEngine, SLOT(turnIntoVent(NENode*, NENode*)) );
        connect( this, SIGNAL(sendTurnIntoMechVent(NENode*, NENode*)), spatialEngine, SLOT(turnIntoMechVent(NENode*, NENode*)) );

        connect( this, SIGNAL(sendConvertToSource(NENode*, NENode*)), spatialEngine, SLOT(convertToSource(NENode*, NENode *)) );
        connect( this, SIGNAL(sendConvertToItem(NENode*, NENode*)), spatialEngine, SLOT(convertToItem(NENode*, NENode *)) );

        connect( this, SIGNAL(sendAddFireDetection(NENode*)), spatialEngine, SLOT(addFireDetection(NENode*)) );

        connect( this, SIGNAL(sendAddCompartment(NENode*, NENode*)),
                 spatialEngine, SLOT(addCompartment(NENode*,NENode*)) );

        //
        connect( spatialEngine, SIGNAL(sendUpdateSimTime(double)), this, SLOT(updateSimulationEndTime(double) ) );

        //
        // surface material signals
        //
        connect( this, SIGNAL(sendLoadDefaultSurfaceMaterials()),
                 spatialEngine, SLOT(loadDefaultSurfaceMaterials()) );

        connect( this, SIGNAL(sendAddSurfaceMaterial(NESurfaceMaterial)),
                 spatialEngine, SLOT(addSurfaceMaterial(NESurfaceMaterial)) );
        connect( this, SIGNAL(sendRemoveSurfaceMaterial(NESurfaceMaterial)),
                 spatialEngine, SLOT(removeSurfaceMaterial(NESurfaceMaterial)) );
        connect( this, SIGNAL(sendAssignSurfaceMaterial(NENode*, NESurfaceMaterial, QString)),
                 spatialEngine, SLOT(assignSurfaceMaterial(NENode *, NESurfaceMaterial, QString)) );
        connect( this, SIGNAL(sendSurfaceMaterialChanged(NESurfaceMaterial)),
                 spatialEngine, SLOT(changeSurfaceMaterial(NESurfaceMaterial)) );
        connect( spatialEngine, SIGNAL(sendUpdateSurfaceMaterials(QList<NESurfaceMaterial>)),
                 this, SLOT(updateSurfaceMaterials(QList<NESurfaceMaterial>) ) );
        connect( spatialEngine, SIGNAL(sendUpdateSurfaceMaterial(NESurfaceMaterial)),
                 this, SLOT(updateSurfaceMaterial(NESurfaceMaterial) ) );

        //
        // Fire material signals
        connect( this, SIGNAL(sendAddFireMaterial(NEFireMaterial)),
                 spatialEngine, SLOT(addFireMaterial(NEFireMaterial)) );
        connect( this, SIGNAL(sendRemoveFireMaterial(NEFireMaterial)),
                 spatialEngine, SLOT(removeFireMaterial(NEFireMaterial)) );
        connect( this, SIGNAL(sendAssignFireMaterial(NENode *, NEFireMaterial)),
                 spatialEngine, SLOT(assignFireMaterial(NENode *, NEFireMaterial)) );
        connect( spatialEngine, SIGNAL(sendUpdateFireMaterials(QList<NEFireMaterial>)),
                 this, SLOT(updateFireMaterials(QList<NEFireMaterial>) ) );
        connect( spatialEngine, SIGNAL(sendUpdateFireMaterial(NEFireMaterial) ),
                 this, SLOT(updateFireMaterial(NEFireMaterial) ) );

        //
        // compute signals
        //
        connect( spatialEngine, SIGNAL(sendUpdateComputeScenarios(QList<FRANXProperties>) ),
                 this, SLOT(updateComputeScenarios(QList<FRANXProperties>) ) );

        connect( this, SIGNAL(sendUpdateComputeScenario(FRANXProperties) ),
                 spatialEngine, SLOT(updateComputeScenarios(FRANXProperties) ) );

        // settings related signals
        connect( this, SIGNAL(sendUpdateWorkingFolder(QString)), spatialEngine, SLOT(updateWorkingFolder(QString) ) );
        connect( spatialEngine, SIGNAL(sendUpdateWorkingFolder(QString)), this, SLOT(updateTempFolder(QString) ) );

        connect( this, SIGNAL(sendUpdateUIFolder(QString)), spatialEngine, SLOT(updateUIFolder(QString) ) );
        connect( spatialEngine, SIGNAL(sendUpdateUIFolder(QString)), this, SLOT(updateUIFolder(QString) ) );

        connect( this, SIGNAL(sendLoadUIData(bool)), spatialEngine, SLOT(updateLoadUI(bool) ) );
        connect( this, SIGNAL(sendSaveUIData(bool)), spatialEngine, SLOT(updateSaveUI(bool) ) );
        connect( spatialEngine, SIGNAL(sendLoadUIData(QString)), this, SLOT(loadUI(QString) ) );
        connect( spatialEngine, SIGNAL(sendSaveUIData(QString)), this, SLOT(saveUI(QString) ) );

        // Simulation signals
        connect( this, SIGNAL(sendSimulateScenario(QString, QString, QString ) ),
                 spatialEngine, SLOT(simulateScenario(QString, QString, QString) ) );

        // Failure criteria signals
        connect( this, SIGNAL(sendAddFailureCriteria(NEFailureCriteria)),
                 spatialEngine, SLOT(addFailureCriteria(NEFailureCriteria)) );
        connect( this, SIGNAL(sendRemoveFailureCriteria(NEFailureCriteria)),
                 spatialEngine, SLOT(removeFailureCriteria(NEFailureCriteria)) );
        connect( this, SIGNAL(sendAssignFailureCriteria(NENode *, NEFailureCriteria)),
                 spatialEngine, SLOT(assignFailureCriteria(NENode *, NEFailureCriteria)) );
        connect( spatialEngine, SIGNAL(sendUpdateFailureCriterias(QList<NEFailureCriteria>)),
                 this, SLOT(updateFailureCriterias(QList<NEFailureCriteria>) ) );
        connect( spatialEngine, SIGNAL(sendUpdateFailureCriteria(NEFailureCriteria) ),
                 this, SLOT(updateFailureCriteria(NEFailureCriteria ) ) );

        connect( this, SIGNAL(sendAddCableProperty(NECableProperty)),
                 spatialEngine, SLOT(addCableProperty(NECableProperty)) );
        connect( this, SIGNAL(sendRemoveCableProperty(NECableProperty)),
                 spatialEngine, SLOT(removeCableProperty(NECableProperty)) );
        connect( this, SIGNAL(sendAssignCableProperty(NENode*,NECableProperty)),
                 spatialEngine, SLOT(assignCableProperty(NENode *, NECableProperty)) );
        connect( spatialEngine, SIGNAL(sendUpdateCableProperties(QList<NECableProperty>, std::vector<QList<QString>>)),
                 this, SLOT(updateCableProperties(QList<NECableProperty>, std::vector<QList<QString>>)));
        connect( spatialEngine, SIGNAL(sendUpdateCableProperty(NECableProperty) ),
                 this, SLOT(updateCableProperty(NECableProperty ) ) );

        connect( this, SIGNAL(sendAddVentCriteria(NEVentCriteria)),
                 spatialEngine, SLOT(addVentCriteria(NEVentCriteria)) );
        connect( this, SIGNAL(sendRemoveVentCriteria(NEVentCriteria)),
                 spatialEngine, SLOT(removeVentCriteria(NEVentCriteria)) );
        connect( this, SIGNAL(sendAssignVentCriteria(NENode *, NEVentCriteria, QString)),
                 spatialEngine, SLOT(assignVentCriteria(NENode *, NEVentCriteria, QString)) );
        connect( spatialEngine, SIGNAL(sendUpdateVentCriterias(QList<NEVentCriteria>)),
                 this, SLOT(updateVentCriterias(QList<NEVentCriteria>) ) );
        connect( spatialEngine, SIGNAL(sendUpdateVentCriteria(NEVentCriteria) ),
                 this, SLOT(updateVentCriteria(NEVentCriteria ) ) );

        connect(this, SIGNAL(sendSetDynamicCombo(NENode*, NEDynamicCombo, QString)),
                spatialEngine, SLOT(setDynamicCombo(NENode*, NEDynamicCombo, QString)));

        connect(this, SIGNAL(sendChangeColor(double)), spatialEngine, SLOT(changeColorOfZoneItem(double)));
        connect(spatialEngine, SIGNAL(allFailedItems( QList<int>, QList<double>, QList<int>, QList<QString>)),
                this, SLOT(processAboutFailedItems(QList<int>, QList<double>, QList<int>, QList<QString>)));

        connect(spatialEngine, SIGNAL(repaintGLWindows()), this, SLOT(repaintGLWindows()));
    }
    else if( qobject_cast< NEZone *>( object ))
    {
        // Create a zone and make it active under the current engine with active scenario
        //emit zoneCreated( qobject_cast< NEZone *>( object ) );
    }
    else if( qobject_cast< NEScenario *>( object) )
    {
        // Create a scenario and make it active under the current engine with active zone
//        NESpatialEngine *engine = qobject_cast<NESpatialEngine *>(mainWindow()->zoneInspectorWindow()->root());
//        if(engine && object->parent() == &NESCENE){
//            object->setParent(engine);
//            engine->scenarioCreated( qobject_cast< NEScenario *>( object ) );
//        }
    }
    else if( qobject_cast< NEZoneItem *>( object ) )
    {
        NEZoneItem *zitem = qobject_cast<NEZoneItem*>(object);
        // Create a zone item and make it active under the latest selection
        //emit zoneItemCreated( qobject_cast< NEZoneItem *>( object ), qobject_cast< NENode *>( object->parent() ) );
    }
}

void FDYNGUIManager::activeZone(QObject*& fengine, QObject *& zone)
{
    //std::cout << "Active Zone: " << zone->objectName().toStdString() << std::endl;
    if( zone )
    {
        mainWindow()->zoneInspectorWindow()->setRoot( zone->parent() );
        mainWindow()->franxInspectorWindow()->setRoot(zone );
    }
    else if( fengine )
    {
        mainWindow()->zoneInspectorWindow()->setRoot(fengine);
        mainWindow()->franxInspectorWindow()->setRoot(0);
    }
    else
    {
        mainWindow()->zoneInspectorWindow()->setRoot(0);
        mainWindow()->franxInspectorWindow()->setRoot(0);
    }
}

///
/// \brief FDYNGUIManager::activeScenario
/// \param fengine
/// \param scenario
///

void FDYNGUIManager::activeScenario(QObject*& fengine, QObject *& scenario)
{
    if(scenario)
    {
        mainWindow()->scenarioInspectorWindow()->setRoot( scenario->parent());
    }
    else if( fengine)
    {
        mainWindow()->scenarioInspectorWindow()->setRoot(fengine);
    }
    else
    {
        mainWindow()->scenarioInspectorWindow()->setRoot(0);
    }
}

void FDYNGUIManager::showWarningMessage(QString message)
{
    QMessageBox msgBox(QMessageBox::Warning, QString("FRI3D"),message, QMessageBox::Ok, mainWindow());
    msgBox.exec();
}

void FDYNGUIManager::allZones(QList<QString> &allZones)
{
    P->m_Zones = allZones;
    mainWindow()->zoneInspectorWindow()->zoneInspector()->updateAllZones(P->m_Zones);
}

void FDYNGUIManager::allScenarios(QList<QString> &allScenarios)
{
    P->m_Scenarios = allScenarios;
    mainWindow()->scenarioInspectorWindow()->scenarioInspector()->updateAllScenarios(P->m_Scenarios);
}

void FDYNGUIManager::relatedZones(QList<QString> &zonelist)
{
    mainWindow()->zoneInspectorWindow()->zoneInspector()->updateOtherZones( zonelist );
}

void FDYNGUIManager::relatedScenarios(QList<QString> &scnlist)
{
    mainWindow()->scenarioInspectorWindow()->scenarioInspector()->updateOtherScenarios( scnlist );
}

void FDYNGUIManager::objectRemoved(NENode* object)
{
    P->prepareForObjectRemoval( object );
}

void FDYNGUIManager::setCurrentFrame(int val)
{
    if(P->m_TimelineUnits == FDYNGUIManager::Seconds){
        P->setCurrentFrame( val / NESCENE.RecordFrameRate() );
    }
    else
        P->setCurrentFrame( val );

    emit sendChangeColor(NESCENE.ElapsedTime());
}

void FDYNGUIManager::repaintTimeline()
{
    P->repaintTimeline();
}

void FDYNGUIManager::setPlay(bool val)
{
    P->setPlayButton( val );
}

void FDYNGUIManager::setTimelineKeyframeMarks(QList<uint> marks)
{
    P->setTimelineKeyframeMarks( marks );
}

void FDYNGUIManager::setSimulationStartFrame(int val)
{
    P->setSimulationEndFrameOrTime(val);
}
void FDYNGUIManager::setSimulationEndFrame(int val)
{
    if(P->m_TimelineUnits == FDYNGUIManager::Frames)
    {
        P->setSimulationEndFrameOrTime( val );
    }
}

void FDYNGUIManager::setPlaybackStartFrame(int val)
{
    if(P->m_TimelineUnits == FDYNGUIManager::Frames)
    {
        P->setPlaybackStartFrameOrTime( val );
    }
}

void FDYNGUIManager::setPlaybackEndFrame(int val)
{
    if(P->m_TimelineUnits == FDYNGUIManager::Frames)
    {
        P->setPlaybackEndFrameOrTime( val );
    }
}

void FDYNGUIManager::updateSimulationEndTime( double val )
{
    QObject *whot = P->m_pTimeline->rootObject()->findChild<QObject*>("spinLastTime");
    if( whot )
    {
        if(P->m_TimelineUnits == FDYNGUIManager::Seconds)
        {
            whot->setProperty("value", (double) val * NESCENE.RecordFrameRate());
        }
        else if( P->m_TimelineUnits == FDYNGUIManager::Frames )
        {
            whot->setProperty("value", (double) val/NESCENE.RecordFrameRate());
        }
    }
}

void FDYNGUIManager::setSimulationEndTime(double val)
{
    if(P->m_TimelineUnits == FDYNGUIManager::Seconds)
    {
        P->setSimulationEndFrameOrTime(val*NESCENE.RecordFrameRate());
    }
}

void FDYNGUIManager::setPlaybackStartTime(double val)
{
    if(P->m_TimelineUnits == FDYNGUIManager::Seconds)
    {
        P->setPlaybackStartFrameOrTime(val*NESCENE.RecordFrameRate());
    }
}

void FDYNGUIManager::setPlaybackEndTime(double val)
{
    if(P->m_TimelineUnits == FDYNGUIManager::Seconds)
    {
        P->setPlaybackEndFrameOrTime(val*NESCENE.RecordFrameRate());
    }
}

void FDYNGUIManager::setSimulate(bool val)
{
    if (!val)
        P->setPlayButton( false );
    P->setSimulateButton( val );
}

void FDYNGUIManager::setRepeat(bool val)
{
    P->setRepeatButton( val );
}

void FDYNGUIManager::triggerSavingData()
{
#if 0
    P->triggerSavingGLData();
#endif
}

void FDYNGUIManager::registerActionOnGUI(QString actionName, QObject* &action)
{
    QAction* objectAction = new QAction(actionName, NULL);

    if(objectAction && objectAction->text() == "Delete")
        if( mainWindow() )
            mainWindow()->registerRemoveAction(objectAction);
    action = objectAction;
}

void FDYNGUIManager::unregisterActionOnGUI(QString actionName, QObject* &action)
{
    QAction* objectAction = new QAction(actionName, NULL);

    action = objectAction;
}

void FDYNGUIManager::addCreateActionToGUI(const QString &type, const NEManager *manager, const uint nodeType)
{
    P->addCreateActionToGUI( type, manager, nodeType );
}

void FDYNGUIManager::removeCreateActionToGUI(const QString &type, const NEManager *manager, const uint nodeType)
{
    P->removeCreateActionToGUI( type, manager, nodeType );
}

void FDYNGUIManager::updatePropertyEditorTitle(QString val)
{
#if 0
    mainWindow()->propertyEditorDock()->setTitleText("SETTINGS - " + val);
#endif
}


void FDYNGUIManager::updateTempFolder(QString folderName)
{
    P->m_tempFolder = folderName;
    FDYNSettingsDialog* dlg = mainWindow()->settingsDialog();
    if( !dlg )
        return;

    dlg->setTempDataDir( P->m_tempFolder );

    return;
}

void FDYNGUIManager::updateUIFolder(QString folderName)
{
    P->m_tempFolder = folderName;
    FDYNSettingsDialog* dlg = mainWindow()->settingsDialog();
    if( !dlg )
        return;

    dlg->setUIDataDir( P->m_tempFolder );

    return;
}


void FDYNGUIManagerPrivate::mapActionToCreateObject(QAction* action, const QString& type, const NEManager* manager)
{
    QSignalMapper* signalMapper = new QSignalMapper( action );
    QObject::connect( action, SIGNAL(triggered()), signalMapper, SLOT(map()) );
    if( manager != NULL )
    {
        signalMapper->setMapping( action, type );
        QObject::connect( signalMapper, SIGNAL(mapped(const QString&)), manager, SLOT(callPyScriptForObjectCreation(const QString&)) );
    }
}

void FDYNGUIManagerPrivate::addCreateActionToGUI(const QString& type, const NEManager* manager, const uint nodeType)
{
    QMenu* targetMenu = m_pMainWindow->creationMenu( nodeType );

    if( type == "Spatial Mesh" ||
            type == "Fire Engine" ||
            type == "Fire Material" ||
            type == "Group" ||
            type == "Zone" )
    {
        return;
    }

    QAction* action = new QAction( type, m_pMainWindow );
    targetMenu->addAction( action );
    QFont f = action->font();
    f.setFamily("Segoe UI");
    //f.setStyleHint(QFont::Helvetica);
    f.setStyleStrategy(QFont::PreferAntialias);
    //f.setPointSize(8);

    f.setCapitalization( QFont::AllUppercase );
    f.setBold(true);
    action->setFont(f);
#if 0
    if( m_pMainWindow->sceneBuilder() )
        m_pMainWindow->sceneBuilder()->addCreateAction( action, nodeType );
#endif

    if(type == "Fire Scenario")
    {
        QObject::connect( action, SIGNAL(triggered()), S, SLOT(addScenario()) );
    }
    else
    {
        mapActionToCreateObject( action, type, manager );
    }
}

void FDYNGUIManagerPrivate::removeCreateActionToGUI(const QString& type, const NEManager* manager, const uint nodeType)
{
    QMenu* targetMenu = m_pMainWindow->creationMenu(nodeType);
    QList<QAction*> la = targetMenu->actions();
    foreach(QAction* a, la)
        if(a->text() == type)
        {
#if 0
            m_pMainWindow->sceneBuilder()->removeCreateAction(a, nodeType);
#endif
            a->disconnect();
            la.removeAll(a);
            delete a;
        }
}

QDebug& FDYNGUIManager::log()
{
#if 1
    return mainWindow()->messageWindow()->getNewDebugStream();
#else
    qDebug() << P->logStr;
    return *(P->log);
#endif
}

void FDYNGUIManager::endLog(NE::NELogType type)
{
    if( NEBASE.guiMode() )
        if(mainWindow() && mainWindow()->isVisible())
            mainWindow()->messageWindow()->pushLogInformation(type);
}

void FDYNGUIManager::EvalPyGUI(const QString& script)
{
    //consoleMessage(">> " + code);

    getPyMain().evalScript(script);

    if(UpdateUI())
    {
        //outputWindow->appendHtml("<font color=#003A00>" + code + "</font>");
        //flushStdOut();
        updateSceneInspector();
        updateSystemInspector();
        updatePropertyEditor();
        updateNodeGraphEditor();
        updateGLWindows();
    }

    NESCENE.cleanup();
}

void FDYNGUIManager::updateGLWindowsFromBase()
{
    updateGLWindows();
}

void FDYNGUIManager::addCameraIntoContextMenus(NENode *node)
{
    if(NECamera* tmpObject = qobject_cast<NECamera*> (node))
    {
        P->addCameraIntoContextMenus(tmpObject);
    }
}

void FDYNGUIManager::removeCameraFromContextMenus(NENode *node)
{
    if(NECamera* tmpObject = qobject_cast<NECamera*> (node))
    {
        P->removeCameraFromContextMenus(tmpObject);
    }
}


void FDYNGUIManager::removeItem(NEZoneItem *zoneItem, bool deleteItem )
{
    if( mainWindow()->sceneInspectorWindow() )
        mainWindow()->sceneInspectorWindow()->removeSelectedItem();

    emit itemDeleted( zoneItem, deleteItem );
}

void FDYNGUIManager::createNewPropertyEditor()
{
    FDYNDockWidget* pDock = new FDYNDockWidget(mainWindow());
    QSizePolicy sp(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sp.setHorizontalStretch(2);
    sp.setVerticalStretch(2);
    sp.setHeightForWidth(pDock->sizePolicy().hasHeightForWidth());
    pDock->setSizePolicy(sp);
    pDock->setMinimumSize(QSize(250, 58));
    pDock->setMaximumSize(QSize(524287, 524287));
    FDYNPropertyEditorWindow* p = new FDYNPropertyEditorWindow();

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

void FDYNGUIManager::showMainWindow()
{
    if( mainWindow() )
        P->showMainWindow();
}

void FDYNGUIManager::hideSceneLoadingSplash()
{
#if 0
    if( mainWindow() )
        mainWindow()->hideSceneLoadingSplash();
#endif
}

void FDYNGUIManager::showSceneManagementDialog()
{
#if 0
    if( mainWindow() )
        mainWindow()->showSceneManagementDialog();
#endif
}

void FDYNGUIManager::postInit()
{
    P->postInit();
}

void FDYNGUIManager::layout()
{

}

void FDYNGUIManager::updateInitFlagForNodeGraph(bool aInitFlag)
{
;
}

void FDYNGUIManager::updateNodeRenameForNodeGraph(const QString &oldName, const QString &newName)
{

}

void FDYNGUIManager::deInit()
{
    P->deInit();
}

void FDYNGUIManager::hideSceneLoadingWidget()
{
#if 1
    if( mainWindow() ){
        mainWindow()->loadingWidget()->hide();
    }
#endif
}

void FDYNGUIManager::showSceneLoadingWidget()
{
#if 1
    if( mainWindow() )
        mainWindow()->showLoadingWidget();
#endif
}

void FDYNGUIManager::createGLWindow(QString name)
{
#if 0
    if( P->m_pMainGLWidget->objectName() != name )
    {
        P->createGLWindowOnLoad( name );
    }
#endif
}

void FDYNGUIManager::createPropertyEditor(QString name)
{
    P->m_PropEdObjs.append( name );
    createNewPropertyEditor();
}

void FDYNGUIManager::restoreGUIState(QXmlStreamReader *xmlReader)
{
    P->restoreGUIStateFromScene( xmlReader );
}

void FDYNGUIManager::setCurrentOperation(QString msg)
{
#if 1
    if( mainWindow() &&  mainWindow()->loadingWidget() )
        mainWindow()->loadingWidget()->setCurrentOperation( msg );
    return;
#endif
}

void FDYNGUIManager::setCurrentSubOperation(QString msg)
{
#if 1
    if( mainWindow() && mainWindow()->loadingWidget() )
        mainWindow()->loadingWidget()->setCurrentSubOperation( msg );
    return;
#endif
}

void FDYNGUIManager::loadGUIState()
{
    if( !NEBASE.sceneDataPath().isEmpty() && NEBASE.guiMode() )
    {
        P->m_GUIStatePath = NEBASE.sceneDataPath() + "/neutrino.state";
        if( NEBASE.initialized() )
            P->loadGUIState();
    }
}

void FDYNGUIManager::deleteMainWindow()
{
    if( mainWindow() )
        delete mainWindow();
}

void FDYNGUIManager::setWindowTitle( QString title )
{
    if( mainWindow() )
        mainWindow()->setWindowTitle( title );
}

void FDYNGUIManager::splitGLDockHorizontally(NEGLWidget *wid)
{
    return P->splitGLDockHorizontally( wid );
}

void FDYNGUIManager::splitGLDockVertically(NEGLWidget *wid)
{
    return P->splitGLDockVertically( wid );
}

void FDYNGUIManager::splitGLDockToQuad(NEGLWidget *wid)
{
    return P->splitGLDockToQuad( wid );
}

void FDYNGUIManager::writeAllGUIState( QXmlStreamWriter *xmlWriter )
{
    if( !xmlWriter )
        return;

    xmlWriter->writeStartElement( "GUI_STATE" );
    P->saveGUIStateIntoScene( xmlWriter );
    xmlWriter->writeEndElement();
}

void FDYNGUIManager::saveGUIState()
{
    P->m_GUIStatePath = NEBASE.sceneDataPath() + "/neutrino.state";

    if( NEBASE.initialized() )
        P->saveGUIState();
}

void FDYNGUIManager::writeAllGUICreations( QXmlStreamWriter *xmlWriter )
{
    if( !xmlWriter )
        return;


    // Write all the objects in the scene
    xmlWriter->writeStartElement("guicreations");

    P->writeAllGUISubCreations( xmlWriter );

    // Writing objects ends here
    xmlWriter->writeEndElement();
}

void FDYNGUIManager::setParent(QObject *varRoot)
{
    NEObject::setParent( varRoot );
}

void FDYNGUIManager::updateGLWindows()
{
    foreach(const GLDockWidgetBundle* w, P->m_IDtoGLDockWidget)
        w->glWidget->update();
}

void FDYNGUIManager::repaintGLWindows()
{
    foreach(const GLDockWidgetBundle* w, P->m_IDtoGLDockWidget)
        w->glWidget->repaint();
}

void FDYNGUIManager::resizeGLWindows()
{
    foreach(const GLDockWidgetBundle* w, P->m_IDtoGLDockWidget){
        // If there is no any OpenGL context assigned for the widget, return immediately
        if(!w->glWidget->isVisible() || !w->glWidget->isValid() || !NESCENE.initialized() || !NEBASE.initialized())
            return;
        w->glWidget->resize(w->glWidget->width(), w->glWidget->height());
    }
}

QStringList FDYNGUIManager::fileImportBrowser(FilePath &filePath, QString orgName, QString appName, QString settingsName)
{
    QStringList fileNames;

    QSettings settings( orgName, appName );

    FDYNFileDialog dlg(mainWindow(), settings.value(settingsName).toByteArray(),
                     tr(""), filePath.filter(), FDYNFileDialog::OPEN,
                     QFileDialog::ExistingFiles);
    if( dlg.exec() == QDialog::Accepted )
        fileNames = dlg.selectedFiles();

    if( fileNames.size() > 0 )
        settings.setValue( settingsName, QFileInfo(fileNames[0]).path());

    return fileNames;

}

void FDYNGUIManager::updateGLMotionMap()
{
    //((NEGLWidget*)m_pGLWidgetPersp)->renderMotionVectors();
}


void FDYNGUIManager::updateNodeRenameToAllGUI(NENode* node)
{
    if(NESCENE.selectedObject() == node)
    {
        updatePropertyEditorTitle(node->objectName());
    }
}

void FDYNGUIManager::showMainGLWidget()
{
    return P->showMainGLWidget();
}

FDYNMainWindow *FDYNGUIManager::mainWindow()
{
    return P->m_pMainWindow;
}

bool FDYNGUIManagerPrivate::resizeGLWidget(int width, int height, bool fixed)
{
    m_IDtoGLDockWidget.last()->glWidget->setMaximumSize(16777215, 16777215);
    m_IDtoGLDockWidget.last()->glWidget->setMinimumSize(0, 0);
    m_IDtoGLDockWidget.last()->glWidget->adjustSize();
    return true;
}

void FDYNGUIManagerPrivate::makeConnections()
{
    if( !NEBASE.guiMode() )
        return;

#if 1
    // Initialize the timeline related connections
    timeline()->init();
#endif
    QObject::connect( &NEBASE, SIGNAL(showMainWindow()), S, SLOT(showMainWindow()) );
    QObject::connect( &NEBASE, SIGNAL(layout()), S, SLOT(layout()) );

    QObject::connect( &NEBASE, SIGNAL(createPropertyEditor(QString)),
                      S, SLOT(createPropertyEditor(QString)) );
    QObject::connect( &NEBASE, SIGNAL(deInitUI()), S, SLOT(deInit()) );
    QObject::connect( &NEBASE, SIGNAL(postInit()), S, SLOT(postInit()) );

    QObject::connect( &NEBASE, SIGNAL(updatePropertyEditor()),
                      S, SLOT(updatePropertyEditor()) );
    QObject::connect( &NESCENE, SIGNAL(updatePropertyEditor()),
                      S, SLOT(updatePropertyEditor()) );

    QObject::connect( &NEBASE, SIGNAL(updatePropertyEditorsAtPostInit()), S,
                      SLOT(updatePropertyEditorsAtPostInit()) );

    QObject::connect( &NEBASE, SIGNAL(selectObject(QObject *)), S,
                      SLOT(selectObject(QObject *)) );

    QObject::connect( &NEBASE, SIGNAL(updateSceneInspector()), S,
                      SLOT(updateInspectors()) );
    QObject::connect( &NESCENE, SIGNAL(updateSceneInspector()), S,
                      SLOT(updateInspectors()) );
    QObject::connect( &NEBASE, SIGNAL(updateSystemInspector()), S,
                      SLOT(updateSystemInspector()) );
    QObject::connect( &NEBASE, SIGNAL(writeAllGUICreations( QXmlStreamWriter *) ),
                      S, SLOT(writeAllGUICreations( QXmlStreamWriter *) ) );

    QObject::connect( &NEBASE, SIGNAL(writeAllGUIState( QXmlStreamWriter *)),
                      S, SLOT(writeAllGUIState( QXmlStreamWriter *)) );

    QObject::connect( &NEBASE, SIGNAL(setParentUI(QObject *)),
                      S, SLOT(setParent(QObject *)));

    QObject::connect( &NEBASE, SIGNAL(closeAllPropertyEditors()),
                      S, SLOT(closeAllPropertyEditors()));
    QObject::connect( &NEBASE, SIGNAL(closeAllGLDocks()),
                      S, SLOT(closeAllGLDocks()) );

    QObject::connect( &NEBASE, SIGNAL(deleteMainWindow()), S, SLOT(deleteMainWindow()));
    QObject::connect( &NEBASE, SIGNAL(setWindowTitle(QString)), S, SLOT(setWindowTitle(QString)) );

    QObject::connect( &NEBASE, SIGNAL(loadGUIState()), S, SLOT(loadGUIState()) );
    QObject::connect( &NEBASE, SIGNAL(saveGUIState()), S, SLOT(saveGUIState()) );
    QObject::connect( &NESCENE, SIGNAL(pushUserEvent(NEUserEvent*)),
                      S, SLOT(pushUserEvent(NEUserEvent*)) );
    QObject::connect( &NESCENE, SIGNAL(resetWidgets()), S, SLOT( resetWidgets()) );
    QObject::connect( &NESCENE, SIGNAL(managersCreated(QList<std::shared_ptr<NEManager> >) ),
                      S, SLOT(managersCreated(QList<std::shared_ptr<NEManager> >)));

    QObject::connect( &NESCENE, SIGNAL(selectObjectUI(QObject *)),
                      S, SLOT( selectObject(QObject *)));

    QObject::connect( &NESCENE, SIGNAL(triggerSavingData( ) ), S, SLOT(triggerSavingData(  ))
                     );

    QObject::connect( &NESCENE, SIGNAL(updateWidgetsForCreation()),
                      S, SLOT(updateWidgetsForCreation()) );
    QObject::connect( &NESCENE, SIGNAL(updateWidgetsForDeletion(QObject *)),
                      S, SLOT(updateWidgetsForDeletion(QObject *)) );

    QObject::connect( &NEBASE, SIGNAL(hideSceneLoadingSplash()), S, SLOT(hideSceneLoadingSplash()));
    QObject::connect( &NEBASE, SIGNAL(showSceneLoadingWidget()), S, SLOT(showSceneLoadingWidget()) );
    QObject::connect( &NEBASE, SIGNAL(hideSceneLoadingWidget()), S, SLOT(hideSceneLoadingWidget()) );
    QObject::connect( &NEBASE, SIGNAL(showSceneManagementDialog()), S, SLOT(showSceneManagementDialog()) );
    //    QObject::connect( &NEBASE, SIGNAL(updateInitFlagForNodeGraph(bool)), S, SLOT(updateInitFlag(bool)) );

    QObject::connect( &NEBASE, SIGNAL(setCurrentOperation(QString)), S, SLOT(setCurrentOperation(QString)) );
    QObject::connect( &NEBASE, SIGNAL(setCurrentSubOperation(QString)), S, SLOT(setCurrentSubOperation(QString)) );

    QObject::connect( &NEBASE, SIGNAL(updateAllConnectionTables()), S, SLOT(updateAllConnectionTables()) );
    //QObject::connect( &NEBASE, SIGNAL(updateExpressionInspector()), S, SLOT(updateExpressionInspector()));

    QObject::connect( &NEBASE, SIGNAL(updateGLWindows()), S, SLOT(updateGLWindows()) );
    QObject::connect( &NESCENE, SIGNAL(updateGLWindows()), S, SLOT(updateGLWindows()) );

    QObject::connect( &NEBASE, SIGNAL(createGLWindow(QString)),
                      S, SLOT(createGLWindow(QString)) );
//    QObject::connect( &NEBASE, SIGNAL(createGLWindowOnLoad(QString)),
//                      S, SLOT(createGLWindowOnLoad(QString)));

    QObject::connect( &NESCENE, SIGNAL(CurrentFrameChanged(int)), S, SLOT( setCurrentFrame(int)));

    QObject::connect( &NESCENE, SIGNAL(repaintTimeline()), S, SLOT( repaintTimeline()) );
    QObject::connect( &NESCENE, SIGNAL(setPlay(bool)), S, SLOT( setPlay(bool)) );

    QObject::connect( &NESCENE, SIGNAL(setTimelineKeyframeMarks(QList<uint>)),
                      S, SLOT(setTimelineKeyframeMarks(QList<uint>)));

    QObject::connect( &NESCENE, SIGNAL(SimulationEndFrameChanged( int) ),
                      S, SLOT(setSimulationEndFrame( int )));
    QObject::connect( &NESCENE, SIGNAL(PlaybackStartFrameChanged( int) ),
                      S, SLOT(setPlaybackStartFrame( int )));
    QObject::connect( &NESCENE, SIGNAL(PlaybackEndFrameChanged( int) ),
                      S, SLOT(setPlaybackEndFrame( int )));

    QObject::connect(&NESCENE, SIGNAL(SimulationEndTimeChanged(double)),
                      S, SLOT(setSimulationEndTime(double)));
    QObject::connect(&NESCENE, SIGNAL(PlaybackStartTimeChanged(double)),
                      S, SLOT(setPlaybackStartTime(double)));
    QObject::connect(&NESCENE, SIGNAL(PlaybackEndTimeChanged(double)),
                      S, SLOT(setPlaybackEndTime(double)));
    QObject::connect(S, SIGNAL(TimelineUnitsChanged(FDYNTimelineUnits)),
                        S, SLOT(setTimeLineUnitsToScene(FDYNTimelineUnits)));

#if 0

    QObject::connect( &NESCENE, SIGNAL(setRepeat( int) ), S, SLOT(setRepeat( int )),
                       );

    QObject::connect( &NEBASE, SIGNAL(updateWindowVisibilityFlags()), S, SLOT(updateWindowVisibilityFlags()) );

    QObject::connect( &NESCENE, SIGNAL(updateCharts( ) ), S, SLOT(updateCharts(  )),
                       );

#endif

    QObject::connect( &NEBASE, SIGNAL(updateWindowVisibilityFlags()), S->mainWindow(), SLOT(updateWindowVisibilityFlags()) );

    QObject::connect( &NESCENE, SIGNAL(setSimulate( bool) ), S, SLOT(setSimulate( bool )));

    QObject::connect( &NERENDERMAN, SIGNAL(managerCreated(NEManager*) ),
                      S, SLOT(managerCreated(NEManager*)));

    //QObject* btnMeshSize = S->mainWindow()->CADModelingTool()->rootObject()->findChild<QObject *>("btnMeshSize");
    //QObject::connect(btnMeshSize, SIGNAL(onButtonClicked()), S, SLOT(showZoneSizeTool()));


    //QObject* btnMeshPos = S->mainWindow()->CADModelingTool()->rootObject()->findChild<QObject *>("btnMeshPos");
    //QObject::connect(btnMeshPos, SIGNAL(onButtonClicked()), S, SLOT(showZoneLocTool()));

    QObject* btnSnapBoundary = S->mainWindow()->CADModelingTool()->rootObject()->findChild<QObject *>("btnSnapToBoundary");
    QObject::connect(btnSnapBoundary, SIGNAL(onToggled(bool)), S, SLOT(setSnapToBoundary(bool)));

    QObject* btnSnapObj = S->mainWindow()->CADModelingTool()->rootObject()->findChild<QObject *>("btnSnapToObj");
    QObject::connect(btnSnapObj, SIGNAL(onToggled(bool)), S, SLOT(setSnapToObj(bool)));

    QObject* btnSnapUnit = S->mainWindow()->CADModelingTool()->rootObject()->findChild<QObject *>("btnSnapUnit");
    QObject::connect(btnSnapUnit, SIGNAL(onToggled(bool)), S, SLOT(setSnapUnit(bool)));
    QObject::connect(btnSnapUnit, SIGNAL(onRightButtonClicked()), S, SLOT(showSnapUnitProperty()));

    //QObject* btnSnapEdge = S->mainWindow()->CADModelingTool()->rootObject()->findChild<QObject *>("btnSnapToEdge");
    //QObject::connect(btnSnapEdge, SIGNAL(onToggled(bool)), S, SLOT(setSnapToEdge(bool)));

    QObject* btnMeasure = S->mainWindow()->CADModelingTool()->rootObject()->findChild<QObject *>("btnMeasure");
    QObject::connect(btnMeasure, SIGNAL(onToggled(bool)), S, SLOT(setMeasure(bool)));

    QObject* btnMeasureEdge = S->mainWindow()->CADModelingTool()->rootObject()->findChild<QObject *>("btnMeasureEdge");
    QObject::connect(btnMeasureEdge, SIGNAL(onToggled(bool)), S, SLOT(setMeasureEdge(bool)));

    foreach(const GLDockWidgetBundle* w, m_IDtoGLDockWidget)
    {
        QObject::connect( w->glWidget, SIGNAL(selectObject(QObject *)), S, SLOT(selectObject(QObject *)) );
        QObject::connect( w->glWidget, SIGNAL(selectObjects(QList<NENode*> &)), S, SLOT(selectObjects(QList<NENode*> &))  );

        QObject::connect( w->glWidget, SIGNAL(updatePropertyEditorUI()), S, SLOT(updatePropertyEditor()) ) ;
        QObject::connect( w->glWidget, SIGNAL(updateGLWindows()), S, SLOT(updateGLWindows()) );

        QObject::connect(w->glWidget, SIGNAL(dropCalled(matrix44f , matrix44f , double, double, double, double, void *)),
                         S, SIGNAL(dropCalled(matrix44f, matrix44f, double, double, double, double, void*)));

        QObject::connect(w->glWidget, SIGNAL(popupDescriptionAndName(const QPoint&, QWidget *, NENode *, QPainter *)),
                         S, SLOT(popupDescriptionAndName(const QPoint&, QWidget *, NENode *, QPainter *)));

        QObject::connect(w->glWidget, SIGNAL(showPropertyShortCut(const QPoint,QWidget*,NENode*)),
                         S, SLOT(showPropertyShortCutDialog(const QPoint,QWidget*,NENode*)));
    }
}


void FDYNGUIManagerPrivate::setTimelineMinFrame(int val)
{
#if 0
    timeline()->setPlaybackStartFrame(val);
#endif
}

void FDYNGUIManagerPrivate::setCurrentFrame(double val)
{
#if 1
    timeline()->setCurrentFrame(val);
#endif
}

void FDYNGUIManagerPrivate::setTimelineMinSimulated(int start, int end)
{
    //timeline()->timelineWidget->setSimulatedMin(start,end);
}

void FDYNGUIManagerPrivate::setTimelineKeyframeMarks(QList<uint> marks)
{
#if 0
    timeline()->timelineWidget->setKeyframeMarks(marks);
#endif
}

void FDYNGUIManagerPrivate::setTimelineMaxSimulated(int start, int end)
{
    //timeline()->setSimulatedMax(start,end);
}

void FDYNGUIManagerPrivate::prepareForObjectRemoval(NENode* object)
{

    if(S->mainWindow()->sceneInspectorWindow()->isSelectedObject(object))
        S->mainWindow()->sceneInspectorWindow()->setParentCurrentItem();
#if 1
//    if( FDYNPropertyEditorWindow::m_pPrevObject == object)
//    {
//        FDYNPropertyEditorWindow::m_pPrevObject = 0;
//    }
#endif
}

void FDYNGUIManagerPrivate::writeAllGUISubCreations( QXmlStreamWriter *xmlWriter )
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

#if 0
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
#endif
}


void FDYNGUIManager::setTimeLineUnitsToScene(FDYNTimelineUnits unit){
//    NESCENE.setTimeLineUnits((NESceneManager::NETimelineUnits)unit);
    updatePropertyEditor();
}

void FDYNGUIManager::popupDescriptionAndName(const QPoint& pos, QWidget *w, NENode *selSpatial, QPainter *painter)
{
    if(selSpatial)
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0, 0, 0, 127));

        QPainterPath path;
        path.addRoundedRect(pos.x() - 120, pos.y() - 120, 180, 90, 8, 8);

        path.moveTo(pos.x(), pos.y());
        path.lineTo(pos.x() - 112, pos.y() - 30);
        path.lineTo(pos.x() + 52, pos.y() - 30);
        path.closeSubpath();

        painter->drawPath(path);

        QFont font(QString("Segoe UI"));
        font.setPixelSize(12);
        font.setBold(true);
        painter->setFont(font);

        QPen pen(QColor(255, 255, 255, 150));
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);

        NESpatialEngine *engine = qobject_cast<NESpatialEngine *>(selSpatial);
        NEZone *zone = qobject_cast<NEZone *>(selSpatial);
        NEScenario *scn = qobject_cast<NEScenario *>(selSpatial);
        NEZoneItem *zitem = qobject_cast<NEZoneItem *>(selSpatial);
        QString tooltipText;
        if(zone)
            tooltipText = zone->objectName() + "\n" + zone->nodeDescription();
        else if(scn)
            tooltipText = scn->objectName() + "\n" + scn->nodeDescription();
        else if(zitem)
            tooltipText = zitem->objectName() + "\n" + zitem->nodeDescription();
        else if(engine)
            tooltipText = engine->objectName() + "\n" + engine->nodeDescription();

        painter->drawText( QRect(pos.x() - 120, pos.y() - 120, 180, 90), Qt::AlignHCenter | Qt::AlignVCenter, tooltipText);

        QTimer::singleShot(2000, w, SLOT(hideDescription()));
    }
}

void FDYNGUIManager::showZoneSizeTool()
{
    if(NEZoneCompartment *compartment = qobject_cast<NEZoneCompartment *>(NESCENE.selectedObject())){
        //selectSizeMode(true);
        mainWindow()->showPropertyShortCutDlg(NULL, compartment, NEGLWidget::NE3DTool::kSize);
    }
}

void FDYNGUIManager::showZoneLocTool()
{
    if(NEZoneCompartment *compartment = qobject_cast<NEZoneCompartment *>(NESCENE.selectedObject()))
    {
        //selectLocationMode(true);
        mainWindow()->showPropertyShortCutDlg(NULL, compartment, NEGLWidget::NE3DTool::kLocation);
    }
}

void FDYNGUIManager::setSnapToBoundary(bool bSnap)
{
    NEGLWidget::m_SnapToBoundary = bSnap;

    QVariant returnValue;
    QObject* cadToolObj = mainWindow()->CADModelingTool()->rootObject();

    QMetaObject::invokeMethod(cadToolObj, "setBtnSnapToBoundary", Q_RETURN_ARG(QVariant, returnValue), Q_ARG(QVariant, bSnap));


    updateGLWindows();
}


void FDYNGUIManager::setSnapToObj(bool bSnap)
{
    NEGLWidget::m_SnapToObj = bSnap;

    QVariant returnValue;
    QObject* cadToolObj = mainWindow()->CADModelingTool()->rootObject();

    QMetaObject::invokeMethod(cadToolObj, "setBtnSnapToObj", Q_RETURN_ARG(QVariant, returnValue), Q_ARG(QVariant, bSnap));

    updateGLWindows();
}

void FDYNGUIManager::showSnapUnitProperty()
{
    mainWindow()->showPropertyShortCutDlg(mainWidget());
}


void FDYNGUIManager::setSnapUnit(bool bSnapUnit)
{
    NEGLWidget::m_bSnapUnit = bSnapUnit;
    QVariant returnValue;
    QObject* cadToolObj = mainWindow()->CADModelingTool()->rootObject();

    QMetaObject::invokeMethod(cadToolObj, "setBtnSnapToUnit", Q_RETURN_ARG(QVariant, returnValue), Q_ARG(QVariant, bSnapUnit));

    updateGLWindows();
}

void FDYNGUIManager::setMeasure(bool bMeasure)
{
    NEGLWidget::m_Measure = bMeasure;
    if(bMeasure)
        NEGLWidget::m_MeasureEdge = false;
    NEGLWidget::m_SnapModeChanged = true;

    QVariant returnValue;
    QObject* cadToolObj = mainWindow()->CADModelingTool()->rootObject();

    QMetaObject::invokeMethod(cadToolObj, "setBtnMeasure", Q_RETURN_ARG(QVariant, returnValue), Q_ARG(QVariant, bMeasure));

    updateGLWindows();
}


void FDYNGUIManager::setMeasureEdge(bool bMeasureEdge)
{
    NEGLWidget::m_MeasureEdge = bMeasureEdge;
    if(bMeasureEdge)
        NEGLWidget::m_Measure = false;
    NEGLWidget::m_SnapModeChanged = true;

    QVariant returnValue;
    QObject* cadToolObj = mainWindow()->CADModelingTool()->rootObject();

    QMetaObject::invokeMethod(cadToolObj, "setBtnMeasureEdge", Q_RETURN_ARG(QVariant, returnValue), Q_ARG(QVariant, bMeasureEdge));
    updateGLWindows();
}




void FDYNGUIManager::setSnapToEdge(bool bSnap)
{
    NEGLWidget::m_SnapToEdge = bSnap;
    if(NEZoneCompartment *selCompart = qobject_cast<NEZoneCompartment*>(NESCENE.selectedObject()))
    {

        NEZone *zone = NULL;

        QObject *parent = selCompart->parent();
        while( parent ){
            if( NEZone *z = qobject_cast<NEZone *>(parent) )
            {
                zone = z;
                break;
            }
            parent = parent->parent();
        }

        QList<NEZoneCompartment *> compartments = zone->findChildren<NEZoneCompartment *>();
        compartments.removeOne(selCompart);

        NEIndexedTriMesh &selTriMesh = selCompart->triangleMesh();
        std::vector<NEPrimitivePolygon> &selFaces = selCompart->quadFaces();

        float minDistance = FLT_MAX;
        QPair<int, NEZoneCompartment *> selPair;
        QPair<int, NEZoneCompartment *> otherPair;
        for( unsigned i = 0; i < selFaces.size(); i++)
        {
            NEPrimitivePolygon selFace = selFaces[i];
            vec3f selSum(0,0,0);
            int selCount = 0;
            for ( unsigned j = 0; j < selFace.vertices.size(); j++)
            {
                selSum += selCompart->transform() * selTriMesh.vertices()[selFace.vertices[j]].position;
                selCount ++;
            }
            selSum = selSum / selCount;

            foreach (NEZoneCompartment *otherCompart, compartments)
            {
                NEIndexedTriMesh &otherTriMesh = otherCompart->triangleMesh();
                std::vector<NEPrimitivePolygon> &otherFaces = otherCompart->quadFaces();

                for( unsigned oi = 0; oi < otherFaces.size(); oi++ )
                {
                    NEPrimitivePolygon otherFace = otherFaces[oi];

                    vec3f otherSum(0,0,0);
                    int otherCount = 0;
                    for ( unsigned j = 0; j < otherFace.vertices.size(); j++){
                        otherSum += otherCompart->transform() * otherTriMesh.vertices()[otherFace.vertices[j]].position;
                        otherCount ++;
                    }
                    otherSum = otherSum / otherCount;

                    if ((selSum - otherSum).length() < minDistance)
                    {
                        minDistance = (selSum - otherSum).length();
                        selPair.first = i;
                        selPair.second = selCompart;

                        otherPair.first = oi;
                        otherPair.second = otherCompart;
                    }
                }
            }
        }

        vec3f selSum(0,0,0);
        int selCount = 0;
        NEPrimitivePolygon selFace = selFaces[selPair.first];
        for ( unsigned j = 0; j < selFace.vertices.size(); j++)
        {
            selSum += selCompart->transform() * selTriMesh.vertices()[selFace.vertices[j]].position;
            selCount ++;
        }
        selSum = selSum / selCount;

        vec3f otherSum(0,0,0);
        int otherCount = 0;
        NEZoneCompartment *otherCompart = otherPair.second;
        NEPrimitivePolygon otherFace = otherCompart->quadFaces()[otherPair.first];
        NEIndexedTriMesh &otherTriMesh = otherCompart->triangleMesh();

        for ( unsigned j = 0; j < otherFace.vertices.size(); j++)
        {
            otherSum += otherCompart->transform() * otherTriMesh.vertices()[otherFace.vertices[j]].position;
            otherCount ++;
        }
        otherSum = otherSum / otherCount;

    }
    else
    {
        QObject* cadToolObj = mainWindow()->CADModelingTool()->rootObject();
        QVariant returnValue;
        QMetaObject::invokeMethod(cadToolObj, "setBtnSnapToEdge", Q_RETURN_ARG(QVariant, returnValue), Q_ARG(QVariant, false));
    }

    updateGLWindows();
}


void FDYNGUIManager::showPropertyShortCutDialog(const QPoint& pos, QWidget *w, NENode *selSpatial)
{
    if(selSpatial && NEGLWidget::m_ActiveManipulator != NEManipulator::kManipNone)
    {
        mainWindow()->showPropertyShortCutDlg(w, selSpatial, NEGLWidget::m_ActiveManipulator);
    }
}

void FDYNGUIManager::selectObject(QObject* object)
{
    if (isDuringSelectionProcess())
        return;

    if (NESCENE.selectedObject() == nullptr && object == nullptr)
        return;

    if(object && !qobject_cast<NEObject*>(object))
        return;

    SetSelectionProcess(true);
    NESCENE.selectObject(qobject_cast<NENode*>(object));
    if(NENode *nd = qobject_cast<NENode*>(object))
        NESCENE.selectAdditionalObject(nd);

    mainWindow()->sceneInspectorWindow()->setNewCurrentItem(object);
    mainWindow()->zoneInspectorWindow()->setNewCurrentItem(object);
    mainWindow()->scenarioInspectorWindow()->setNewCurrentItem(object);
    mainWindow()->propertyEditorWindow()->showObjectProperty(object);
#if OLD_PROPERTY
    hideObjectFromOtherPropEditors(object, mainWindow()->propertyEditorWindow());
#endif

    if(NESpatial* sp = qobject_cast<NESpatial*>(object))
        P->setTimelineMinSimulated(sp->FrameInfo().firstFrame(), sp->FrameInfo().lastFrame());
    else
        P->setTimelineMinSimulated(0, 0);
    updateGLWindows();
    SetSelectionProcess(false);

    if(object && mainWindow()->zoneInspectorWindow()->zoneInspector()->m_currentItem)
    {
        FDYNTreeItem *item = mainWindow()->zoneInspectorWindow()->zoneInspector()->m_currentItem->parent();
        while(object && item)
        {
            mainWindow()->zoneInspectorWindow()->zoneInspector()->expandItem( item);
            item = item->parent();
        }
    }

    if(object && mainWindow()->scenarioInspectorWindow()->scenarioInspector()->m_currentItem)
    {
        FDYNTreeItem *item = mainWindow()->scenarioInspectorWindow()->scenarioInspector()->m_currentItem->parent();
        while(object && item)
        {
            mainWindow()->scenarioInspectorWindow()->scenarioInspector()->expandItem( item);
            item = item->parent();
        }
    }

    setPropertyVisibleAsActiveDock();
}

void FDYNGUIManager::setPropertyVisibleAsActiveDock()
{
    if( NESCENE.selectedObject() == NULL || NESCENE.selectedObjects()->size() <= 0)
        return;
    if(mainWindow()->topDockWidget() == mainWindow()->scenarioInspectorDock()){
        foreach (NEObject *nobj, NESCENE.selectedObjects()->values())
        {
            nobj->hideProperty("FailedTime", false);
        }
    }
    else {
        foreach (NEObject *nobj, NESCENE.selectedObjects()->values())
        {
            nobj->hideProperty("FailedTime", true);
        }
    }
    updatePropertyEditor();
}

void FDYNGUIManager::selectGUIObject(QObject* object)
{
    //mainWindow()->systemInspectorWindow()->setNewCurrentItem(object);
    P->m_pMainWindow->propertyEditorWindow()->showObjectProperty(object);
}

void FDYNGUIManager::selectObjects(QList<NENode*>& objects)
{
    if (isDuringSelectionProcess())
        return;
    if(objects.size() == 0)
        return;
    SetSelectionProcess(true);
    NESCENE.selectedObjects()->clear();
    foreach(NENode* np, objects)
    {
        if(np)
            NESCENE.selectAdditionalObject(np);
    }
    mainWindow()->sceneInspectorWindow()->setCurrentItems(objects);
    mainWindow()->zoneInspectorWindow()->setCurrentItems(objects);
    mainWindow()->scenarioInspectorWindow()->setCurrentItems(objects);

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

void FDYNGUIManagerPrivate::addCameraIntoContextMenus(NECamera* pCamera)
{
    foreach(const GLDockWidgetBundle* w, m_IDtoGLDockWidget)
        w->mainWindow->addCamera(pCamera);
}

void FDYNGUIManagerPrivate::removeCameraFromContextMenus(NECamera* pCamera)
{
    foreach(const GLDockWidgetBundle* w, m_IDtoGLDockWidget)
        w->mainWindow->removeCamera(pCamera);
}

void FDYNGUIManagerPrivate::createGLWindowOnLoad(QString name)
{
    GLDockWidgetBundle* bundle = createGLWidget(name);
    S->mainWindow()->splitDockWidget(m_GLtoGLDockWidget[m_pMainGLWidget]->dockWidget, bundle->dockWidget, Qt::Horizontal);
    m_GLtoGLDockWidget[bundle->glWidget]->mainWindow->addCameras( NERENDERMAN.cameras() );
}

GLDockWidgetBundle* FDYNGUIManagerPrivate::createGLWidget(QString name)
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
    wBundle->dockWidget = new FDYNDockWidget(S->mainWindow());
    // Create a GL main window
    wBundle->mainWindow = new FDYNGLMainWindow();
    wBundle->mainWindow->m_pBundle = wBundle;
    //wBundle->mainWindow->menuBar()->hide();
    wBundle->dockWidget->setContainedMainWindow(wBundle->mainWindow);

    QQuickWidget *qWidget = new QQuickWidget(wBundle->mainWindow);
    qWidget->setSource(QUrl("qrc:/FOpenGL.qml"));

    wBundle->mainWindow->setCentralWidget(qWidget);
    //wBundle->mainWindow->centralWidget()->resize(1400, 600);
    // Set object and window name for the dock widget
    //wBundle->dockWidget->setWindowTitle(name);
    wBundle->dockWidget->setTitleText(name);
    wBundle->dockWidget->setObjectName(name);

    QApplication::processEvents();

    // Create the GL Widget
    wBundle->glWidget = new NEGLWidget(qWidget);

    wBundle->glWidget->setObjectName(name);
    wBundle->glWidget->m_pBundle = wBundle;
    wBundle->glWidget->P()->setObjectName(name);
    wBundle->glWidget->installEventFilter( reinterpret_cast< QMainWindow* >( wBundle->mainWindow) );

    //for only fdyn
    //QQuickWidget *tmp = qobject_cast<QQuickWidget *>(wBundle->mainWindow->centralWidget());
    // Create a layout for the gl widget
    QHBoxLayout* layout = new QHBoxLayout(wBundle->mainWindow->centralWidget());
    layout->setObjectName("CADLayout");
    layout->setContentsMargins(0, 35, 0, 0);
    // Insert the gl widget into the created layout
    layout->addWidget(wBundle->glWidget);

    QObject *dropArea = qWidget->rootObject()->findChild<QObject *>("dropAreaOpenGL");
    QObject::connect(dropArea, SIGNAL(sigDropped(QVariant, QVariant, QVariant)), wBundle->glWidget, SLOT(slotDropped(QVariant, QVariant, QVariant)));

    // Connect actions of the Mainwindow of the gl widget to the appropriate slots
    wBundle->mainWindow->setupConnections();
    wBundle->dockWidget->setWidget(wBundle->mainWindow);
    m_IDtoGLDockWidget.insert(m_IDtoGLDockWidget.size(), wBundle);
    m_GLtoGLDockWidget.insert(wBundle->glWidget, wBundle);
    // Set the properties of the widget as the child of the GUI manager
    wBundle->glWidget->P()->setParent(S);
    getPyMain().addObject(wBundle->glWidget->P()->objectName(), wBundle->glWidget->P());

    S->mainWindow()->connectGLDockWidgetToAtcion(wBundle->dockWidget);

    return wBundle;
}

void FDYNGUIManagerPrivate::updatePropertyEditorsAtPostInit()
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

    foreach(FDYNPropertyEditorWindow* p, m_PropEdWins)
    {
        p->showObjectProperty(objs.first());
        objs.pop_front();

        if(objs.isEmpty())
            break;
    }
}

void FDYNGUIManagerPrivate::closeGLWidget(NEGLWidget* val)
{
    GLDockWidgetBundle* dock = m_GLtoGLDockWidget[val];
    if(!dock)
        return;
    // Don't delete the main OpenGL widget, just hide it
    if(val == m_pMainGLWidget)
    {
        hideGLWidget(val);
        return;
    }

    val->setCamera(0);
    S->mainWindow()->propertyEditorWindow()->removeObject(val->P());
    m_IDtoGLDockWidget.remove(dock->id);
    m_GLtoGLDockWidget.remove(val);
    getPyMain().removeVariable(val->objectName());
    S->mainWindow()->removeDockWidget(dock->dockWidget);
    dock->dockWidget->hide();
    //dock->dockWidget->close();
    dock->glWidget->P()->setParent(0);
    dock->dockWidget->setParent(0);
    //delete dock;
    S->selectGUIObject(0);
    S->updateSystemInspector();
    val->releaseBuffer();
    delete dock;
}

void FDYNGUIManagerPrivate::hideGLWidget(NEGLWidget* val)
{
    GLDockWidgetBundle* dock = m_GLtoGLDockWidget[val];
    dock->dockWidget->hide();
}

void FDYNGUIManagerPrivate::showMainGLWidget()
{
    GLDockWidgetBundle* dock = m_GLtoGLDockWidget[m_pMainGLWidget];
    dock->dockWidget->show();
    m_pMainGLWidget->setCamera(NERENDERMAN.defaultCamera());
}

void FDYNGUIManagerPrivate::setCameraForGLDock(GLDockWidgetBundle* bundle, NECamera* cam)
{
    bundle->mainWindow->setCamera(cam);
}

void FDYNGUIManagerPrivate::closePropertyEditor(FDYNPropertyEditorWindow *w)
{
    if(w == S->mainWindow()->propertyEditorWindow())
    {
        w->parentWidget()->hide();
        return;
    }

    QList<FDYNPropertyEditorWindow*>::iterator i = m_PropEdWins.begin();
    for(; i != m_PropEdWins.end(); ++i)
        if(*i == w)
            break;

    m_PropEdWins.erase(i);

    FDYNDockWidget* dw = static_cast<FDYNDockWidget*>(w->parentWidget());
    S->mainWindow()->removeDockWidget(dw);
    dw->close();
}

void FDYNGUIManager::closeAllPropertyEditors()
{
#if 1
    //mainWindow()->propertyEditorWindow()->parentWidget()->hide();

    P->m_PropEdWins.clear();
    P->m_PropEdObjs.clear();

    FDYNObjectController::clearPropertiesMap();
#endif
    return;
}

void FDYNGUIManager::closeAllGLDocks()
{
    P->closeAllGLDocks();
}

void FDYNGUIManagerPrivate::enableSimulationWidgets(bool b)
{
#if 1
    timeline()->enableSimulationUI(b);
#endif
}

void FDYNGUIManagerPrivate::enablePlaybackWidgets(bool b)
{
#if 1
    timeline()->enablePlaybackUI(b);
#endif
}

void FDYNGUIManagerPrivate::setSimulateButton(bool val)
{
#if 0
    timeline()->repeatButton->blockSignals(true);
    timeline()->simulateButton->setChecked(val);
    timeline()->repeatButton->blockSignals(false);
#endif
    QObject *btnSim = timeline()->rootObject()->findChild<QObject*>("btnSim");
    btnSim->setProperty("checked", val);
}

void FDYNGUIManagerPrivate::setRepeatButton(bool val)
{
#if 0
    timeline()->repeatButton->blockSignals(true);
    timeline()->repeatButton->setChecked(val);
    timeline()->repeatButton->blockSignals(false);
#endif
}

bool FDYNGUIManagerPrivate::simulateButtonState()
{
#if 0
    return timeline()->simulateButton->isChecked();
#endif
    QObject *btnSim = timeline()->rootObject()->findChild<QObject*>("btnSim");
    return btnSim->property("checked").value<bool>();
}

void FDYNGUIManagerPrivate::setPlayButton(bool val)
{
#if 0
    timeline()->repeatButton->blockSignals(true);
    timeline()->playButton->setChecked(val);
    timeline()->repeatButton->blockSignals(false);
#endif
    QObject *btnPlay = timeline()->rootObject()->findChild<QObject*>("btnPlay");
    btnPlay->setProperty("checked", val);
}

bool FDYNGUIManagerPrivate::playButtonState()
{
#if 0
    return timeline()->playButton->isChecked();
#endif
    QObject *btnPlay = timeline()->rootObject()->findChild<QObject*>("btnPlay");
    return btnPlay->property("checked").value<bool>();

}

void FDYNGUIManagerPrivate::repaintTimeline()
{
#if 1
    if( timeline() )
        timeline()->repaint();
#endif
}

// int FDYNGUIManagerPrivate::playbackStartFrameOrTime()
// {
//     return timeline()->playbackStartSpinBox->value();
// }

void FDYNGUIManagerPrivate::setPlaybackStartFrameOrTime(double val)
{
#if 0
    if(timeline()->playbackStartSpinBox->value() != val)
        timeline()->playbackStartSpinBox->setValue(val);
#endif
    QObject *spinStartTime = timeline()->rootObject()->findChild<QObject*>("spinStartTime");
    if(spinStartTime->property("value") != val)
        spinStartTime->setProperty("value", val);
}

// int FDYNGUIManagerPrivate::playbackEndFrameOrTime()
// {
//     return timeline()->playbackEndSpinBox->value();
// }

void FDYNGUIManagerPrivate::setPlaybackEndFrameOrTime(double val)
{
#if 0
    if( timeline()->playbackEndSpinBox->value() != val)
        timeline()->playbackEndSpinBox->setValue(val);
#endif
    QObject *spinLastTime = timeline()->rootObject()->findChild<QObject*>("spinLastTime");
    if(spinLastTime->property("value") != val)
        spinLastTime->setProperty("value", val);
}

// int FDYNGUIManagerPrivate::simulationEndFrameOrTime()
// {
//     return timeline()->maxFramesSpinBox->value();
// }

void FDYNGUIManagerPrivate::setSimulationEndFrameOrTime(double val)
{
#if 0
    if(timeline()->maxFramesSpinBox->value() != val)
        timeline()->maxFramesSpinBox->setValue(val);
#endif
    QObject *spinLastTime = timeline()->rootObject()->findChild<QObject*>("spinLastTime");
    if(spinLastTime->property("value") != val)
        spinLastTime->setProperty("value", val);
}

void FDYNGUIManager::updateAllConnectionTables()
{
}

void FDYNGUIManagerPrivate::closeAllGLDocks()
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

void FDYNGUIManager::resetWidgets()
{
#if 0
    P->timeline()->simulateButton->setChecked(false);
#endif
    QObject *btnSim = P->timeline()->rootObject()->findChild<QObject*>("btnSim");
    btnSim->setProperty("checked", false);
}

void FDYNGUIManager::undo()
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
    updateInspectors();
    updateSystemInspector();
    updateGLWindows();
    updatePropertyEditor();

    NEPopAttrib(NE_UNDO_REDO_STATE);
}

void FDYNGUIManager::redo()
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
    updateInspectors();
    updateSystemInspector();
    updateGLWindows();
    updatePropertyEditor();
    if (undoevent->type == kUserEventObjectCreation ||
            undoevent->type == kUserEventObjectDeletion ||
            undoevent->type == kUserEventNodeConnection ||
            undoevent->type == kUserEventNodeDisconnection)
    NEPopAttrib(NE_UNDO_REDO_STATE);
}

void FDYNGUIManager::showNodePropertyExpression(const NENode* val, const QString P)
{
    // Remove the previous object
    getPyMain().removeVariable("this");
    // Add the object
    getPyMain().addObject("this", static_cast<QObject*>(const_cast<NENode*>(val)));
}

bool FDYNGUIManager::OpenGLUpdate() const
{
    return P->m_OpenGLUpdate;
}

void FDYNGUIManager::showHelp()
{
#if 0
    NEHELP.show();
#endif
}

void FDYNGUIManagerPrivate::updateWidgetsForDeletion(QObject* object)
{
    S->mainWindow()->propertyEditorWindow()->removeObject(object);
}

void FDYNGUIManagerPrivate::updateWidgetsForCreation()
{

}

bool FDYNGUIManagerPrivate::printGLInfo()
{
    int temp;
    printf("FRI3D: Info - OpenGL version: %s by %s\n", GL->glGetString(GL_VERSION), GL->glGetString(GL_VENDOR));
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

void FDYNGUIManagerPrivate::saveGUIStateIntoScene(QXmlStreamWriter* writer)
{
    //std::cout << "Write GUI State into scene" << std::endl;
    writer->writeStartElement("PropertyEditor");
    FDYNObjectController::saveUIStates(writer);
    writer->writeEndElement();

    S->mainWindow()->sceneInspectorWindow()->writeGUIState(writer);
}

void FDYNGUIManagerPrivate::restoreGUIStateFromScene(QXmlStreamReader* reader)
{
    while(reader->readNextStartElement())
    {
        if(reader->name() == "PropertyEditor")
        {
            FDYNObjectController::loadUIStates(reader);
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

void FDYNGUIManager::setOpenGLUpdate(bool val)
{
    P->m_OpenGLUpdate = val;
    if(P->m_OpenGLUpdate)
        updateGLWindows();
}

void FDYNGUIManagerPrivate::saveGUIState()
{
    QFile file(m_GUIStatePath);
    if(!file.open(QFile::WriteOnly | QFile::Truncate))
        return;
    QDataStream out(&file);
    out << S->mainWindow()->saveState();
    file.close();
    //qDebug() << "Program state saved to: "<< m_GUIStatePath;
}

void FDYNGUIManagerPrivate::loadGUIState()
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

void FDYNGUIManagerPrivate::saveMainWindowGeometry()
{
    QSettings settings("Fire Dynamics", "FRI3D");
    settings.setValue("geometry", S->mainWindow()->saveGeometry());
}

void FDYNGUIManagerPrivate::loadMainWindowGeometry()
{
    QSettings settings("Fire Dynamics", "FRI3D");
    S->mainWindow()->restoreGeometry(settings.value("geometry").toByteArray());
}

void FDYNGUIManager::selectCursorMode(bool val)
{
    if(!val)
        return;
    //Log() << "Cursor mode";
    EndLog(NE::kLogInfo);
    NEGLWidget::m_ActiveManipulator = NEManipulator::kManipNone;
    updateGLWindows();
}

void FDYNGUIManager::selectTranslationMode(bool val)
{
    if(!val)
        return;
    EndLog(NE::kLogInfo);
    NEGLWidget::m_ActiveManipulator = NEManipulator::kManipTranslate;
    updateGLWindows();
}

void FDYNGUIManager::selectRotationMode(bool val)
{
    if(!val)
        return;
    EndLog(NE::kLogInfo);
    NEGLWidget::m_ActiveManipulator = NEManipulator::kManipRotate;
    updateGLWindows();
}

void FDYNGUIManager::selectScaleMode(bool val)
{
    if(!val)
        return;
    EndLog(NE::kLogInfo);
    NEGLWidget::m_ActiveManipulator = NEManipulator::kManipScale;
    updateGLWindows();
}

void FDYNGUIManager::selectSizeMode(bool val)
{
    if(!val)
        return;

    foreach(GLDockWidgetBundle* b, FDYNGUI.P->m_IDtoGLDockWidget)
    {
        b->mainWindow->selectCursorMode(false);
    }

    EndLog(NE::kLogInfo);
    NEGLWidget::m_ActiveManipulator = NEGLWidget::kSize;
    updateGLWindows();
}

void FDYNGUIManager::selectLocationMode(bool val)
{
    if(!val)
        return;

    foreach(GLDockWidgetBundle* b, FDYNGUI.P->m_IDtoGLDockWidget)
    {
        b->mainWindow->selectCursorMode(false);
    }
    EndLog(NE::kLogInfo);
    NEGLWidget::m_ActiveManipulator = NEGLWidget::kLocation;
    updateGLWindows();
}

void FDYNGUIManager::setSelectedVisibile(bool val)
{
    if(!NESCENE.selectedNode())
        return;
    if(NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode()))
    {
        spatial->SetRenderMode(NESpatial::SmoothShaded);
        updateInspectors();
        updatePropertyEditor();
    }
}

void FDYNGUIManager::setSelectedWireframe()
{
    if(!NESCENE.selectedNode())
        return;
    if(NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode()))
    {
        spatial->SetRenderMode(NESpatial::Wireframe);
        updateInspectors();
        updatePropertyEditor();
    }
}

void FDYNGUIManager::playButtonToggled(bool val)
{
#if 0
    P->timeline()->play(val);
#endif
}

void FDYNGUIManager::setSelectedFlatShaded(bool val)
{
    if(!NESCENE.selectedNode())
        return;
    if(NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode()))
    {
        spatial->SetRenderMode(NESpatial::FlatShaded);
        updateInspectors();
        updatePropertyEditor();
    }
}

void FDYNGUIManager::setSelectedSmoothShaded()
{
    if(!NESCENE.selectedNode())
        return;
    if(NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode()))
    {
        spatial->SetRenderMode(NESpatial::SmoothShaded);
        updateInspectors();
        updatePropertyEditor();
    }
}

void FDYNGUIManager::setSelectedGroup()
{
    QList<NENode*> tmp;
    tmp.append(NESCENE.selectedNode());
    foreach(NEObject* obj, *NESCENE.selectedObjects())
        if(NENode* node = qobject_cast<NENode*>(obj))
            tmp.append(node);
    NESCENE.groupObjects(tmp);
}

void FDYNGUIManager::setSelectedUngroup()
{
    if(NESpatialGroup* group = qobject_cast<NESpatialGroup*>(NESCENE.selectedNode()))
    {
        if(group->children().size() > 0){
            NESCENE.ungroupNode(group);
            mainWindow()->removeSelectedItem();
            //mainWindow()->sceneInspectorWindow()->removeSelectedItem();
        }
    }
}

///
/// \brief FDYNGUIManager::deleteScenario
///

void FDYNGUIManager::deleteScenario()
{
    if (NEScenario* scn = qobject_cast<NEScenario*>(NESCENE.selectedNode()))
    {
        P->m_Scenarios.removeAll(scn->objectName());
        mainWindow()->scenarioInspectorWindow()->scenarioInspector()->updateAllScenarios(P->m_Scenarios);

        emit sendScenarioDeleted(scn);

        updateInspectors();
    }
}

///
/// \brief FDYNGUIManager::addScenarioToSelected
///

void FDYNGUIManager::addScenarioToSelected()
{
    if (NESpatialEngine* parent = qobject_cast<NESpatialEngine*>(NESCENE.selectedNode()))
    {
        QString nextName = NESCENE.geometryManager().get()->nextObjectName("Fire Scenario", "");
        while (P->m_Scenarios.contains(nextName))
            nextName = NESCENE.geometryManager().get()->nextObjectName("Fire Scenario", "");

        NEScenario *scn = qobject_cast<NEScenario*>(NESCENE.createObject("Fire Scenario", nextName, NESCENE.selectedNode()));

        if (scn != nullptr)
        {
            P->m_Scenarios.push_back(nextName);
            mainWindow()->scenarioInspectorWindow()->scenarioInspector()->updateAllScenarios(P->m_Scenarios);

            emit sendScenarioCreated(scn);
        }
    }
}

///
/// \brief FDYNGUIManager::addZoneToSelected
///

void FDYNGUIManager::addZoneToSelected()
{
    if (NESpatialEngine* parent = qobject_cast<NESpatialEngine*>(NESCENE.selectedNode()))
    {
        QString nextName = NESCENE.geometryManager().get()->nextObjectName("Fire Zone", "");
        while (P->m_Zones.contains(nextName))
            nextName = NESCENE.geometryManager().get()->nextObjectName("Fire Zone", "");

        NEZone *zone = qobject_cast<NEZone*>(NESCENE.createObject("Fire Zone", nextName, NESCENE.selectedNode()));

        if (zone != nullptr)
        {
            P->m_Zones.push_back(nextName);
            mainWindow()->zoneInspectorWindow()->zoneInspector()->updateAllZones(P->m_Zones);
//            emit sendZoneCreated(zone);
        }
    }
}

///
/// \brief FDYNGUIManager::addComponent
///

void FDYNGUIManager::addComponent()
{
    NEZone *parent = qobject_cast<NEZone*>(NESCENE.selectedNode());
    if( parent == nullptr )
    {
        NESpatialEngine *fEngine = qobject_cast<NESpatialEngine *>(mainWindow()->zoneInspectorWindow()->root());
        if(!fEngine)
            return ;
        QString zoneName = fEngine->activeZoneName();
        parent = qobject_cast<NEZone *>(NESCENE.findChildNode(zoneName));
    }

    if( !parent )
    {
        return;
    }

    NEZoneItem *item = qobject_cast< NEZoneItem *>( NESCENE.createObject("Fire Plant Item",
                                        NESCENE.geometryManager().get()->nextObjectName("Fire Plant Item", ""),
                                        parent ) );

    if( item )
    {
        item->setParent( parent );
        emit componentCreated( item, parent );
        updateZoneInspector();
        //addItemToZoneTree( item );
        selectObject( item );
        mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();
        selectObject( nullptr );
    }
}

///
/// \brief FDYNGUIManager::addItemToSelected
///

void FDYNGUIManager::addItemToSelected()
{
    NEZone *parent = qobject_cast<NEZone*>(NESCENE.selectedNode());
    if( parent == nullptr )
    {
        NESpatialEngine *fEngine = qobject_cast<NESpatialEngine *>(mainWindow()->zoneInspectorWindow()->root());
        if(!fEngine)
            return ;
        QString zoneName = fEngine->activeZoneName();
        parent = qobject_cast<NEZone *>(NESCENE.findChildNode(zoneName));
    }

    if( !parent )
    {
        return;
    }


    NEZoneItem *item = qobject_cast< NEZoneItem *>( NESCENE.createObject("Fire Plant Item",
                                        NESCENE.geometryManager().get()->nextObjectName("Fire Plant Item", ""),
                                        parent ) );

    if( item )
    {
        item->setParent( parent );
        emit itemCreated( item, parent );
        updateZoneInspector();
        //addItemToZoneTree(item);
        selectObject(item);
        mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();
    }
}

///
/// \brief FDYNGUIManager::duplicateObject
///

void FDYNGUIManager::duplicateObject()
{
    NEZoneItem* zi = qobject_cast< NEZoneItem *>( NESCENE.selectedObject() );
    if( zi )
    {
        emit sendDuplicateObject( zi );
    }

    updateZoneInspector();
    updateGLWindows();
}

///
/// \brief FDYNGUIManager::copyProperties
///

void FDYNGUIManager::copyProperties()
{
    NEZoneItem* zi = qobject_cast< NEZoneItem *>( NESCENE.selectedObject() );
    if( zi )
        emit sendCopyProperties( zi );
}

///
/// \brief FDYNGUIManager::pasteProperties
///

void FDYNGUIManager::pasteProperties()
{
    QList<NEZoneItem *> selectedList;

    foreach (NEObject *obj, *NESCENE.selectedObjects())
    {
        if(NEZoneItem *zitem = qobject_cast<NEZoneItem*>(obj))
        {
            if(selectedList.indexOf(zitem) == -1)
                selectedList.push_back(zitem);
        }
    }
    if( selectedList.size() > 0 )
    {
        emit sendPasteProperties( selectedList, "" );
        emit sendPasteProperties( selectedList, "" );

    }

    //updateSceneInspector();
    updateGLWindows();
}

///
/// \brief FDYNGUIManager::pasteDataProperties
///

void FDYNGUIManager::pasteDataProperties()
{
    QList<NEZoneItem *> selectedList;

    foreach (NEObject *obj, *NESCENE.selectedObjects())
    {
        if(NEZoneItem *zitem = qobject_cast<NEZoneItem*>(obj))
        {
            if(selectedList.indexOf(zitem) == -1)
                selectedList.push_back(zitem);
        }
    }
    if( selectedList.size() > 0 )
        emit sendPasteProperties( selectedList, "FVent|FMechVent|FFire|FZoneItem|NEZoneItem" );

    //updateSceneInspector();
    updateGLWindows();
}

///
/// \brief FDYNGUIManager::paste3DProperties
///

void FDYNGUIManager::paste3DProperties()
{
    QList<NEZoneItem *> selectedList;

    foreach (NEObject *obj, *NESCENE.selectedObjects())
    {
        if(NEZoneItem *zitem = qobject_cast<NEZoneItem*>(obj))
        {
            if(selectedList.indexOf(zitem) == -1)
                selectedList.push_back(zitem);
        }
    }
    if( selectedList.size() > 0 )
        emit sendPasteProperties( selectedList, "NESpatialMesh|NESpatial|NESpatialGroup|NENode|NEObject" );

    //updateSceneInspector();
    updateGLWindows();
}

///
/// \brief FDYNGUIManager::addItemToSelected
///

void FDYNGUIManager::addCableToSelected()
{
    NEZoneItem *item = qobject_cast< NEZoneItem *>( NESCENE.createObject("Cables",
                                        NESCENE.geometryManager().get()->nextObjectName("Cables", ""),
                                        NESCENE.selectedNode() ) );
    if( item )
    {
        item->setParent( NESCENE.selectedNode() );
        emit cableCreated( item, NESCENE.selectedNode() );
        updateZoneInspector();
        //addItemToZoneTree(item);
        selectObject(item);
        mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();
    }
}

///
/// \brief FDYNGUIManager::addItemToSelected
///

void FDYNGUIManager::addTrayToSelected()
{
    NEZone *parent = qobject_cast<NEZone*>(NESCENE.selectedNode());
    if( parent == nullptr )
    {
        NESpatialEngine *fEngine = qobject_cast<NESpatialEngine *>(mainWindow()->zoneInspectorWindow()->root());
        if(!fEngine)
            return ;
        QString zoneName = fEngine->activeZoneName();
        parent = qobject_cast<NEZone *>(NESCENE.findChildNode(zoneName));
    }

    if( !parent )
    {
        return;
    }
    NEZoneItem *item = qobject_cast< NEZoneItem *>( NESCENE.createObject("Fire Plant Item",
                                        NESCENE.geometryManager().get()->nextObjectName("Fire Plant Item", ""),
                                        parent ) );
    if( item )
    {
        item->setParent( parent );
        emit trayCreated( item, parent );
        updateZoneInspector();
        //addItemToZoneTree(item);
        selectObject(item);
        mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();
    }
}

///
/// \brief FDYNGUIManager::renameNode
/// \param node
/// \param newName
///

void FDYNGUIManager::renameNode(NENode* node, QString newName)
{
    if (NEZone *zone = qobject_cast<NEZone*>(node))
    {
        int index = P->m_Zones.indexOf(zone->objectName());
        if (index != -1)
            P->m_Zones.replace(index, newName);
        mainWindow()->zoneInspectorWindow()->zoneInspector()->updateAllZones(P->m_Zones);
    }
    else if (NEScenario *scn = qobject_cast<NEScenario*>(node))
    {
        int index = P->m_Scenarios.indexOf(scn->objectName());
        if (index != -1)
            P->m_Scenarios.replace(index, newName);
        mainWindow()->scenarioInspectorWindow()->scenarioInspector()->updateAllScenarios(P->m_Scenarios);
    }

    emit sendNodeRenamed(node, newName);
}

void FDYNGUIManager::addZoneAndScenario()
{
    QString nextZoneName = NESCENE.geometryManager().get()->nextObjectName("Fire Zone", "");
    while (P->m_Zones.contains(nextZoneName))
        nextZoneName = NESCENE.geometryManager().get()->nextObjectName("Fire Zone", "");

    QString nextScnName = NESCENE.geometryManager().get()->nextObjectName("Fire Scenario", "");
    while (P->m_Scenarios.contains(nextScnName))
        nextScnName = NESCENE.geometryManager().get()->nextObjectName("Fire Scenario", "");

    NEZone *zone = qobject_cast<NEZone*>(NESCENE.createObject("Fire Zone", nextZoneName, NESCENE.selectedNode()));
    NEScenario *scn = qobject_cast<NEScenario*>(NESCENE.createObject("Fire Scenario", nextScnName, NESCENE.selectedNode()));

    if (zone && scn)
    {
        P->m_Zones.push_back(nextZoneName);
        P->m_Scenarios.push_back(nextScnName);
        mainWindow()->zoneInspectorWindow()->zoneInspector()->updateAllZones(P->m_Zones);
        mainWindow()->scenarioInspectorWindow()->scenarioInspector()->updateAllScenarios(P->m_Scenarios);

        zone->setParent(NESCENE.selectedNode());
        scn->setParent(NESCENE.selectedNode());
        emit sendAddNewZoneAndScenario(zone, scn);
        selectObject(zone);
        mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();
    }
}

void FDYNGUIManager::addScenario()
{
    QString nextName = NESCENE.geometryManager().get()->nextObjectName("Fire Scenario", "");
    while (P->m_Scenarios.contains(nextName))
        nextName = NESCENE.geometryManager().get()->nextObjectName("Fire Scenario", "");

    NEScenario *scn = qobject_cast<NEScenario *>(NESCENE.createObject("Fire Scenario", nextName, NESCENE.selectedNode()));

    if (scn)
    {
        P->m_Scenarios.push_back(nextName);
        mainWindow()->scenarioInspectorWindow()->scenarioInspector()->updateAllScenarios(P->m_Scenarios);

        scn->setParent(NESCENE.selectedNode());
        emit sendAddNewScenario(scn);
        selectObject(scn);
    }
}

void FDYNGUIManager::addFire()
{
    NEZone *parent = qobject_cast<NEZone*>(NESCENE.selectedNode());
    if( parent == nullptr )
    {
        NESpatialEngine *fEngine = qobject_cast<NESpatialEngine *>(mainWindow()->zoneInspectorWindow()->root());
        if(!fEngine)
            return;
        QString zoneName = fEngine->activeZoneName();
        parent = qobject_cast<NEZone *>(NESCENE.findChildNode(zoneName));
    }
    if(parent)
    {
        NENode *node = NESCENE.createObject( "Fire Definition",
                                             NESCENE.geometryManager().get()->nextObjectName("Fire Definition", ""),
                                             NESCENE.selectedNode() );
        if( node )
        {
            node->setParent( parent );
            emit sendAddFireDesc( node );
            updateZoneInspector();
            //addItemToZoneTree(node);
            selectObject(node);
            mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();
        }
    }
}

void FDYNGUIManager::addVent()
{
    NEZone *parent = qobject_cast<NEZone*>(NESCENE.selectedNode());
    if( parent == nullptr )
    {
        NESpatialEngine *fEngine = qobject_cast<NESpatialEngine *>(mainWindow()->zoneInspectorWindow()->root());
        if(!fEngine)
            return;
        QString zoneName = fEngine->activeZoneName();
        parent = qobject_cast<NEZone *>(NESCENE.findChildNode(zoneName));
    }

    if(NEZone *parent = qobject_cast<NEZone*>(NESCENE.selectedNode()))
    {
        NENode *node = NESCENE.createObject( "Fire Vent",
                                             NESCENE.geometryManager().get()->nextObjectName("Fire Vent", ""),
                                             NESCENE.selectedNode() );
        if( node )
        {
            node->setParent( NESCENE.selectedNode() );
            emit sendAddVent( node );
            //updateZoneInspector();
            //addItemToZoneTree(node);
            selectObject(node);
            mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();
        }
    }
}

void FDYNGUIManager::addMechVent()
{
    if(NEZone *parent = qobject_cast<NEZone*>(NESCENE.selectedNode()))
    {
        NENode *node = NESCENE.createObject( "Fire Mech Vent",
                                             NESCENE.geometryManager().get()->nextObjectName("Fire Mech Vent", ""),
                                             NESCENE.selectedNode() );
        if( node )
        {
            node->setParent( NESCENE.selectedNode() );
            emit sendAddVent( node );
            updateZoneInspector();
            //addItemToZoneTree(node);
            selectObject(node);
            mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();
        }
    }
}

void FDYNGUIManager::turnIntoVent()
{
    if(NEZoneItem* item = qobject_cast<NEZoneItem*>(NESCENE.selectedNode()))
    {
        if (NENode* parentNode = qobject_cast<NENode*>(item->parent()))
        {
            NENode *ventNode = NESCENE.createObject("Fire Vent", NESCENE.geometryManager().get()->nextObjectName("Fire Vent", ""), parentNode);
            if (ventNode != nullptr)
            {
                ventNode->setParent(parentNode);

                emit sendTurnIntoVent(item, ventNode);

                mainWindow()->sceneInspectorWindow()->removeSelectedItem();
                selectObject(ventNode);
                mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();

                NESCENE.deleteObject(item, true);

                updateInspectors();
                updateGLWindows();
            }
        }
    }
}

void FDYNGUIManager::turnIntoMechVent()
{
    if(NEZoneItem* item = qobject_cast<NEZoneItem*>(NESCENE.selectedNode()))
    {
        if (NENode* parentNode = qobject_cast<NENode*>(item->parent()))
        {
            NENode *mechVentNode = NESCENE.createObject("Fire Mech Vent", NESCENE.geometryManager().get()->nextObjectName("Fire Mech Vent", ""), parentNode);
            if (mechVentNode != nullptr)
            {
                mechVentNode->setParent(parentNode);

                emit sendTurnIntoMechVent(item, mechVentNode);

                mainWindow()->sceneInspectorWindow()->removeSelectedItem();
                selectObject(mechVentNode);
                mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();

                NESCENE.deleteObject(item, true);

                updateInspectors();
                updateGLWindows();
            }
        }
    }
}

void FDYNGUIManager::loadDefaultSurfaceMaterials()
{
    emit sendLoadDefaultSurfaceMaterials();
}

void FDYNGUIManager::addSurfaceMaterial(SurfaceMaterial s)
{
    NESurfaceMaterial sm;
    sm.setName( s.m_Name );
    sm.setUniqueID( s.m_ID );
    sm.setIndex( s.m_Index );
    sm.setConductivity( s.m_Conductivity );
    sm.setDensity( s.m_Density );
    sm.setHeat( s.m_Heat );
    sm.setThickness( s.m_Thickness );
    sm.setEmissivity( s.m_Emissivity );

    emit sendAddSurfaceMaterial( sm );
}

void FDYNGUIManager::removeSurfaceMaterial(SurfaceMaterial s)
{
    NESurfaceMaterial sm;
    sm.setName( s.m_Name );
    sm.setUniqueID( s.m_ID );
    sm.setIndex( s.m_Index );
    sm.setConductivity( s.m_Conductivity );
    sm.setDensity( s.m_Density );
    sm.setHeat( s.m_Heat );
    sm.setThickness( s.m_Thickness );
    sm.setEmissivity( s.m_Emissivity );

    emit sendRemoveSurfaceMaterial( sm );
}

void FDYNGUIManager::assignSurfaceMaterial(NENode *node, SurfaceMaterial s, QString matKind)
{
    NESurfaceMaterial sm = s;

    emit sendAssignSurfaceMaterial( node, sm, matKind );
}

void FDYNGUIManager::changeSurfaceMaterial(SurfaceMaterial s)
{
    NESurfaceMaterial sm;
    sm.setName( s.m_Name );
    sm.setUniqueID( s.m_ID );
    sm.setIndex( s.m_Index );
    sm.setConductivity( s.m_Conductivity );
    sm.setDensity( s.m_Density );
    sm.setHeat( s.m_Heat );
    sm.setThickness( s.m_Thickness );
    sm.setEmissivity( s.m_Emissivity );

    emit sendSurfaceMaterialChanged( sm );
}

void FDYNGUIManager::updateSurfaceMaterial(NESurfaceMaterial mat)
{
    SurfaceMaterial sm = mat;
    if( mainWindow()->surfaceMaterialDialog() )
    {
        mainWindow()->surfaceMaterialDialog()->updateMaterial( sm );
    }
}

void FDYNGUIManager::updateSurfaceMaterials(QList<NESurfaceMaterial> slist)
{
    if( mainWindow()->surfaceMaterialDialog() )
    {
        mainWindow()->surfaceMaterialDialog()->clear();

        SurfaceMaterial nullMat = mainWindow()->surfaceMaterialDialog()->createNullMaterial();
        slist.push_front(nullMat);

        for( int i = 0; i < slist.size(); ++i )
        {
            SurfaceMaterial m = slist[i];
            mainWindow()->surfaceMaterialDialog()->addMaterial( m );
        }
    }
}

void FDYNGUIManager::updateComputeScenarios(QList<FRANXProperties> fplist)
{
    if( mainWindow()->franxCalcDialog() )
    {
        mainWindow()->franxCalcDialog()->updateScenarios( fplist );
    }
}

void FDYNGUIManager::addFireMaterial(FireMaterial f)
{
    NEFireMaterial fm;
    fm.setName( f.m_Name );
    fm.setUniqueID( f.m_ID );
    fm.setIndex( f.m_Index );
    fm.setC( f.m_C );
    fm.setCI( f.m_Cl );
    fm.setH( f.m_H );
    fm.setN( f.m_N );
    fm.setO( f.m_O );
    fm.setCombuHeat( f.m_CombuHeat );
    fm.setRadFraction( f.m_RadFraction );
    fm.setInfos(f.m_Infos);

    emit sendAddFireMaterial( fm );
}

void FDYNGUIManager::removeFireMaterial(FireMaterial f)
{
    NEFireMaterial fm;
    fm.setName( f.m_Name );
    fm.setUniqueID( f.m_ID );
    fm.setIndex( f.m_Index );
    fm.setC( f.m_C );
    fm.setCI( f.m_Cl );
    fm.setH( f.m_H );
    fm.setN( f.m_N );
    fm.setO( f.m_O );
    fm.setCombuHeat( f.m_CombuHeat );
    fm.setRadFraction( f.m_RadFraction );
    fm.setInfos(f.m_Infos);

    emit sendRemoveFireMaterial( fm );
}

void FDYNGUIManager::assignFireMaterial(NENode *node, FireMaterial f)
{
    NEFireMaterial fm = f;

    emit sendAssignFireMaterial( node, fm );
}

void FDYNGUIManager::updateFireMaterial(NEFireMaterial fmat)
{
    FireMaterial fm = fmat;
    if( mainWindow()->fireMaterialDialog() )
    {
        mainWindow()->fireMaterialDialog()->updateMaterial( fm );
    }
}

void FDYNGUIManager::updateFireMaterials(QList<NEFireMaterial> flist)
{
    if( mainWindow()->fireMaterialDialog() )
    {
        mainWindow()->fireMaterialDialog()->clear();

        FireMaterial nullMat = mainWindow()->fireMaterialDialog()->createNullMaterial();
        flist.push_front(nullMat);

        for( int i = 0; i < flist.size(); ++i )
        {
            FireMaterial m = flist[i];
            mainWindow()->fireMaterialDialog()->addMaterial( m );
        }
    }
}

void FDYNGUIManager::addFailureCriteria(FailureCriteria fail)
{
    NEFailureCriteria failure;
    failure.setName( fail.m_Name );
    failure.setID( fail.m_ID );
    failure.setIndex( fail.m_Index );
    failure.setType( fail.m_Type);
    failure.setThreshold( fail.m_Threshold );

    emit sendAddFailureCriteria( failure );
}

void FDYNGUIManager::removeFailureCriteria(FailureCriteria fail)
{
    NEFailureCriteria failure;
    failure.setName( fail.m_Name );
    failure.setID( fail.m_ID );
    failure.setIndex( fail.m_Index );
    failure.setType( fail.m_Type);
    failure.setThreshold( fail.m_Threshold );

    emit sendRemoveFailureCriteria( failure );
}

void FDYNGUIManager::assignFailureCriteria(NENode *node, FailureCriteria fail)
{
    NEFailureCriteria failure = fail;

    emit sendAssignFailureCriteria( node, failure );
}

void FDYNGUIManager::updateFailureCriteria(NEFailureCriteria failure)
{
    FailureCriteria fail = failure;
    if( mainWindow()->failureCriteriaDialog() )
    {
        mainWindow()->failureCriteriaDialog()->updateFailureCriteria( fail );
    }
}

void FDYNGUIManager::updateFailureCriterias(QList<NEFailureCriteria> flist)
{
    if( mainWindow()->failureCriteriaDialog() )
    {
        mainWindow()->failureCriteriaDialog()->clear();

        FailureCriteria nullFail = mainWindow()->failureCriteriaDialog()->createNullFailureCriteria();
        flist.push_front(nullFail);

        for( int i = 0; i < flist.size(); ++i )
        {
            FailureCriteria fail = flist[i];
            mainWindow()->failureCriteriaDialog()->addFailureCriteria( fail );
        }
    }
}

void FDYNGUIManager::addCableProperty(CableProperty p)
{
    NECableProperty prop;
    prop.setName(p.m_Name);
    prop.setCleanName(p.m_CleanName);
    prop.setUniqueID(p.m_ID);
    prop.setIndex(p.m_Index);
    prop.setManufacturer(p.m_Manufacturer);
    prop.setCategory(p.m_Category);
    prop.setModel(p.m_Model);
    prop.setIssues(p.m_Issues);
    prop.setInsulationType(p.m_InsulationType);
    prop.setJacketType(p.m_JacketType);
    prop.setOutsideDiameterMM(p.m_OutSideDiameterMM);
    prop.setOutSideDiameterKgPerMeter(p.m_OutSideDiameterKgPerMeter);
    prop.setJacketThicknessMM(p.m_JacketThicknessMM);
    prop.setGauges(p.m_Gauges);
    prop.setConductors(p.m_Conductors);
    prop.setDescription(p.m_Description);
    prop.setGrade(p.m_Grade);
    prop.setPart(p.m_Part);
    prop.setItem(p.m_Item);

    emit sendAddCableProperty( prop );
}

void FDYNGUIManager::removeCableProperty(CableProperty p)
{
    NECableProperty prop;
    prop.setName(p.m_Name);
    prop.setCleanName(p.m_CleanName);
    prop.setUniqueID(p.m_ID);
    prop.setIndex(p.m_Index);
    prop.setManufacturer(p.m_Manufacturer);
    prop.setCategory(p.m_Category);
    prop.setModel(p.m_Model);
    prop.setIssues(p.m_Issues);
    prop.setInsulationType(p.m_InsulationType);
    prop.setJacketType(p.m_JacketType);
    prop.setOutsideDiameterMM(p.m_OutSideDiameterMM);
    prop.setOutSideDiameterKgPerMeter(p.m_OutSideDiameterKgPerMeter);
    prop.setJacketThicknessMM(p.m_JacketThicknessMM);
    prop.setGauges(p.m_Gauges);
    prop.setConductors(p.m_Conductors);
    prop.setDescription(p.m_Description);
    prop.setGrade(p.m_Grade);
    prop.setPart(p.m_Part);
    prop.setItem(p.m_Item);

    emit sendRemoveCableProperty(prop);
}

void FDYNGUIManager::assignCableProperty(NENode *node, CableProperty p)
{
    NECableProperty prop = p;

    emit sendAssignCableProperty( node, prop );
}

void FDYNGUIManager::updateCableProperty(NECableProperty prop)
{
    CableProperty p = prop;
    if( mainWindow()->cableMaterialDialog() )
    {
        mainWindow()->cableMaterialDialog()->updateCableProperty( p );
    }
}

void FDYNGUIManager::updateCableProperties(QList<NECableProperty> props, std::vector<QList<QString>> options)
{
    if( mainWindow()->cableMaterialDialog() )
    {
        mainWindow()->cableMaterialDialog()->clear();

        mainWindow()->cableMaterialDialog()->updateFilterCombos(options);

        CableProperty nullProp = mainWindow()->cableMaterialDialog()->createNullCableProperty();
        props.push_front(nullProp);

        for( int i = 0; i < props.size(); ++i )
        {
            CableProperty prop = props[i];
            mainWindow()->cableMaterialDialog()->addCableProperty( prop );
        }
    }
}

void FDYNGUIManager::addVentCriteria(VentCriteria crit)
{
    NEVentCriteria criterion;
    criterion.setName( crit.m_Name );
    criterion.setID( crit.m_ID );
    criterion.setIndex( crit.m_Index );
    criterion.setType( crit.m_Type);
    criterion.setValues(crit.m_Values);

    emit sendAddVentCriteria( criterion );
}

void FDYNGUIManager::removeVentCriteria(VentCriteria crit)
{
    NEVentCriteria criterion;
    criterion.setName( crit.m_Name );
    criterion.setID( crit.m_ID );
    criterion.setIndex( crit.m_Index );
    criterion.setType( crit.m_Type);
    criterion.setValues(crit.m_Values);

    emit sendRemoveVentCriteria( criterion );
}

void FDYNGUIManager::assignVentCriteria(NENode *node, VentCriteria crit, QString criterionKind)
{
    NEVentCriteria criterion = crit;

    emit sendAssignVentCriteria( node, criterion, criterionKind );
}

void FDYNGUIManager::updateVentCriteria(NEVentCriteria criterion)
{
    VentCriteria crit = criterion;
    if( mainWindow()->ventCriteriaDialog() )
    {
        mainWindow()->ventCriteriaDialog()->updateVentCriteria( crit );
    }
}

void FDYNGUIManager::updateVentCriterias(QList<NEVentCriteria> criteria)
{
    if( mainWindow()->ventCriteriaDialog() )
    {
        mainWindow()->ventCriteriaDialog()->clear();

        VentCriteria nullCrit = mainWindow()->ventCriteriaDialog()->createNullVentCriteria();
        criteria.push_front(nullCrit);

        for( int i = 0; i < criteria.size(); ++i )
        {
            VentCriteria crit = criteria[i];
            mainWindow()->ventCriteriaDialog()->addVentCriteria( crit );
        }
    }
}

void FDYNGUIManager::processAboutFailedItems( QList<int> failedItemID,
                                              QList<double> failedTime,
                                              QList<int> scenarioID,
                                              QList<QString> failOutput )
{
    QObject* timelineView = this->mainWindow()->Timeline()->rootObject();
    QVariant returnValue;

    QList<QVariant> qmlFailedTimes;
    foreach (double doubleValue, failedTime) {
        int v = doubleValue*NESCENE.RecordFrameRate();
        qmlFailedTimes.append(v);
    }

    QVariant val(qmlFailedTimes);

    QMetaObject::invokeMethod(timelineView, "setFailedTimesFromCpp",
                              Q_RETURN_ARG(QVariant, returnValue),
                              Q_ARG(QVariant, val));

}

///
/// \brief FDYNGUIManager::addCompartment
/// \return
///

NENode *FDYNGUIManager::addCompartment()
{
    NEZone *parent = nullptr;
    NENode* node = nullptr;

    NESpatialEngine *fEngine = qobject_cast<NESpatialEngine *>(mainWindow()->zoneInspectorWindow()->root());
    if(!fEngine)
        return nullptr;
    QString zoneName = fEngine->activeZoneName();
    parent = qobject_cast<NEZone *>(NESCENE.findChildNode(zoneName));

    node = NESCENE.createObject( "Fire Zone Boundary",
                                 NESCENE.geometryManager().get()->nextObjectName("Fire Zone Boundary", ""),
                                 parent );

    if( node )
    {
        node->setParent( parent );
        emit sendAddCompartment( node, parent );
        //addItemToZoneTree(node);
        updateZoneInspector();
        selectObject(node);
        mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();
    }

    return node;
}


void FDYNGUIManager::addFireDetection()
{
    if(NEScenario *parent = qobject_cast<NEScenario*>(NESCENE.selectedNode()))
    {
//        std::cout << "Adding Fire Detection " << std::endl;
        NENode *node = NESCENE.createObject( "Fire Detection",
                                             NESCENE.geometryManager().get()->nextObjectName("Fire Detection", ""),
                                             NESCENE.selectedNode() );
        if( node )
        {
            node->setParent( NESCENE.selectedNode() );
            emit sendAddFireDetection( node );
            //addItemToZoneTree(node);
            updateZoneInspector();
            selectObject(node);
            mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();
        }
    }
}

void FDYNGUIManager::addItemsToScenario(QList<std::pair<QObject*, QObject*>>& childParentPairs)
{
    for (std::pair<QObject*, QObject*>& childParentPair : childParentPairs)
    {
        if( NEZoneItem* zoneItem = qobject_cast< NEZoneItem *>(childParentPair.first) )
        {
            NENode* parentNode = qobject_cast< NENode *>(childParentPair.second);
            emit sendAddItemToScenario(zoneItem, parentNode);
            updateInspectors();
            //addItemToScenarioTree(zoneItem, parentNode);
            //mainWindow()->zoneInspectorWindow()->zoneInspector()->displayItemAsActive(zoneItem);
        }
    }

    updateGLWindows();
}

void FDYNGUIManager::removeItemsFromScenario(QList<std::pair<QObject*, QObject*>>& childParentPairs)
{
    QSettings settings("Fire Dynamics", "FRI3D");
    bool checked = settings.value("dontShowAgainInScenario", false).toBool();
    if(!checked)
    {
        QMessageBox msgBox(QMessageBox::Warning, QString("FRI3D"),QString("Do you want to remove item from scenario?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, mainWindow());
        QCheckBox dontShowCheck("Don't show again this message.");
        dontShowCheck.setChecked(false);
        msgBox.setCheckBox(&dontShowCheck );
        int result = msgBox.exec();

        settings.setValue("dontShowAgainInScenario", dontShowCheck.isChecked());
        if(result == QMessageBox::Yes)
        {
            for (std::pair<QObject*, QObject*>& childParentPair : childParentPairs)
            {
                if( NEZoneItem* zi = qobject_cast< NEZoneItem *>(childParentPair.first) )
                {
                    NENode* parentNode = qobject_cast< NENode *>(childParentPair.second);
                    emit sendRemoveItemFromScenario( zi, parentNode );
                }
            }
        }
    }
    else
    {
        for (std::pair<QObject*, QObject*>& childParentPair : childParentPairs)
        {
            if( NEZoneItem* zi = qobject_cast< NEZoneItem *>(childParentPair.first) )
            {
                NENode* parentNode = qobject_cast< NENode *>(childParentPair.second);
                emit sendRemoveItemFromScenario( zi, parentNode );
            }
        }
    }

    updateInspectors();
}

void FDYNGUIManager::removeItemFromAllScenarios()
{
    QSettings settings("Fire Dynamics", "FRI3D");
    bool checked = settings.value("dontShowAgainInAllScenario", false).toBool();
    //bool checked = false;
    if(!checked)
    {
        QMessageBox msgBox(QMessageBox::Warning, QString("FRI3D"),QString("Do you want to remove item from scenario?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, mainWindow());
        QCheckBox dontShowCheck("Don't show again this message.");
        dontShowCheck.setChecked(false);
        msgBox.setCheckBox(&dontShowCheck );
        int result = msgBox.exec();

        settings.setValue("dontShowAgainInAllScenario", dontShowCheck.isChecked());
        if(result == QMessageBox::Yes)
        {
            if( NEZoneItem* zi = qobject_cast< NEZoneItem *>( NESCENE.selectedObject() ) )
                emit sendRemoveItemFromScenarios( zi, QList<NENode*>() );

            updateInspectors();
        }
    }
    else
    {
        if( NEZoneItem* zi = qobject_cast< NEZoneItem *>( NESCENE.selectedObject() ) )
            emit sendRemoveItemFromScenarios( zi, QList<NENode*>() );

        updateInspectors();
    }
}

void FDYNGUIManager::simulateScenario()
{
    emit sendSimulateScenario( "", "", P->m_cfastEngine->objectName() );
}

void FDYNGUIManager::simulateNewScenario()
{
    FDYNSimulateScenarioDialog dlg(mainWindow(), P->m_cfastEngine );

    if(NEZoneItem* item = qobject_cast<NEZoneItem*>(NESCENE.selectedNode()) )
    {
        dlg.SetScenarioName( item->objectName() + "_CFAST" );
    }

    if(dlg.exec() == QDialog::Accepted)
    {
        emit sendSimulateScenario( dlg.ScenarioName(), dlg.Desc(), P->m_cfastEngine->objectName() );
    }
}

void FDYNGUIManager::convertToSource()
{
    if(NEZoneItem* item = qobject_cast<NEZoneItem*>(NESCENE.selectedNode()))
    {
        if (NENode* parentNode = qobject_cast<NENode*>(item->parent()))
        {
            NENode *fireNode = NESCENE.createObject("Fire Definition",
                                                    NESCENE.geometryManager().get()->nextObjectName("Fire Definition", ""),
                                                    parentNode);
            if (fireNode != nullptr)
            {
                fireNode->setParent(parentNode);

                emit sendConvertToSource(item, fireNode);

                mainWindow()->sceneInspectorWindow()->removeSelectedItem();
                selectObject(fireNode);
                mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();

                updateInspectors();
                updateGLWindows();
            }
        }
    }
}

void FDYNGUIManager::convertToItem()
{
    if(NEZoneItem* item = qobject_cast<NEZoneItem*>(NESCENE.selectedNode()))
    {
        if (NENode* parentNode = qobject_cast<NENode*>(item->parent()))
        {

            NENode *itemNode = NESCENE.createObject("Fire Plant Item",
                                                    NESCENE.geometryManager().get()->nextObjectName("Fire Plant Item", ""), parentNode);
            if (itemNode != nullptr)
            {
                itemNode->setParent(parentNode);

                emit sendConvertToItem(item, itemNode);

                mainWindow()->sceneInspectorWindow()->removeSelectedItem();
                selectObject(itemNode);
                mainWindow()->zoneInspectorWindow()->zoneInspector()->positionToSelected();

                NESCENE.deleteObject(item, true);

                updateInspectors();
                updateGLWindows();
            }
        }
    }
}

void FDYNGUIManager::saveAsScenario()
{
    NEScenario* s = qobject_cast< NEScenario *>( NESCENE.selectedObject() );
    if( s )
    {
        //we should decide new scenario nam in here.
        emit saveAs( s, "" );
    }
    //updateInspectors();
}

void FDYNGUIManager::addItemToZone()
{

}

void FDYNGUIManager::removeItemFromZone()
{

}

void FDYNGUIManager::updateComputeScenario(FRANXProperties fp)
{
    emit sendUpdateComputeScenario( fp );
}

void FDYNGUIManager::computeFRANXOptions()
{
    FDYNFRANXCalcDialog* dlg = mainWindow()->franxCalcDialog();
    dlg->show();
}

void FDYNGUIManager::computeFRANX()
{
//    if( dlg->exec() == QDialog::Accepted)
    {
        emit sendComputeFranx();
    }
}

void FDYNGUIManager::setSelectedParent()
{
    QList<NENode*> tmp;
    foreach(NEObject* obj, *NESCENE.selectedObjects())
    {
        if(NENode* node = qobject_cast<NENode*>(obj))
        {
            tmp.append(node);
        }
    }
    foreach(NENode* node, tmp)
    {
        if(NESpatialGroup* group = qobject_cast<NESpatialGroup*>(node))
        {
            tmp.removeAt(tmp.indexOf(node));
            NESCENE.parentObjects(tmp, group);
            return;
        }
    }
}

void FDYNGUIManager::setSelectedUnparent()
{
    NENode* node = NESCENE.selectedNode();
    if(NESpatialGroup* group = qobject_cast<NESpatialGroup*>(node))
    {
        NESCENE.unparentObjects(node, group);
        return;
    }
}

void FDYNGUIManager::setSelectedHidden(bool val)
{
    if(!NESCENE.selectedNode())
        return;
    if(NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode()))
    {
        spatial->SetRenderMode(NESpatial::Invisible);
        updateInspectors();
        updatePropertyEditor();
    }
}

void FDYNGUIManager::setSelectedBoundingBox(bool val)
{
    if(!NESCENE.selectedNode())
        return;
    if(NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode()))
    {
        spatial->SetRenderMode(NESpatial::Invisible);
        updateInspectors();
        updatePropertyEditor();
    }
}

void FDYNGUIManager::setSelectedActive()
{
    if(!NESCENE.selectedNode())
        return;
    NESCENE.selectedNode()->SetBehavior(NENode::Active);
    updateInspectors();
    updatePropertyEditor();
}

void FDYNGUIManager::setSelectedCache()
{
    if(!NESCENE.selectedNode())
        return;
    NESCENE.selectedNode()->SetBehavior(NENode::Cache);
    updateInspectors();
    updatePropertyEditor();
}

void FDYNGUIManager::setSelectedInactive()
{
    if(!NESCENE.selectedNode())
        return;
    NESCENE.selectedNode()->SetBehavior(NENode::Inactive);
    updateInspectors();
    updatePropertyEditor();
}

void FDYNGUIManager::fullscreenToggle(bool val)
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

void FDYNGUIManager::maximize()
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

void FDYNGUIManager::playToggled(bool val)
{
}

NEGLWidget* FDYNGUIManager::mainWidget()
{
    return P->mainGLWidget();
}

const FDYNGUIManagerUserProps::NEGUIItemSize& FDYNGUIManagerUserProps::LargeFontSize() const
{
    return P->m_LargeFontSize;
}

uint FDYNGUIManagerUserProps::SetLargeFontSize(const FDYNGUIManagerUserProps::NEGUIItemSize& LargeFontSize)
{
    P->m_LargeFontSize = LargeFontSize;
    return NE::kReturnSuccess;
}

const FDYNGUIManagerUserProps::NEGUIItemSize& FDYNGUIManagerUserProps::SmallFontSize() const
{
    return P->m_SmallFontSize;

}

uint FDYNGUIManagerUserProps::SetSmallFontSize(const FDYNGUIManagerUserProps::NEGUIItemSize& SmallFontSize)
{
    P->m_SmallFontSize = SmallFontSize;
    return NE::kReturnSuccess;
}

const FDYNGUIManagerUserProps::NEGUIItemSize& FDYNGUIManagerUserProps::NormalFontSize() const
{
    return P->m_NormalFontSize;
}

uint FDYNGUIManagerUserProps::SetNormalFontSize(const FDYNGUIManagerUserProps::NEGUIItemSize& NormalFontSize)
{
    P->m_NormalFontSize = NormalFontSize;
    return NE::kReturnSuccess;
}

uint FDYNGUIManagerUserProps::SetOSDFontSize(const FDYNGUIManagerUserProps::NEGUIItemSize& val)
{
    P->m_OSDFontSize = val;
    NERENDERMAN.GLManager()->setOSDFontSize((NEGLManager::NEGUIItemSize)val);
    //NEOSD.setOffsety(10 + 2 * val);

    return NE::kReturnSuccess;
}

const FDYNGUIManagerUserProps::NEGUIItemSize& FDYNGUIManagerUserProps::OSDFontSize() const
{
    return P->m_OSDFontSize;
}

void FDYNGUIManagerPrivate::setWindowTitle(QString scene)
{
    S->mainWindow()->setWindowTitle(scene);
}

void FDYNGUIManagerPrivate::makeGLConnection(NEGLWidget *wid)
{
    QObject::connect( wid, SIGNAL(selectObject(QObject *)), S, SLOT(selectObject(QObject *)) );
    QObject::connect( wid, SIGNAL(selectObjects(QList<NENode*> &)), S, SLOT(selectObjects(QList<NENode*> &))  );

    QObject::connect( wid, SIGNAL(updatePropertyEditorUI()), S, SLOT(updatePropertyEditor()) ) ;
    QObject::connect( wid, SIGNAL(updateGLWindows()), S, SLOT(updateGLWindows()) );

    QObject::connect(wid, SIGNAL(dropCalled(matrix44f , matrix44f , double, double, double, double, void *)),
                     S, SIGNAL(dropCalled(matrix44f, matrix44f, double, double, double, double, void*)));

    QObject::connect(wid, SIGNAL(popupDescriptionAndName(const QPoint&, QWidget *, NENode *, QPainter *)),
                     S, SLOT(popupDescriptionAndName(const QPoint&, QWidget *, NENode *, QPainter *)));

    QObject::connect(wid, SIGNAL(showPropertyShortCut(const QPoint,QWidget*,NENode*)),
                     S, SLOT(showPropertyShortCutDialog(const QPoint,QWidget*,NENode*)));
}

void FDYNGUIManagerPrivate::splitGLDockHorizontally(NEGLWidget* wid)
{
    GLDockWidgetBundle* bundle = createGLWidget();
    QDockWidget* parDock = m_GLtoGLDockWidget[wid]->dockWidget;
    QSize pdSize = QSize(parDock->size().width() / 2, parDock->size().height());
    S->mainWindow()->splitDockWidget(parDock, bundle->dockWidget, Qt::Horizontal);

    m_GLtoGLDockWidget[ bundle->glWidget]->mainWindow->addCameras(NERENDERMAN.cameras() );
    setCameraForGLDock( bundle, NERENDERMAN.topCamera() );

    makeGLConnection(bundle->glWidget);

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

void FDYNGUIManagerPrivate::splitGLDockVertically(NEGLWidget* wid)
{
    GLDockWidgetBundle* bundle = createGLWidget();
    QDockWidget* parDock = m_GLtoGLDockWidget[wid]->dockWidget;
    QSize pdSize = QSize(parDock->size().width(), parDock->size().height() / 2);
    S->mainWindow()->splitDockWidget(parDock, bundle->dockWidget, Qt::Vertical);
    m_GLtoGLDockWidget[bundle->glWidget]->mainWindow->addCameras(NERENDERMAN.cameras());

    setCameraForGLDock( bundle, NERENDERMAN.topCamera() );

    makeGLConnection(bundle->glWidget);

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

void FDYNGUIManagerPrivate::splitGLDockToQuad(NEGLWidget* wid)
{
    QSize mainsize = S->mainWindow()->size();
    // Create upper-right part by diving the given window into two
    GLDockWidgetBundle* upperRight = createGLWidget();
    QDockWidget* parDock = m_GLtoGLDockWidget[wid]->dockWidget;
    QSize pdSize = QSize(parDock->size().width() / 2, parDock->size().height() / 2);
    S->mainWindow()->splitDockWidget(m_GLtoGLDockWidget[wid]->dockWidget, upperRight->dockWidget, Qt::Vertical);
    m_GLtoGLDockWidget[upperRight->glWidget]->mainWindow->addCameras(NERENDERMAN.cameras());
    setCameraForGLDock(upperRight, NERENDERMAN.leftCamera());
    makeGLConnection(upperRight->glWidget);
    // Create lower-left part by diving the upper-left window into two horizontally
    GLDockWidgetBundle* bottomLeft = createGLWidget();
    S->mainWindow()->splitDockWidget(m_GLtoGLDockWidget[wid]->dockWidget, bottomLeft->dockWidget, Qt::Horizontal);
    m_GLtoGLDockWidget[bottomLeft->glWidget]->mainWindow->addCameras(NERENDERMAN.cameras());
    setCameraForGLDock(bottomLeft, NERENDERMAN.topCamera());
    makeGLConnection(bottomLeft->glWidget);
    // Create lower-left part by diving the upper-right window into two horizontally
    GLDockWidgetBundle* bottomRight = createGLWidget();
    S->mainWindow()->splitDockWidget(upperRight->dockWidget, bottomRight->dockWidget, Qt::Horizontal);
    m_GLtoGLDockWidget[bottomRight->glWidget]->mainWindow->addCameras(NERENDERMAN.cameras());
    setCameraForGLDock(bottomRight, NERENDERMAN.frontCamera());
    makeGLConnection(bottomRight->glWidget);
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

NEGLWidget* FDYNGUIManagerPrivate::mainGLWidget()
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

FDYNGUIManagerUserProps::FDYNGUIManagerUserProps(FDYNGUIManager* s) : S(s)
{
#if 0
    P = new FDYNGUIManagerUserPropsPrivateager(this);
    setObjectName("UIProperties");
#endif
}

FDYNGUIManagerUserProps::~FDYNGUIManagerUserProps()
{
#if 0
    delete P;
#endif
}

const FDYNGUIManagerUserProps::NEGUIItemSize& FDYNGUIManagerUserProps::ToolbarSize() const
{
    return P->m_ToolbarSize;
}

uint FDYNGUIManagerUserProps::SetToolbarSize(const FDYNGUIManagerUserProps::NEGUIItemSize& ToolbarSize)
{
    P->m_ToolbarSize = ToolbarSize;
    return NE::kReturnSuccess;
}

const FDYNGUIManager::FDYNTimelineUnits& FDYNGUIManager::TimelineUnits() const
{
    return P->m_TimelineUnits;
}

uint FDYNGUIManager::SetTimelineUnits(const FDYNTimelineUnits& val)
{
    NE_PROP_SET_AND_NOTIFY(TimelineUnits, P, val);
    P->m_pTimeline->changeUnits((int)val);
    P->m_pTimeline->setValueWithTimelineUnits();
    return NE::kReturnSuccess;
}

#ifdef OLD_PROPERTY
void FDYNGUIManager::hideObjectFromOtherPropEditors(QObject* obj, FDYNPropertyEditorWindow* pe)
{
    foreach(FDYNPropertyEditorWindow* w, P->m_PropEdWins)
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
#endif

void FDYNGUIManager::addItemToZoneTree(NENode* node)
{
    if (mainWindow())
        mainWindow()->zoneInspectorWindow()->zoneInspector()->addItemToTree(node);
}

void FDYNGUIManager::addItemToScenarioTree(NENode* node, NENode* parentNode)
{
    if (mainWindow())
        mainWindow()->scenarioInspectorWindow()->scenarioInspector()->addItemToTree(node, parentNode);
}
