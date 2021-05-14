#include "NENodeGraphEditorWindow.h"
#include "ui_NENodeGraphEditorWindow.h"
#include "NENodeGraphEditorScene.h"
#include "NEConnectionGEItem.h"
#include "NENode.h"
#include "NEBlock.h"
#include "NEPort.h"
#include "NEConnection.h"
#include "NodesEditor.h"


#define USE_OLD_PORT
NENodeGraphEditorWindow::NENodeGraphEditorWindow(QWidget *parent) :
    QMainWindow(parent)
  #ifdef USE_OLD_PORT
  ,	ui(new Ui::NENodeGraphEditorWindow)
  #endif
{
#ifdef USE_OLD_PORT
    ui->setupUi(this);

    // Initialize actions
    m_pActionGroup = new QActionGroup( this );
    m_pActionGroup->setObjectName("Curves");
    m_pActionGroup->addAction( ui->actionLinear );
    m_pActionGroup->addAction( ui->actionCubic );
    m_pActionGroup->addAction( ui->actionQuadratic );

    connect( ui->actionLinear, &QAction::toggled, this, &NENodeGraphEditorWindow::setLinearConnectionCurves );
    connect( ui->actionQuadratic, &QAction::toggled, this, &NENodeGraphEditorWindow::setQuadraticConnectionCurves );
    connect( ui->actionCubic, &QAction::toggled, this, &NENodeGraphEditorWindow::setCubicConnectionCurves );

    connect( ui->actionCircle, &QAction::triggered, this, &NENodeGraphEditorWindow::layoutCircle );
    connect( ui->actionRandom, &QAction::triggered, this, &NENodeGraphEditorWindow::layoutRandom );
    connect( ui->actionForce_Directed, &QAction::triggered, this, &NENodeGraphEditorWindow::layoutForceDirected );
    connect( ui->actionSpring, &QAction::triggered, this, &NENodeGraphEditorWindow::layoutSpring );

    connect( ui->actionShow_ALL, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterALl );
    connect( ui->actionKIOExtent, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIOExtent );
    connect( ui->actionKIOFlowRate, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIOFlowRate );
    connect( ui->actionKIOFluidHeight, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIOFluidHeight );
    connect( ui->actionKIOForceField, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIOForceField );
    connect( ui->actionKIOGeoConversion, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIOGeoConversion );
    connect( ui->actionKIOGizmo, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIOGizmo );
    connect( ui->actionKIOHydrostaticPressure, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIOHydrostaticPressure );
    connect( ui->actionKIOMeasurement, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIOMeasurement );
    connect( ui->actionKIONumParticles, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIONumParticles );
    connect( ui->actionKIONumParticlesDrain, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIONumParticlesDrain );
    connect( ui->actionKIOParticles, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIOParticles );
    connect( ui->actionKIOFluid, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIOFluid );
    connect( ui->actionKIORigid, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIORigid );
    connect( ui->actionKIOTriangleMesh, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIOTriangleMesh );
    connect( ui->actionKIOVolume, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterBykIOVolume );
    connect( ui->actionAny, &QAction::triggered, this, &NENodeGraphEditorWindow::setHideNoConnnectionFilter );

    connect( ui->actionBy_Node_Without_Connection, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeByConnection );

    connect( ui->actionNode_All, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeByAll);
    connect( ui->actionKObjectExperimental, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectExperimental);
    connect( ui->actionkObjectFluid, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectFluid);
    connect( ui->actionkObjectChemistry, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectChemistry);
    connect( ui->actionkObjectParticleFluidSolver, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectParticleFluidSolver);
    connect( ui->actionKObjectParticleRigid, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectParticleRigid);
    connect( ui->actionKObjectParticleDeformable, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectParticleDeformable);
    connect( ui->actionKObjectRigidSolver, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectRigidSolver);
    connect( ui->actionKObjectKiller, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectKiller);
    connect( ui->actionkObjectGridFluidSolver, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectGridFluidSolver);
    connect( ui->actionKObjectCollider, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectCollider);
    connect( ui->actionKObjectMeshing, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectMeshing);
    connect( ui->actionKObjectMaterial , &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectMaterial );
    connect( ui->actionKObjectParticleSource, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectParticleSource);
    connect( ui->actionKObjectCamera, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectCamera);
    connect( ui->actionKObjectLight, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectLight);
    connect( ui->actionKObjectMeshPP, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectMeshPP);
    connect( ui->actionKObjectDataField, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectDataField);
    connect( ui->actionKObjectForceField, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectForceField);
    connect( ui->actionKObjectParticleEmitter, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectParticleEmitter);
    connect( ui->actionKObjectMeshVolumeOP, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectMeshVolumeOP);
    connect( ui->actionKObjectVolumeOP, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectVolumeOP);
    connect( ui->actionKObjectMeasurementField, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectMeasurementField);
    connect( ui->actionKObjectGroup, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectGroup);
    connect( ui->actionKObjectDataCollector, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectDataCollector);
    connect( ui->actionKObjectDualBoundary, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectDualBoundary);
    connect( ui->actionKObjectOSD, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectOSD);
    connect( ui->actionKObjectClipPlane, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectClipPlane);
    connect( ui->actionKObjectGizmo, &QAction::triggered, this, &NENodeGraphEditorWindow::setFilterNodeBykObjectGizmo);
    connect( ui->nodeTabWidget, SIGNAL(currentChanged(int)), this, SLOT(nodeTabSelected()));
    connect( ui->actionFrame_Selection, &QAction::triggered, this, &NENodeGraphEditorWindow::frameSelection );
    connect( ui->actionSnap_to_Grid, &QAction::triggered, this, &NENodeGraphEditorWindow::snapToGrid );

    addActions(m_pActionGroup->actions());

    showModelingNodeGraphScene();
#else
    NENodeGraphEditorScene* scene = new NENodeGraphEditorScene();
    QDockWidget *dock = new QDockWidget(tr("Nodes"), this, Qt::SubWindow);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    NENodeGraphEditor* view = new NENodeGraphEditor(this);
    view->setScene(scene);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    view->setRenderHint(QPainter::Antialiasing, true);

    dock->setWidget(view);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    NodesEditor* nodesEditor = new NodesEditor(this);
    nodesEditor->install(scene);

    NEBlock *b = new NEBlock(0);
    scene->addItem(b);
    b->addPort("test", 0, NEPort::NamePort);
    b->addPort("TestBlock", 0, NEPort::TypePort);
    b->addInputPort("in1");
    b->addInputPort("in2");
    b->addInputPort("in3");
    b->addOutputPort("out1");
    b->addOutputPort("out2");
    b->addOutputPort("out3");

    b = b->clone();
    b->setPos(150, 0);

    b = b->clone();
    b->setPos(150, 150);
#endif
}

