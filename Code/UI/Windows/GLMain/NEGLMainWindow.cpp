#include <QSignalMapper>

#include "NEGLMainWindow.h"
#include "ui_NEGLMainWindow.h"
#include "NEGLWidget.h"
//#include "Rendering/Utility/NEOsd.h"

#include "Managers/NEGUIManager.h"
#include "Managers/NEGUIManager_p.h"
#include "Windows/Main/NEMainWindow.h"


#include "Widgets/Dock/NEDockWidget.h"
#include "Base/NESceneManager.h"
#include "Utilities/NEGUIStyle.h"

#include "Rendering/Spatial/NECamera.h"
#include "Rendering/Manager/NERenderManager.h"


NEGLMainWindow::NEGLMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NEGLMainWindow)
{
    ui->setupUi(this);
    // Create the menu where the cameras will be stored
    m_pCamMenu = new QMenu("Camera", menuBar());
    //    m_pCamMenu->setFont(font);
    // Add the camera menu into the menubar
    this->menuBar()->addMenu(m_pCamMenu);
    // Create an action group for camera selection
    m_pActionGroup = new QActionGroup(this);
    m_pActionGroup->setObjectName("Camera");
    // Add the action group actions into the main window
    addActions(m_pActionGroup->actions());

}

NEGLMainWindow::~NEGLMainWindow()
{
    delete ui;
}

QMenu *NEGLMainWindow::camMenu()
{
    return m_pCamMenu;
}

void NEGLMainWindow::addCamera(NECamera* cam)
{
    if (!cam)
        return;
    // Create the action that is triggered when the camera is selected
    QAction* camAction = m_pActionGroup->addAction(cam->objectName());
    // Add the action into the camera menu
    camMenu()->addAction(camAction);
    camAction->setCheckable(true);
    // Use a signal mapper to map the triggered signal of the action to camera selection
    QSignalMapper* signalMapper = new QSignalMapper(camAction);
    connect(camAction, SIGNAL(triggered()), signalMapper, SLOT(map())); //ATTENTION
    signalMapper->setMapping(camAction, cam);
    connect(signalMapper, SIGNAL(mapped(QObject*)), this, SLOT(setCamera(QObject*)));
    // Put the generated camera to the camera to action map
    m_CameraToAction[cam] = camAction;
}

void NEGLMainWindow::addCameras(QMap<QString, NECamera*> cams)
{
    foreach(NECamera* cam, cams)
        addCamera(cam);
}

void NEGLMainWindow::removeCamera(NECamera* cam)
{
    // If the camera is used; set the window's camera to another camera before removing the camera.
    if(cam == m_pBundle->glWidget->camera())
        setCamera( NERENDERMAN.defaultCamera() );

    // Retrive the action of the camera
    QAction* camAction = m_CameraToAction[cam];
    // Remove the action from the action group
    m_pActionGroup->removeAction(camAction);
    // Remove the action from the camera menu
    camMenu()->removeAction(camAction);
    // Remove the camera from the camera to action map
    m_CameraToAction.remove(cam);
}

