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

    case NE::kObjectGridFluidSolver:
        ui->gridFluidSolverLayout->insertWidget(0, button);
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
    case (NE::kObjectLight):
        ui->lightsLayout->insertWidget(0, button);
        break;
    case (NE::kObjectCamera):
        ui->camerasLayout->insertWidget(0, button);
        break;
    case (NE::kObjectOSD):
        ui->osdLayout->insertWidget(0, button);
        break;
    case (NE::kObjectGizmo):
        ui->gizmoLayout->insertWidget(0, button);
        break;
#if 0
    case (NE::kObjectMSS):
        ui->mssLayout->insertWidget(0, button);
        break;
    case (NE::kObjectParticleSand):
        ui->sandLayout->insertWidget(0, button);
        break;
#endif
    case (NE::kObjectMeshVolumeOP):
        ui->volumeConvertionsLayout->insertWidget(0, button);
        break;
    case (NE::kObjectVolumeOP):
        ui->volumeConvertionsLayout->insertWidget(0, button);
        break;
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



