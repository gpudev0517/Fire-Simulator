#pragma once

class NENodeGraphEditorScene;
class NENode;

#include "Widgets/Menu/NEMenu.h"

class NENodeGraphEditor : public QGraphicsView
{
    Q_OBJECT


    enum NavigationMode
    {
        kNaviNone = 0,
        kNaviPan = 1,
        kNaviRotate = 2,
        kNaviZoom = 4
    };


public:
    NENodeGraphEditor(QWidget *parent = 0);
    virtual ~NENodeGraphEditor();

    void selectItem(NENode* node);
    void selectItem(QList<NENode*>& nodes);
    void update();

    void frameSelection();
    void snapToGrid(bool bChecked);

public slots:
    void zoomIn();
    void zoomOut();
    void showContextMenu(const QPoint& pos);
    void createContextMenuTree();
    void removeContextMenuTree();
    void addCreateAction(QAction *action, uint nodeType);
    void removeCreateAction(QAction* action, uint nodeType);
    void removeSelectedItem();

protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    QGraphicsItem* findNEConnectionItem(const QPointF posF);

    void scaleView(double scaleFactor);
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;

private:

    NENodeGraphEditorScene* m_pScene;
    NavigationMode m_NaviMode;
    QPoint m_MousePosOld;

    NEMenu* m_pContextMenu;
    NEMenu *menuAdd;
    NEMenu *menuPhysicals;
    NEMenu *menuColliders;
    NEMenu *menuFluids;
    NEMenu *menuChemistry;
    NEMenu *menuParticleFluidSolvers;
    NEMenu *menuGridFluidSolvers;
    NEMenu *menuRigid;
    NEMenu *menuDeformable;
    NEMenu *menuRigidSolvers;
    NEMenu *menuDaemons;
    NEMenu *menuKillerFields;
    NEMenu *menuEmitters;
    NEMenu *menuDualBoundaries;
    NEMenu *menuForceFields;
    NEMenu *menuDataFields;
    NEMenu *menuMeshing;
    NEMenu *menuVisual;
    NEMenu *menuCameras;
    NEMenu *menuLights;
    NEMenu *menuMaterials;
    NEMenu *menuOSD;
    NEMenu *menuGizmo;
    NEMenu *menuPostProcessors;
    NEMenu *menuVolume;

    QAction *actionRemove;
};

