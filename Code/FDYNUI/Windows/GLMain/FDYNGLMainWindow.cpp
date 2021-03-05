#include <QSignalMapper>
#include <QQuickItem>

#include "FDYNGLMainWindow.h"
//#include "ui_NEGLMainWindow.h"
#include "NEGLWidget.h"
//#include "Rendering/Utility/NEOsd.h"

#include "Managers/FDYNGUIManager.h"
#include "Managers/FDYNGUIManager_p.h"
#include "Windows/Main/FDYNMainWindow.h"
#include "Widgets/MenuBar/FDYNMenuBar.h"


#include "Widgets/Dock/FDYNDockWidget.h"
#include "Widgets/CADModeling/FDYNCADModelingTool.h"
#include "Base/NESceneManager.h"

#include "Rendering/Spatial/NECamera.h"
#include "Rendering/Manager/NERenderManager.h"


FDYNGLMainWindow::FDYNGLMainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setMenuBar(new FDYNMenuBar);

    m_pViewMenu = new QMenu("View", menuBar());

    actionGrid = new QAction(this);
    actionGrid->setObjectName(QLatin1String("actionGrid"));
    actionGrid->setCheckable(true);
    actionGrid->setChecked(true);

    actionDraw_render_area = new QAction(this);
    actionDraw_render_area->setObjectName(QLatin1String("actionDraw_render_area"));
    actionDraw_render_area->setCheckable(true);
    QIcon icon;
    icon.addFile(QLatin1String(":/Neutrino/Resources/16/photo.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionDraw_render_area->setIcon(icon);

    actionSplit_horizontally = new QAction(this);
    actionSplit_horizontally->setObjectName(QLatin1String("actionSplit_horizontally"));
    actionSplit_horizontally->setCheckable(false);
    actionSplit_horizontally->setChecked(false);
    QIcon icon1;
    icon1.addFile(QLatin1String(":/Neutrino/Resources/16/application-split.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionSplit_horizontally->setIcon(icon1);
    actionSplit_vertically = new QAction(this);
    actionSplit_vertically->setObjectName(QLatin1String("actionSplit_vertically"));
    actionSplit_vertically->setCheckable(false);
    QIcon icon2;
    icon2.addFile(QLatin1String(":/Neutrino/Resources/16/application-split-vertical.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionSplit_vertically->setIcon(icon2);
    actionSplit_to_quad = new QAction(this);
    actionSplit_to_quad->setObjectName(QLatin1String("actionSplit_to_quad"));
    QIcon icon3;
    icon3.addFile(QLatin1String(":/Neutrino/Resources/16/application-split-tile.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionSplit_to_quad->setIcon(icon3);
    actionShow_OSD = new QAction(this);
    actionShow_OSD->setObjectName(QLatin1String("actionShow_OSD"));
    actionShow_OSD->setCheckable(true);
    actionShow_OSD->setChecked(true);
//    QIcon icon4;
//    icon4.addFile(QLatin1String(":/Neutrino/Resources/16/application_side_boxes.png"), QSize(), QIcon::Normal, QIcon::Off);
//    actionShow_OSD->setIcon(icon4);
    actionClip = new QAction(this);
    actionClip->setObjectName(QLatin1String("actionClip"));
    actionClip->setCheckable(true);
    QIcon icon5;
    icon5.addFile(QLatin1String(":/Neutrino/Resources/16/cut.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionClip->setIcon(icon5);
    actionClose = new QAction(this);
    actionClose->setObjectName(QLatin1String("actionClose"));
    QIcon icon6;
    icon6.addFile(QLatin1String(":/Neutrino/Resources/16/application_delete.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionClose->setIcon(icon6);
    actionShow_Axes = new QAction(this);
    actionShow_Axes->setObjectName(QLatin1String("actionShow_Axes"));
    actionShow_Axes->setCheckable(true);
    actionShow_Axes->setChecked(true);
    actionFrameSelection = new QAction(this);
    actionFrameSelection->setObjectName(QLatin1String("actionFrameSelection"));
    actionFrameSelection->setCheckable(false);
    QIcon icon7;
    icon7.addFile(QLatin1String(":/Neutrino/Resources/16/shape_handles.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionFrameSelection->setIcon(icon7);
    actionFrameSelection->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    actionFrameSelection->setAutoRepeat(false);
    actionHide = new QAction(this);
    actionHide->setObjectName(QLatin1String("actionHide"));
    QIcon icon8;
    icon8.addFile(QLatin1String(":/Neutrino/Resources/16/application_put.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionHide->setIcon(icon8);
    actionMaximize = new QAction(this);
    actionMaximize->setObjectName(QLatin1String("actionMaximize"));
    actionMaximize->setCheckable(true);
    QIcon icon9;
    icon9.addFile(QLatin1String(":/Neutrino/Resources/16/arrow_out.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionMaximize->setIcon(icon9);
    actionMaximize->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    actionMaximize->setAutoRepeat(false);
    actionDynamicQuality = new QAction(this);
    actionDynamicQuality->setObjectName(QLatin1String("actionDynamicQuality"));
    actionDynamicQuality->setCheckable(true);

    m_pViewMenu->addAction(actionFrameSelection);
    m_pViewMenu->addSeparator();
    m_pViewMenu->addAction(actionGrid);
    //m_pViewMenu->addAction(actionDraw_render_area);
    m_pViewMenu->addAction(actionShow_Axes);
    m_pViewMenu->addAction(actionShow_OSD);
    m_pViewMenu->addAction(actionDynamicQuality);
    m_pViewMenu->addSeparator();
    m_pViewMenu->addAction(actionSplit_horizontally);
    m_pViewMenu->addAction(actionSplit_vertically);
    m_pViewMenu->addAction(actionSplit_to_quad);
    m_pViewMenu->addSeparator();
    //m_pViewMenu->addAction(actionClip);
    m_pViewMenu->addSeparator();
    m_pViewMenu->addAction(actionMaximize);
    m_pViewMenu->addAction(actionHide);
    //m_pViewMenu->addAction(actionClose);

    actionGrid->setText("Show grid");
    //actionDraw_render_area->setText("Show render area");
    actionSplit_horizontally->setText("Split horizontally");
    actionSplit_vertically->setText("Split vertically");
    actionSplit_to_quad->setText("Split to quad");
    actionShow_OSD->setText("Show OSD");
    //actionClip->setText("Clip");
    actionClose->setText("Close");
    actionShow_Axes->setText("Show Axes");
    actionFrameSelection->setText("Frame Selection");
#ifndef QT_NO_SHORTCUT
    actionFrameSelection->setShortcut(QKeySequence("F"));
#endif // QT_NO_SHORTCUT
    actionHide->setText("Hide");
    actionMaximize->setText("Maximized");
    actionDynamicQuality->setText("Dynamic Quality");


    // Create the menu where the cameras will be stored
    m_pCamMenu = new QMenu("Camera", menuBar());

    actionResetCamera = new QAction(this);
    actionResetCamera->setObjectName(QLatin1String("actionMaximize"));
    actionResetCamera->setCheckable(false);
    actionResetCamera->setText("Reset");

    m_pCamMenu->addAction(actionResetCamera);


    //    m_pCamMenu->setFont(font);
    // Add the camera menu into the menubar
    this->menuBar()->addMenu(m_pViewMenu);
    this->menuBar()->addMenu(m_pCamMenu);
    // Create an action group for camera selection
    m_pActionGroup = new QActionGroup(this);
    m_pActionGroup->setObjectName("Camera");
    // Add the action group actions into the main window
    addActions(m_pActionGroup->actions());

}

FDYNGLMainWindow::~FDYNGLMainWindow()
{
    QQuickWidget *qWidget = qobject_cast<QQuickWidget *>(centralWidget());
    if(qWidget)
    {
        delete qWidget;
        qWidget = 0;
    }

    //delete ui;

    delete actionGrid;
    delete actionDraw_render_area;
    delete actionSplit_horizontally;
    delete actionSplit_vertically;
    delete actionSplit_to_quad;
    delete actionShow_OSD;
    delete actionClip;
    delete actionClose;
    delete actionShow_Axes;
    delete actionFrameSelection;
    delete actionHide;
    delete actionMaximize;
    delete actionDynamicQuality;
    delete actionResetCamera;

//    delete m_pViewMenu;
//    delete m_pActionGroup;
//    delete m_pCamMenu;


}

QMenu *FDYNGLMainWindow::camMenu()
{
    return m_pCamMenu;
}

void FDYNGLMainWindow::addCamera(NECamera* cam)
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

void FDYNGLMainWindow::addCameras(QMap<QString, NECamera*> cams)
{
    foreach(NECamera* cam, cams)
        addCamera(cam);
}

void FDYNGLMainWindow::removeCamera(NECamera* cam)
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

void FDYNGLMainWindow::selectCursorMode(bool checked)
{
    FDYNGUI.selectCursorMode(checked);

    if(checked)
        btnCursor->setProperty("checked", true);
    else
        btnCursor->setProperty("checked", false);

    QVariant returnValue;
    QObject* cadToolObj = FDYNGUI.mainWindow()->CADModelingTool()->rootObject();

    QMetaObject::invokeMethod(cadToolObj, "deactivateCADTools",
            Q_RETURN_ARG(QVariant, returnValue));

    btnTranslate->setProperty("checked", false);
    btnRotate->setProperty("checked", false);
    btnScale->setProperty("checked", false);
}


void FDYNGLMainWindow::selectTranslationMode(bool checked)
{
    FDYNGUI.selectTranslationMode(checked);

    btnCursor->setProperty("checked", false);
    if(checked)
        btnTranslate->setProperty("checked", true);
    else
        btnTranslate->setProperty("checked", false);

    btnRotate->setProperty("checked", false);
    btnScale->setProperty("checked", false);
}
void FDYNGLMainWindow::selectRotationMode(bool checked)
{
    FDYNGUI.selectRotationMode(checked);

    btnCursor->setProperty("checked", false);
    btnTranslate->setProperty("checked", false);
    if(checked)
        btnRotate->setProperty("checked", true);
    else
        btnRotate->setProperty("checked", false);
    btnScale->setProperty("checked", false);
}
void FDYNGLMainWindow::selectScaleMode(bool checked)
{
    FDYNGUI.selectScaleMode(checked);

    btnCursor->setProperty("checked", false);
    btnTranslate->setProperty("checked", false);
    btnRotate->setProperty("checked", false);
    if(checked)
        btnScale->setProperty("checked", true);
    else
        btnScale->setProperty("checked", false);
}

void FDYNGLMainWindow::setupConnections()
{
    QQuickWidget *qView = qobject_cast<QQuickWidget *>(centralWidget());

    btnCursor = qView->rootObject()->findChild<QObject *>("btnCursor");
    btnTranslate = qView->rootObject()->findChild<QObject *>("btnTranslate");
    btnRotate = qView->rootObject()->findChild<QObject *>("btnRotate");
    btnScale = qView->rootObject()->findChild<QObject *>("btnScale");
    btnResetCamera = qView->rootObject()->findChild<QObject *>("btnResetCamera");

    btnSingleView = qView->rootObject()->findChild<QObject *>("btnSinglView");
    btnHorizontalSplit = qView->rootObject()->findChild<QObject *>("btnHorizontalSplit");
    btnVerticalSplit = qView->rootObject()->findChild<QObject *>("btnVerticalSplit");
    btnGridSplit = qView->rootObject()->findChild<QObject *>("btnGridSplit");

    btnWireFrame = qView->rootObject()->findChild<QObject *>("btnWireFrame");
    btnShading = qView->rootObject()->findChild<QObject *>("btnShading");


    connect(btnCursor, SIGNAL(onToggled(bool)), this, SLOT(selectCursorMode(bool)));
    connect(btnTranslate, SIGNAL(onToggled(bool)), this, SLOT(selectTranslationMode(bool)));
    connect(btnRotate, SIGNAL(onToggled(bool)), this, SLOT(selectRotationMode(bool)));
    connect(btnScale, SIGNAL(onToggled(bool)), this, SLOT(selectScaleMode(bool)));

    connect(btnResetCamera, SIGNAL(onButtonClicked()), m_pBundle->glWidget, SLOT(resetCamera()));

    connect(btnHorizontalSplit, SIGNAL(onButtonClicked()), m_pBundle->glWidget, SLOT(splitHorizontally()));
    connect(btnVerticalSplit, SIGNAL(onButtonClicked()), m_pBundle->glWidget, SLOT(splitVertically()));
    connect(btnGridSplit, SIGNAL(onButtonClicked()), m_pBundle->glWidget, SLOT(splitToQuad()));

    connect(btnWireFrame, SIGNAL(onButtonClicked()), &FDYNGUI, SLOT(setSelectedWireframe()));
    connect(btnShading, SIGNAL(onButtonClicked()), &FDYNGUI, SLOT(setSelectedSmoothShaded()));

    // Connect relavent menu actions to the appropriate slow of the glwidget's propery class
    connect(m_pBundle->mainWindow->actionGrid, &QAction::toggled, m_pBundle->glWidget->P(), &NEGLWidgetProperties::SetDrawGrid);
    connect(m_pBundle->mainWindow->actionShow_OSD, &QAction::toggled, m_pBundle->glWidget->P(), &NEGLWidgetProperties::setShowOsd);

    connect(actionResetCamera, &QAction::triggered, m_pBundle->glWidget, &NEGLWidget::resetCamera);



//    connect(m_pBundle->mainWindow->actionDynamicQuality, &QAction::toggled, m_pBundle->glWidget->P(), &NEGLWidgetProperties::setDynamicQuality);
//    connect(m_pBundle->mainWindow->actionClip, &QAction::toggled, m_pBundle->glWidget->P(), &NEGLWidgetProperties::setClipping);

    //
    // Actions from the menu/other items first sent to GLWidget and then forwarded from there onto GUIManager
    //


#if 0
    connect(m_pBundle->mainWindow->ui->actionHide, &QAction::triggered, m_pBundle->glWidget, &NEGLWidget::hide);
    connect(m_pBundle->mainWindow->ui->actionClose, &QAction::triggered, m_pBundle->glWidget, &NEGLWidget::close);
#endif

    connect(m_pBundle->glWidget, &NEGLWidget::splitGLDockHorizontally,&FDYNGUI, &FDYNGUIManager::splitGLDockHorizontally);
    connect(m_pBundle->glWidget, &NEGLWidget::splitGLDockVertically, &FDYNGUI, &FDYNGUIManager::splitGLDockVertically);
    connect(m_pBundle->glWidget, &NEGLWidget::splitGLDockToQuad, &FDYNGUI, &FDYNGUIManager::splitGLDockToQuad);
    connect(m_pBundle->glWidget, &NEGLWidget::hideGLWidget, &FDYNGUI, &FDYNGUIManager::hideGLWidget);
    connect(m_pBundle->glWidget, &NEGLWidget::closeGLWidget, &FDYNGUI, &FDYNGUIManager::closeGLWidget);

#if 1
//    //
//    // Direct ACtions which can be done in GLWidget
//    connect(m_pBundle->mainWindow->ui->actionDraw_render_area, &QAction::toggled, m_pBundle->glWidget->P(), &NEGLWidgetProperties::setDrawRenderArea);
//    connect(m_pBundle->mainWindow->ui->actionShow_Axes, &QAction::toggled, m_pBundle->glWidget->P(), &NEGLWidgetProperties::setViewAxes);

    // Connect relavent menu actions to update the opengl windows
    connect(m_pBundle->mainWindow->actionGrid, &QAction::toggled, &FDYNGUI, &FDYNGUIManager::updateGLWindows);
    connect(m_pBundle->mainWindow->actionShow_OSD, &QAction::toggled, &FDYNGUI, &FDYNGUIManager::updateGLWindows);
//    connect(m_pBundle->mainWindow->ui->actionClip, &QAction::toggled, &NEGUI, &NEGUIManager::updateGLWindows);
//    connect(m_pBundle->mainWindow->ui->actionDraw_render_area, &QAction::toggled, &NEGUI, &NEGUIManager::updateGLWindows);
//    connect(m_pBundle->mainWindow->ui->actionShow_Axes, &QAction::toggled, &NEGUI, &NEGUIManager::updateGLWindows);

    // Connect relavent menu actions to update the property editor
    connect(m_pBundle->mainWindow->actionGrid, &QAction::toggled, &FDYNGUI, &FDYNGUIManager::updatePropertyEditor);
    connect(m_pBundle->mainWindow->actionShow_OSD, &QAction::toggled, &FDYNGUI, &FDYNGUIManager::updatePropertyEditor);
//    connect(m_pBundle->mainWindow->ui->actionClip, &QAction::toggled, &NEGUI, &NEGUIManager::updatePropertyEditor);
//    connect(m_pBundle->mainWindow->ui->actionDraw_render_area, &QAction::toggled, &NEGUI, &NEGUIManager::updatePropertyEditor);
//    connect(m_pBundle->mainWindow->ui->actionShow_Axes, &QAction::toggled, &NEGUI, &NEGUIManager::updatePropertyEditor);

    connect(m_pBundle->glWidget, &NEGLWidget::updatePropertyEditorUI, &FDYNGUI, &FDYNGUIManager::updatePropertyEditor);

    // Connect the changed signals of the properties to update the menu action states appropriately
    connect(m_pBundle->glWidget->P(), &NEGLWidgetProperties::showOsdChanged, actionShow_OSD, &QAction::setChecked);
    connect(m_pBundle->glWidget->P(), &NEGLWidgetProperties::DrawGridChanged, actionGrid, &QAction::setChecked);
//    connect(m_pBundle->glWidget->P(), &NEGLWidgetProperties::dynamicQualityChanged, ui->actionDynamicQuality, &QAction::setChecked);
//    connect(m_pBundle->glWidget->P(), &NEGLWidgetProperties::clippingChanged, ui->actionClip, &QAction::setChecked);
//    connect(m_pBundle->glWidget->P(), &NEGLWidgetProperties::viewAxesChanged, ui->actionShow_Axes, &QAction::setChecked);
//    connect(m_pBundle->glWidget->P(), &NEGLWidgetProperties::drawRenderAreaChanged, ui->actionDraw_render_area, &QAction::setChecked);



//    connect(m_pBundle->glWidget, &NEGLWidget::drawOSD, &NEOSD, &NEOsd::draw);
    connect(m_pBundle->glWidget, &NEGLWidget::setCameraUI, this, &FDYNGLMainWindow::setCamera);
    connect(m_pBundle->glWidget, &NEGLWidget::setMenuContextMenuPolicySignal, this, &FDYNGLMainWindow::setContextMenuPolicyFromGLWidget);




//    connect(ui->actionFrameSelection, &QAction::triggered, m_pBundle->glWidget, &NEGLWidget::frameSelection);
//    connect(ui->actionMaximize, SIGNAL(toggled(bool)) , this, SLOT(maximizeGLWindow(bool)));
#endif

}


void FDYNGLMainWindow::setContextMenuPolicyFromGLWidget(int menuPolicy)
{
    setContextMenuPolicy((Qt::ContextMenuPolicy)menuPolicy);
}



void FDYNGLMainWindow::setCamera(QObject *cam)
{
    // If camera is already set, return early
    if(!cam || !m_pBundle->glWidget->P()->setCamera(cam->objectName()))
        return;

    m_pBundle->glWidget->update();
    updateUIForCurrentCamera(cam);


    // Set the given camera as the OpenGL camera
    //m_pBundle->glWidget->setCamera(cam);
    //m_pBundle->glWidget->P()->setCamera(cam->objectName());
}



void FDYNGLMainWindow::updateUIForCurrentCamera(QObject* cam)
{
    if (!cam)
        return;
    // Toggle the action, if camera is set by other means (e.g. console)
    if(!m_CameraToAction[static_cast<NECamera*>(cam)]->isChecked())
        m_CameraToAction[static_cast<NECamera*>(cam)]->toggle();

    // Change the dock widget's title text as the camera name
    m_pBundle->dockWidget->setTitleText(m_pBundle->glWidget->objectName() + " - " + cam->objectName());

    if(UpdateUI())
        FDYNGUI.updatePropertyEditor();

}



void FDYNGLMainWindow::maximizeGLWindow(bool val)
{
    //    if(!m_pBundle->glWidget->hasFocus())
    //        return;


    //qDebug() << Q_FUNC_INFO <<" "<< m_pBundle->glWidget;
    if(val)
    {
        uiStateBeforeMaxed = FDYNGUI.mainWindow()->saveState();
        foreach(GLDockWidgetBundle* b, FDYNGUI.P->m_IDtoGLDockWidget)
            if(b->mainWindow != this)
                FDYNGUI.hideGLWidget( b->glWidget );
    }
    else
        FDYNGUI.mainWindow()->restoreState(uiStateBeforeMaxed);
}


bool FDYNGLMainWindow::isMaximized()
{
#if 0
    return ui->actionMaximize->isChecked();
#endif
    return QMainWindow::isMaximized();

}


void FDYNGLMainWindow::maximizeToggle()
{
#if 0
    ui->actionMaximize->toggle();
#endif
    if(isMaximized())
        QMainWindow::showNormal();
    else
        QMainWindow::showMaximized();

}


