#include "Windows/Main/NEMainWindow.h"
#include "Windows/SceneInspector/NESceneInspectorWindow.h"
#include "NENodeGraphEditor.h"
#include "NENodeGraphEditorScene.h"
#include "NENodeGraphEditorWindow.h"
#include "ui_NENodeGraphEditorWindow.h"
#include "NENodeGraphEditorItem.h"
#include "Managers/NEGUIManager.h"
#include "NEConnection.h"

NENodeGraphEditor::NENodeGraphEditor(QWidget *parent): QGraphicsView(parent)
{
    m_pScene = new NENodeGraphEditorScene();
    setScene( m_pScene );

    m_pScene->setItemIndexMethod( QGraphicsScene::NoIndex );
    setCacheMode( CacheNone );
    setViewportUpdateMode( FullViewportUpdate );
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSceneRect(INT_MIN/2, INT_MIN/2, INT_MAX, INT_MAX);
    centerOn(-400, -200);
    scale( 1.0, 1.0 );

    setRenderHint( QPainter::Antialiasing );
    setTransformationAnchor( AnchorUnderMouse );

    //m_pScene->setSceneRect( -200, -200, 200, 200 );
    m_NaviMode = kNaviNone;
    fitInView(-400, -400, 100, 100, Qt::KeepAspectRatio);

    // create context menu
    m_pContextMenu = new NEMenu(this);
    m_pContextMenu->setTitle("Context Menu");
    createContextMenuTree();
}

NENodeGraphEditor::~NENodeGraphEditor()
{
    m_pScene->setSelectionProcess(true);
    m_pScene->clearSelection();
    delete m_pScene;
    removeContextMenuTree();
    delete m_pContextMenu;
}

void NENodeGraphEditor::selectItem(NENode *node)
{
    m_pScene->selectItem(node);
}

void NENodeGraphEditor::selectItem(QList<NENode*>& nodes)
{
    m_pScene->selectItem(nodes);
}


void NENodeGraphEditor::update()
{
    m_pScene->update();
}


void NENodeGraphEditor::zoomIn()
{
    scaleView(qreal(1.2));
}


void NENodeGraphEditor::zoomOut()
{
    scaleView(1 / qreal(1.2));
}


void NENodeGraphEditor::timerEvent( QTimerEvent *event )
{
    //std::cout << "Timer " << std::endl;
    foreach( QGraphicsItem *item, scene()->items())
    {
        if (NENodeGraphEditorItem *node = qgraphicsitem_cast<NENodeGraphEditorItem *>(item))
        {
            node->advance();
        }
    }
}

void NENodeGraphEditor::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Up:
        //centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        //centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        //centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        //centerNode->moveBy(20, 0);
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        this->layout();
        break;
    default:
        QGraphicsView::keyPressEvent( event );
    }
}


void NENodeGraphEditor::wheelEvent( QWheelEvent *event )
{
    QPoint scrollAmount = event->angleDelta();

    scrollAmount.y() > 0 ?
                scaleView(1.05) : scaleView(0.95);
    //scaleView( pow((double)2, -event->delta() / 240.0 ) );
}

QGraphicsItem* NENodeGraphEditor::findNEConnectionItem(const QPointF posF)
{
    QList<QGraphicsItem*> items = m_pScene->items(QRectF(posF - QPointF(1,1), QSize(3,3)));

    foreach(QGraphicsItem *item, items)
        if (item->type() == NEConnection::Type)
            return item;

    return 0;
}