bool NENodeGraphEditorWindow::init()
{
    //m_pScene = new NENodeGraphEditorScene();
    //currentNodeGraphicsView()->setScene(m_pScene);
    return true;
}

//void NENodeGraphEditorWindow::setScene(NENodeGraphEditorScene* scene)
//{
////	m_pScene = scene;
////	currentNodeGraphicsView()->setScene(static_cast<NENodeGraphEditorScene*>(m_pScene));
//}

NENodeGraphEditorWindow::~NENodeGraphEditorWindow()
{
    delete ui;
}

void NENodeGraphEditorWindow::showEvent(QShowEvent *event)
{
	layout();
}
void NENodeGraphEditorWindow::update()
{
    layout();
#ifdef USE_OLD_PORT
    ui->modelingNodeGraphicsView->update();
    ui->simulationNodeGraphicsView->update();
    ui->visualizationNodeGraphicsView->update();
#endif
}

void NENodeGraphEditorWindow::updateForDeletion()
{
#ifdef USE_OLD_PORT
    ui->modelingNodeGraphicsView->update();
    ui->simulationNodeGraphicsView->update();
    ui->visualizationNodeGraphicsView->update();
#endif
}

void NENodeGraphEditorWindow::layout(bool bFullPerform)
{
#ifdef USE_OLD_PORT
    static_cast<NENodeGraphEditorScene*>(ui->modelingNodeGraphicsView->scene())->layout(bFullPerform);
    static_cast<NENodeGraphEditorScene*>(ui->simulationNodeGraphicsView->scene())->layout(bFullPerform);
    static_cast<NENodeGraphEditorScene*>(ui->visualizationNodeGraphicsView->scene())->layout(bFullPerform);
#endif
}

