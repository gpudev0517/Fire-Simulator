#pragma once

class NENodeGraphEditorScene;
class NENode;
class NENodeGraphEditor;

namespace Ui {
class NENodeGraphEditorWindow;
}

class NENodeGraphEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NENodeGraphEditorWindow(QWidget *parent = 0);
    bool init();
    //void setScene(NENodeGraphEditorScene *scene);
    ~NENodeGraphEditorWindow();

    void update();
    void updateForDeletion();
    void selectItem(NENode* node);
    void selectItem(QList<NENode*>& nodes);
    void writeGUIState(QXmlStreamWriter* writer);
    void readGUIState(QXmlStreamReader* reader);


    void layout(bool bFullPerform = false);
    void updateInitFlag(bool aInitFlag);
    void updateNodeRenameForNodeGraph(const QString &oldName, const QString &newName);

    void setFilterByConnection(int portType);
    void setFilterByNode(int nodeType);

public slots:
    void setLinearConnectionCurves();
    void setQuadraticConnectionCurves();
    void setCubicConnectionCurves();
    void addCreateAction(QAction *action, uint nodeType);
    void removeCreateAction(QAction* action, uint nodeType);

    void setFilterALl();
    void setFilterBykIOExtent();
    void setFilterBykIOParticles();
    void setFilterBykIOTriangleMesh();
    void setFilterBykIOForceField();
    void setFilterBykIOVolume();
    void setFilterBykIOFlowRate();
    void setFilterBykIOFluidHeight();
    void setFilterBykIONumParticles();
    void setFilterBykIONumParticlesDrain();
    void setFilterBykIOHydrostaticPressure();
    void setFilterBykIOFluid();
    void setFilterBykIORigid();
    void setFilterBykIOMeasurement();
    void setFilterBykIOGizmo();
    void setFilterBykIOGeoConversion();
    void setHideNoConnnectionFilter();
    void setFilterNodeByConnection();
    void setFilterNodeBykObjectExperimental();
    void setFilterNodeByAll();
    void setFilterNodeBykObjectFluid();
    void setFilterNodeBykObjectChemistry();
    void setFilterNodeBykObjectParticleFluidSolver();
    void setFilterNodeBykObjectParticleRigid();
    void setFilterNodeBykObjectParticleDeformable();
    void setFilterNodeBykObjectRigidSolver();
    void setFilterNodeBykObjectKiller();
    void setFilterNodeBykObjectGridFluidSolver();
    void setFilterNodeBykObjectCollider();
    void setFilterNodeBykObjectMeshing();
    void setFilterNodeBykObjectMaterial();
    void setFilterNodeBykObjectParticleSource();
    void setFilterNodeBykObjectCamera();
    void setFilterNodeBykObjectLight();
    void setFilterNodeBykObjectMeshPP();
    void setFilterNodeBykObjectDataField();
    void setFilterNodeBykObjectForceField();
    void setFilterNodeBykObjectParticleEmitter();
    void setFilterNodeBykObjectMeshVolumeOP();
    void setFilterNodeBykObjectVolumeOP();
    void setFilterNodeBykObjectMeasurementField();
    void setFilterNodeBykObjectGroup();
    void setFilterNodeBykObjectDataCollector();
    void setFilterNodeBykObjectDualBoundary();
    void setFilterNodeBykObjectOSD();
    void setFilterNodeBykObjectClipPlane();
    void setFilterNodeBykObjectGizmo();
    void showModelingNodeGraphScene();
    void showSimulationNodeGraphScene();
    void showVisualizationNodeGraphScene();
    void nodeTabSelected();
    NENodeGraphEditor* currentNodeGraphicsView();
    void frameSelection();
    void snapToGrid(bool bChecked);

    void layoutCircle();
    void layoutForceDirected();
    void layoutSpring();
    void layoutRandom();
public:
    Ui::NENodeGraphEditorWindow *ui;
    QActionGroup* m_pActionGroup;
    QMenu* m_pCurveViewMenu;
    QActionGroup* m_pNodeGraphSceneGroup;
    //NENodeGraphEditorScene* m_pScene;

protected:
	virtual void showEvent(QShowEvent *event);
};
