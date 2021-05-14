#include "NEUIApplication.h"
#include "Managers/NEGUIManager.h"

#include "Base/NEBase.h"
#include "Managers/NEGUIManager.h"
#include "Manager/NESimulationManager.h"
#include "Windows/Main/NEMainWindow.h"
#include "Base/NEApplicationInfo.h"
#include "Base/NESceneManager.h"
#if NE_VOLUME
#include "Manager/NEMeshVolGeometryManager.h"
#endif



#include "Rendering/Manager/NERenderNEOManager.h"

NEUIApplication::NEUIApplication(int &argc, char **argv)
{
//    Don't remove yet.
//    QSurfaceFormat format;
//    //format.setOptions(QSurfaceFormat::DeprecatedFunctions);
//    format.setRenderableType(QSurfaceFormat::OpenGL);
//    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
//    //format.setVersion(4, 3);
//    format.setProfile(QSurfaceFormat::CompatibilityProfile);
//    QSurfaceFormat::setDefaultFormat(format);



    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
    //QApplication::setAttribute(Qt::AA_DisableHighDpiScaling, true);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL, true);

    m_app   = std::shared_ptr< QApplication >( new QApplication( argc, argv ) );
    m_info  = std::shared_ptr< NEApplicationInfo >( new NEApplicationInfo( m_app, argc, argv ) );

    QApplication* app = qobject_cast< QApplication *>( m_app.get() );
    if( app )
        app->setStyle( QStyleFactory::create( "Fusion" ) );

    m_info->setGuiMode( true );
    parseOptions();

    if( m_info->GuiMode() )
    {
        NEGUI.Instance();
    }

    NERenderManager::Instance( std::shared_ptr< NERenderNEOManager >( new NERenderNEOManager() ) );


#if NE_VOLUME

    std::shared_ptr<NEMeshVolGeometryManager> m = std::shared_ptr< NEMeshVolGeometryManager>
            ( new NEMeshVolGeometryManager( &NESCENE ) );
    NESCENE.setGeometryManager( m );

#endif

    NESIMMGR.Instance();

    if( m_info->GuiMode() )
        NEBASE.init( this, static_cast< ICallInterfaceFromBaseToGUI* > ( &NEGUI ) );
    else
        NEBASE.init( this, static_cast< ICallInterfaceFromBaseToGUI* > ( NULL ) );
}

NEUIApplication::~NEUIApplication()
{
}

void NEUIApplication::run() const
{
    QObject::connect( &NEBASE, SIGNAL(quitApp(int)), QApplication::instance(), SLOT(quit()) );
    m_app->exec();
}

void NEUIApplication::setStyle(QStyle* style)
{
    QApplication* app = qobject_cast< QApplication *>( m_app.get() );
    if( app )
        app->setStyle( style );
}



bool NEUIApplication::event( QEvent* e )
{
    switch( e->type() )
    {
    // Handles Mac finder open event
    case QEvent::FileOpen:
    {
        bool loadFile = false;
        if( !m_info->passedFile().isEmpty() )
            loadFile = true;
        m_info->setPassedFile( static_cast<QFileOpenEvent*>(e)->file() );

        // This is to load files when Neutrino is already running
        if( loadFile && NeutrinoInitialized() )
        {
            if( m_info->GuiMode() )
                NEGUI.mainWindow()->open( m_info->passedFile() );
        }
        return true;
    }

    default:
        ;
        //return QApplication::event(e);
    }
    return true;
}



