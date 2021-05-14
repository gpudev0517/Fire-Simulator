
#include <QQuickWidget>

#include "FDYNUIApplication.h"
#include "Managers/FDYNGUIManager.h"
#include "Rendering/Manager/NERenderManager.h"

#include "Base/NEBase.h"
#include "Windows/Main/FDYNMainWindow.h"
#include "Base/NEApplicationInfo.h"
#include "Base/NESceneManager.h"

#include "Physics/globals_Physics.h"
#include "Physics/Manager/NESimulationManager.h"
#include "Manager/NEMeshVolGeometryManager.h"

#include "Utilities/FDYNTreeCustomtype.h"
#include "Utilities/FDYNTreeWidget.h"


FDYNUIApplication::FDYNUIApplication(int &argc, char **argv)
{
//    QSurfaceFormat format;
////    format.setDepthBufferSize(24);
////    format.setStencilBufferSize(8);
////    format.setVersion(4, 3);
////    format.setSamples(16);
////    format.setProfile(QSurfaceFormat::CoreProfile);
//    format.setSwapBehavior(QSurfaceFormat::SingleBuffer);
//    QSurfaceFormat::setDefaultFormat(format);



    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL, true);


    m_app   = std::shared_ptr< QApplication >( new QApplication( argc, argv ) );
    m_info  = std::shared_ptr< NEApplicationInfo >( new NEApplicationInfo( m_app, argc, argv ) );

    QCoreApplication::setApplicationName( "FRI3D" );
    QCoreApplication::setApplicationVersion( "0.0" );
    QCoreApplication::setOrganizationName( "F Dynamics" );

    m_info->parser().process( *(m_app.get()) );
    m_info->setGuiMode( true );
    parseOptions();

    QApplication* app = qobject_cast< QApplication *>( m_app.get() );


    QString STYLE_COLOR_SEPARATOR = "#0c0c0c";
    QString STYLE_COLOR_BACKGROUND = "#343235";
    QString STYLE_COLOR_FOREGROUND = "white";
    QString STYLE_COLOR_DARK_FOREGROUND = "#aaa";
    QString STYLE_COLOR_TRANSPARENT = "transparent";
    QString STYLE_COLOR_ITEM_SELECT = "#282629";
    QString STYLE_COLOR_ITEM_PRESS = "#0c0c0c";
    QString STYLE_COLOR_TOOLBAR_BACKGROUND = "#282629";
    QString STYLE_COLOR_DOCK_TITLE_BACKGROUND = "#282629";
    QString STYLE_COLOR_SCROLLBAR = "#a9a9a9";
    QString STYLE_COLOR_TABLE_BACKGROUND = "#343235";

    QString uiStyle =
        //QMainWindow Style
        "QMainWindow{"
            "background: " + STYLE_COLOR_BACKGROUND + ";}"
        "QMainWindow::separator {"
            "background: " + STYLE_COLOR_SEPARATOR + ";"
            "width: 2px;"
            "height: 2px;}"

        "QPlainTextEdit {"
            "background-color:#282629;border:0px;}"

        //QMenuBar Style
        "QMenuBar {"
            "font-family: \"Segoe UI\"; "
            "font-size: 9pt;"
            "font-weight: bold; "
            "background-color: " + STYLE_COLOR_BACKGROUND + "; padding: 1px; color: " + STYLE_COLOR_DARK_FOREGROUND + "; }"

        "QMenuBar::item {"
            "padding: 4px 10px;"
            "background: " + STYLE_COLOR_TRANSPARENT + ";}"

        "QMenuBar::item:selected {background: " + STYLE_COLOR_ITEM_SELECT + "; color: " + STYLE_COLOR_FOREGROUND + "; }"

        "QMenuBar::item:pressed {background: " + STYLE_COLOR_ITEM_PRESS + "; color: " + STYLE_COLOR_FOREGROUND + "; }"

        //QMenu Style
        "QMenu {"
            "font-family: \"Segoe UI\"; "
            "font-size: 9pt;"
            "font-weight: bold; "
            "background-color:" + STYLE_COLOR_ITEM_SELECT + ";"
            "border: 1px solid " + STYLE_COLOR_SEPARATOR + ";"
            "color: " + STYLE_COLOR_FOREGROUND + ";}"

        "QMenu::item {"
            "background-color: " + STYLE_COLOR_TRANSPARENT + ";"
            "color:" + STYLE_COLOR_DARK_FOREGROUND + ";}"

        "QMenu::item:selected {"
            "background-color: " + STYLE_COLOR_ITEM_PRESS + ";"
            "color:" + STYLE_COLOR_FOREGROUND + ";}"

        //QToolBar Style
        "QToolBar {"
            "background: " + STYLE_COLOR_TOOLBAR_BACKGROUND + ";"
            "spacing: 5px;"
            "border: 2px solid " + STYLE_COLOR_SEPARATOR + ";}"
        "QToolBar::separator {"
            "background: " + STYLE_COLOR_TOOLBAR_BACKGROUND + ";"
            "width: 1px;"
            "height: 1px;}"

        //QDockWidget Style
        "QDockWidget {"
            "background: " + STYLE_COLOR_BACKGROUND + ";"
            "titlebar-close-icon: url(:img/32/window_close_01.png);"
            "titlebar-normal-icon: url(:img/32/window_float_01.png);"
            "color: " + STYLE_COLOR_FOREGROUND + ";}"

        "QDockWidget::title {"
            "font-family: 'Segoe UI';"
            "text-align: center;"
            "background: " +  STYLE_COLOR_DOCK_TITLE_BACKGROUND + ";"
            "font-size: 12pt;"
            "font-weight:bold;"
            "padding-top: 5px;"
            "padding-bottom: 5px;}"

        "QDockWidget::float-button, QDockWidget::close-button {"
            "border: 1px solid " + STYLE_COLOR_TRANSPARENT + ";"
            "background: " + STYLE_COLOR_DOCK_TITLE_BACKGROUND + ";"
            "padding: 0px;}"

        "QDockWidget::close-button:hover, QDockWidget::float-button:hover {"
            "background: " + STYLE_COLOR_DARK_FOREGROUND + ";}"

        "QDockWidget::close-button:pressed, QDockWidget::float-button:pressed {"
            "padding: 1px -1px -1px 1px;}"

         "QToolButton {"
               //"border-style: solid;"
                "border-width: 2px;"
                "margin: 2px;"
              "}"

        "QTabBar{"
            "background: " + STYLE_COLOR_DOCK_TITLE_BACKGROUND + ";}"

        "QTabBar::tab{"

            "background: " + STYLE_COLOR_DOCK_TITLE_BACKGROUND + ";"
            "color: " + STYLE_COLOR_FOREGROUND + ";"
            "height: 35px;"
