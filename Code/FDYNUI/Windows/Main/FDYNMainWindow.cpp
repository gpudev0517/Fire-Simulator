#include <QQuickWidget>
#include <QQuickView>
#include <QtQuickControls2>
#include <QShortcut>

#include "FDYNMainWindow.h"

#include "Managers/FDYNGUIManager.h"
#include "Managers/FDYNGUIManager_p.h"
#include "Windows/GLMain/FDYNGLMainWindow.h"

#include "Base/NEBase.h"

#include "Base/NESceneManager.h"
#include "Utility/FilePath.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Mesh/NESpatialEngine.h"

#include "Widgets/Dock/FDYNDockWidget.h"
#include "Spatial/NESpatial.h"

#include "Windows/PropertyManager/FDYNPropertyEditorWindow.h"
#include "Widgets/Timeline/FDYNTimeline.h"
#include "Widgets/CADModeling/FDYNCADModelingTool.h"
#include "Widgets/MenuBar/FDYNMenuBar.h"
#include "Dialogs/File/FDYNFileDialog.h"
#include "Dialogs/About/FDYNAboutDialog.h"
#include "Utilities/FDYNTreeModel.h"
#include "Utilities/FDYNTreeWidget.h"
#include "Windows/SceneInspector/FDYNSceneInspectorWindow.h"
#include "Windows/SceneInspector/FDYNSceneInspector.h"
#include "Windows/ZoneInspector/FDYNZoneInspectorWindow.h"
#include "Windows/ZoneInspector/FDYNZoneInspector.h"
#include "Windows/ScenarioInspector/FDYNScenarioInspectorWindow.h"
#include "Windows/FRANXInspector/FDYNFRANXInspectorWindow.h"
#include "Windows/FRANXInspector/FDYNFRANXInspector.h"
#include "Windows/SystemInspector/FDYNSystemInspector.h"
#include "Windows/SystemInspector/FDYNSystemInspectorWindow.h"

#include "Windows/ScenarioInspector/FDYNScenarioInspector.h"
#include "Windows/Message/FDYNMessageWindow.h"
#include "Dialogs/Process/FDYNProcessDialog.h"
#include "Dialogs/PropertyShortCut/FDYNPropertyShortCut.h"
#include "Dialogs/MaterialSelector/FDYNSurfaceMaterialDialog.h"
#include "Dialogs/MaterialSelector/FDYNFireMaterialDialog.h"
#include "Dialogs/CriteriaSelector/FDYNFailureCriteriaDialog.h"
#include "Dialogs/VentCriteriaSelector/FDYNVentCriteriaDialog.h"
#include "Dialogs/CableSelector/FDYNCableDialog.h"
#include "Dialogs/FRANXCalc/FDYNFRANXCalcDialog.h"
#include "Dialogs/Settings/FDYNSettingsDialog.h"

#include "NEGLWidget.h"

#define MAINWINDOW_DEBUG 0


class FDYNCADToolbar : public QToolBar
{
protected:
    void resizeEvent(QResizeEvent *e)
    {
        QToolBar::resizeEvent(e);
        if(subWidget)
        {
            subWidget->resize(this->size());
        }

    }
public:

    FDYNCADToolbar(QWidget *parent) : QToolBar(parent)
    {

    }

    QWidget *subWidget;

    void addWidget(QWidget *widget)
    {
        QToolBar::addWidget(widget);
        subWidget = widget;
    }
};


class FDYNTimelineToolbar : public QToolBar
{
protected:
    void resizeEvent(QResizeEvent *e)
    {
        QToolBar::resizeEvent(e);
        if(subWidget)
        {
            subWidget->resize(this->size());
        }

    }
public:

    FDYNTimelineToolbar(QWidget *parent) : QToolBar(parent)
    {

    }

    QWidget *subWidget;

    void addWidget(QWidget *widget)
    {
        QToolBar::addWidget(widget);
        subWidget = widget;
    }
};

class FDYNMainWindowPrivate
{
public:
    FDYNMainWindowPrivate(FDYNMainWindow* s) : S(s) {}

    //! Does some tweaks on the user interface which are not possible in the Qt designer
    void tweakUI();
    //!
    bool okToContinue();

    FDYNLoadingDialog* m_pLoadingDialog;

    QActionGroup* m_pTransformActions;          /**< Pointer to tranfrom actions */
    FDYNTimeline* m_pTimeline;                /**< Pointer to the timeline */
    QTimer m_ResizeTimer;                       /**< Pointer to window resize timer */
    FDYNAboutDialog* m_pAboutDialog;          /**< Pointer to about dialog */
    QPixmap* splashPixmap;
    QSplashScreen* m_pSceneLoadingSplash;   /**< Pointer to scene loading splash */

    FDYNPropertyShortCut *m_pPropertyShortCutDialg;

    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

    enum { MaxRecentFiles = 5 };
    void setCurrentFile(const QString &fileName);
    QString &getCurrentFile()
    {
        return m_curFile;
    }
    QAction *recentFileActs[MaxRecentFiles];

    QMenuBar *menuBar;
    QToolBar *shelfBar;

    QMenu *menuAdd;
    QMenu *menuMaterials;
    QMenu *menuCameras;
    QMenu *menuLights;
    QMenu *menuOSD;
    QMenu *menuVisual;
    QMenu *menuGizmo;

    QAction *removeAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *preferencesAct;
    QAction *groupAct;
    QAction *unGroupAct;
    QAction *parentAct;
    QAction *unParentAct;
    QAction *aboutAct;


    QAction *zoneAct;
    QAction *scenaAct;
    QAction *sceneAct;
    QAction *systemAct;
    QAction *franxAct;
    QAction *msgAct;
    QAction *shelfAct;
    QAction *glInspectorAct;
    QAction *propertyEditorAct;
    QAction *computefranxAct;

    FDYNDockWidget *dockScene;
    FDYNSceneInspectorWindow *sceneMainWindow;

    FDYNDockWidget *dockZone;
    FDYNZoneInspectorWindow *zoneMainWindow;

    FDYNDockWidget *dockScenario;
    FDYNScenarioInspectorWindow *scenarioMainWindow;

    FDYNDockWidget *dockSystem;
    FDYNSystemInspectorWindow *systemMainWindow;

    FDYNDockWidget *dockProperty;
    FDYNPropertyEditorWindow* propertyMainWindow;

    FDYNDockWidget *dockFRANX;
    FDYNFRANXInspectorWindow *franxMainWindow;

    FDYNDockWidget *dockMsg;
    //QQuickWidget *dockMsgView;
    FDYNMessageWindow *dockMsgView;

    FDYNCADToolbar *cadToolbar;
    FDYNTimelineToolbar *timelineToolbar;

    QDockWidget *dockCADTool;
    FDYNCADModelingTool* m_pCADTool;

    QDockWidget *currentTopWidget;

    QAction *separatorAct;
    QString m_curFile;

    FDYNSurfaceMaterialDialog *surfaceMaterialDlg;
    FDYNFireMaterialDialog *fireMaterialDlg;
    FDYNFailureCriteriaDialog *failureCriteriaDlg;
    FDYNVentCriteriaDialog *ventCriteriaDlg;
    FDYNCableDialog *cableMaterialDlg;

    FDYNFRANXCalcDialog *franxCalcDlg;
    FDYNSettingsDialog *settingsDlg;

private:
    FDYNMainWindow* S;
};


void FDYNMainWindow::selectCursorMode()
{
    foreach(GLDockWidgetBundle* b, FDYNGUI.P->m_IDtoGLDockWidget)
    {
        b->mainWindow->selectCursorMode(true);
    }
}