void NENodeGraphEditor::mousePressEvent( QMouseEvent *event )
{
    if(event->modifiers() == Qt::AltModifier)
    {
        switch(event->button())
        {
        case Qt::LeftButton:
            m_NaviMode = kNaviRotate;
            break;
        case Qt::RightButton:
            m_NaviMode = kNaviZoom;
            break;
        case Qt::MiddleButton:
            m_NaviMode = kNaviPan;
            break;
        default:
            break;
        }
        m_MousePosOld = event->pos();
        QGraphicsView::mousePressEvent( event );
    } else {
        switch(event->button())
        {
        case Qt::RightButton:
        {
            QPoint  curPos(event->pos());
            QPointF curPosF(mapToScene(curPos));
            QGraphicsItem *item = findNEConnectionItem(curPosF);
            if (item)
            {
                QGraphicsView::mousePressEvent( event );
                break;
            }
            NENodeGraphEditorWindow* editorWindow = NEGUI.mainWindow()->nodeGraphEditorWindow();
            m_pContextMenu->addMenu(editorWindow->ui->menuFilter);
        }
            //right clicks should open a pop-up
            m_pContextMenu->exec(mapToGlobal(event->pos()));
            break;
        default:
            QGraphicsView::mousePressEvent( event );
            break;
        }
    }
}

void NENodeGraphEditor::mouseReleaseEvent( QMouseEvent *event )
{
    m_NaviMode = kNaviNone;
    QGraphicsView::mouseReleaseEvent(event);
}


void NENodeGraphEditor::mouseMoveEvent( QMouseEvent *event )
{
    QPointF delta = mapToScene( event->pos() ) - mapToScene( m_MousePosOld );

    if(m_NaviMode > 0)
    {
        switch(m_NaviMode)
        {
        case kNaviRotate:
            break;
        case kNaviZoom:
            scaleView(pow((double)2, delta.y() / 500.0));
            break;
        case kNaviPan:
            //this->scene()->setSceneRect( sceneRect().x() - delta.x(), sceneRect().y() - delta.y(), sceneRect().width(), sceneRect().height() );
        {
            delta *= transform().m11();
            // Have panning be anchored from the mouse.
            setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
            QPoint newCenter(viewport()->rect().width() / 2 - delta.x(),  viewport()->rect().height() / 2 - delta.y());
            centerOn(mapToScene(newCenter));

            // For zooming to anchor from the view center.
            setTransformationAnchor(QGraphicsView::AnchorViewCenter);
        }
            break;
        default:break;
        }
    }
    QGraphicsView::mouseMoveEvent( event );
    m_MousePosOld = event->pos();
}

void NENodeGraphEditor::scaleView( double scaleFactor )
{
    double factor = transform().scale( scaleFactor, scaleFactor ).mapRect( QRectF( 0, 0, 1, 1 ) ).width();
    if( factor < 0.07 || factor > 100 )
        return;
    if (scaleFactor > 2)
        scaleFactor = 2;

    scale( scaleFactor, scaleFactor );
}