//            "padding-left:15px;"
//            "padding-right:15px;"
            "width:  100px;"
            "font: 8pt 'Segoe UI';"
            "font-weight:bold;"
            "border-top-left-radius: 10px;"
            "border-top-right-radius: 10px;}"

        "QTabBar::tab:selected {"
            "background: " + STYLE_COLOR_BACKGROUND + ";}"
        "QTabWidget::tab-bar {"
          "background: " + STYLE_COLOR_BACKGROUND + ";}"

        //QScrollBar Style
        "QScrollBar::handle:horizontal {"
            "background: " + STYLE_COLOR_SCROLLBAR+ ";"
            "border-radius: 4px;}"
        "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {"
            "background: " + STYLE_COLOR_TRANSPARENT + ";}"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {"
            "background: " + STYLE_COLOR_TRANSPARENT + ";}"
        "QScrollBar:horizontal {"
            "background: " + STYLE_COLOR_TRANSPARENT + ";"
            "height: 8px;}"

        "QScrollBar::handle:vertical {"
            "background: " + STYLE_COLOR_SCROLLBAR+ ";"
            "border-radius: 4px;"
            "min-height: 20px;}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
            "background: " + STYLE_COLOR_TRANSPARENT + ";}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
            "background: " + STYLE_COLOR_TRANSPARENT + ";}"
        "QScrollBar:vertical {"
            "background: " + STYLE_COLOR_TRANSPARENT + ";"
            "width: 8px;}"

        //QDialog Style
        "QDialog, QSplitter::handle {"
            "background: " + STYLE_COLOR_BACKGROUND + ";}"


        "QListView, QTreeView {"
            "border: none;"
            "color: " + STYLE_COLOR_FOREGROUND + ";"
            "background: " + STYLE_COLOR_TOOLBAR_BACKGROUND + ";}"

       "QHeaderView, QHeaderView:section {"
           "color: " + STYLE_COLOR_FOREGROUND + ";"
           "background: " + STYLE_COLOR_BACKGROUND + ";}"


       "QComboBox, QSpinBox, QDoubleSpinBox, QPushButton, QLineEdit {"
             "color: " + STYLE_COLOR_FOREGROUND + ";"
             "background: " + STYLE_COLOR_TOOLBAR_BACKGROUND + ";}"

       "QCheckBox::indicator {"
                "width: 13px;"
                "height: 13px; }"

        "QCheckBox::indicator:checked { "
               "image: url(:/img/checkbox.png); "
        "}"


       "QLabel, QCheckBox {"
           "color: " + STYLE_COLOR_FOREGROUND + ";"
           "background: " + STYLE_COLOR_BACKGROUND + ";}"

        "QTableView::indicator:checked { "
            "image: url(:/img/checkbox.png);"
        "}"

        "QTableView::indicator:unchecked { "
            "image: url(:/img/checkbox_uncheck.png);"
        "}"


        "QTableView {"
            "background: " + STYLE_COLOR_TABLE_BACKGROUND + ";}"
        ;

    app->setStyleSheet(uiStyle);
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Normal, QPalette::Dark, 0x282629);
    darkPalette.setColor(QPalette::Inactive, QPalette::Dark, 0x282629);
    darkPalette.setColor(QPalette::Disabled, QPalette::Dark, 0x282629);
    app->setPalette(darkPalette);