void FDYNMainWindow::selectTranslationMode()
{
    foreach(GLDockWidgetBundle* b, FDYNGUI.P->m_IDtoGLDockWidget)
    {
        b->mainWindow->selectTranslationMode(true);
    }
}

void FDYNMainWindow::selectRotationMode()
{
    foreach(GLDockWidgetBundle* b, FDYNGUI.P->m_IDtoGLDockWidget)
    {
        b->mainWindow->selectRotationMode(true);
    }
}

void FDYNMainWindow::selectScaleMode()
{
    foreach(GLDockWidgetBundle* b, FDYNGUI.P->m_IDtoGLDockWidget)
    {
        b->mainWindow->selectScaleMode(true);
    }
}

void FDYNMainWindow::setCADModelingMode()
{
    P->dockCADTool->setVisible(true);

#if CADTOOLBAR
    if( P->m_pCADTool )
        P->m_pCADTool->setVisible(true);
    if( P->cadToolbar )
        P->cadToolbar->setVisible(true);
#endif
}

void FDYNMainWindow::exitCADModelingMode()
{
    P->dockCADTool->setVisible(false);
}

FDYNMainWindow::FDYNMainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    P = new FDYNMainWindowPrivate(this);

    setMinimumWidth(1200);
    setMinimumHeight(800);

    //	setWindowFlags(windowFlags() ^ Qt::WindowMaximizeButtonHint);
    // Prepare scene loading splash
    P->splashPixmap = new QPixmap(":/Resources/banner/fdyn-banner.png");
    P->m_pSceneLoadingSplash = new QSplashScreen( *P->splashPixmap );

    P->m_pLoadingDialog = new FDYNLoadingDialog(this);
    P->m_pLoadingDialog->hide();

    P->m_pAboutDialog = new FDYNAboutDialog( this );

    P->m_pPropertyShortCutDialg = new FDYNPropertyShortCut(this);
    P->m_pPropertyShortCutDialg->hide();


    P->menuBar = new QMenuBar(this);
    //P->menuBar->setStyleSheet(menuBarStyle);

    QMenu *fileMenu = P->menuBar->addMenu("&File");

    P->currentTopWidget = NULL;


    //fileMenu->setStyleSheet(menuStyle);

    // shelf bar icons
    const QIcon newIcon = QIcon(":/img/32/shelf_new_file_01.png");
    const QIcon openIcon = QIcon(":/img/32/shelf_open_file_01.png");
    const QIcon saveIcon = QIcon(":/img/32/shelf_save_file_01.png");
    const QIcon saveAsIcon = QIcon(":/img/32/shelf_save_as_file_01.png");
    const QIcon importDrawingIcon = QIcon(":/img/32/shelf_open_file_01.png");
    const QIcon importFRANXIcon = QIcon(":/img/32/shelf_franxInspector_01.png");
    const QIcon importPDMSIcon = QIcon(":/img/32/shelf_pdms_01.png");


    const QIcon removeIcon = QIcon(":/img/32/remove_01.png");
    const QIcon undoIcon = QIcon(":/img/32/undo_01.png");
    const QIcon redoIcon = QIcon(":/img/32/redo_01.png");


    const QIcon zoneInspectorIcon = QIcon(":/img/32/shelf_zoneInspector_01.png");
    const QIcon scenarioInspectorIcon = QIcon(":/img/32/shelf_scenarioInspector_01.png");
    const QIcon franxInspectorIcon = QIcon(":/img/32/shelf_franxInspector_01.png");


    const QIcon sceneInspectorIcon = QIcon(":/img/32/shelf_sceneInspector_01.png");
    const QIcon systemInspectorIcon = QIcon(":/img/32/shelf_systemInspector_01.png");
    const QIcon glInspectorIcon = QIcon(":/img/32/shelf_openGLInspector_01.png");
    const QIcon messagesInspectorIcon = QIcon(":/img/32/shelf_messages_01.png");
    const QIcon computefranxIcon = QIcon(":/img/32/shelf_computefranx_01.png");

    const QIcon propertyEditorIcon = QIcon(":/img/32/shelf_settings_01.png");
    const QIcon preferencesIcon = QIcon(":/img/32/shelf_preferences_01.png");

    // shelfbar actions
    QAction *newAct = new QAction(newIcon, "&New");
    QAction *openAct = new QAction(openIcon, "&Open");
    QAction *saveAct = new QAction(saveIcon, "&Save");
    QAction *saveAsAct = new QAction(saveAsIcon, "&Save As");
    QAction *importDrawingAct = new QAction(importDrawingIcon, "&Drawing");
    QAction *importFRANXAct = new QAction(importFRANXIcon, "&FRANX");
    QAction *importPDMSAct = new QAction(importPDMSIcon, "&PDMS");



    P->zoneAct = new QAction(zoneInspectorIcon, "&Zone Inspector");
    P->zoneAct->setCheckable(true);
    P->scenaAct = new QAction(scenarioInspectorIcon, "&Scenario Inspector");
    P->scenaAct->setCheckable(true);
    P->sceneAct = new QAction(sceneInspectorIcon, "&Scene Inspector");
    P->sceneAct->setCheckable(true);
    P->systemAct = new QAction(systemInspectorIcon, "&System Inspector");
    P->systemAct->setCheckable(true);
    P->franxAct = new QAction(franxInspectorIcon, "&Franx Inspector");
    P->franxAct->setCheckable(true);
    P->msgAct = new QAction(messagesInspectorIcon, "&Message Inspector");
    P->msgAct->setCheckable(true);
    P->shelfAct = new QAction(newIcon, "&Shelf Toolbar");
    P->shelfAct->setCheckable(true);
    P->glInspectorAct = new QAction(glInspectorIcon, "&3D Inspector");
    P->glInspectorAct->setCheckable(true);
    P->propertyEditorAct = new QAction(propertyEditorIcon, "&Property Editor");
    P->propertyEditorAct->setCheckable(true);

    P->computefranxAct = new QAction( computefranxIcon, "&Compute FRANX");
    P->computefranxAct->setCheckable(false);


    P->preferencesAct = new QAction(preferencesIcon, "&Preferences");

    P->shelfBar = addToolBar("Shelf");
    P->shelfBar->setObjectName(QLatin1String("shelfBar"));
    P->shelfBar->setIconSize(QSize(32, 32));
    //P->shelfBar->setStyleSheet(toolBarStyle);

    P->shelfBar->addAction(newAct);
    P->shelfBar->addAction(openAct);
    P->shelfBar->addAction(saveAct);
    P->shelfBar->addAction(saveAsAct);
    P->shelfBar->addSeparator();
    P->shelfBar->addAction(P->zoneAct);
    P->shelfBar->addAction(P->scenaAct);
    P->shelfBar->addAction(P->franxAct);
    P->shelfBar->addAction(P->sceneAct);
    P->shelfBar->addAction(P->systemAct);
    P->shelfBar->addAction(P->glInspectorAct);
    P->shelfBar->addAction(P->msgAct);
    P->shelfBar->addAction(P->propertyEditorAct);

    P->shelfBar->addAction(P->computefranxAct);

    P->shelfBar->addAction(P->preferencesAct);

    new QShortcut(QKeySequence(Qt::Key_Q), this, SLOT(selectCursorMode()));
    new QShortcut(QKeySequence(Qt::Key_W), this, SLOT(selectTranslationMode()));
    new QShortcut(QKeySequence(Qt::Key_E), this, SLOT(selectRotationMode()));
    new QShortcut(QKeySequence(Qt::Key_R), this, SLOT(selectScaleMode()));

    //    new QShortcut(QKeySequence(Qt::Key_C), this, SLOT(setCADModelingMode()));
    //    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(exitCADModelingMode()));



    // menu actions
    QAction *closeAct = new QAction(newIcon, "&Quit");
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);

    QMenu *importMenu = fileMenu->addMenu("&Import");
    importMenu->addAction(importDrawingAct);
    importMenu->addAction(importFRANXAct);
    importMenu->addAction(importPDMSAct);


    fileMenu->addSeparator();
    fileMenu->addMenu(importMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAct);
    P->separatorAct = fileMenu->addSeparator();

    QMenu *editMenu = P->menuBar->addMenu("&Edit");

    const QIcon addIcon = QIcon(":/img/32/add_01.png");