void NENodeGraphEditorWindow::updateInitFlag(bool aInitFlag)
{
#ifdef USE_OLD_PORT
    static_cast<NENodeGraphEditorScene*>(ui->modelingNodeGraphicsView->scene())->updateInitFlag(aInitFlag);
    static_cast<NENodeGraphEditorScene*>(ui->simulationNodeGraphicsView->scene())->updateInitFlag(aInitFlag);
    static_cast<NENodeGraphEditorScene*>(ui->visualizationNodeGraphicsView->scene())->updateInitFlag(aInitFlag);
#endif
}

void NENodeGraphEditorWindow::updateNodeRenameForNodeGraph(const QString &oldName, const QString &newName)
{
#ifdef USE_OLD_PORT
    static_cast<NENodeGraphEditorScene*>(ui->modelingNodeGraphicsView->scene())->updateNodeRenameForNodeGraph(oldName, newName);
    static_cast<NENodeGraphEditorScene*>(ui->simulationNodeGraphicsView->scene())->updateNodeRenameForNodeGraph(oldName, newName);
    static_cast<NENodeGraphEditorScene*>(ui->visualizationNodeGraphicsView->scene())->updateNodeRenameForNodeGraph(oldName, newName);
#endif
}

void NENodeGraphEditorWindow::selectItem(NENode *node)
{
#ifdef USE_OLD_PORT
    currentNodeGraphicsView()->selectItem(node);
#endif
}

void NENodeGraphEditorWindow::selectItem(QList<NENode*>& nodes)
{
#ifdef USE_OLD_PORT
    currentNodeGraphicsView()->selectItem(nodes);
#endif
}

void NENodeGraphEditorWindow::writeGUIState( QXmlStreamWriter *writer )
{
    writer->writeStartElement("NodeGraphEditor");
    static_cast<NENodeGraphEditorScene*>(ui->modelingNodeGraphicsView->scene())->saveNodeStates( writer );
    static_cast<NENodeGraphEditorScene*>(ui->simulationNodeGraphicsView->scene())->saveNodeStates( writer );
    static_cast<NENodeGraphEditorScene*>(ui->visualizationNodeGraphicsView->scene())->saveNodeStates( writer );
    writer->writeEndElement();
}

void NENodeGraphEditorWindow::readGUIState( QXmlStreamReader *reader )
{
#ifdef USE_OLD_PORT
    static_cast<NENodeGraphEditorScene*>(ui->modelingNodeGraphicsView->scene())->loadNodeStates(reader);
    static_cast<NENodeGraphEditorScene*>(ui->simulationNodeGraphicsView->scene())->loadNodeStates(reader);
    static_cast<NENodeGraphEditorScene*>(ui->visualizationNodeGraphicsView->scene())->loadNodeStates(reader);
#endif
}

void NENodeGraphEditorWindow::layoutCircle()
{
#ifdef USE_OLD_PORT
    NENodeGraphEditorScene::setLayoutType( NENodeGraphEditorScene::LayoutCircle );
    layout(true);
#endif
}

void NENodeGraphEditorWindow::layoutRandom()
{
#ifdef USE_OLD_PORT
    NENodeGraphEditorScene::setLayoutType( NENodeGraphEditorScene::LayoutRandom );
    layout(true);
#endif
}

void NENodeGraphEditorWindow::layoutForceDirected()
{
#ifdef USE_OLD_PORT
    NENodeGraphEditorScene::setLayoutType( NENodeGraphEditorScene::LayoutForceDirected );
    layout(true);
#endif
}

void NENodeGraphEditorWindow::layoutSpring()
{
#ifdef USE_OLD_PORT
    NENodeGraphEditorScene::setLayoutType( NENodeGraphEditorScene::LayoutSpring );
    layout(true);
#endif
}

void NENodeGraphEditorWindow::setLinearConnectionCurves()
{
#ifdef USE_OLD_PORT
    NEConnectionGEItem::setCurveType(NEConnectionGEItem::Linear);
    NEConnection::setCurveType(NEConnection::Linear);
    update();
    //    currentNodeGraphicsView()->scene()->update(currentNodeGraphicsView()->sceneRect());
#endif
}

void NENodeGraphEditorWindow::setQuadraticConnectionCurves()
{
#ifdef USE_OLD_PORT
    NEConnectionGEItem::setCurveType(NEConnectionGEItem::Quadratic);
    NEConnection::setCurveType(NEConnection::Quadratic);
    update();
    //    currentNodeGraphicsView()->scene()->update(currentNodeGraphicsView()->sceneRect());
#endif
}