void NEGLMainWindow::setupConnections()
{
    // Connect relavent menu actions to the appropriate slow of the glwidget's propery class
    connect(m_pBundle->mainWindow->ui->actionGrid, &QAction::toggled, m_pBundle->glWidget->P(), &NEGLWidgetProperties::setDrawGrid);
    connect(m_pBundle->mainWindow->ui->actionShow_OSD, &QAction::toggled, m_pBundle->glWidget->P(), &NEGLWidgetProperties::setShowOsd);
    connect(m_pBundle->mainWindow->ui->actionDynamicQuality, &QAction::toggled, m_pBundle->glWidget->P(), &NEGLWidgetProperties::setDynamicQuality);
    connect(m_pBundle->mainWindow->ui->actionClip, &QAction::toggled, m_pBundle->glWidget->P(), &NEGLWidgetProperties::setClipping);

    //
    // Actions from the menu/other items first sent to GLWidget and then forwarded from there onto GUIManager
    //
    connect(m_pBundle->mainWindow->ui->actionSplit_horizontally, &QAction::triggered, m_pBundle->glWidget, &NEGLWidget::splitHorizontally);
    connect(m_pBundle->mainWindow->ui->actionSplit_vertically, &QAction::triggered, m_pBundle->glWidget, &NEGLWidget::splitVertically);
    connect(m_pBundle->mainWindow->ui->actionSplit_to_quad, &QAction::triggered, m_pBundle->glWidget, &NEGLWidget::splitToQuad);
    connect(m_pBundle->mainWindow->ui->actionHide, &QAction::triggered, m_pBundle->glWidget, &NEGLWidget::hide);
    connect(m_pBundle->mainWindow->ui->actionClose, &QAction::triggered, m_pBundle->glWidget, &NEGLWidget::close);

    connect(m_pBundle->glWidget, &NEGLWidget::splitGLDockHorizontally,&NEGUI, &NEGUIManager::splitGLDockHorizontally);
    connect(m_pBundle->glWidget, &NEGLWidget::splitGLDockVertically, &NEGUI, &NEGUIManager::splitGLDockVertically);
    connect(m_pBundle->glWidget, &NEGLWidget::splitGLDockToQuad, &NEGUI, &NEGUIManager::splitGLDockToQuad);
    connect(m_pBundle->glWidget, &NEGLWidget::hideGLWidget, &NEGUI, &NEGUIManager::hideGLWidget);
    connect(m_pBundle->glWidget, &NEGLWidget::closeGLWidget, &NEGUI, &NEGUIManager::closeGLWidget);

    //
    // Direct ACtions which can be done in GLWidget
    connect(m_pBundle->mainWindow->ui->actionDraw_render_area, &QAction::toggled, m_pBundle->glWidget->P(), &NEGLWidgetProperties::setDrawRenderArea);
    connect(m_pBundle->mainWindow->ui->actionShow_Axes, &QAction::toggled, m_pBundle->glWidget->P(), &NEGLWidgetProperties::setViewAxes);

    // Connect relavent menu actions to update the opengl windows
    connect(m_pBundle->mainWindow->ui->actionGrid, &QAction::toggled, &NEGUI, &NEGUIManager::updateGLWindows);
    connect(m_pBundle->mainWindow->ui->actionShow_OSD, &QAction::toggled, &NEGUI, &NEGUIManager::updateGLWindows);
    connect(m_pBundle->mainWindow->ui->actionClip, &QAction::toggled, &NEGUI, &NEGUIManager::updateGLWindows);
    connect(m_pBundle->mainWindow->ui->actionDraw_render_area, &QAction::toggled, &NEGUI, &NEGUIManager::updateGLWindows);
    connect(m_pBundle->mainWindow->ui->actionShow_Axes, &QAction::toggled, &NEGUI, &NEGUIManager::updateGLWindows);

    // Connect relavent menu actions to update the property editor
    connect(m_pBundle->mainWindow->ui->actionGrid, &QAction::toggled, &NEGUI, &NEGUIManager::updatePropertyEditor);
    connect(m_pBundle->mainWindow->ui->actionShow_OSD, &QAction::toggled, &NEGUI, &NEGUIManager::updatePropertyEditor);
    connect(m_pBundle->mainWindow->ui->actionClip, &QAction::toggled, &NEGUI, &NEGUIManager::updatePropertyEditor);
    connect(m_pBundle->mainWindow->ui->actionDraw_render_area, &QAction::toggled, &NEGUI, &NEGUIManager::updatePropertyEditor);
    connect(m_pBundle->mainWindow->ui->actionShow_Axes, &QAction::toggled, &NEGUI, &NEGUIManager::updatePropertyEditor);

    connect(m_pBundle->glWidget, &NEGLWidget::updatePropertyEditorUI, &NEGUI, &NEGUIManager::updatePropertyEditor);

    // Connect the changed signals of the properties to update the menu action states appropriately
    connect(m_pBundle->glWidget->P(), &NEGLWidgetProperties::showOsdChanged, ui->actionShow_OSD, &QAction::setChecked);
    connect(m_pBundle->glWidget->P(), &NEGLWidgetProperties::drawGridChanged, ui->actionGrid, &QAction::setChecked);
    connect(m_pBundle->glWidget->P(), &NEGLWidgetProperties::dynamicQualityChanged, ui->actionDynamicQuality, &QAction::setChecked);
    connect(m_pBundle->glWidget->P(), &NEGLWidgetProperties::clippingChanged, ui->actionClip, &QAction::setChecked);
    connect(m_pBundle->glWidget->P(), &NEGLWidgetProperties::viewAxesChanged, ui->actionShow_Axes, &QAction::setChecked);
    connect(m_pBundle->glWidget->P(), &NEGLWidgetProperties::drawRenderAreaChanged, ui->actionDraw_render_area, &QAction::setChecked);


    /*
    connect( m_pBundle->glWidget, SIGNAL(addOSD(uint, const NEOsdMsg&)),
             &NEOSD, SLOT(add(uint, const NEOsdMsg&)),
             Qt::DirectConnection );
             */

    //connect(m_pBundle->glWidget, &NEGLWidget::drawOSD, &NEOSD, &NEOsd::draw);
    connect(m_pBundle->glWidget, &NEGLWidget::setCameraUI, this, &NEGLMainWindow::setCamera);
    connect(m_pBundle->glWidget, &NEGLWidget::setMenuContextMenuPolicySignal, this, &NEGLMainWindow::setContextMenuPolicyFromGLWidget);




    connect(ui->actionFrameSelection, &QAction::triggered, m_pBundle->glWidget, &NEGLWidget::frameSelection);
    connect(ui->actionMaximize, SIGNAL(toggled(bool)) , this, SLOT(maximizeGLWindow(bool)));

}