//    P->menuAdd = editMenu->addMenu(addIcon, "&Add");
//    P->menuVisual = P->menuAdd->addMenu("&VISUAL");
//    P->menuCameras = P->menuVisual->addMenu("&CAMERAS");
//    P->menuLights = P->menuVisual->addMenu("&LIGHTS");
//    P->menuMaterials = P->menuVisual->addMenu("&MATERIALS");
//    P->menuOSD = P->menuVisual->addMenu("&OSD");
//    P->menuGizmo = P->menuVisual->addMenu("&GIZMOS");

    P->removeAct = new QAction(removeIcon, "&Remove");
    P->undoAct = new QAction(undoIcon, "&Undo");
    P->redoAct = new QAction(redoIcon, "&Redo");


    P->groupAct = new QAction(newIcon, "&Group");
    P->unGroupAct = new QAction(newIcon, "&Ungroup");
    P->parentAct = new QAction(newIcon, "&Parent");
    P->unParentAct = new QAction(newIcon, "&Unparent");

    editMenu->addAction(P->removeAct);
    editMenu->addAction(P->undoAct);
    editMenu->addAction(P->redoAct);
    editMenu->addSeparator();
    editMenu->addAction(P->preferencesAct);
    //editMenu->addAction(P->groupAct);
    //editMenu->addAction(P->unGroupAct);
    //editMenu->addAction(P->parentAct);
    //editMenu->addAction(P->unParentAct);

    P->removeAct->setShortcut(QApplication::translate("GUI", "Del", nullptr));
    P->undoAct->setShortcut(QApplication::translate("GUI", "Ctrl+Z", nullptr));
    P->redoAct->setShortcut(QApplication::translate("GUI", "Ctrl+Y", nullptr));

    QMenu *windowMenu = P->menuBar->addMenu("&Window");


    windowMenu->addAction(P->zoneAct);
    windowMenu->addAction(P->scenaAct);
    windowMenu->addAction(P->franxAct);
    windowMenu->addAction(P->sceneAct);
    windowMenu->addAction(P->systemAct);
    windowMenu->addAction(P->glInspectorAct);
    windowMenu->addAction(P->msgAct);
    windowMenu->addAction(P->propertyEditorAct);
    windowMenu->addAction(P->shelfAct);

#if VIEW_MENU
    QMenu *viewMenu = P->menuBar->addMenu("&View");
    //viewMenu->setStyleSheet(menuStyle);
    QAction *layerAct = new QAction(newIcon, "&Interactive");
    viewMenu->addAction(layerAct);
#endif

#if 0
    QMenu *renderMenu = P->menuBar->addMenu("&Visualization");
    //renderMenu->setStyleSheet(menuStyle);
    QAction *textureAct = new QAction(newIcon, "&Offline Render Settings");
    QAction *renderAct = new QAction(newIcon, "&Offline Render Sequence");
    renderMenu->addAction(textureAct);
    renderMenu->addAction(renderAct);
#endif

    QMenu *helpMenu = P->menuBar->addMenu( "&Help" );
    //helpMenu->setStyleSheet(menuStyle);
    P->aboutAct = new QAction( newIcon, "&About" );
    helpMenu->addAction( P->aboutAct );

    setMenuBar( P->menuBar );

    setTabPosition( Qt::AllDockWidgetAreas, QTabWidget::North );

    connect(newAct, SIGNAL(triggered()), this, SLOT(newScene()));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    //connect( newAct, SIGNAL(triggered()), &FDYNGUI, SLOT(newProject()) );
    //connect( openAct, SIGNAL(triggered()), &FDYNGUI, SLOT(loadDB()) );
    //connect( saveAct, SIGNAL(triggered()), &FDYNGUI, SLOT(saveDB()) );
    //connect( saveAsAct, SIGNAL(triggered()), &FDYNGUI, SLOT(saveAsDB()) );

    connect( importDrawingAct, SIGNAL(triggered()), &FDYNGUI, SLOT(importDrawing()) );
    connect( importFRANXAct, SIGNAL(triggered()), &FDYNGUI, SLOT(importFRANX()) );
    connect( importPDMSAct, SIGNAL(triggered()), &FDYNGUI, SLOT(importPDMS()) );

    connect( closeAct, SIGNAL(triggered()), this, SLOT(close()));

    connect( P->preferencesAct , SIGNAL(triggered()), &FDYNGUI, SLOT(editPreferences() ) );

    // Timeline
    P->timelineToolbar = new FDYNTimelineToolbar(this);
    P->timelineToolbar->setObjectName(QLatin1String("timelineToolbar"));
    P->timelineToolbar->setWindowTitle("Timeline");
    addToolBar(Qt::BottomToolBarArea, P->timelineToolbar);
    P->m_pTimeline = new FDYNTimeline(this);   // Create the timeline widget
    P->m_pTimeline->setObjectName( "TIMELINE" );

    //Zone Inspector
    P->dockZone = new FDYNDockWidget(this);
    P->dockZone->setObjectName(QLatin1String("dockZone"));
    P->dockZone->setTitleText( "COMPARTMENT" );
    P->dockZone->setFeatures(QDockWidget::AllDockWidgetFeatures);

    P->zoneMainWindow = new FDYNZoneInspectorWindow();
    P->dockZone->setWidget(P->zoneMainWindow);
    addDockWidget(Qt::LeftDockWidgetArea, P->dockZone);

    //FDYNZoneInspector* zoneTree = P->zoneMainWindow->zoneInspector();

#if CADTOOLBAR
    P->cadToolbar = new FDYNCADToolbar(this);
    P->cadToolbar->setObjectName(QLatin1String("cadToolbar"));
    P->cadToolbar->setWindowTitle("CAD");
    addToolBar(Qt::LeftToolBarArea, P->cadToolbar);
