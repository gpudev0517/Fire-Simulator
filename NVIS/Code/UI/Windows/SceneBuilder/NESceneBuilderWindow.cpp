#include "NESceneBuilderWindow.h"
#include "ui_NESceneBuilderWindow.h"
#include "Utilities/NEGUIStyle.h"

NESceneBuilderWindow::NESceneBuilderWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NESceneBuilderWindow)
{
    ui->setupUi(this);
}

NESceneBuilderWindow::~NESceneBuilderWindow()
{
    delete ui;
}

void NESceneBuilderWindow::addCreateAction(QAction *action, uint nodeType)
{
    // Create a button
    QPushButton* button = new QPushButton(action->text());
    button->setObjectName(action->text());
    // Connect the button's clicked action to the action's activate signal
    connect(button, &QPushButton::clicked, action, &QAction::trigger);

    // Depending on the type of the action, add the button to the appropriate layout
    switch(nodeType)
    {
    case NE::kObjectFluid:
        ui->fluidLayout->insertWidget(0, button);
        break;
    case NE::kObjectChemistry:
        ui->chemistryLayout->insertWidget(0, button);
        break;
    case NE::kObjectParticleFluidSolver:
        ui->particleFluidSolverLayout->insertWidget(0, button);
        break;
    case NE::kObjectGridFluidSolver:
        ui->gridFluidSolverLayout->insertWidget(0, button);
        break;
    case (NE::kObjectParticleRigid):
        ui->rigidLayout->insertWidget(0, button);
        break;
    case (NE::kObjectParticleDeformable):
        ui->deformableLayout->insertWidget(0, button);
        break;
    case (NE::kObjectRigidSolver):
        ui->rigidSolverLayout->insertWidget(0, button);
        break;
    case (NE::kObjectMeshing):
        ui->meshConstructorLayout->insertWidget(0, button);
        break;
    case (NE::kObjectMeshing + NE::kObjectExperimental):
        ui->meshingExpLayout->insertWidget(0, button);
        break;
    case (NE::kObjectMaterial):
        ui->materialsLayout->insertWidget(0, button);
        break;
    case (NE::kObjectParticleSource):
        ui->emittersLayout->insertWidget(0, button);
        break;
    case (NE::kObjectMeshPP):
        ui->meshPPLayout->insertWidget(0, button);
        break;
    case (NE::kObjectDataField):
        ui->dataFieldLayout->insertWidget(0, button);
        break;
    case (NE::kObjectForceField):
        ui->forceFieldLayout->insertWidget(0, button);
        break;
    case (NE::kObjectKiller):
        ui->killerFieldsLayout->insertWidget(0, button);
        break;
    case (NE::kObjectLight):
        ui->lightsLayout->insertWidget(0, button);
        break;
    case (NE::kObjectCamera):
        ui->camerasLayout->insertWidget(0, button);
        break;
    case (NE::kObjectParticleEmitter):
        ui->emittersLayout->insertWidget(0, button);
        break;
    case (NE::kObjectDualBoundary):
        ui->dualBoundariesLayout->insertWidget(0, button);
        break;
    case (NE::kObjectOSD):
        ui->osdLayout->insertWidget(0, button);
        break;
    case (NE::kObjectGizmo):
        ui->gizmoLayout->insertWidget(0, button);
        break;
    case (NE::kObjectMeshVolumeOP):
        ui->volumeConvertionsLayout->insertWidget(0, button);
        break;
    case (NE::kObjectVolumeOP):
        ui->volumeConvertionsLayout->insertWidget(0, button);
        break;
    case (NE::kObjectMeasurementField):
        ui->measurementFieldLayout->insertWidget(0, button);
        break;
    default:
        ui->particleFluidSolverLayout->insertWidget(0, button);
    }
}

void NESceneBuilderWindow::removeCreateAction(QAction* action, uint nodeType)
{
    QList<QPushButton*> pbl = ui->sceneBuilderTabWidget->findChildren<QPushButton*>(action->text());
    if(pbl.isEmpty())
        return;
    QPushButton* button = pbl.first();
    delete button;
}



