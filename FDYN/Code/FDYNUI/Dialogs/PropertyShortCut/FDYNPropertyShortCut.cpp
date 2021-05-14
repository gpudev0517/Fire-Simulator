#include "FDYNPropertyShortCut.h"
#include "ui_FDYNPropertyShortCut.h"
#include "Spatial/NESpatial.h"
#include "Managers/FDYNGUIManager.h"
#include "Windows/Main/FDYNMainWindow.h"
#include "Managers/FDYNGUIManager_p.h"
#include "Geometry/Mesh/NEZone.h"
#include "Geometry/Mesh/NEZoneCompartment.h"

FDYNPropertyShortCut::FDYNPropertyShortCut(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FDYNPropertyShortCut)
{
    ui->setupUi(this);
    setWindowFlags(Qt::SplashScreen);

    //ui->lineXValue->setValidator(new QDoubleValidator(-1000, 1000, 5, this));
    //ui->lineYValue->setValidator(new QDoubleValidator(-1000, 1000, 5, this));
    //ui->lineZValue->setValidator(new QDoubleValidator(-1000, 1000, 5, this));

    QObject::connect(ui->lineXValue, SIGNAL(editingFinished()), this, SLOT(performChangedX()));
    QObject::connect(ui->lineYValue, SIGNAL(editingFinished()), this, SLOT(performChangedY()));
    QObject::connect(ui->lineZValue, SIGNAL(editingFinished()), this, SLOT(performChangedZ()));

    QString uiStyleRed =
    "QLabel {"
        "color: #FF0000;"
        "background: none;}"
     ;

    QString uiStyleGreen =
    "QLabel {"
        "color: #00FF00;"
        "background: none;}"
     ;

    QString uiStyleBlue =
    "QLabel {"
        "color: #0000FF;"
        "background: none;}"
     ;

    ui->label->setStyleSheet(uiStyleRed);
    ui->label_2->setStyleSheet(uiStyleGreen);
    ui->label_3->setStyleSheet(uiStyleBlue);
}

FDYNPropertyShortCut::~FDYNPropertyShortCut()
{
    delete ui;
}

void FDYNPropertyShortCut::performChangedX()
{
    bool ok = false;
    float f = ui->lineXValue->text().toFloat(&ok);
    if(ui->topLabel->text() == "Snap Unit" && m_GLWidget)
    {
        float f2 = ui->lineYValue->text().toFloat(&ok);
        float f3 = ui->lineZValue->text().toFloat(&ok);
        m_GLWidget->setSnapUnit(vec3f(f,f2,f3));
        emit m_GLWidget->updatePropertyEditorUI();
        emit m_GLWidget->updateGLWindows();

    }
    else if(m_CurSpatial && ok)
    {
        switch (m_curMode)
        {
        case NEManipulator::kManipTranslate:
            m_CurSpatial->SetPositionx(f);
            break;
        case NEManipulator::kManipRotate:
        {
            eulerf angle = m_CurSpatial->Orientation();
            angle.setPhi(f);
            m_CurSpatial->SetOrientation(angle);
            break;
        }
        case NEManipulator::kManipScale:
            m_CurSpatial->SetScalex(f);
            break;
        case NEGLWidget::NE3DTool::kSize:
            break;
        case NEGLWidget::NE3DTool::kLocation:
            break;
        default:
            break;
        }
    }

    foreach(const GLDockWidgetBundle* w, FDYNGUI.P->m_IDtoGLDockWidget)
    {
        w->glWidget->update();
    }
    FDYNGUI.updatePropertyEditor();

//    if(m_CurGLWidget){
//        m_CurGLWidget->update();
//        FDYNGUI.updatePropertyEditor();
//    }
}

void FDYNPropertyShortCut::performChangedY()
{
    bool ok = false;
    float f = ui->lineYValue->text().toFloat(&ok);    
    if(ui->topLabel->text() == "Snap Unit" && m_GLWidget)
    {
        float f2 = ui->lineXValue->text().toFloat(&ok);
        float f3 = ui->lineZValue->text().toFloat(&ok);
        m_GLWidget->setSnapUnit(vec3f(f2,f,f3));
        emit m_GLWidget->updatePropertyEditorUI();
        emit m_GLWidget->updateGLWindows();
    }
    else if(m_CurSpatial && ok)
    {
        switch (m_curMode)
        {
        case NEManipulator::kManipTranslate:
            m_CurSpatial->SetPositiony(f);
            break;
        case NEManipulator::kManipRotate:
        {
            eulerf angle = m_CurSpatial->Orientation();
            angle.setTheta(f);
            m_CurSpatial->SetOrientation(angle);
            break;
        }
        case NEManipulator::kManipScale:
            m_CurSpatial->SetScaley(f);
            break;
        case NEGLWidget::NE3DTool::kSize:
            break;
        case NEGLWidget::NE3DTool::kLocation:
            break;
        default:
            break;
        }
    }

    foreach(const GLDockWidgetBundle* w, FDYNGUI.P->m_IDtoGLDockWidget)
    {
        w->glWidget->update();
    }
    FDYNGUI.updatePropertyEditor();

//    if(m_CurGLWidget){
//        m_CurGLWidget->update();
//        FDYNGUI.updatePropertyEditor();
//    }
}