#endif

    P->dockCADTool = new QDockWidget( this );
    P->dockCADTool->setObjectName(QLatin1String("dockCADTool"));
    //P->dockCADTool->setWindowTitle( "CAD Modeling" );
    P->dockCADTool->setFeatures(QDockWidget::NoDockWidgetFeatures);

    P->m_pCADTool = new FDYNCADModelingTool(this);
    P->dockCADTool->setWidget(P->m_pCADTool);
    //P->cadToolbar->addWidget( P->m_pCADTool );
    addDockWidget(Qt::LeftDockWidgetArea, P->dockCADTool);


    //Scenario Inspector
    P->dockScenario = new FDYNDockWidget(this);
    P->dockScenario->setObjectName(QLatin1String("dockScenario"));
    P->dockScenario->setTitleText( "SCENARIO" );
    P->dockScenario->setFeatures(QDockWidget::AllDockWidgetFeatures);

    P->scenarioMainWindow = new FDYNScenarioInspectorWindow();
    P->dockScenario->setWidget(P->scenarioMainWindow);
    addDockWidget(Qt::LeftDockWidgetArea, P->dockScenario);

    //FDYNScenarioInspector* scenarioTree = P->scenarioMainWindow->scenarioInspector();

    //Scene Inspector
    P->dockScene = new FDYNDockWidget(this);
    P->dockScene->setObjectName(QLatin1String("dockScene"));
    P->dockScene->setTitleText( "SCENE" );
    P->dockScene->setFeatures(QDockWidget::AllDockWidgetFeatures);

    P->sceneMainWindow = new FDYNSceneInspectorWindow();
    P->dockScene->setWidget(P->sceneMainWindow);
    addDockWidget(Qt::LeftDockWidgetArea, P->dockScene);

    //FDYNSceneInspector* sceneTree = P->sceneMainWindow->sceneInspector();

    //System Inspector
    P->dockSystem = new FDYNDockWidget(this);
    P->dockSystem->setObjectName(QLatin1String("dockSystem"));
    P->dockSystem->setTitleText( "SYSTEM" );
    P->dockSystem->setFeatures(QDockWidget::AllDockWidgetFeatures);

    P->systemMainWindow = new FDYNSystemInspectorWindow();
    P->dockSystem->setWidget(P->systemMainWindow);
    addDockWidget(Qt::LeftDockWidgetArea, P->dockSystem);

    //FDYNSystemInspector* systemTree = P->systemMainWindow->systemInspector();

    P->dockProperty = new FDYNDockWidget(this);
    P->dockProperty->setObjectName(QLatin1String("dockProperty"));
    P->dockProperty->setTitleText( "SETTINGS" );
    P->dockProperty->setFeatures(QDockWidget::AllDockWidgetFeatures);
    P->propertyMainWindow = new FDYNPropertyEditorWindow( );
    P->dockProperty->setWidget( P->propertyMainWindow );
    P->dockProperty->setMinimumWidth(350);
    addDockWidget(Qt::LeftDockWidgetArea, P->dockProperty);

    //QObject *propertyObj =  P->propertyMainWindow->objectController()->rootObject()->findChild<QObject*>("property_editor");


    P->dockFRANX     = new FDYNDockWidget( this );
    P->dockFRANX->setObjectName(QLatin1String("dockFRANX"));
    P->dockFRANX->setTitleText( "FRANX" );
    P->dockFRANX->setFeatures(QDockWidget::AllDockWidgetFeatures);

    P->franxMainWindow = new FDYNFRANXInspectorWindow();
    P->dockFRANX->setWidget(P->franxMainWindow);

    P->surfaceMaterialDlg = new FDYNSurfaceMaterialDialog(this);
    P->surfaceMaterialDlg->hide();

    P->fireMaterialDlg = new FDYNFireMaterialDialog(this);
    P->fireMaterialDlg->hide();

    P->failureCriteriaDlg = new FDYNFailureCriteriaDialog(this);
    P->failureCriteriaDlg->hide();

    P->ventCriteriaDlg = new FDYNVentCriteriaDialog(this);
    P->ventCriteriaDlg->hide();

    P->cableMaterialDlg = new FDYNCableDialog(this);
    P->cableMaterialDlg->hide();

    P->settingsDlg = new FDYNSettingsDialog( this );
    P->settingsDlg->hide();

    P->franxCalcDlg = new FDYNFRANXCalcDialog(this);
    P->franxCalcDlg->hide();

    /*
    QQuickWidget* dockFRANXView = new QQuickWidget(P->dockFRANX);
    dockFRANXView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    if( dockFRANXView )
    {
        dockFRANXView->setSource(QUrl("qrc:/FranxZone.qml"));
        P->dockFRANX->setWidget( dockFRANXView );
    }
    */
    addDockWidget(Qt::RightDockWidgetArea, P->dockFRANX);


    P->dockMsg = new FDYNDockWidget( this );
    P->dockMsg->setObjectName(QLatin1String("dockMsg"));
    P->dockMsg->setTitleText( "MESSAGES" );
    P->dockMsg->setFeatures(QDockWidget::AllDockWidgetFeatures);

    P->dockMsgView = new FDYNMessageWindow();
    P->dockMsg->setWidget(P->dockMsgView);
    addDockWidget(Qt::BottomDockWidgetArea, P->dockMsg);
    P->dockMsg->setContainedMainWindow(P->dockMsgView);


    //    P->dockMsgView = new QQuickWidget(P->dockMsg);
    //    P->dockMsgView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    //    P->dockMsgView->setSource(QUrl("qrc:/MessageZone.qml"));
    //    P->dockMsg->setWidget(P->dockMsgView);
    //    addDockWidget(Qt::BottomDockWidgetArea, P->dockMsg);


    // Unset the central widget,  completely dock based
    this->setCentralWidget(0);


    P->m_ResizeTimer.setParent(this);
    QObject::connect( &P->m_ResizeTimer, &QTimer::timeout, this, &FDYNMainWindow::resizeTimeout );

    for (int i = 0; i < FDYNMainWindowPrivate::MaxRecentFiles; ++i)
    {
        P->recentFileActs[i] = new QAction(this);
        P->recentFileActs[i]->setVisible( false );
        QObject::connect( P->recentFileActs[i], &QAction::triggered, this, &FDYNMainWindow::openRecentFile );
    }

    for (int i = 0; i < FDYNMainWindowPrivate::MaxRecentFiles; ++i)
        fileMenu->addAction( P->recentFileActs[i] );

    P->updateRecentFileActions();


    setDockOptions(QMainWindow::AllowTabbedDocks | QMainWindow::AllowNestedDocks);
    setDockNestingEnabled(true);
    //show();

    //    addActions(m_pTransformActions->actions());

    P->tweakUI();

    //P->timelineToolbar->setStyleSheet(toolBarStyle);

    this->setWindowTitle("");
#if 0
    // Shorten the interval
    P->m_tracker.setSendInterval( 5 * 1000 );

    P->m_tracker.startSession();
#endif

    QApplication::processEvents();
}

FDYNMainWindow::~FDYNMainWindow()
{
#if 0
    P->m_tracker.endSession();
#endif

    delete P->redoAct;
    delete P->undoAct;
    delete P->preferencesAct;

    delete P->dockScene;

    delete P->dockZone;
    delete P->dockScenario;
    delete P->dockSystem;


    delete P->dockProperty;

    delete P->dockFRANX;

    delete P->dockMsg;
    //delete P->dockMsgView;
    //    delete m_pSceneLoadingDialog;
    delete P->splashPixmap;
    delete P->m_pSceneLoadingSplash;

    delete P->separatorAct;

    delete P;
}

FDYNTimeline* FDYNMainWindow::Timeline() const
{
    return P->m_pTimeline;
}


void FDYNMainWindow::setWindowTitle(QString scene)
{
    QString arch("arch-unknown");
    QString bits("bit-unknown");
    QString compile("version-unknown");
    QString openmp("");
    QString toolset("");
#ifdef _DEBUG
    compile = QString("Debug");
#else
    compile = QString("Release");
#endif

#ifdef _OPENMP
    openmp = QString("OpenMP");
#endif

#ifdef NE_BUILD_64
    arch = QString("Windows");
#elif NE_BUILD_32
    bits = QString("32-bit");
#endif

    bits = QString("64-bit");

#if defined(_MSC_VER) && defined(__INTEL_COMPILER)
    if (__INTEL_COMPILER >= 1800)
        toolset = QString("ICC-18.0");
    else if (__INTEL_COMPILER >= 1700)
        toolset = QString("ICC-17.0");
#elif defined(_MSC_VER)
    if (_MSC_VER == 1900)
        toolset = QString("VC-140");
    else if (_MSC_VER >= 1910)
        toolset = QString("VC-141");
#endif

    arch = QString("Windows");

#if defined(NE_FOR_WINDOWS)
#elif defined(NE_FOR_LINUX)
    arch = QString("Linux");
#elif defined(NE_FOR_MAC)
    arch = QString("Mac");
#endif


    QMainWindow::setWindowTitle(tr("FRI3D v0.%6 - %1 (%2-%3-%4-%5)").arg(scene, arch, toolset, bits, compile, openmp));

}