void NENodeGraphEditor::showContextMenu(const QPoint& pos)
{
    //    QMenu contextMenu(tr("Context menu"), this);

    //    QAction action1("Remove Data Point", this);
    //    connect(&action1, SIGNAL(triggered()), this, SLOT(removeDataPoint()));
    //    contextMenu.addAction(&action1);

    //    contextMenu.exec(mapToGlobal(pos));
    m_pContextMenu->exec(mapToGlobal(pos));
}
void NENodeGraphEditor::createContextMenuTree()
{
    menuAdd = new NEMenu(this);
    menuAdd->setObjectName(QStringLiteral("menuAdd"));
    menuAdd->setTitle(QStringLiteral("Add"));
    menuAdd->setTearOffEnabled(true);
    QIcon iconAdd;
    iconAdd.addFile(QStringLiteral(":/Neutrino/Resources/16/list-add.png"), QSize(), QIcon::Normal, QIcon::Off);
    menuAdd->setIcon(iconAdd);

    menuPhysicals = new NEMenu(menuAdd);
    menuPhysicals->setObjectName(QStringLiteral("menuPhysicals"));
    menuPhysicals->setTitle(QStringLiteral("Physicals"));
    menuPhysicals->setTearOffEnabled(true);

    menuColliders = new NEMenu(menuAdd);
    menuColliders->setObjectName(QStringLiteral("menuColliders"));
    menuColliders->setTitle(QStringLiteral("Colliders"));
    menuColliders->setTearOffEnabled(true);

    menuFluids = new NEMenu(menuPhysicals);
    menuFluids->setObjectName(QStringLiteral("menuFluids"));
    menuFluids->setTitle(QStringLiteral("Fluids"));
    menuFluids->setTearOffEnabled(true);
    menuChemistry = new NEMenu(menuPhysicals);
    menuChemistry->setObjectName(QStringLiteral("menuChemistry"));
    menuChemistry->setTitle(QStringLiteral("Chemistry"));
    menuChemistry->setTearOffEnabled(true);
    menuParticleFluidSolvers = new NEMenu(menuPhysicals);
    menuParticleFluidSolvers->setObjectName(QStringLiteral("menuParticleFluidSolvers"));
    menuParticleFluidSolvers->setTitle(QStringLiteral("Fluid Solvers"));
    menuParticleFluidSolvers->setTearOffEnabled(true);
    menuGridFluidSolvers = new NEMenu(menuPhysicals);
    menuGridFluidSolvers->setObjectName(QStringLiteral("menuGridFluidSolvers"));
    menuGridFluidSolvers->setTitle(QStringLiteral("Grid Fluid Solvers"));
    menuGridFluidSolvers->setEnabled(false);
    menuGridFluidSolvers->setTearOffEnabled(true);
    menuRigid = new NEMenu(menuPhysicals);
    menuRigid->setObjectName(QStringLiteral("menuRigid"));
    menuRigid->setTitle(QStringLiteral("Rigid"));
    menuRigid->setTearOffEnabled(true);
    menuDeformable = new NEMenu(menuPhysicals);
    menuDeformable->setObjectName(QStringLiteral("menuDeformable"));
    menuDeformable->setTitle(QStringLiteral("Deformable"));
    menuDeformable->setTearOffEnabled(true);
    menuRigidSolvers = new NEMenu(menuPhysicals);
    menuRigidSolvers->setObjectName(QStringLiteral("menuRigidSolvers"));
    menuRigidSolvers->setTitle(QStringLiteral("Rigid Solvers"));
    menuRigidSolvers->setTearOffEnabled(true);
    menuDaemons = new NEMenu(menuAdd);
    menuDaemons->setObjectName(QStringLiteral("menuDaemons"));
    menuDaemons->setTitle(QStringLiteral("Daemons"));
    menuDaemons->setTearOffEnabled(true);
    menuKillerFields = new NEMenu(menuDaemons);
    menuKillerFields->setObjectName(QStringLiteral("menuKillerFields"));
    menuKillerFields->setTitle(QStringLiteral("Killer Fields"));
    menuKillerFields->setTearOffEnabled(true);
    menuEmitters = new NEMenu(menuDaemons);
    menuEmitters->setObjectName(QStringLiteral("menuEmitters"));
    menuEmitters->setTitle(QStringLiteral("Emitters"));
    menuEmitters->setTearOffEnabled(true);
    menuDualBoundaries = new NEMenu(menuDaemons);
    menuDualBoundaries->setObjectName(QStringLiteral("menuDualBoundaries"));
    menuDualBoundaries->setTitle(QStringLiteral("Dual Boundaries"));
    menuDualBoundaries->setTearOffEnabled(true);
    menuForceFields = new NEMenu(menuDaemons);
    menuForceFields->setObjectName(QStringLiteral("menuForceFields"));
    menuForceFields->setTitle(QStringLiteral("Force Fields"));
    menuForceFields->setTearOffEnabled(true);
    menuDataFields = new NEMenu(menuDaemons);
    menuDataFields->setObjectName(QStringLiteral("menuDataFields"));
    menuDataFields->setTitle(QStringLiteral("Data Fields"));
    menuDataFields->setTearOffEnabled(true);
    menuVisual = new NEMenu(menuAdd);
    menuVisual->setObjectName(QStringLiteral("menuVisual"));
    menuVisual->setTitle(QStringLiteral("Visual"));
    menuVisual->setTearOffEnabled(true);
    menuCameras = new NEMenu(menuVisual);
    menuCameras->setObjectName(QStringLiteral("menuCameras"));
    menuCameras->setTitle(QStringLiteral("Cameras"));
    menuCameras->setTearOffEnabled(true);
    menuLights = new NEMenu(menuVisual);
    menuLights->setObjectName(QStringLiteral("menuLights"));
    menuLights->setTitle(QStringLiteral("Lights"));
    menuLights->setTearOffEnabled(true);
    menuMaterials = new NEMenu(menuVisual);
    menuMaterials->setObjectName(QStringLiteral("menuMaterials"));
    menuMaterials->setTitle(QStringLiteral("Materials"));
    menuMaterials->setTearOffEnabled(true);
    menuOSD = new NEMenu(menuVisual);
    menuOSD->setObjectName(QStringLiteral("menuOSD"));
    menuOSD->setTitle(QStringLiteral("OSD"));
    menuOSD->setTearOffEnabled(true);
    menuGizmo = new NEMenu(menuVisual);
    menuGizmo->setObjectName(QStringLiteral("menuGizmo"));
    menuGizmo->setTitle(QStringLiteral("Gizmo"));
    menuGizmo->setTearOffEnabled(true);
    menuPostProcessors = new NEMenu(menuAdd);
    menuPostProcessors->setObjectName(QStringLiteral("menuPostProcessors"));
    menuPostProcessors->setTitle(QStringLiteral("Post-Processors"));
    menuPostProcessors->setTearOffEnabled(true);
    //menuMeshPostProcessors = new NEMenu(menuPostProcessors);
    //menuMeshPostProcessors->setObjectName(QStringLiteral("menuMeshPostProcessors"));
    //menuMeshPostProcessors->setTitle(QStringLiteral("Mesh Post-Processors"));
    //menuMeshPostProcessors->setTearOffEnabled(true);
    //menuParticlePostProcessors = new NEMenu(menuPostProcessors);
    //menuParticlePostProcessors->setObjectName(QStringLiteral("menuParticlePostProcessors"));
    //menuParticlePostProcessors->setTitle(QStringLiteral("Particle Post-Processors"));
    //menuParticlePostProcessors->setTearOffEnabled(true);
    menuVolume = new NEMenu(menuAdd);
    menuVolume->setObjectName(QStringLiteral("menuVolume"));
    menuVolume->setTitle(QStringLiteral("Volume"));
    menuVolume->setTearOffEnabled(true);

    m_pContextMenu->addMenu(menuAdd);
    menuAdd->addMenu(menuPhysicals);
    menuAdd->addMenu(menuColliders);
    menuPhysicals->addMenu(menuFluids);
    menuPhysicals->addMenu(menuChemistry);
    menuPhysicals->addMenu(menuParticleFluidSolvers);
    menuPhysicals->addMenu(menuGridFluidSolvers);
    menuPhysicals->addMenu(menuRigid);
    menuPhysicals->addMenu(menuDeformable);
    menuPhysicals->addMenu(menuRigidSolvers);
    menuAdd->addMenu(menuDaemons);
    menuDaemons->addMenu(menuKillerFields);
    menuDaemons->addMenu(menuEmitters);
    menuDaemons->addMenu(menuDualBoundaries);
    menuDaemons->addMenu(menuForceFields);
    menuDaemons->addMenu(menuDataFields);

    menuAdd->addMenu(menuVolume);
    menuAdd->addMenu(menuVisual);
    menuVisual->addMenu(menuCameras);
    menuVisual->addMenu(menuLights);
    menuVisual->addMenu(menuMaterials);
    menuVisual->addMenu(menuOSD);
    menuVisual->addMenu(menuGizmo);
    menuAdd->addMenu(menuPostProcessors);
    //menuPostProcessors->addMenu(menuMeshPostProcessors);
    //menuPostProcessors->addMenu(menuParticlePostProcessors);


    actionRemove = new QAction(this);
    actionRemove->setObjectName(QStringLiteral("actionRemove"));
    actionRemove->setText(QStringLiteral("Remove"));
    QIcon iconRemove;
    iconRemove.addFile(QStringLiteral(":/Neutrino/Resources/16/list-remove.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionRemove->setIcon(iconRemove);
    connect(actionRemove, &QAction::triggered, this, &NENodeGraphEditor::removeSelectedItem);
    m_pContextMenu->addAction(actionRemove);
}

void NENodeGraphEditor::removeContextMenuTree()
{
    delete menuFluids;
    delete menuChemistry;
    delete menuParticleFluidSolvers;
    delete menuGridFluidSolvers;
    delete menuRigid;
    delete menuDeformable;
    delete menuRigidSolvers;
    delete menuPhysicals;

    delete menuKillerFields;
    delete menuEmitters;
    delete menuDualBoundaries;
    delete menuForceFields;
    delete menuDataFields;
    delete menuDaemons;



    delete menuCameras;
    delete menuLights;
    delete menuMaterials;
    delete menuOSD;
    delete menuGizmo;
    delete menuVisual;

    //delete menuMeshPostProcessors;
    //delete menuParticlePostProcessors;
    delete menuPostProcessors;

    delete menuVolume;

    delete menuAdd;

    m_pContextMenu->removeAction(actionRemove);
    delete actionRemove;
}

void NENodeGraphEditor::addCreateAction(QAction *action, uint nodeType)
{
    // Depending on the type of the action, add the action to the appropriate menu
    switch(nodeType)
    {
    case NE::kObjectCollider:
        menuColliders->addAction(action);
        break;
    case NE::kObjectFluid:
        menuFluids->addAction(action);
        break;
    case NE::kObjectChemistry:
        menuChemistry->addAction(action);
        break;
    case NE::kObjectParticleFluidSolver:
        menuParticleFluidSolvers->addAction(action);
        break;
    case NE::kObjectGridFluidSolver:
        menuGridFluidSolvers->addAction(action);
        break;
    case (NE::kObjectParticleRigid):
        menuRigid->addAction(action);
        break;
    case (NE::kObjectParticleDeformable):
        menuDeformable->addAction(action);
        break;
    case (NE::kObjectRigidSolver):
        menuRigidSolvers->addAction(action);
        break;
    case (NE::kObjectMeshing):
    case (NE::kObjectMeshing + NE::kObjectExperimental):
        menuPostProcessors->addAction(action);
        break;
    case (NE::kObjectMaterial):
        menuMaterials->addAction(action);
        break;
    case (NE::kObjectParticleSource):
        menuEmitters->addAction(action);
        break;
    case (NE::kObjectMeshPP):
        menuPostProcessors->addAction(action);
        break;
    case (NE::kObjectDataField):
        menuDataFields->addAction(action);
        break;
    case (NE::kObjectForceField):
        menuForceFields->addAction(action);
        break;
    case (NE::kObjectKiller):
        menuKillerFields->addAction(action);
        break;
    case (NE::kObjectLight):
        menuLights->addAction(action);
        break;
    case (NE::kObjectCamera):
        menuCameras->addAction(action);
        break;
    case (NE::kObjectParticleEmitter):
        menuEmitters->addAction(action);
        break;
    case (NE::kObjectDualBoundary):
        menuDualBoundaries->addAction(action);
        break;
    case (NE::kObjectOSD):
        menuOSD->addAction(action);
        break;
    case (NE::kObjectGizmo):
        menuGizmo->addAction(action);
        break;
    case (NE::kObjectMeshVolumeOP):
        menuVolume->addAction(action);
        break;
    case (NE::kObjectVolumeOP):
        menuVolume->addAction(action);
        break;
    case (NE::kObjectMeasurementField):
        menuDaemons->addAction(action);
        break;
    case (NE::kObjectClipPlane):
        menuVisual->addAction(action);
        break;
    default:
        menuAdd->addAction(action);
    }
}

void NENodeGraphEditor::removeCreateAction(QAction* action, uint nodeType)
{
    // Depending on the type of the action, remove the action to the appropriate menu
    switch(nodeType)
    {
    case NE::kObjectFluid:
        menuFluids->removeAction(action);
        break;
    case NE::kObjectChemistry:
        menuChemistry->removeAction(action);
        break;
    case NE::kObjectParticleFluidSolver:
        menuParticleFluidSolvers->removeAction(action);
        break;
    case NE::kObjectGridFluidSolver:
        menuGridFluidSolvers->removeAction(action);
        break;
    case (NE::kObjectParticleRigid):
        menuRigid->removeAction(action);
        break;
    case (NE::kObjectParticleDeformable):
        menuDeformable->removeAction(action);
        break;
    case (NE::kObjectRigidSolver):
        menuRigidSolvers->removeAction(action);
        break;
    case (NE::kObjectMeshing):
    case (NE::kObjectMeshing + NE::kObjectExperimental):
        menuPostProcessors->removeAction(action);
        break;
    case (NE::kObjectMaterial):
        menuMaterials->removeAction(action);
        break;
    case (NE::kObjectParticleSource):
        menuEmitters->removeAction(action);
        break;
    case (NE::kObjectMeshPP):
        menuPostProcessors->removeAction(action);
        break;
    case (NE::kObjectDataField):
        menuDataFields->removeAction(action);
        break;
    case (NE::kObjectForceField):
        menuForceFields->removeAction(action);
        break;
    case (NE::kObjectKiller):
        menuKillerFields->removeAction(action);
        break;
    case (NE::kObjectLight):
        menuLights->removeAction(action);
        break;
    case (NE::kObjectCamera):
        menuCameras->removeAction(action);
        break;
    case (NE::kObjectParticleEmitter):
        menuEmitters->removeAction(action);
        break;
    case (NE::kObjectDualBoundary):
        menuDualBoundaries->removeAction(action);
        break;
    case (NE::kObjectOSD):
        menuOSD->removeAction(action);
        break;
    case (NE::kObjectGizmo):
        menuGizmo->removeAction(action);
        break;
    case (NE::kObjectMeshVolumeOP):
        menuVolume->removeAction(action);
        break;
    case (NE::kObjectVolumeOP):
        menuVolume->removeAction(action);
        break;
    case (NE::kObjectMeasurementField):
        menuDaemons->removeAction(action);
        break;
    case (NE::kObjectClipPlane):
        menuVisual->removeAction(action);
        break;
    default:
        menuAdd->removeAction(action);
    }
}

void NENodeGraphEditor::removeSelectedItem()
{
    if (NEGUI.mainWindow() && NEGUI.mainWindow()->sceneInspectorWindow())
        NEGUI.mainWindow()->sceneInspectorWindow()->removeSelectedItem();
}

void NENodeGraphEditor::frameSelection()
{
    resetTransform();
    QRectF rect = m_pScene->getFrameSelectionRect();
    if(rect.width() > INT_MAX || rect.height() > INT_MAX ||
            rect.width() < INT_MIN || rect.height() < INT_MIN)
        return;
    setSceneRect(INT_MIN/2, INT_MIN/2, INT_MAX, INT_MAX);
    float scalexy = viewport()->rect().width() / rect.width();
    if(scalexy > viewport()->rect().height() / rect.height())
        scalexy = viewport()->rect().height() / rect.height();
    scale( scalexy, scalexy);
    centerOn(rect.center());
}

void NENodeGraphEditor::snapToGrid(bool bChecked)
{
    m_pScene->setSnapToGrid(bChecked);
    m_pScene->layout();
    m_pScene->update();
}