void NENodeGraphEditorWindow::setCubicConnectionCurves()
{
#ifdef USE_OLD_PORT
    NEConnectionGEItem::setCurveType(NEConnectionGEItem::Cubic);
    NEConnection::setCurveType(NEConnection::Cubic);
    update();
    //    currentNodeGraphicsView()->scene()->update(currentNodeGraphicsView()->sceneRect());
#endif
}

void NENodeGraphEditorWindow::addCreateAction(QAction *action, uint nodeType)
{
    if( ui )
    {
        if( ui->modelingNodeGraphicsView )
            ui->modelingNodeGraphicsView->addCreateAction(action, nodeType);
        if( ui->simulationNodeGraphicsView )
            ui->simulationNodeGraphicsView->addCreateAction(action, nodeType);
        if( ui->visualizationNodeGraphicsView )
            ui->visualizationNodeGraphicsView->addCreateAction(action, nodeType);
    }
}

void NENodeGraphEditorWindow::removeCreateAction(QAction* action, uint nodeType)
{
    if( ui )
    {
        if( ui->modelingNodeGraphicsView )
            ui->modelingNodeGraphicsView->removeCreateAction(action, nodeType);
        if( ui->simulationNodeGraphicsView )
            ui->simulationNodeGraphicsView->removeCreateAction(action, nodeType);
        if( ui->visualizationNodeGraphicsView )
            ui->visualizationNodeGraphicsView->removeCreateAction(action, nodeType);
    }
}

void NENodeGraphEditorWindow::setFilterBykIOExtent()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIOExtent);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIOParticles()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIOParticles);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIOTriangleMesh()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIOTriangleMesh);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIOForceField()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIOForceField);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIOVolume()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIOVolume);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIOFlowRate()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIOFlowRate);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIOFluidHeight()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIOFluidHeight);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIONumParticles()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIONumParticles);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIONumParticlesDrain()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIONumParticlesDrain);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIOHydrostaticPressure()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIOHydrostaticPressure);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIOFluid()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIOFluid);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIORigid()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIORigid);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIOMeasurement()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIOMeasurement);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIOGizmo()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIOGizmo);
#endif
}

void NENodeGraphEditorWindow::setFilterBykIOGeoConversion()
{
#ifdef USE_OLD_PORT
    setFilterByConnection(NE::kIOGeoConversion);
#endif
}

void NENodeGraphEditorWindow::setFilterALl()
{
#ifdef USE_OLD_PORT
    if(ui->actionShow_ALL->isChecked()){
        ui->actionKIOExtent->setChecked(false);
        ui->actionKIOFlowRate->setChecked(false);
        ui->actionKIOFluidHeight->setChecked(false);
        ui->actionKIOForceField->setChecked(false);
        ui->actionKIOGeoConversion->setChecked(false);
        ui->actionKIOGizmo->setChecked(false);
        ui->actionKIOHydrostaticPressure->setChecked(false);
        ui->actionKIOMeasurement->setChecked(false);
        ui->actionKIONumParticles->setChecked(false);
        ui->actionKIONumParticlesDrain->setChecked(false);
        ui->actionKIOParticles->setChecked(false);
        ui->actionKIOFluid->setChecked(false);
        ui->actionKIORigid->setChecked(false);
        ui->actionKIOTriangleMesh->setChecked(false);
        ui->actionKIOVolume->setChecked(false);
        ui->actionKIOVolume->setChecked(false);
        ui->actionAny->setChecked(false);

        NENodeGraphEditorScene::setPortsFilterType(NE::kIONone);
        update();
    }
    else{
        ui->actionShow_ALL->setChecked(true);
    }
#endif
}