bool FDYNMainWindow::newScene()
{
    FDYNGUI.newDB();

#if 0
    QString fileName;
    FDYNFileDialog dlg(this, NEBASE.SaveDir().path(), tr("Save Database"), tr("Scene files (*.fri3d)"),
                       FDYNFileDialog::SAVE);
    if (dlg.exec() == QDialog::Accepted)
        fileName = dlg.selectedFile();
    else
        return false;

    if(!fileName.isEmpty())
    {
        QString strippedFileName = QFileInfo(fileName).baseName();
        QString path = QFileInfo(fileName).path();
        QString dataPath = dlg.dataPath();

        P->setCurrentFile(path + "/" + strippedFileName + "." + NEBASE.FileExt() );
        NEBASE.create(fileName);
        return true;
    }
#endif

    return false;
}

///
/// \brief FDYNMainWindow::open
/// \param filePath
///

void FDYNMainWindow::open( QString filePath )
{
    QSettings settings("Fire Dynamics", "FRI3D");
    FDYNFileDialog dlg(this, settings.value("LastDir/LoadDB").toByteArray(),
                     tr("Load Database"), "*.sqlite;;*.f3d", FDYNFileDialog::OPEN,
                     QFileDialog::ExistingFiles);

    QStringList fileNames;
    if( !filePath.isEmpty() )
    {
        fileNames.append( filePath );
        FDYNGUI.loadDB(fileNames[0]);
        if (!fileNames[0].isEmpty())
            P->setCurrentFile(fileNames[0]);
    }
    else
    {
        if( dlg.exec() == QDialog::Accepted )
        {
            fileNames = dlg.selectedFiles();
            settings.setValue("LastDir/LoadDB", dlg.directory());
            FDYNGUI.loadDB(fileNames[0]);
            if (!fileNames[0].isEmpty())
                P->setCurrentFile(fileNames[0]);
        }
    }


#if 0
    if( !filePath.isNull() && filePath == P->getCurrentFile())
    {
        return;
    }
    QString dataPath = QString();
    if (filePath.isNull())
    {
        QSettings settings("Fire Dynamics", "FRI3D");
        FDYNFileDialog dlg(this, settings.value("lastOpenScenePath").toByteArray(),
                           tr("Open Scene"), tr("FDYN Scene files (*.fri3d)"), FDYNFileDialog::OPEN);
        if (dlg.exec() == QDialog::Accepted)
            filePath = dlg.selectedFile();
        else
            return;

        dataPath = dlg.dataPath();
    }

    if (!filePath.isEmpty() && QFile::exists(filePath))
    {
        NEBASE.open(filePath, dataPath);
    }
#endif



}


///
/// \brief FDYNMainWindow::openRecentFile
///

void FDYNMainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        open(action->data().toString());
}

bool FDYNMainWindow::save()
{
    FDYNGUI.saveDB();

#if 0
    if(NEBASE.SceneFilePath().path().isEmpty())
        return saveAs();
    else
    {
        NEBASE.save();
        return true;
    }
#endif

    return true;
}


bool FDYNMainWindow::saveAs()
{
    FDYNGUI.saveAsDB();
#if 0
    QString fileName;
    FDYNFileDialog dlg(this, NEBASE.SaveDir().path(), tr("Save scene as"), tr("Scene files (*.fri3d)"),
                       FDYNFileDialog::SAVE, QFileDialog::AnyFile );
    if (dlg.exec() == QDialog::Accepted)
        fileName = dlg.selectedFile();
    else
        return false;

    if (fileName.isEmpty())
        return false;

    QString strippedFileName = QFileInfo(fileName).baseName();
    QString path = QFileInfo(fileName).path();
    QString dataPath = dlg.dataPath();
    if (!dataPath.isEmpty() && QDir(dataPath).exists())
        path = dataPath;

    NEBASE.save(strippedFileName, path, false);


    P->setCurrentFile(path + "/" + strippedFileName + ".fri3d");
#endif
    return true;
}





void FDYNMainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    //
    // check for our needed mime type, here a file or a list of files
    //
    if( mimeData->hasUrls() )
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        // extract the local paths of the files
        for( int i = 0; i < urlList.size() && i < 32; ++i )
        {
            pathList.append( urlList.at(i).toLocalFile() );
            //qDebug() << "File Name " << pathList[i].toUtf8() ;
            NEBASE.import( pathList[i] );
        }
    }
}

QString FDYNMainWindowPrivate::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void FDYNMainWindowPrivate::setCurrentFile(const QString &fileName)
{
    QSettings settings( "Fire Dynamics", "FRI3D" );
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll( fileName );
    if (QFile::exists( fileName ))
        files.prepend( fileName );
    while( files.size() > MaxRecentFiles )
        files.removeLast();

    settings.setValue( "recentFileList", files );


    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        FDYNMainWindow *mainWin = qobject_cast<FDYNMainWindow *>(widget);
        if (mainWin)
        {
            updateRecentFileActions();
        }
    }

    m_curFile = fileName;
}

void FDYNMainWindowPrivate::updateRecentFileActions()
{

    QSettings settings( "Fire Dynamics", "FRI3D" );
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin( files.size(), (int)MaxRecentFiles );

    for( int i = 0; i < numRecentFiles; ++i )
    {
        QString text = QString("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText( text );
        recentFileActs[i]->setData( files[i] );
        recentFileActs[i]->setVisible( true );
    }


    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
    {

        recentFileActs[j]->setVisible( false );
    }


    separatorAct->setVisible( numRecentFiles > 0 );

}


bool FDYNMainWindow::exportCurrentFrame()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty())
        return false;

    NEBASE.exportCurrentFrame(dir);
    Log() << "Exporting frame " << NESCENE.CurrentFrame();
    EndLog(NE::kLogInfo);
    return true;
}

void FDYNMainWindow::renderCurrentFrame()
{
#if 0
    QString script = NERENDERMAN.objectName()+".exportFrameToExternalRenderer()";
    //FDYNGUI.EvalPyGUI(script);
#endif
}

void FDYNMainWindow::renderSequence()
{
#if 0
    QString startFrame = QString::number(FDYNGUI.P->timeline()->playbackStartSpinBox->value());
    QString endFrame = QString::number(FDYNGUI.P->timeline()->playbackEndSpinBox->value());
    QString script = NERENDERMAN.objectName()+".exportSequenceToExternalRenderer("+startFrame+","+endFrame+")";
    FDYNGUI.EvalPyGUI(script);
#endif
}

void FDYNMainWindow::renderDeleteSequence()
{
#if 0
    QString startFrame = QString::number(FDYNGUI.P->timeline()->playbackStartSpinBox->value());
    QString endFrame = QString::number(FDYNGUI.P->timeline()->playbackEndSpinBox->value());
    QString script = NERENDERMAN.objectName()+".exportSequenceRenderDelete("+startFrame+","+endFrame+")";
    FDYNGUI.EvalPyGUI(script);
#endif
}