void NEGLMainWindow::setContextMenuPolicyFromGLWidget(int menuPolicy)
{
    setContextMenuPolicy((Qt::ContextMenuPolicy)menuPolicy);
}



void NEGLMainWindow::setCamera(QObject *cam)
{
    // If camera is already set, return early
    if(!cam || !m_pBundle->glWidget->P()->setCamera(cam->objectName()))
        return;

    updateUIForCurrentCamera(cam);


    // Set the given camera as the OpenGL camera
    //m_pBundle->glWidget->setCamera(cam);
    //m_pBundle->glWidget->P()->setCamera(cam->objectName());
}

void NEGLMainWindow::updateUIForCurrentCamera(QObject* cam)
{
    if (!cam)
        return;
    // Toggle the action, if camera is set by other means (e.g. console)
    if(!m_CameraToAction[static_cast<NECamera*>(cam)]->isChecked())
        m_CameraToAction[static_cast<NECamera*>(cam)]->toggle();

    // Change the dock widget's title text as the camera name
    m_pBundle->dockWidget->setTitleText(m_pBundle->glWidget->objectName() + " - " + cam->objectName());

    if(UpdateUI())
        NEGUI.updatePropertyEditor();

}

void NEGLMainWindow::maximizeGLWindow(bool val)
{
    //    if(!m_pBundle->glWidget->hasFocus())
    //        return;


    //qDebug() << Q_FUNC_INFO <<" "<< m_pBundle->glWidget;
    if(val)
    {
        uiStateBeforeMaxed = NEGUI.mainWindow()->saveState();
        foreach(GLDockWidgetBundle* b, NEGUI.P->m_IDtoGLDockWidget)
            if(b->mainWindow != this)
                NEGUI.hideGLWidget( b->glWidget );
    }
    else
        NEGUI.mainWindow()->restoreState(uiStateBeforeMaxed);
}

bool NEGLMainWindow::isMaximized()
{
    return ui->actionMaximize->isChecked();

}

void NEGLMainWindow::maximizeToggle()
{
    ui->actionMaximize->toggle();

}