void FDYNPropertyShortCut::performChangedZ()
{
    bool ok = false;
    float f = ui->lineZValue->text().toFloat(&ok);    
    if(ui->topLabel->text() == "Snap Unit" && m_GLWidget)
    {
        float f2 = ui->lineXValue->text().toFloat(&ok);
        float f3 = ui->lineYValue->text().toFloat(&ok);
        m_GLWidget->setSnapUnit(vec3f(f2,f3,f));
        emit m_GLWidget->updatePropertyEditorUI();
        emit m_GLWidget->updateGLWindows();
    }
    else if(m_CurSpatial && ok)
    {
        switch (m_curMode)
        {
        case NEManipulator::kManipTranslate:
            m_CurSpatial->SetPositionz(f);
            break;
        case NEManipulator::kManipRotate:
        {
            eulerf angle = m_CurSpatial->Orientation();
            angle.setPsi(f);
            m_CurSpatial->SetOrientation(angle);
            break;
        }
        case NEManipulator::kManipScale:
            m_CurSpatial->SetScalez(f);
            break;
        case NEGLWidget::NE3DTool::kSize:
            break;
        case NEGLWidget::NE3DTool::kLocation:
            break;
        default:
            break;
        }
    }

    foreach(const GLDockWidgetBundle* w, FDYNGUI.P->m_IDtoGLDockWidget)
    {
        w->glWidget->update();
    }
    FDYNGUI.updatePropertyEditor();
//    if(m_CurGLWidget){
//        m_CurGLWidget->update();
//        FDYNGUI.updatePropertyEditor();
//    }
}

void FDYNPropertyShortCut::setMode(QWidget *glWidget, NESpatial* spatial, uint mode)
{
    m_CurGLWidget = glWidget;
    m_CurSpatial = spatial;
    m_curMode = mode;
    if(mode == NEManipulator::kManipTranslate)
    {
        ui->topLabel->setText("Position");
        ui->lineXValue->setText(QString::number(spatial->Positionx()));
        ui->lineYValue->setText(QString::number(spatial->Positiony()));
        ui->lineZValue->setText(QString::number(spatial->Positionz()));
    }
    else if(mode ==  NEManipulator::kManipRotate)
    {
        ui->topLabel->setText("Orientation");
        ui->lineXValue->setText(QString::number(spatial->Orientation().phi()));
        ui->lineYValue->setText(QString::number(spatial->Orientation().theta()));
        ui->lineZValue->setText(QString::number(spatial->Orientation().psi()));
    }
    else if( mode == NEManipulator::kManipScale)
    {
        ui->topLabel->setText("Scale");
        ui->lineXValue->setText(QString::number(spatial->Scalex()));
        ui->lineYValue->setText(QString::number(spatial->Scaley()));
        ui->lineZValue->setText(QString::number(spatial->Scalez()));
    }
    else if( mode == NEGLWidget::NE3DTool::kSize ) // zone size mode
    {
        ui->topLabel->setText("Size");
    }
    else if( mode == NEGLWidget::NE3DTool::kLocation ) // zone location mode
    {
        ui->topLabel->setText("Location");
    }
}

void FDYNPropertyShortCut::setSnapUnitMode(QWidget* glWidget)
{
    m_GLWidget = qobject_cast<NEGLWidget *>(glWidget);
    ui->topLabel->setText("Snap Unit");
    ui->lineXValue->setText(QString::number(m_GLWidget->snapUnit().x()));
    ui->lineYValue->setText(QString::number(m_GLWidget->snapUnit().y()));
    ui->lineZValue->setText(QString::number(m_GLWidget->snapUnit().z()));
    //performChangeSnapUnit(widget);

}

void FDYNPropertyShortCut::setGridMode(vec3f selPt){
    ui->topLabel->setText("Position");
    ui->lineXValue->setReadOnly(true);
    ui->lineYValue->setReadOnly(true);
    ui->lineZValue->setReadOnly(true);

    ui->lineXValue->setAlignment(Qt::AlignHCenter);
    ui->lineYValue->setAlignment(Qt::AlignHCenter);
    ui->lineZValue->setAlignment(Qt::AlignHCenter);

    ui->lineXValue->setText(QString::number(selPt.x()));
    ui->lineYValue->setText(QString::number(selPt.y()));
    ui->lineZValue->setText(QString::number(selPt.z()));
}

void FDYNPropertyShortCut::accept()
{
    FDYNGUI.mainWindow()->selectCursorMode();
    QDialog::accept();

}
void FDYNPropertyShortCut::reject()
{
    FDYNGUI.mainWindow()->selectCursorMode();
    QDialog::reject();
}

void FDYNPropertyShortCut::mousePressEvent(QMouseEvent *e)
{
    QDialog::mousePressEvent(e);
    m_pressPos = e->globalPos() - pos();

}
void FDYNPropertyShortCut::mouseReleaseEvent(QMouseEvent *e)
{
    QDialog::mouseReleaseEvent(e);
}
void FDYNPropertyShortCut::mouseMoveEvent(QMouseEvent *e)
{
    QDialog::mouseMoveEvent(e);
    move(e->globalPos()-m_pressPos);
}