void FDYNMainWindow::about()
{
    P->m_pAboutDialog->exec();
    QPoint windowCenter(this->width()*0.5, this->height()*0.5);
    QPoint dialogCenter(P->m_pAboutDialog->width(), P->m_pAboutDialog->height());
}

void FDYNMainWindow::closeEvent(QCloseEvent* event)
{
#if 0
    P->m_tracker.endSession();
#endif

    if (P->okToContinue() || !NEBASE.initialized())
    {
        event->accept();
        NEBASE.quit();
    }
    else
        event->ignore();
}

void FDYNMainWindow::hideEverythingForStartup()
{
    foreach(QObject* obj, children())
    {
        if(QWidget* w = qobject_cast<QWidget*>(obj))
        {
            w->hide();
        }
    }
    P->menuBar->show();
}

void FDYNMainWindow::hideAllWindows()
{
    P->sceneAct->setChecked(false);
    P->zoneAct->setChecked(false);
    P->scenaAct->setChecked(false);
    P->franxAct->setChecked(false);
    P->msgAct->setChecked(false);
    P->systemAct->setChecked(false);
    P->glInspectorAct->setChecked(false);
    P->propertyEditorAct->setChecked(false);
    P->shelfAct->setChecked(false);
}

void FDYNMainWindow::showAllWindows()
{
    P->sceneAct->setChecked(true);
    P->zoneAct->setChecked(true);
    P->scenaAct->setChecked(true);
    P->franxAct->setChecked(true);
    P->msgAct->setChecked(true);
    P->systemAct->setChecked(true);
    P->glInspectorAct->setChecked(true);
    P->propertyEditorAct->setChecked(true);
    P->shelfAct->setChecked(true);
    P->dockZone->raise();

}

void FDYNMainWindow::showGlWindow()
{
}

void FDYNMainWindow::maximizeGLWindowUnderCursor()
{
}


bool FDYNMainWindowPrivate::okToContinue()
{
    /*
    if(!NEBASE.SceneFilePath().path().isEmpty())
    {
        int r = QMessageBox::warning(S, QString("FRI3D"),
                                     QString("Do you want to save your changes?"),
                                     QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if( r == QMessageBox::Yes)
            return S->save();
        else if(r == QMessageBox::Cancel)
            return false;
    }
    */
    return true;
}

void FDYNMainWindow::resizeTimeout()
{
}

//QToolBar *FDYNMainWindow::timelineToolbar()
//{
//    return P->timelineToolbar;
//}



QMenu *FDYNMainWindow::creationMenu(uint nodeType)
{
    switch(nodeType)
    {
    case (NE::kObjectMaterial):
        return P->menuMaterials;
    case (NE::kObjectCamera):
        return P->menuCameras;
    case (NE::kObjectLight):
        return P->menuLights;
    case (NE::kObjectOSD):
        return P->menuOSD;
    case (NE::kObjectClipPlane):
    case (NE::kObjectGizmo):
        return P->menuGizmo;
    default:
        return P->menuAdd;
    }
}


void FDYNMainWindow::lockDocks(Qt::DockWidgetArea area)
{

}

void FDYNMainWindow::unlockDocks(Qt::DockWidgetArea area)
{

}

FDYNLoadingDialog* FDYNMainWindow::loadingWidget() const
{
    return P->m_pLoadingDialog;
}

void FDYNMainWindow::showLoadingWidget()
{
    QPoint windowCenter(this->width()*0.5, this->height()*0.5);
    windowCenter += this->pos();
    QPoint dialogCenter(P->m_pLoadingDialog->width()*0.5, P->m_pLoadingDialog->height()*0.5);

    P->m_pLoadingDialog->move(windowCenter-dialogCenter);
    P->m_pLoadingDialog->show();
    qApp->processEvents();
}

void FDYNMainWindow::showPropertyShortCutDlg(QWidget* glWidget, QObject *node, uint mode)
{
    NESpatial *spatial = qobject_cast<NESpatial *>(node);

    P->m_pPropertyShortCutDialg->setMode(glWidget, spatial, mode);

    QPoint windowCenter(this->width()*0.5, this->height()*0.5);
    windowCenter += this->pos();
    QPoint dialogCenter(P->m_pPropertyShortCutDialg->width()*0.5, P->m_pPropertyShortCutDialg->height()*0.5);

    P->m_pPropertyShortCutDialg->move(windowCenter-dialogCenter);
    P->m_pPropertyShortCutDialg->show();
    qApp->processEvents();
}

void FDYNMainWindow::showPropertyShortCutDlg(QWidget* glWidget)
{
    P->m_pPropertyShortCutDialg->setSnapUnitMode(glWidget);

    QPoint windowCenter(this->width()*0.5, this->height()*0.5);
    windowCenter += this->pos();
    QPoint dialogCenter(P->m_pPropertyShortCutDialg->width()*0.5, P->m_pPropertyShortCutDialg->height()*0.5);

    P->m_pPropertyShortCutDialg->move(windowCenter-dialogCenter);
    P->m_pPropertyShortCutDialg->show();
    qApp->processEvents();
}


void FDYNMainWindow::updateWindowVisibilityFlags()
{
    P->sceneAct->blockSignals(true);
    P->sceneAct->setChecked(P->dockScene->isVisible());
    P->sceneAct->blockSignals(false);

    P->zoneAct->blockSignals(true);
    P->zoneAct->setChecked(P->dockZone->isVisible());
    P->zoneAct->blockSignals(false);

    P->scenaAct->blockSignals(true);
    P->scenaAct->setChecked(P->dockScenario->isVisible());
    P->scenaAct->blockSignals(false);

    P->systemAct->blockSignals(true);
    P->systemAct->setChecked(P->dockSystem->isVisible());
    P->systemAct->blockSignals(false);

    P->franxAct->blockSignals(true);
    P->franxAct->setChecked(P->dockFRANX->isVisible());
    P->franxAct->blockSignals(false);

    P->glInspectorAct->blockSignals(true);
    foreach(GLDockWidgetBundle* b, FDYNGUI.P->m_IDtoGLDockWidget){
        P->glInspectorAct->setChecked(b->dockWidget->isVisible());
    }
    P->glInspectorAct->blockSignals(false);

    P->propertyEditorAct->blockSignals(true);
    P->propertyEditorAct->setChecked(P->dockProperty->isVisible());
    P->propertyEditorAct->blockSignals(false);

    P->msgAct->blockSignals(true);
    P->msgAct->setChecked(P->dockMsg->isVisible());
    P->msgAct->blockSignals(false);

    P->shelfAct->blockSignals(true);
    P->shelfAct->setChecked(P->shelfBar->isVisible());
    P->shelfAct->blockSignals(false);

}

FDYNPropertyEditorWindow *FDYNMainWindow::propertyEditorWindow()
{
    return P->propertyMainWindow;
}

FDYNDockWidget *FDYNMainWindow::sceneInspectorDock()
{
    return P->dockScene;
}

FDYNDockWidget *FDYNMainWindow::zoneInspectorDock()
{
    return P->dockZone;
}

FDYNSurfaceMaterialDialog *FDYNMainWindow::surfaceMaterialDialog()
{
    return P->surfaceMaterialDlg;
}

FDYNCableDialog *FDYNMainWindow::cableMaterialDialog()
{
    return P->cableMaterialDlg;
}

FDYNSettingsDialog *FDYNMainWindow::settingsDialog()
{
    return P->settingsDlg;
}