void NENodeGraphEditorWindow::setHideNoConnnectionFilter(){
#ifdef USE_OLD_PORT
    if(ui->actionAny->isChecked()){
        ui->actionKIOExtent->setChecked(true);
        ui->actionKIOFlowRate->setChecked(true);
        ui->actionKIOFluidHeight->setChecked(true);
        ui->actionKIOForceField->setChecked(true);
        ui->actionKIOGeoConversion->setChecked(true);
        ui->actionKIOGizmo->setChecked(true);
        ui->actionKIOHydrostaticPressure->setChecked(true);
        ui->actionKIOMeasurement->setChecked(true);
        ui->actionKIONumParticles->setChecked(true);
        ui->actionKIONumParticlesDrain->setChecked(true);
        ui->actionKIOParticles->setChecked(true);
        ui->actionKIOFluid->setChecked(true);
        ui->actionKIORigid->setChecked(true);
        ui->actionKIOTriangleMesh->setChecked(true);
        ui->actionKIOVolume->setChecked(true);
        ui->actionKIOVolume->setChecked(true);
        ui->actionShow_ALL->setChecked(false);

        NENodeGraphEditorScene::setPortsFilterType(-1);
        update();
    }
    else{
        ui->actionKIOExtent->setChecked(false);
        ui->actionKIOFlowRate->setChecked(false);
        ui->actionKIOFluidHeight->setChecked(false);
        ui->actionKIOForceField->setChecked(false);
        ui->actionKIOGeoConversion->setChecked(false);
        ui->actionKIOGizmo->setChecked(false);
        ui->actionKIOHydrostaticPressure->setChecked(false);
        ui->actionKIOMeasurement->setChecked(false);
        ui->actionKIONumParticles->setChecked(false);
        ui->actionKIONumParticlesDrain->setChecked(false);
        ui->actionKIOParticles->setChecked(false);
        ui->actionKIOFluid->setChecked(false);
        ui->actionKIORigid->setChecked(false);
        ui->actionKIOTriangleMesh->setChecked(false);
        ui->actionKIOVolume->setChecked(false);
        ui->actionKIOVolume->setChecked(false);
        ui->actionShow_ALL->setChecked(false);

        NENodeGraphEditorScene::setPortsFilterType(-2);
        update();
    }
#endif
}

void NENodeGraphEditorWindow::setFilterNodeByConnection(){
#ifdef USE_OLD_PORT
    NENodeGraphEditorScene::setNodeFilterByConnection(ui->actionBy_Node_Without_Connection->isChecked());
    update();
#endif
}

void NENodeGraphEditorWindow::setFilterNodeByAll()
{
#ifdef USE_OLD_PORT
    ui->actionNode_All->setChecked(true);
    ui->actionKObjectExperimental->setChecked(false);
    ui->actionkObjectFluid->setChecked(false);
    ui->actionkObjectChemistry->setChecked(false);
    ui->actionkObjectParticleFluidSolver->setChecked(false);
    ui->actionKObjectParticleRigid->setChecked(false);
    ui->actionKObjectParticleDeformable->setChecked(false);
    ui->actionKObjectRigidSolver->setChecked(false);
    ui->actionKObjectKiller->setChecked(false);
    ui->actionkObjectGridFluidSolver->setChecked(false);
    ui->actionKObjectCollider->setChecked(false);
    ui->actionKObjectMeshing->setChecked(false);
    ui->actionKObjectMaterial->setChecked(false);
    ui->actionKObjectParticleSource->setChecked(false);
    ui->actionKObjectCamera->setChecked(false);
    ui->actionKObjectLight->setChecked(false);
    ui->actionKObjectMeshPP->setChecked(false);
    ui->actionKObjectDataField->setChecked(false);
    ui->actionKObjectForceField->setChecked(false);
    ui->actionKObjectParticleEmitter->setChecked(false);
    ui->actionKObjectMeshVolumeOP->setChecked(false);
    ui->actionKObjectVolumeOP->setChecked(false);
    ui->actionKObjectMeasurementField->setChecked(false);
    ui->actionKObjectGroup->setChecked(false);
    ui->actionKObjectDataCollector->setChecked(false);
    ui->actionKObjectDualBoundary->setChecked(false);
    ui->actionKObjectOSD->setChecked(false);
    ui->actionKObjectClipPlane->setChecked(false);
    ui->actionKObjectGizmo->setChecked(false);

    NENodeGraphEditorScene::setNodeFilterType(-1);
    update();
#endif
}


void NENodeGraphEditorWindow::setFilterNodeBykObjectExperimental()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectExperimental);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectFluid()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectFluid);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectChemistry()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectChemistry);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectParticleFluidSolver()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectParticleFluidSolver);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectParticleRigid()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectParticleRigid);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectParticleDeformable()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectParticleDeformable);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectRigidSolver()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectRigidSolver);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectKiller()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectKiller);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectGridFluidSolver()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectGridFluidSolver);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectCollider()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectCollider);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectMeshing()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectMeshing);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectMaterial()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectMaterial);
#endif
}


