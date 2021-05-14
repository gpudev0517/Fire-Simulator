#include "Rendering/Manager/NERenderManager.h"
#include "FDYNExternalRendererSettingsWindow.h"
#include "ui_FDYNExternalRendererSettingsWindow.h"

#include "Rendering/Utility/RenderSettings/NERenderSetting.h"
#include "Rendering/Utility/RenderSettings/NESpecificRenderSetting.h"

#define RENDERSET (NERENDERMAN.RenderSetting())

FDYNExternalRendererSettingsWindow::FDYNExternalRendererSettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FDYNExternalRendererSettingsWindow)
{
    QString STYLE_COLOR_TABWIDGET_BACKGROUND_SELECTED = "#282629";
    QString STYLE_COLOR_TABWIDGET_BACKGROUND = "#343235";
    QString STYLE_COLOR_FOREGROUND = "white";
    QString uiStyle =
        "QWidget{"
            "background: " + STYLE_COLOR_TABWIDGET_BACKGROUND_SELECTED + ";}"
        "  QTabWidget::pane {"
            "border: 2px " + STYLE_COLOR_TABWIDGET_BACKGROUND + ";}"
        "QTabBar::tab{"
            "background: " + STYLE_COLOR_TABWIDGET_BACKGROUND_SELECTED + ";"
            "color: " + STYLE_COLOR_FOREGROUND + ";"
            "height: 35px;"
            "width:  150px;"
            "font: 8pt 'Segoe UI';"
            "font-weight:bold;"
            "border-top-left-radius: 10px;"
            "border-top-right-radius: 10px;}"
        "QTabBar::tab:selected {"
            "background: " + STYLE_COLOR_TABWIDGET_BACKGROUND + ";}"
    ;

    this->setStyleSheet(uiStyle);

    ui->setupUi(this);
    connect(&RENDERSET, &NERenderSetting::RendererChanged, this, &FDYNExternalRendererSettingsWindow::changeRenderer);
    connect(ui->actionRefresh, &QAction::triggered, this, &FDYNExternalRendererSettingsWindow::refresh);
    ui->generalTab->setObject(&RENDERSET);
}

FDYNExternalRendererSettingsWindow::~FDYNExternalRendererSettingsWindow()
{
    delete ui;
}

void FDYNExternalRendererSettingsWindow::changeRenderer()
{
    ui->specificTab->setObject(static_cast<QObject*>(RENDERSET.specificSetting()));
}

void FDYNExternalRendererSettingsWindow::refresh()
{
    ui->generalTab->updateProperties();
    ui->specificTab->updateProperties();
}

void FDYNExternalRendererSettingsWindow::showEvent(QShowEvent* event)
{
    refresh();
    QMainWindow::showEvent(event);
}