FDYNFRANXCalcDialog *FDYNMainWindow::franxCalcDialog()
{
    return P->franxCalcDlg;
}

FDYNFireMaterialDialog *FDYNMainWindow::fireMaterialDialog()
{
    return P->fireMaterialDlg;
}

FDYNFailureCriteriaDialog *FDYNMainWindow::failureCriteriaDialog()
{
    return P->failureCriteriaDlg;
}

FDYNVentCriteriaDialog *FDYNMainWindow::ventCriteriaDialog()
{
    return P->ventCriteriaDlg;
}


QDockWidget *FDYNMainWindow::CADDock()
{
    return P->dockCADTool;
}

FDYNCADModelingTool *FDYNMainWindow::CADModelingTool()
{
    return P->m_pCADTool;
}

FDYNSceneInspectorWindow *FDYNMainWindow::sceneInspectorWindow()
{
    return P->sceneMainWindow;
}

FDYNZoneInspectorWindow *FDYNMainWindow::zoneInspectorWindow()
{
    return P->zoneMainWindow;
}

FDYNFRANXInspectorWindow *FDYNMainWindow::franxInspectorWindow()
{
    return P->franxMainWindow;
}

FDYNDockWidget *FDYNMainWindow::scenarioInspectorDock()
{
    return P->dockScenario;
}

FDYNScenarioInspectorWindow* FDYNMainWindow::scenarioInspectorWindow()
{
    return P->scenarioMainWindow;
}


FDYNSystemInspectorWindow* FDYNMainWindow::systemInspectorWindow()
{
    return P->systemMainWindow;
}

FDYNMessageWindow* FDYNMainWindow::messageWindow()
{
    return P->dockMsgView;

}