void NENodeGraphEditorWindow::setFilterNodeBykObjectParticleSource()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectParticleSource);
#endif
}





void NENodeGraphEditorWindow::setFilterNodeBykObjectCamera()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectCamera);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectLight()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectLight);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectMeshPP()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectMeshPP);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectDataField()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectDataField);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectForceField()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectForceField);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectParticleEmitter()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectParticleEmitter);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectMeshVolumeOP()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectMeshVolumeOP);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectVolumeOP()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectVolumeOP);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectMeasurementField()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectMeasurementField);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectGroup()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectGroup);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectDataCollector()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectDataCollector);
#endif
}




void NENodeGraphEditorWindow::setFilterNodeBykObjectDualBoundary()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectDualBoundary);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectOSD()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectOSD);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectClipPlane()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectClipPlane);
#endif
}

void NENodeGraphEditorWindow::setFilterNodeBykObjectGizmo()
{
#ifdef USE_OLD_PORT
    setFilterByNode(NE::kObjectGizmo);
#endif
}



void NENodeGraphEditorWindow::showModelingNodeGraphScene()
{
#ifdef USE_OLD_PORT
    NENodeGraphEditorScene::setSceneType( NENodeGraphEditorScene::Modeling );
    ui->modelingNodeGraphicsView->show();
    ui->simulationNodeGraphicsView->hide();
    ui->visualizationNodeGraphicsView->hide();
    ui->modelingNodeGraphicsView->update();
#endif
}

void NENodeGraphEditorWindow::showSimulationNodeGraphScene(){
#ifdef USE_OLD_PORT
    NENodeGraphEditorScene::setSceneType( NENodeGraphEditorScene::Simulation );
    ui->modelingNodeGraphicsView->hide();
    ui->simulationNodeGraphicsView->show();
    ui->visualizationNodeGraphicsView->hide();
    ui->simulationNodeGraphicsView->update();
#endif
}

void NENodeGraphEditorWindow::showVisualizationNodeGraphScene()
{
#ifdef USE_OLD_PORT
    NENodeGraphEditorScene::setSceneType( NENodeGraphEditorScene::Visualization );
    ui->modelingNodeGraphicsView->hide();
    ui->simulationNodeGraphicsView->hide();
    ui->visualizationNodeGraphicsView->show();
    ui->visualizationNodeGraphicsView->update();
#endif
}

void NENodeGraphEditorWindow::setFilterByNode(int nodeType){
    ui->actionNode_All->setChecked(false);
    NENodeGraphEditorScene::setNodeFilterType(nodeType);
    update();
}


void NENodeGraphEditorWindow::setFilterByConnection(int portType)
{
    ui->actionShow_ALL->setChecked(false);
    ui->actionAny->setChecked(false);

    NENodeGraphEditorScene::setPortsFilterType(portType);
    update();
}

void NENodeGraphEditorWindow::nodeTabSelected()
{
    if(ui->nodeTabWidget->currentIndex() == 0){ // Modeling Node Graph
        showModelingNodeGraphScene();
    }
    else if(ui->nodeTabWidget->currentIndex() == 1){ // Simulation Node Graph
        showSimulationNodeGraphScene();
    }
    else if(ui->nodeTabWidget->currentIndex() == 2){ // Visualization Node Graph
        showVisualizationNodeGraphScene();
    }
}

NENodeGraphEditor* NENodeGraphEditorWindow::currentNodeGraphicsView()
{
    if(!ui->modelingNodeGraphicsView->isHidden())
        return ui->modelingNodeGraphicsView;
    else if(!ui->simulationNodeGraphicsView->isHidden())
        return ui->simulationNodeGraphicsView;
    else
        return ui->visualizationNodeGraphicsView;
}

void NENodeGraphEditorWindow::frameSelection()
{
    currentNodeGraphicsView()->frameSelection();
}

void NENodeGraphEditorWindow::snapToGrid(bool bChecked)
{
    ui->modelingNodeGraphicsView->snapToGrid(bChecked);
    ui->simulationNodeGraphicsView->snapToGrid(bChecked);
    ui->visualizationNodeGraphicsView->snapToGrid(bChecked);
}
