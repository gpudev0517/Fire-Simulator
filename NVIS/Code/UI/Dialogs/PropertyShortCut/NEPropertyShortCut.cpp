#include "NEPropertyShortCut.h"
#include "ui_NEPropertyShortCut.h"
#include "Spatial/NESpatial.h"
#include "Managers/NEGUIManager.h"

NEPropertyShortCut::NEPropertyShortCut(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NEPropertyShortCut)
{
    ui->setupUi(this);
    setWindowFlags(Qt::SplashScreen);

    ui->lineXValue->setValidator(new QDoubleValidator(0, 1000, 5, this));
    ui->lineYValue->setValidator(new QDoubleValidator(0, 1000, 5, this));
    ui->lineZValue->setValidator(new QDoubleValidator(0, 1000, 5, this));

    QObject::connect(ui->lineXValue, SIGNAL(editingFinished()), this, SLOT(performChangedX()));
    QObject::connect(ui->lineYValue, SIGNAL(editingFinished()), this, SLOT(performChangedY()));
    QObject::connect(ui->lineZValue, SIGNAL(editingFinished()), this, SLOT(performChangedZ()));
}

NEPropertyShortCut::~NEPropertyShortCut()
{
    delete ui;
}

void NEPropertyShortCut::performChangedX()
{
    bool ok = false;
    float f = ui->lineXValue->text().toFloat(&ok);
    if(m_CurSpatial && ok){
        switch (m_curMode) {
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
        default:
            break;
        }
    }
    if(m_CurGLWidget){
        m_CurGLWidget->update();
        NEGUI.updatePropertyEditor();
    }
}

void NEPropertyShortCut::performChangedY()
{
    bool ok = false;
    float f = ui->lineYValue->text().toFloat(&ok);
    if(m_CurSpatial && ok){
        switch (m_curMode) {
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
        default:
            break;
        }
    }
    if(m_CurGLWidget){
        m_CurGLWidget->update();
        NEGUI.updatePropertyEditor();
    }
}

void NEPropertyShortCut::performChangedZ()
{
    bool ok = false;
    float f = ui->lineZValue->text().toFloat(&ok);
    if(m_CurSpatial && ok){
        switch (m_curMode) {
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
        default:
            break;
        }
    }
    if(m_CurGLWidget){
        m_CurGLWidget->update();
        NEGUI.updatePropertyEditor();
    }
}


void NEPropertyShortCut::setMode(QWidget *glWidget, NESpatial* spatial, uint mode)
{
    m_CurGLWidget = glWidget;
    m_CurSpatial = spatial;
    m_curMode = mode;
    if(mode == NEManipulator::kManipTranslate){

        ui->topLabel->setText("Position");
        ui->lineXValue->setText(QString::number(spatial->Positionx()));
        ui->lineYValue->setText(QString::number(spatial->Positiony()));
        ui->lineZValue->setText(QString::number(spatial->Positionz()));
    }
    else if(mode ==  NEManipulator::kManipRotate){
        ui->topLabel->setText("Orientation");
        ui->lineXValue->setText(QString::number(spatial->Orientation().phi()));
        ui->lineYValue->setText(QString::number(spatial->Orientation().theta()));
        ui->lineZValue->setText(QString::number(spatial->Orientation().psi()));
    }
    else if( mode == NEManipulator::kManipScale){
        ui->topLabel->setText("Scale");
        ui->lineXValue->setText(QString::number(spatial->Scalex()));
        ui->lineYValue->setText(QString::number(spatial->Scaley()));
        ui->lineZValue->setText(QString::number(spatial->Scalez()));
    }

}

void NEPropertyShortCut::accept()
{
    QDialog::accept();

}
void NEPropertyShortCut::reject()
{
    QDialog::reject();
}
