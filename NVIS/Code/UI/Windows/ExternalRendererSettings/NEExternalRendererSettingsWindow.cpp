#include "Rendering/Manager/NERenderNEOManager.h"
#include "NEExternalRendererSettingsWindow.h"
#include "ui_NEExternalRendererSettingsWindow.h"

#include "Rendering/Utility/RenderSettings/NERenderSetting.h"
#include "Rendering/Utility/RenderSettings/NESpecificRenderSetting.h"

#define RENDERSET (NERENDERMAN.RenderSetting())

NEExternalRendererSettingsWindow::NEExternalRendererSettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NEExternalRendererSettingsWindow)
{
    ui->setupUi(this);
    connect(&RENDERSET, &NERenderSetting::RendererChanged, this, &NEExternalRendererSettingsWindow::changeRenderer);
    connect(ui->actionRefresh, &QAction::triggered, this, &NEExternalRendererSettingsWindow::refresh);
    ui->generalTab->setObject(&RENDERSET);
}

NEExternalRendererSettingsWindow::~NEExternalRendererSettingsWindow()
{
    delete ui;
}

void NEExternalRendererSettingsWindow::changeRenderer()
{
    ui->specificTab->setObject(static_cast<QObject*>(RENDERSET.specificSetting()));
}

void NEExternalRendererSettingsWindow::refresh()
{
    ui->generalTab->updateProperties();
    ui->specificTab->updateProperties();
}

void NEExternalRendererSettingsWindow::showEvent(QShowEvent* event)
{
    refresh();
    QMainWindow::showEvent(event);
}