void FDYNMainWindow::removeSelectedItem()
{
    QSettings settings("Fire Dynamics", "FRI3D");
    bool checked = settings.value("dontShowAgainInSelected", false).toBool();

    if(!checked)
    {
        QMessageBox msgBox(QMessageBox::Warning,
                           QString("FRI3D"),QString("Do you want to delete selected item?"),
                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
        QCheckBox dontShowCheck("Don't show again this message.");
        dontShowCheck.setChecked(false);
        msgBox.setCheckBox(&dontShowCheck );
        int result = msgBox.exec();

        settings.setValue("dontShowAgainInSelected", dontShowCheck.isChecked());
        if(result == QMessageBox::Yes)
        {
            P->sceneMainWindow->removeSelectedItem();
            P->zoneMainWindow->removeSelectedItem();
            P->scenarioMainWindow->removeSelectedItem();
        }
    }
    else
    {
        P->sceneMainWindow->removeSelectedItem();
        P->zoneMainWindow->removeSelectedItem();
        P->scenarioMainWindow->removeSelectedItem();
    }
}

void FDYNMainWindow::connectGLDockWidgetToAtcion(FDYNDockWidget *glDock)
{
    connect(P->glInspectorAct, &QAction::triggered, glDock, &QWidget::setVisible);
    connect(glDock, &FDYNDockWidget::dockVisible, this, &FDYNMainWindow::checkGLInspectorAct);
}


void FDYNMainWindow::checkGLInspectorAct(bool checked)
{
    bool isChecked = false;
    foreach (GLDockWidgetBundle *bundle, FDYNGUI.P->m_IDtoGLDockWidget)
    {
        if(bundle->dockWidget->isVisible()){
            isChecked = true;
            break;
        }
    }

    if(isChecked)
        P->glInspectorAct->setChecked(true);
    else
        P->glInspectorAct->setChecked(false);
}

QDockWidget *FDYNMainWindow::topDockWidget()
{
    return P->currentTopWidget;
}

void FDYNMainWindow::setTopDockWidget(QDockWidget *activatedWidget)
{
    P->currentTopWidget = activatedWidget;

    FDYNGUI.setPropertyVisibleAsActiveDock();

}

void FDYNMainWindowPrivate::tweakUI()
{

    //    timelineToolbar->addWidget(m_pTimeline);
    //    S->tabifyDockWidget(dockScene, dockZone);
#if 1
    // Set the corner occupation for dock widgets
    S->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    S->setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
    // Tabify dock widgets
    //S->tabifyDockWidget(S->ui->propertyEditorDock, S->ui->connectionInspectorDock);
    //S->tabifyDockWidget(S->ui->connectionInspectorDock, S->ui->keyframeInspectorDock);
    //S->ui->propertyEditorDock->raise();

    S->tabifyDockWidget(dockZone, dockScenario);
    S->tabifyDockWidget(dockScenario, dockScene);
    S->tabifyDockWidget(dockScene, dockSystem);
    dockZone->raise();

    //    S->tabifyDockWidget(S->ui->sceneInspectorDock, S->ui->systemInspectorDock);
    //    S->tabifyDockWidget(S->ui->systemInspectorDock, S->ui->sceneBuilderDock);

    //    S->tabifyDockWidget(S->ui->sceneBuilderDock, S->ui->expressionInspectorDock);
    //    S->ui->sceneInspectorDock->raise();
    //    S->ui->messageDock->raise();

    // Add the timeline widget into the timeline toolbar
    timelineToolbar->addWidget(m_pTimeline);

#if CADTOOLBAR
    if( m_pCADTool )
        cadToolbar->addWidget(m_pCADTool);
#endif
    //S->ui->timelineToolbar->addWidget(m_pTimeline);


    //    S->ui->consoleDock->setContainedMainWindow(S->ui->consoleWindowContents);
    //    S->ui->expressionEditorDock->setContainedMainWindow(S->ui->expressionEditor);
    //    S->ui->expressionInspectorDock->setContainedMainWindow(S->ui->expressionInspector);
    //    S->ui->keyframeInspectorDock->setContainedMainWindow(S->ui->keyframeInspector);
    //    S->ui->connectionInspectorDock->setContainedMainWindow(S->ui->connectionInspector);
    //dockScene->setContainedMainWindow(sceneMainWindow);
    //    S->ui->systemInspectorDock->setContainedMainWindow(S->ui->systemInspector);
    //    S->ui->messageDock->setContainedMainWindow(S->ui->messageWindow);
    //    S->ui->sceneBuilderDock->setContainedMainWindow(S->ui->sceneBuilder);
    //    S->ui->propertyEditorDock->setContainedMainWindow(S->ui->propertyEditor);
    //    S->ui->nodeGraphEditorDock->setContainedMainWindow(S->ui->nodeGraphEditor);
    //    S->ui->gpuBuffersDock->setContainedMainWindow(S->ui->gpuBuffersWindow);

    S->connect(sceneAct, &QAction::triggered, dockScene, &QWidget::setVisible);
    S->connect(dockScene, &FDYNDockWidget::dockVisible, sceneAct, &QAction::setChecked);

    S->connect(zoneAct, &QAction::triggered, dockZone, &QWidget::setVisible);
    S->connect(dockZone, &FDYNDockWidget::dockVisible, zoneAct, &QAction::setChecked);

    S->connect(scenaAct, &QAction::triggered, dockScenario, &QWidget::setVisible);
    S->connect(dockScenario, &FDYNDockWidget::dockVisible, scenaAct, &QAction::setChecked);

    S->connect(systemAct, &QAction::triggered, dockSystem, &QWidget::setVisible);
    S->connect(dockSystem, &FDYNDockWidget::dockVisible, systemAct, &QAction::setChecked);

    S->connect(franxAct, &QAction::triggered, dockFRANX, &QWidget::setVisible);
    S->connect(dockFRANX, &FDYNDockWidget::dockVisible, franxAct, &QAction::setChecked);

    S->connect(propertyEditorAct, &QAction::triggered, dockProperty, &QWidget::setVisible);
    S->connect(dockProperty, &FDYNDockWidget::dockVisible, propertyEditorAct, &QAction::setChecked);

    S->connect(msgAct, &QAction::triggered, dockMsg, &QWidget::setVisible);
    S->connect(dockMsg, &FDYNDockWidget::dockVisible, msgAct, &QAction::setChecked);

    S->connect(shelfAct, &QAction::triggered, shelfBar, &QWidget::setVisible);

    //    S->connect(S->ui->actionActivatePropertyEditor, &QAction::triggered, S->ui->propertyEditorDock, &QWidget::setVisible);
    //    S->connect(S->ui->propertyEditorDock, &NEDockWidget::dockVisible, S->ui->actionActivatePropertyEditor, &QAction::setChecked);

    //    S->connect(S->ui->actionGL_Buffers, &QAction::triggered, S->ui->gpuBuffersDock, &QWidget::setVisible);
    //    S->connect(S->ui->gpuBuffersDock, &NEDockWidget::dockVisible, S->ui->actionGL_Buffers, &QAction::setChecked);

    //    S->connect(S->ui->actionActivateMainWindow, &QAction::triggered, S, &NEMainWindow::showGlWindow);

    //    S->connect(S->ui->actionShowAllWindows, &QAction::triggered, S, &FDYNMainWindow::showAllWindows);
    //    S->connect(S->ui->actionHideAllWindows, &QAction::triggered, S, &FDYNMainWindow::hideAllWindows);

    S->connect(aboutAct, &QAction::triggered, S, &FDYNMainWindow::about);
    //    S->connect(S->ui->actionReload_Stylesheet, &QAction::triggered, &NEGUISTYLE, &NEGUIStyle::reloadAndApplyStyleSheet);

    S->connect(removeAct, &QAction::triggered, S, &FDYNMainWindow::removeSelectedItem);


    //    S->connect(S->ui->actionExternal_Renderer_Settings, &QAction::triggered, &NEGUI, &NEGUIManager::showExternalRendererSettingsDialog);
    //    S->connect(S->ui->actionColliderDialog, &QAction::triggered, &NEGUI, &NEGUIManager::showColliderDialog);
    //    S->connect(S->ui->actionHelp, &QAction::triggered, &NEGUI, &NEGUIManager::showHelp);
    //    S->connect(S->ui->actionOpenGL_update, &QAction::toggled, &NEGUI, &NEGUIManager::setOpenGLUpdate);
    //    //connect(mainWindow()->actionGL_Buffer&NEGUI, SIGNAL(toggled(bool)), thi&NEGUI, SLOT(setShowGLBuffers(bool)));
    //    S->connect(hidenActionCursor, &QAction::toggled, &FDYNGUI, &FDYNGUIManager::selectCursorMode);
    //    S->connect(hidenActionTranslate, &QAction::toggled, &FDYNGUI, &FDYNGUIManager::selectTranslationMode);
    //    S->connect(hidenActionRotate, &QAction::toggled, &FDYNGUI, &FDYNGUIManager::selectRotationMode);
    //    S->connect(hidenActionScale, &QAction::toggled, &FDYNGUI, &FDYNGUIManager::selectScaleMode);
    //    S->connect(S->ui->actionSet_Selected_Active, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedActive);
    //    S->connect(S->ui->actionSet_Selected_Cache, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedCache);
    //    S->connect(S->ui->actionSet_Selected_Inactive, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedInactive);
    //    S->connect(S->ui->actionSet_Selected_Visible, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedVisibile);
    //    S->connect(S->ui->actionSet_Selected_Hidden, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedHidden);
    //    S->connect(S->ui->actionSet_Selected_Bounding_Box, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedBoundingBox);
    //    S->connect(S->ui->actionWireframe, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedWireframe);
    //    S->connect(S->ui->actionSet_Selected_Flat_Shaded, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedFlatShaded);
    //    S->connect(S->ui->actionSet_Selected_Smooth_Shaded, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedSmoothShaded);
    S->connect(groupAct, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::setSelectedGroup);
    S->connect(unGroupAct, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::setSelectedUngroup);
    S->connect(parentAct, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::setSelectedParent);
    S->connect(unParentAct, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::setSelectedUnparent);
    //    S->connect(S->ui->actionFull_screen, &QAction::toggled, &NEGUI, &NEGUIManager::fullscreenToggle);
    //    S->connect(S->ui->actionPreferences, &QAction::triggered, &NEGUI, &NEGUIManager::showPreferencesDialog);
    //    //    S->connect(S->ui->actionMaximize, &QAction::triggered, &NEGUI, &NEGUIManager::maximize);
    S->connect(undoAct, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::undo);
    S->connect(redoAct, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::redo);

    S->connect(computefranxAct, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::computeFRANXOptions );

    //    S->connect(S->ui->actionPreferences, &QAction::triggered, S, &NEMainWindow::showPreferencesDialog);

    S->connect(S, SIGNAL(tabifiedDockWidgetActivated(QDockWidget *)), S, SLOT(setTopDockWidget(QDockWidget *)));
#endif

    //QApplication::processEvents();


}


void FDYNMainWindow::keyPressEvent( QKeyEvent* event )
{
#if 0
    if(event->isAutoRepeat())
        return;

    if(event->modifiers() == Qt::ControlModifier)
    {
        //if(event->modifiers() == Qt::KeypadModifier)
        //{
        QStringList tmp;
        switch(event->key())
        {
        case Qt::Key_Right:
            NESCENE.startUpdateTimer(false);
            if(MAINWINDOW_DEBUG)
            {
                Log() << "Ctrl + Right pressed.";
                EndLog(NE::kLogInfo);
            }
            //update scene
            NESCENE.update();
            //update property editor
            FDYNGUI.updatePropertyEditor();
            //update gl content
            FDYNGUI.updateGLWindows();
            break;
        case Qt::Key_Down:
            NESCENE.startUpdateTimer(true);
            break;
        case Qt::Key_K:
            tmp.push_back("Position");
            tmp.push_back("Orientation");
            tmp.push_back("Scale");
            NESCENE.setKeyFrameForSelectedNode(tmp, NE::kLinear);
            break;
        case Qt::Key_L:
            tmp.push_back("Position");
            tmp.push_back("Orientation");
            tmp.push_back("Scale");
            NESCENE.setKeyFrameForSelectedNode(tmp, NE::kSinusoidal);
            break;


        }
    }
    else
    {
        switch(event->key())
        {
        case Qt::Key_Escape:
            if(FDYNGUI.P->timeline()->playButton->isChecked())
            {
                FDYNGUI.P->timeline()->playButton->toggle();
                Log() << "Simulation/Playback stopped";
                EndLog(NE::kLogInfo);
            }
            break;

        case Qt::Key_Space:
            maximizeGLWindowUnderCursor();
            break;

        }
    }
    event->ignore();
#endif
}

void FDYNMainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    P->timelineToolbar->resize(event->size().width(), P->timelineToolbar->height());
#if CADTOOLBAR
    P->cadToolbar->resize(event->size().width(), P->cadToolbar->height());
#endif

}

void FDYNMainWindow::on_actionDuplicate_triggered()
{
    NESCENE.duplicateSelectedObjects();
}

void FDYNMainWindow::registerRemoveAction(QAction* a)
{
    QObject::connect(a, &QAction::triggered, P->removeAct, &QAction::triggered);
}