//    QFont font("Segoe UI");
//    //font.setStyleHint(QFont::Monospace);
//    QApplication::setFont(font);

    qmlRegisterType<FDYNTreeCustomType>("FTreeModel", 1, 0, "FDYNTreeCustomType");
    qmlRegisterType<FDYNTreeWidget>("FTreeView", 1, 0, "FDYNTreeWidget");

    qmlRegisterType<FDYNTreeCustomType>("FPropertyModel", 1, 0, "FDYNPropertyCustomType");
    qmlRegisterType<FDYNTreeWidget>("FPropertyView", 1, 0, "FDYNPropertyWidget");

    if( m_info->GuiMode() )
    {
        FDYNGUI.Instance();
    }

    NERenderManager::Instance( std::shared_ptr< NERenderManager >( new NERenderManager() ) );


    std::shared_ptr<NEMeshVolGeometryManager> m = std::shared_ptr< NEMeshVolGeometryManager>
            ( new NEMeshVolGeometryManager( &NESCENE ) );
    NESCENE.setGeometryManager( m );
    NESIMMGR.Instance();



    NEBASE.SetFileExt( "fri3d" );
    if( m_info->GuiMode() )
        NEBASE.init( this, static_cast< ICallInterfaceFromBaseToGUI* > ( &FDYNGUI ) );
    else
        NEBASE.init( this, static_cast< ICallInterfaceFromBaseToGUI* > ( NULL ) );

    //NENode* grav = NESCENE.findChildNode("DefaultGravity");
    //NESCENE.deleteObject( grav, true );
}

FDYNUIApplication::~FDYNUIApplication()
{
}

void FDYNUIApplication::run() const
{
    QObject::connect( &NEBASE, SIGNAL(quitApp(int)), QApplication::instance(), SLOT(quit()) );
    m_app->exec();
}

void FDYNUIApplication::setStyle(QStyle* style)
{
    QApplication* app = qobject_cast< QApplication *>( m_app.get() );
    if( app )
        app->setStyle( style );
}




