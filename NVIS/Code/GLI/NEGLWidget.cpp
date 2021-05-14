// For accessing all OpenGL functions


#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QOpenGLFunctions_4_5_Compatibility>


#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/NEGLRenderer.h"
#include "Utility/NEMatrixStack4x4.h"
#include "Geometry/Resource/Mesh/NEMeshContainer.h"



#include "Utility/NETimer.h"

#include "Base/NESceneManager.h"
#include "Base/NEBase.h"

#include "Rendering/Resource/NEMaterial.h"
#include "Rendering/Resource/NEVertexBuffer.h"
#include "Utility/geometry.h"
#include "Rendering/Spatial/NECamera.h"
#include "Rendering/Manager/NERenderManager.h"



#include "Spatial/NESpatial.h"
#include "Spatial/NESpatialGroup.h"
#include "Utility/EulerAngles.h"
#include <QOpenGLTexture>
#include <glm/glm.hpp>

#include "Rendering/Spatial/NELight.h"
#include "Rendering/Spatial/Light/NEDirectionalLight.h"
#include "Rendering/Spatial/Light/NEPointLight.h"
#include "Rendering/Spatial/Light/NESpotLight.h"
#include "Utility/NEOsdMsg.h"
#include "Rendering/Spatial/NEOSDBase.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Utility/stb_image.h"

#include "Utility/Math/vec234f.h"


#define NEWMANIP
// Gizmo headers
#include "gizmo/IGizmo.h"
#include "gizmo/GizmoTransformMove.h"
#include "gizmo/GizmoTransformRotate.h"
#include "gizmo/GizmoTransformScale.h"


#include "Mesh/NEZone.h"
#include "Mesh/NEZoneCompartment.h"


//#include "Core/Utility/globals.h"
#include "NEGLWidget.h"
#include "NEGLWidget_p.h"

bool NEGLWidget::m_ActiveManipOperation = 0;
uint NEGLWidget::m_ActiveManipulator = NEManipulator::kManipNone;
bool NEGLWidget::m_SnapToBoundary = false;
bool NEGLWidget::m_SnapToObj = false;
bool NEGLWidget::m_bSnapUnit = false;
bool NEGLWidget::m_SnapToEdge = false;
bool NEGLWidget::m_Measure = false;
bool NEGLWidget::m_MeasureEdge = false;
bool NEGLWidget::m_SnapModeChanged = false;

GLuint NEGLWidget::DL_Cone = 0;
GLuint NEGLWidget::DL_Cube = 0;

double NEGLWidget::m_SmallIncrementalAdjustment = 0.1;

enum NETrackballMode
{
    kTrackNone = 0,
    kTrackTranslate = 1,
    kTrackRotate = 2,
    kTrackZoom = 4,
    kTrackSmallTranslate = 5,
    kTrackSmallRotate = 6,
    kTrackSmallZoom = 7
};


enum NEToolId
{
    kToolTranslateX=1,
    kToolTranslateY,
    kToolTranslateZ,
    kToolTranslateXY,
    kToolTranslateXZ,
    kToolTranslateYZ,

    kToolRotateX,
    kToolRotateY,
    kToolRotateZ,
    kToolRotateXY,
    kToolRotateXZ,
    kToolRotateYZ,

    kToolScaleX,
    kToolScaleY,
    kToolScaleZ,
    kToolScaleXY,
    kToolScaleXZ,
    kToolScaleYZ
};

NE_PROP_DEF(bool, NEGLWidgetProperties, DrawGrid, true)
NE_PROP_DEF(vec3f, NEGLWidgetProperties, GridCenter, vec3f(0.0, 0.0, 0.0))
NE_PROP_DEF(eulerf, NEGLWidgetProperties, GridOrientation, eulerf(0.0, 0.0, 0.0))
NE_PROP_DEF(QSizeF, NEGLWidgetProperties, GridSize, QSizeF(50.0, 50.0))
NE_PROP_DEF(QSize, NEGLWidgetProperties, CellCount, QSize(10, 10))
NE_PROP_DEF(QSizeF, NEGLWidgetProperties, CellSize, QSizeF(5.0, 5.0))
NE_PROP_DEF(bool, NEGLWidgetProperties, AddGridLevel, true)
NE_PROP_DEF(QSize, NEGLWidgetProperties, CellDivision, QSize(5, 5))
NE_PROP_DEF(double, NEGLWidgetProperties, AxisThickness, 2.5)
NE_PROP_DEF(double, NEGLWidgetProperties, GridThickness, 2)
NE_PROP_DEF(double, NEGLWidgetProperties, SubGridThickness, 1)
NE_PROP_DEF(QColor, NEGLWidgetProperties, GridColor, QColor(255.0, 255.0, 255.0, 80.0))
NE_PROP_DEF(QColor, NEGLWidgetProperties, SubGridColor, QColor(255.0, 255.0, 255.0, 50.0))

static void initProperties()
{
    NE_INIT_PROP(NEGLWidgetProperties, DrawGrid);
    NE_INIT_PROP(NEGLWidgetProperties, GridCenter);
    NE_INIT_PROP(NEGLWidgetProperties, GridOrientation);
    NE_INIT_PROP(NEGLWidgetProperties, GridSize);
    NE_INIT_PROP(NEGLWidgetProperties, CellCount);
    NE_INIT_PROP(NEGLWidgetProperties, CellSize);
    NE_INIT_PROP(NEGLWidgetProperties, AddGridLevel);
    NE_INIT_PROP(NEGLWidgetProperties, CellDivision);
    NE_INIT_PROP(NEGLWidgetProperties, AxisThickness);
    NE_INIT_PROP(NEGLWidgetProperties, GridThickness);
    NE_INIT_PROP(NEGLWidgetProperties, SubGridThickness);
    NE_INIT_PROP(NEGLWidgetProperties, GridColor);
    NE_INIT_PROP(NEGLWidgetProperties, SubGridColor);
}

#define glVerify(x) x
#define Sign(x) ((0 < x) - (x < 0))
NEGLWidgetProperties::NEGLWidgetProperties(NEGLWidget *val)
{
    CALL_ONCE(initProperties);

    // Keep a pointer to the gl widget
    m_pGLWidget = val;

    // Initialize member variables
    m_DrawGrid = NE_DEFAULT(DrawGrid);
    m_GridCenter = NE_DEFAULT(GridCenter);
    m_GridOrientation = NE_DEFAULT(GridOrientation);
    m_GridSize = NE_DEFAULT(GridSize);
    m_CellCount = NE_DEFAULT(CellCount);
    m_CellSize = NE_DEFAULT(CellSize);
    m_AddGridLevel = NE_DEFAULT(AddGridLevel);
    m_CellDivision = NE_DEFAULT(CellDivision);
    m_AxisThickness = NE_DEFAULT(AxisThickness);
    m_GridThickness = NE_DEFAULT(GridThickness);
    m_SubGridThickness = NE_DEFAULT(SubGridThickness);
    m_GridColor = NE_DEFAULT(GridColor);
    m_SubGridColor = NE_DEFAULT(SubGridColor);

    m_Clipping = false;
    m_ClipPlaneCount = 1;
    //m_ClipPlane = vec4f(0,0,0,0);

    m_OsdTextColor.setRgb( 255, 255, 255 );
    m_SaveGLFrames   = false;
    m_SaveMotionMap      = false;

    m_manipSpace = Local;

    m_msaaSamples = MsaaSampling::x4;
    m_fxaaProfile = FxaaProfile::Fxaa_Extreme;
    m_csaaMode = CsaaMode::mode8x;
    m_bfaaMode = BfaaMode::multi2x;

    m_envList.resize( 6 );

    computeCellCount();
    computeSmallCellCount();
    computeSmallCellSize();

}

void NEGLWidgetProperties::setViewAxes(bool val)
{
    if(val == m_pGLWidget->viewAxes())
        return;

    m_pGLWidget->setViewAxes(val);
    emit viewAxesChanged(m_pGLWidget->viewAxes());
}

void NEGLWidgetProperties::setShowGizmo(bool val)
{
    if(val == m_pGLWidget->showGizmo())
        return;

    m_pGLWidget->setShowGizmo(val);
    //emit viewAxesChanged(m_pGLWidget->viewAxes());
}


void NEGLWidgetProperties::setSnapUnit(vec3f val)
{
    if(val == m_pGLWidget->snapUnit())
        return;

    m_pGLWidget->setSnapUnit(val);
}
void NEGLWidgetProperties::setClipping(bool val)
{
    if(val == m_Clipping)
        return;

    m_Clipping = val;
    emit clippingChanged(m_Clipping);
}

void NEGLWidgetProperties::setShowOsd(bool val)
{
    if(val == m_pGLWidget->showOsd())
        return;

    m_pGLWidget->setShowOsd(val);
    emit showOsdChanged(m_pGLWidget->showOsd());
}

void NEGLWidgetProperties::setShowFPS(bool val)
{
    if(val == m_pGLWidget->showFPS())
        return;

    m_pGLWidget->setShowFPS(val);
    emit showFPSChanged(m_pGLWidget->showFPS());
}

void NEGLWidgetProperties::setDrawRenderArea(bool val)
{
    if(val == m_pGLWidget->m_DrawRenderArea)
        return;

    m_pGLWidget->m_DrawRenderArea = val;
    emit drawRenderAreaChanged(m_pGLWidget->m_DrawRenderArea);
}

void NEGLWidgetProperties::setSaveGLFrames(bool val)
{
    m_SaveGLFrames = val;
}

void NEGLWidgetProperties::setImageSizeForSaving(NEImageSize s)
{
    m_pGLWidget->m_GlSize = s;
}

void NEGLWidgetProperties::setGridMeasurements(bool arg){
    if(m_pGLWidget->gridMeasurements() == arg)
        return;
    m_pGLWidget->setgridMeasurements(arg);
}

const bool& NEGLWidgetProperties::DrawGrid() const
{
    return m_DrawGrid;
}

uint NEGLWidgetProperties::SetDrawGrid(const bool& val)
{
    NE_PROP_SET(DrawGrid, this, val);

    NERENDERMAN.setDrawGrid(m_DrawGrid);

    m_pGLWidget->setDrawGrid(m_DrawGrid);
    emit DrawGridChanged(m_DrawGrid);

    return NE::kReturnSuccess;
}

const vec3f& NEGLWidgetProperties::GridCenter() const
{
    return m_GridCenter;
}

uint NEGLWidgetProperties::SetGridCenter(const vec3f& val)
{
    NE_PROP_SET(GridCenter, this, val);

    m_pGLWidget->buildFloor(m_GridCenter, m_GridOrientation, m_CellCount, m_CellSize, m_CellDivision);

    return NE::kReturnSuccess;
}

const eulerf& NEGLWidgetProperties::GridOrientation() const
{
    return m_GridOrientation;
}

uint NEGLWidgetProperties::SetGridOrientation(const eulerf& val)
{
    NE_PROP_SET(GridOrientation, this, val);

    m_pGLWidget->buildFloor(m_GridCenter, m_GridOrientation, m_CellCount, m_CellSize, m_CellDivision);

    return NE::kReturnSuccess;
}

const QSizeF& NEGLWidgetProperties::GridSize() const
{
    return m_GridSize;
}

uint NEGLWidgetProperties::SetGridSize(const QSizeF& val)
{
    if (val.width() <= 0.0 || val.height() <= 0.0)
        return NE::kReturnNotChanged;

    NE_PROP_SET(GridSize, this, val);

    computeCellCount();
    computeSmallCellCount();
    computeSmallCellSize();

    m_pGLWidget->setGridSize(m_GridSize);
    m_pGLWidget->buildFloor(m_GridCenter, m_GridOrientation, m_CellCount, m_CellSize, m_CellDivision);

    return NE::kReturnSuccess;
}

const QSize& NEGLWidgetProperties::CellCount() const
{
    return m_CellCount;
}

uint NEGLWidgetProperties::SetCellCount(const QSize& val)
{
    if (val.width() <= 0 || val.height() <= 0)
        return NE::kReturnNotChanged;

    NE_PROP_SET(CellCount, this, val);

    computeCellSize();
    computeSmallCellCount();
    computeSmallCellSize();

    m_pGLWidget->setCellCount(m_CellCount);
    m_pGLWidget->buildFloor(m_GridCenter, m_GridOrientation, m_CellCount, m_CellSize, m_CellDivision);

    return NE::kReturnSuccess;
}

const QSizeF& NEGLWidgetProperties::CellSize() const
{
    return m_CellSize;
}

uint NEGLWidgetProperties::SetCellSize(const QSizeF& val)
{
    if (val.width() <= 0.0 || val.height() <= 0.0)
        return NE::kReturnNotChanged;

    NE_PROP_SET(CellSize, this, val);

    computeCellCount();
    computeSmallCellCount();
    computeSmallCellSize();

    m_pGLWidget->setCellCount(m_CellCount);
    m_pGLWidget->buildFloor(m_GridCenter, m_GridOrientation, m_CellCount, m_CellSize, m_CellDivision);

    return NE::kReturnSuccess;
}

const bool& NEGLWidgetProperties::AddGridLevel() const
{
    return m_AddGridLevel;
}

uint NEGLWidgetProperties::SetAddGridLevel(const bool& val)
{
    NE_PROP_SET(AddGridLevel, this, val);

    m_pGLWidget->setAddGridLevel(m_AddGridLevel);
    m_pGLWidget->buildFloor(m_GridCenter, m_GridOrientation, m_CellCount, m_CellSize, m_CellDivision);

    return NE::kReturnSuccess;
}

const QSize& NEGLWidgetProperties::CellDivision() const
{
    return m_CellDivision;
}

uint NEGLWidgetProperties::SetCellDivision(const QSize& val)
{
    if (val.width() < 1 || val.height() < 1)
        return NE::kReturnNotChanged;

    NE_PROP_SET(CellDivision, this, val);

    computeSmallCellCount();
    computeSmallCellSize();

    m_pGLWidget->setCellDivision(m_CellDivision);
    m_pGLWidget->buildFloor(m_GridCenter, m_GridOrientation, m_CellCount, m_CellSize, m_CellDivision);

    return NE::kReturnSuccess;
}

const QSize& NEGLWidgetProperties::SmallCellCount() const
{
    return m_SmallCellCount;
}

const QSizeF& NEGLWidgetProperties::SmallCellSize() const
{
    return m_SmallCellSize;
}

const double& NEGLWidgetProperties::AxisThickness() const
{
    return m_AxisThickness;
}

uint NEGLWidgetProperties::SetAxisThickness(const double& val)
{
    if (val <= 0.0)
        return NE::kReturnNotChanged;

    NE_PROP_SET(AxisThickness, this, val);

    m_pGLWidget->setAxisThickness(m_AxisThickness);

    return NE::kReturnSuccess;
}

const double& NEGLWidgetProperties::GridThickness() const
{
    return m_GridThickness;
}

uint NEGLWidgetProperties::SetGridThickness(const double& val)
{
    if (val <= 0.0)
        return NE::kReturnNotChanged;

    NE_PROP_SET(GridThickness, this, val);

    m_pGLWidget->setGridThickness(m_GridThickness);

    return NE::kReturnSuccess;
}

const double& NEGLWidgetProperties::SubGridThickness() const
{
    return m_SubGridThickness;
}

uint NEGLWidgetProperties::SetSubGridThickness(const double& val)
{
    if (val <= 0.0)
        return NE::kReturnNotChanged;

    NE_PROP_SET(SubGridThickness, this, val);

    m_pGLWidget->setSubGridThickness(m_SubGridThickness);

    return NE::kReturnSuccess;
}

const QColor& NEGLWidgetProperties::GridColor() const
{
    return m_GridColor;
}

uint NEGLWidgetProperties::SetGridColor(const QColor& val)
{
    NE_PROP_SET(GridColor, this, val);

    m_pGLWidget->setGridColor(m_GridColor);
    m_pGLWidget->buildFloor(m_GridCenter, m_GridOrientation, m_CellCount, m_CellSize, m_CellDivision);

    return NE::kReturnSuccess;
}

const QColor& NEGLWidgetProperties::SubGridColor() const
{
    return m_SubGridColor;
}

uint NEGLWidgetProperties::SetSubGridColor(const QColor& val)
{
    NE_PROP_SET(SubGridColor, this, val);

    m_pGLWidget->setSubGridColor(m_SubGridColor);
    m_pGLWidget->buildFloor(m_GridCenter, m_GridOrientation, m_CellCount, m_CellSize, m_CellDivision);

    return NE::kReturnSuccess;
}

void NEGLWidgetProperties::computeCellCount()
{
    m_CellCount = QSize(m_GridSize.width()/m_CellSize.width(), m_GridSize.height()/m_CellSize.height());
}

void NEGLWidgetProperties::computeCellSize()
{
    m_CellSize = QSizeF(m_GridSize.width()/m_CellCount.width(), m_GridSize.height()/m_CellCount.height());
}

void NEGLWidgetProperties::computeSmallCellCount()
{
    m_SmallCellCount = QSize(m_CellCount.width()*m_CellDivision.width(), m_CellCount.height()*m_CellDivision.height());
}

void NEGLWidgetProperties::computeSmallCellSize()
{
    m_SmallCellSize = QSizeF(m_CellSize.width()/m_CellDivision.width(), m_CellSize.height()/m_CellDivision.height());
}

void NEGLWidgetProperties::resizeTimeout()
{

}

bool NEGLWidgetProperties::setCamera(QString arg)
{
    if(!m_Camera.isEmpty() && m_Camera == arg)
        return false;

    if(NERENDERMAN.cameras().contains(arg))
    {
        m_Camera = arg;
        m_pGLWidget->setCamera(NERENDERMAN.cameras()[arg]);
        emit m_pGLWidget->setCameraUI( m_pGLWidget->m_pCamera );
        //emit m_pGLWidget->updateGLWindows();
        m_pGLWidget->update();
        return true;
    }
    return false;
}

void NEGLWidgetProperties::setClipPlaneCount(int arg)
{
    if(arg < 0 || arg > 6)
        return;

    m_ClipPlaneCount = arg;
}

void NEGLWidgetProperties::setClipPlane1(const vec4f &val)
{
    m_ClipPlane[0] = val;
}

void NEGLWidgetProperties::setClipPlane2(const vec4f& arg)
{
    m_ClipPlane[1] = arg;
}

void NEGLWidgetProperties::setClipPlane3(const vec4f &arg)
{
    m_ClipPlane[2] = arg;
}

void NEGLWidgetProperties::setClipPlane4(const vec4f &arg)
{
    m_ClipPlane[3] = arg;
}

void NEGLWidgetProperties::setClipPlane5(const vec4f &arg)
{
    m_ClipPlane[4] = arg;
}

void NEGLWidgetProperties::setClipPlane6(const vec4f &arg)
{
    m_ClipPlane[5] = arg;
}

void NEGLWidgetProperties::setBackgroundTexture( FilePath arg )
{
    m_pGLWidget->setBackgroundTexture(arg);
}

void NEGLWidgetProperties::setSsao(bool ssao)
{
    m_pGLWidget->setSsao(ssao);
}

void NEGLWidgetProperties::setAaType(NEGLWidgetProperties::AntialiasingType aaType)
{
    if (m_pGLWidget->aaType() == aaType)
        return;

    m_pGLWidget->makeCurrent();

    NEGLWidgetProperties::AntialiasingType prevAaType = (AntialiasingType)m_pGLWidget->aaType();
    m_pGLWidget->setAaType((IGLSurface::AntialiasingType)aaType);

    if (aaType == AntialiasingType::Msaa)
        m_pGLWidget->setMsaaSamples(powf(2.0f,m_msaaSamples));
    else
        m_pGLWidget->setMsaaSamples(1);

    if (aaType == AntialiasingType::Csaa) {
        if(isSupportCsaa()){
            m_pGLWidget->setCsaaMode(m_csaaMode);
        }
        else {
            setAaType(prevAaType);
            return;
        }
    }

//    if (aaType == AntialiasingType::Bfaa)
//        m_pGLWidget->setBfaaMultiplier(powf(2.0f,m_bfaaMode));
//    else
        m_pGLWidget->setBfaaMultiplier(1);

    float pixelRatio = m_pGLWidget->devicePixelRatio();
    m_pGLWidget->updateBuffer( m_pGLWidget->width()*pixelRatio, m_pGLWidget->height()*pixelRatio, this->aaType());
}

void NEGLWidgetProperties::setMsaaSampling(MsaaSampling msaaSamples)
{
    if (m_msaaSamples == msaaSamples)
        return;

    m_pGLWidget->makeCurrent();

    m_msaaSamples = msaaSamples;

    if (aaType() == AntialiasingType::Msaa)
    {
        m_pGLWidget->setMsaaSamples(powf(2.0f,m_msaaSamples));
        float pixelRatio = m_pGLWidget->devicePixelRatio();
        m_pGLWidget->updateBuffer( m_pGLWidget->width()*pixelRatio, m_pGLWidget->height()*pixelRatio, aaType() );
    }
}

void NEGLWidgetProperties::setFxaaProfile(FxaaProfile fxaaProfile)
{
    m_fxaaProfile = fxaaProfile;
}

void NEGLWidgetProperties::setCsaaMode(CsaaMode csaaMode)
{
    if (m_csaaMode == csaaMode)
        return;

    m_pGLWidget->makeCurrent();

    m_csaaMode = csaaMode;

    if (aaType() == AntialiasingType::Csaa)
    {
        m_pGLWidget->setCsaaMode(m_csaaMode);
        float pixelRatio = m_pGLWidget->devicePixelRatio();
        m_pGLWidget->updateBuffer( m_pGLWidget->width()*pixelRatio, m_pGLWidget->height()*pixelRatio, aaType() );
    }
}

void NEGLWidgetProperties::setBfaaMode(BfaaMode bfaaMode)
{
//    if (m_bfaaMode == bfaaMode)
//        return;

//    m_pGLWidget->makeCurrent();

//    m_bfaaMode = bfaaMode;

//    if (aaType() == AntialiasingType::Bfaa)
//    {
//        m_pGLWidget->setBfaaMultiplier(powf(2.0f, m_bfaaMode));
//        m_pGLWidget->updateBuffer( m_pGLWidget->width(), m_pGLWidget->height(), m_pGLWidget->devicePixelRatio(), aaType() );
//    }
}

bool NEGLWidgetProperties::isSupportCsaa()
{
    QString strExtensions((char*)(GL->glGetString(GL_EXTENSIONS)));
    if(strExtensions.contains("GL_NV_framebuffer_multisample_coverage"))
        return true;
    return false;
}

void NEGLWidgetProperties::setTransparencyType(GlobalTransparency transparencyType)
{
	if (NERENDERMAN.GLManager()->OpenGLVersion() >= 430 || transparencyType != LinkedListTransparency)
        m_pGLWidget->setTransparencyType((IGLSurface::GlobalTransparency)transparencyType);
	else {
		std::cerr << "Linked List Transparency isn't support on your machine. Your OpenGL version is " << NERENDERMAN.GLManager()->OpenGLVersion() << std::endl;
	}
		
}

void NEGLWidgetProperties::setDepthPeelingIterations(int iters)
{
    m_pGLWidget->setDepthPeelingIterations(iters);
}

void NEGLWidgetProperties::setEnvironmentMap(FilePath environmentMap)
{
    m_environmentMap = environmentMap;

    m_pGLWidget->loadEnvironmentMap(m_environmentMap.path());
}

void NEGLWidgetProperties::setEnvRotation(double envRotation)
{
    m_pGLWidget->setEnvRotation(envRotation);
}

void NEGLWidgetProperties::setRightEnv(FilePath rightEnv)
{
    m_rightEnv = rightEnv;

    QString rightEnv1  = m_rightEnv.path();
    m_envList[0] = (rightEnv1);

    m_pGLWidget->loadCubeMapTexture( m_envList );
}

void NEGLWidgetProperties::setLeftEnv(FilePath leftEnv)
{
    m_leftEnv = leftEnv;

    QString leftEnv1 = m_leftEnv.path();
    m_envList[1] = (leftEnv1);

    m_pGLWidget->loadCubeMapTexture( m_envList );
}

void NEGLWidgetProperties::setTopEnv( FilePath topEnv )
{
    m_topEnv = topEnv;

    QString topEnv1 = m_topEnv.path();
    m_envList[2] = ( topEnv1);

    m_pGLWidget->loadCubeMapTexture( m_envList );
}

void NEGLWidgetProperties::setBottomEnv( FilePath bottomEnv )
{
    m_bottomEnv = bottomEnv;

    QString bottomEnv1 = m_bottomEnv.path();
    m_envList[3] = (bottomEnv1);

    m_pGLWidget->loadCubeMapTexture( m_envList );
}

void NEGLWidgetProperties::setFrontEnv(FilePath frontEnv)
{
    m_frontEnv = frontEnv;

    QString frontEnv1 = m_frontEnv.path();
    m_envList[4] = (frontEnv1 );

    m_pGLWidget->loadCubeMapTexture( m_envList );
}

void NEGLWidgetProperties::setBackEnv(FilePath backEnv)
{
    m_backEnv = backEnv;

    QString backEnv1 = m_backEnv.path();
    m_envList[5] = (backEnv1 );

    m_pGLWidget->loadCubeMapTexture( m_envList );
}

NEGLWidgetProperties::~NEGLWidgetProperties() {}

bool NEGLWidgetProperties::gridMeasurements(){
    return m_pGLWidget->gridMeasurements();
}
//jjj_end

bool NEGLWidgetProperties::overlayGrid() const
{
    return m_pGLWidget->overlayGrid();
}

QString NEGLWidgetProperties::camera() const
{
    return m_Camera;
}

int NEGLWidgetProperties::clipPlaneCount() const
{
    return m_ClipPlaneCount;
}

const vec4f&NEGLWidgetProperties::clipPlane2() const
{
    return m_ClipPlane[1];
}

const vec4f&NEGLWidgetProperties::clipPlane3() const
{
    return m_ClipPlane[2];
}

const vec4f&NEGLWidgetProperties::clipPlane4() const
{
    return m_ClipPlane[3];
}

const vec4f&NEGLWidgetProperties::clipPlane5() const
{
    return m_ClipPlane[4];
}

const vec4f&NEGLWidgetProperties::clipPlane6() const
{
    return m_ClipPlane[5];
}

bool NEGLWidgetProperties::backgroundGradient() const
{
    return m_pGLWidget->backgroundGradient();
}

void NEGLWidgetProperties::setBackgroundGradient(bool arg)
{
    m_pGLWidget->setBackgroundGradient(arg);
}

NEGLWidgetProperties::BackgroundType NEGLWidgetProperties::backgroundType() const
{
    return (BackgroundType)m_pGLWidget->backgroundType();
}

void NEGLWidgetProperties::setBackgroundType(NEGLWidgetProperties::BackgroundType arg)
{
    m_pGLWidget->setBackgroundType((IGLSurface::BackgroundType)arg);
}

QColor NEGLWidgetProperties::backgroundGradColor() const
{
    return m_pGLWidget->backgroundGradColor();
}

QColor NEGLWidgetProperties::backgroundRealColor() const
{
    return m_pGLWidget->backgroundRealColor();
}

void NEGLWidgetProperties::setBackgroundGradColor(QColor backgroundGradColor)
{
    m_pGLWidget->setBackgroundGradColor(backgroundGradColor);
}

void NEGLWidgetProperties::setBackgroundRealColor(QColor backgroundRealcolor)
{
    m_pGLWidget->setBackgroundRealColor(backgroundRealcolor);
}

bool NEGLWidgetProperties::backgroundColorType() const
{
    return( m_pGLWidget->backgroundType() == Color );
}

bool NEGLWidgetProperties::backgroundTextureType() const
{
    return( m_pGLWidget->backgroundType() == Texture );
}

bool NEGLWidgetProperties::backgroundRealType() const
{
    return (m_pGLWidget->backgroundType() == Real);
}

NEGLWidgetProperties::EnvironmentType NEGLWidgetProperties::environmentType() const
{
    return (EnvironmentType)m_pGLWidget->environmentType();
}

bool NEGLWidgetProperties::EnvironmentTypeCube() const
{
    return m_pGLWidget->EnvironmentTypeCube();
}

bool NEGLWidgetProperties::EnvironmentTypeSphere() const
{
    return m_pGLWidget->EnvironmentTypeSphere();
}

bool NEGLWidgetProperties::EnvironmentTypeOn() const
{
    return m_pGLWidget->EnvironmentTypeOn();
}

bool NEGLWidgetProperties::environmentRender() const
{
    return m_pGLWidget->environmentRender();
}

FilePath NEGLWidgetProperties::backgroundTexture() const
{
    return m_pGLWidget->backgroundTexture();
}

bool NEGLWidgetProperties::ssao() const
{
    return m_pGLWidget->ssao();
}

bool NEGLWidgetProperties::available430() const
{
	return NERENDERMAN.GLManager()->OpenGLVersion() >= 430;
}

bool NEGLWidgetProperties::available410() const
{
	return NERENDERMAN.GLManager()->OpenGLVersion() >= 410;
}


NEGLWidgetProperties::HbaoType NEGLWidgetProperties::type() const
{
    return (HbaoType)m_pGLWidget->type();
}

double NEGLWidgetProperties::intensity() const
{
    return m_pGLWidget->intensity();
}

double NEGLWidgetProperties::radius() const
{
    return m_pGLWidget->radius();
}

double NEGLWidgetProperties::blurSharpness() const
{
    return m_pGLWidget->blurSharpness();
}

NEGLWidgetProperties::AntialiasingType NEGLWidgetProperties::aaType() const
{
    return (AntialiasingType)m_pGLWidget->aaType();
}

bool NEGLWidgetProperties::isMsaa() const
{
    return m_pGLWidget->isMsaa();
}

bool NEGLWidgetProperties::isFxaa() const
{
    return m_pGLWidget->isFxaa();
}

bool NEGLWidgetProperties::isCsaa() const
{
    return m_pGLWidget->isCsaa();
}

bool NEGLWidgetProperties::isBfaa() const
{
    return m_pGLWidget->isBfaa();
}

NEGLWidgetProperties::MsaaSampling NEGLWidgetProperties::msaaSampling() const
{
    return m_msaaSamples;
}

NEGLWidgetProperties::FxaaProfile NEGLWidgetProperties::fxaaProfile() const
{
    return m_fxaaProfile;
}

NEGLWidgetProperties::CsaaMode NEGLWidgetProperties::csaaMode() const
{
    return m_csaaMode;
}

NEGLWidgetProperties::BfaaMode NEGLWidgetProperties::bfaaMode() const
{
    return m_bfaaMode;
}

NEGLWidgetProperties::GlobalTransparency NEGLWidgetProperties::transparencyType() const
{
    return (GlobalTransparency)m_pGLWidget->transparencyType();
}

bool NEGLWidgetProperties::isDepthPeeling() const
{
    return m_pGLWidget->isDepthPeeling();
}

int NEGLWidgetProperties::depthPeelingIterations() const
{
    return m_pGLWidget->depthPeelingIterations();
}

NEGLWidgetProperties::ManipSpace NEGLWidgetProperties::manipSpace() const
{
    return m_manipSpace;
}

FilePath NEGLWidgetProperties::rightEnv() const
{
    return m_rightEnv;
}

FilePath NEGLWidgetProperties::leftEnv() const
{
    return m_leftEnv;
}

FilePath NEGLWidgetProperties::topEnv() const
{
    return m_topEnv;
}

FilePath NEGLWidgetProperties::frontEnv() const
{
    return m_frontEnv;
}

FilePath NEGLWidgetProperties::backEnv() const
{
    return m_backEnv;
}

FilePath NEGLWidgetProperties::bottomEnv() const
{
    return m_bottomEnv;
}

double NEGLWidgetProperties::envRotation() const
{
    return m_pGLWidget->envRotation();
}

QColor NEGLWidgetProperties::backgroundColor() const { return m_pGLWidget->backgroundColor(); }

void NEGLWidgetProperties::setBackgroundColor(QColor val) { m_pGLWidget->setBackgroundColor(val); }

bool NEGLWidgetProperties::viewAxes() const { return m_pGLWidget->viewAxes(); }

bool NEGLWidgetProperties::showGizmo() const { return m_pGLWidget->showGizmo(); }

vec3f NEGLWidgetProperties::snapUnit() const { return m_pGLWidget->snapUnit(); }

bool NEGLWidgetProperties::clipping() const { return m_Clipping; }

vec4f NEGLWidgetProperties::clipPlane1() { return m_ClipPlane[0]; }

bool NEGLWidgetProperties::showOsd() const { return m_pGLWidget->showOsd(); }
bool NEGLWidgetProperties::showFPS() const { return m_pGLWidget->showFPS(); }

QColor NEGLWidgetProperties::osdTextColor() const { return m_OsdTextColor; }

void NEGLWidgetProperties::setOsdTextColor(QColor val) { m_OsdTextColor = val; }

bool NEGLWidgetProperties::drawRenderArea() const { return m_pGLWidget->m_DrawRenderArea; }

bool NEGLWidgetProperties::saveGLFrames() const { return m_SaveGLFrames; }

NEGLWidgetProperties::QualityProfile NEGLWidgetProperties::qualityProfile() const
{
    return (QualityProfile)m_pGLWidget->qualityProfile();
}

void NEGLWidgetProperties::setQualityProfile(QualityProfile val)
{
    m_pGLWidget->setQualityProfile((IGLSurface::QualityProfile)val);
}

// Quality profile
// Low Quality = No Shadows + No AO + No AA
// Med Quality = No Shadows + AO
// High Quality = Shadows + AO + High AA
bool NEGLWidgetProperties::checkAOQualityProfile() const
{
    return m_pGLWidget->checkAOQualityProfile();
}

bool NEGLWidgetProperties::checkShadowQualityProfile() const
{
    return (qualityProfile() == QualityProfile::HighQuality);
}

bool NEGLWidgetProperties::checkAAQualityProfile() const
{
    return m_pGLWidget->checkAAQualityProfile();
}

bool NEGLWidgetProperties::checkTransparencyQualityProfile() const
{
    return m_pGLWidget->checkTransparencyQualityProfile();

}

bool NEGLWidgetProperties::dynamicQuality() const
{
    return m_pGLWidget->dynamicQuality();
}

void NEGLWidgetProperties::setDynamicQuality(bool val)
{
    m_pGLWidget->setDynamicQuality(val);
}

NEImageSize NEGLWidgetProperties::imageSizeForSaving() const {return m_pGLWidget->m_GlSize;}

void NEGLWidgetProperties::setOverlayGrid(bool arg)
{
    m_pGLWidget->setOverlayGrid(arg);
}

void NEGLWidgetProperties::setEnvironmentType(NEGLWidgetProperties::EnvironmentType arg)
{
    m_pGLWidget->makeCurrent();

    m_pGLWidget->setEnvironmentType((IGLSurface::EnvironmentType)arg);

    if (arg != NEGLWidgetProperties::NoEnvironment)
    {
        m_pGLWidget->reloadBuffer();
    }
}

void NEGLWidgetProperties::setEnvironmentRender(bool arg)
{
    m_pGLWidget->setEnvironmentRender(arg);
}

void NEGLWidgetProperties::setType(NEGLWidgetProperties::HbaoType type)
{
    m_pGLWidget->setType((IGLSurface::HbaoType)type);
}

void NEGLWidgetProperties::setIntensity(double intensity)
{
    m_pGLWidget->setIntensity(intensity);
}

void NEGLWidgetProperties::setRadius(double radius)
{
    m_pGLWidget->setRadius(radius);
}

void NEGLWidgetProperties::setBlurSharpness(double blurSharpness)
{
    m_pGLWidget->setBlurSharpness(blurSharpness);
}

void NEGLWidgetProperties::setManipSpace(NEGLWidgetProperties::ManipSpace manipSpace)
{
    m_manipSpace = manipSpace;
}

bool NEGLWidgetProperties::clip1Enabled() { return clipping() && clipPlaneCount() > 0; }

bool NEGLWidgetProperties::clip2Enabled() { return clipping() && clipPlaneCount() > 1; }

bool NEGLWidgetProperties::clip3Enabled() { return clipping() && clipPlaneCount() > 2; }

bool NEGLWidgetProperties::clip4Enabled() { return clipping() && clipPlaneCount() > 3; }

bool NEGLWidgetProperties::clip5Enabled() { return clipping() && clipPlaneCount() > 4; }

bool NEGLWidgetProperties::clip6Enabled() { return clipping() && clipPlaneCount() > 5; }




/*!
    All NEGLWidgets are constructed by the NEGUIManager singleton.
*/
NEGLWidget::NEGLWidget(QWidget* parent):
#ifdef CUDA_AVAILABLE
    QOpenGLWidget(parent)
#else
    QOpenGLWidget(parent),
    IGLSurface()
#endif
{
    QObject::connect(this, &NEGLWidget::aboutToCompose, this, &NEGLWidget::lockMainThread);
    QObject::connect(this, &NEGLWidget::frameSwapped, this, &NEGLWidget::unLockMainThread);
    //setAttribute(Qt::WA_NoSystemBackground, false);
    setPropertyObject(new NEGLWidgetProperties(this));

    resizeTimer = new QTimer(this);
    connect(resizeTimer, SIGNAL(timeout()), this, SLOT(resizeTimeout()));
    resizeFinished = false;

    setUpdateBehavior(QOpenGLWidget::PartialUpdate);

    setAutoFillBackground( false );	// Do not automatically fill the background of the widget
    m_Tracking  = kTrackNone;
    m_pCamera = 0;
    camQuat = quatf(1.0,0,0,0);
    mousePosOld = QPoint(0,0);

    m_bSelectionStarted = false;
    m_SelectionArea.setRect(0,0,-1,-1);

    m_GridSize = QSizeF(40.0, 40.0);

    m_manipulatorRenderScale = 300;

    m_PopupSpatial = NULL;

    init();
}

void NEGLWidget::lockMainThread()
{
    m_renderMutex.lock();

}
void NEGLWidget::unLockMainThread()
{
    m_renderMutex.unlock();
}


NEGLWidget::~NEGLWidget()
{
    makeCurrent();
}

void NEGLWidget::init()
{

}

bool NEGLWidget::isSceneDynamic()
{
    return dynamicQuality() && ( m_Tracking != NETrackballMode::kTrackNone || m_ActiveManipOperation);
}



void NEGLWidget::setMsaaSamples(int samples)
{
    m_msaaSamples = samples;
}

void NEGLWidget::setCsaaMode(NEGLWidgetProperties::CsaaMode csaaMode)
{
    m_csaaColorSamples = 8 - (int(csaaMode) % 2) * 4;
    m_csaaCoverageSamples = 8 + (((int(csaaMode) - 1) >> 1) & 0x01) * 8;
}

void NEGLWidget::setBfaaMultiplier(int multiplier)
{
    m_bfaaMultiplier = multiplier;
}

void NEGLWidget::reloadBuffer()
{
    float pixelRatio = devicePixelRatio();
    updateBuffer(width()*pixelRatio, height()*pixelRatio, P()->aaType());
    update();
}

void NEGLWidget::hideDescription()
{
    m_PopupSpatial = NULL;
    emit updateGLWindows();
}

void NEGLWidget::slotDropped(QVariant vx, QVariant vy, QVariant vArea)
{
    float pixelRatio = devicePixelRatio();
    double x = vx.value<double>() * pixelRatio;
    double y = vy.value<double>() * pixelRatio;
    QString sArea = vArea.value<QString>();
    sArea.remove("QModelIndex");
    sArea.remove("FDYNTreeModel");
    sArea.remove("(");
    sArea.remove(")");
    QStringList sList = sArea.split(',');

    sList[2].remove("0x");
    sList[3].remove("0x");

    //qDebug() << sList[0] << sList[1] << sList[2] << sList[3];

    bool ok = false;
    int row = sList[0].toInt(&ok);
    int col = sList[1].toInt(&ok);

    long long nptr = sList[2].toLongLong(&ok, 16);
    long long nmodel = sList[3].toLongLong(&ok, 16);

    void *ptr = (void *)nptr;
    //FDYNTreeModel *model = (FDYNTreeModel *)nmodel;

    //FDYNTreeItem *tItem = (FDYNTreeItem *)ptr;

    emit dropCalled(camera()->viewMatrix(), camera()->projectionMatrix(), x, y, this->getGLWidth(), this->getGLHeight(), ptr);
}

void NEGLWidget::bindFinalFrameBuffer()
{
    GL->glBindFramebuffer( GL_FRAMEBUFFER, m_frameAndPickFBO );
}

void NEGLWidget::bindPickFrameBuffer()
{
    GL->glBindFramebuffer( GL_FRAMEBUFFER, m_frameAndPickFBO );
}

void NEGLWidget::initStaticData()
{
    // Create a display list for the code
    DL_Cone = GL->glGenLists(1);

    // Create display list for a cube
    DL_Cube = GL->glGenLists(1);
    GL->glNewList(DL_Cube, GL_COMPILE);
    cacheUnitCube();
    GL->glEndList();

}

void NEGLWidget::clearStaticData()
{
}


void  MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    if(type == GL_DEBUG_TYPE_ERROR){
        fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
                  type, severity, message );
    }
}


/*!
    OpenGL related setup should be done inside this function.
*/
void NEGLWidget::initializeGL()
{
    //GL = context()->function();
    //s_gl = s_sharedContext->functions();
    
    if(!GL)
    {
        GL = context()->versionFunctions<QOpenGLFunctions_3_3_Compatibility>();
        GL41 = context()->versionFunctions<QOpenGLFunctions_4_1_Compatibility>();
        GL43 = context()->versionFunctions<QOpenGLFunctions_4_3_Compatibility>();

        if(context()->hasExtension("GL_EXT_direct_state_access"))
        {
            GLEXTDSA = new QOpenGLExtension_EXT_direct_state_access();
            bool bSucess = GLEXTDSA->initializeOpenGLFunctions();
            if(!bSucess)
            {
                std::cout << "Current Context is not supported Extension : GL_EXT_direct_state_access" << std::endl;
            }

        }
        if(context()->hasExtension("GL_NV_texture_multisample"))
        {
            GLEXTNTM = new QOpenGLExtension_NV_texture_multisample();
            bool bSucess = GLEXTNTM->initializeOpenGLFunctions();
            if(!bSucess)
            {
                std::cout << "Current Context is not supported Extension : GL_NV_texture_multisample" << std::endl;
            }

        }
    }



    printf("OpenGL: %s %s %s\n", GL->glGetString(GL_VENDOR), GL->glGetString(GL_RENDERER), GL->glGetString(GL_VERSION));

    QOpenGLFunctions_4_5_Compatibility *GL45 = context()->versionFunctions<QOpenGLFunctions_4_5_Compatibility>();
    if(GL45)
    {
        GL->glEnable( GL_DEBUG_OUTPUT );
        GL45->glDebugMessageCallback( MessageCallback, 0 );
    }

    initializeGLSurface();

    m_moveGizmo = CreateMoveGizmo();
    m_scaleGizmo = CreateScaleGizmo();
    m_rotateGizmo = CreateRotateGizmo();
    m_sizeGizmo = CreateScaleGizmo();
    m_locationGizmo = CreateMoveGizmo();

    m_moveGizmo->SetLocation(IGizmo::LOCATE_LOCAL);
    m_scaleGizmo->SetLocation(IGizmo::LOCATE_LOCAL);
    m_rotateGizmo->SetLocation(IGizmo::LOCATE_LOCAL);
    m_sizeGizmo->SetLocation(IGizmo::LOCATE_LOCAL);
    m_locationGizmo->SetLocation(IGizmo::LOCATE_LOCAL);

    m_moveGizmo->SetDisplayScale( 0.8f );
    m_scaleGizmo->SetDisplayScale( 0.8f );
    m_rotateGizmo->SetDisplayScale( 0.8f );
    m_sizeGizmo->SetDisplayScale( 0.8f );
    m_locationGizmo->SetDisplayScale( 0.8f );

    this->setMouseTracking( true );
}

/*!
    It resizes the OpenGL viewport and recomputes projection matrices
*/
void NEGLWidget::resizeGL(int width, int height)
{
    //std::cout << "Before updateBuffer, OpenGL error is " << GL->glGetError() << ", default Framebuffer is " << this->defaultFramebuffer() << std::endl;
    m_moveGizmo->SetScreenDimension( width, height );
    m_rotateGizmo->SetScreenDimension( width, height );
    m_scaleGizmo->SetScreenDimension( width, height );

    m_sizeGizmo->SetScreenDimension( width, height );
    m_locationGizmo->SetScreenDimension( width, height );

    //std::cout << "Device Pixel Ratio " << width << " " << height << " " << devicePixelRatio() << std::endl;
    setGLSurfaceSize(width, height, devicePixelRatio());
    //std::cout << "After updateBuffer, OpenGL error is " << GL->glGetError() << ", default Framebuffer is " << this->defaultFramebuffer() << std::endl;
    //emit updateGLWindows();

}

QSize NEGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

//bool g_bOnce = false; // this is temp code
void NEGLWidget::paintGL()
{
    // If there is no any OpenGL context assigned for the widget, return immediately
    if(!isVisible() || !isValid() ||
            !NESCENE.initialized() ||
            !NEBASE.initialized() ||
            !resizeFinished ||
            NERENDERMAN.m_offlineRenderers.size() > 0)
    {
    }
    else
    {
        //std::cout << "before render scene, opengl error : " << GL->glGetError() << ", default Framebuffer is " << this->defaultFramebuffer() << std::endl;
        //std::cout << width();
        //std::cout << height();
        //std::cout << devicePixelRatio();
        float pixelRatio = devicePixelRatio();
        renderScene(width()*pixelRatio, height()*pixelRatio);
        //std::cout << "after render scene, opengl error : " << GL->glGetError() << ", default Framebuffer is " << this->defaultFramebuffer() << std::endl;

        render2DWidgets();
        GL->glFinish();
    }

    m_frameTime = NETimer::stop( "DisplayFPS" );
    NETimer::start( "DisplayFPS" );
}

NENode *NEGLWidget::pick( int x, int y, int pixelRatio, bool multi /*= false*/, bool bForSelect /*= true */, bool bDeselectedWhenSelected /*= false*/ )
{
    x = x * pixelRatio;
    y = y * pixelRatio;
    // This is required for the picking to work as expected when there are multiple opengl windows open
    makeCurrent();

    bindPickFrameBuffer();

    m_glWidth = width() * pixelRatio;
    m_glHeight = height() * pixelRatio;
    GL->glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    // Disable some opengl states to render each object with single solid color
    //GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
    GL->glDisable( GL_MULTISAMPLE );
    GL->glDisable( GL_POINT_SMOOTH );
    GL->glLineWidth(1.0f);
    GL->glPointSize(1.0f);
    // Clear color & depth buffers
    GL->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    // Supply the topmost projection matrix in the stack to OpenGL
    setProjectionMatrix();
    // If there is an active camera, load its view matrix into the view matrix stack
    if(camera())
    {
        viewStack().loadMatrix(camera()->viewMatrix());
    }

    NERENDERMAN.GLManager()->setFillMode(NEFillMode::kSolidFrontAndBack);
    // Enable depth test to correctly draw the overlapping  primitives
    NERENDERMAN.GLManager()->setDepthTest(kLessEqualDepthTest);

    // Enable clipping planes
    worldStack().pushMatrix(matrix44f());
    setModelViewMatrix();
    worldStack().popMatrix();

    if(P()->clipping())
    {
        for(int i=0; i<P()->clipPlaneCount(); ++i)
        {
            vec4f val = P()->m_ClipPlane[i];
            vec4d planeEqn(val.x(), val.y(), val.z(), val.w());
            GL->glClipPlane(GL_CLIP_PLANE0+i, (double*)&planeEqn);
            GL->glEnable(GL_CLIP_PLANE0+i);
        }
    }


    //
    // Render all spatials in the scene
    //
    foreach( NESpatial* object, NESCENE.spatials() )
    {
        //
        // Load the object's transformation into the world matrix stack if its geometry requires OpenGL transformation
        //
        if( object->needsGLTransform() )
            worldStack().loadMatrix( object->transform() );
        // Do not apply any world transformation if the geometry is bound to the simulation data (does not require OpenGL transformation)
        else
            worldStack().loadMatrix( matrix44f() );

        //
        // Set the model-view matrix in OpenGL side
        //
        setModelViewMatrix();
        //
        // Bind the color shader
        //
        NERENDERMAN.GLManager()->bindPositionShader(this);
        //
        // Render the object using its predefined picking color
        //
        float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        object->getColorID4fv(color);
        GL->glColor4fv( color );
        NERENDERMAN.GLManager()->updatePositionUniforms(this);

        if (object->spatialType() == NESpatial::kOSD)
        {
            projectionStack().pushMatrix( osdOrthoMatrix );
            setProjectionMatrix();
            projectionStack().popMatrix();
        }
        NERENDERMAN.GLManager()->renderPickable(object, this);

        setProjectionMatrix();
    }



    if( P()->clipping() )
    {
        for( int i = 0; i < P()->clipPlaneCount(); ++i )
            GL->glDisable( GL_CLIP_PLANE0+i );
    }

    NENode *sel = NULL;
    if( m_SelectionArea.isEmpty() )
        sel = processPickEvent( x, y, multi, bForSelect, bDeselectedWhenSelected );
    else
        processDragPickEvent(pixelRatio);


    // Restore the opengl states back
    //GL->glPopAttrib();

    bindScreenFrameBuffer();

    doneCurrent();
    return sel;
}

NENode *NEGLWidget::processPickEvent( int x, int y, bool multi/*=false*/, bool bForSelect /*= true*/, bool bDeselectedWhenSelected /*= false*/ )
{
    // Clamp the mouse position
    if( x == 0 )
        x = 1;
    else if( x == width()-1 )
        x = width()-2;
    if( y == 0 )
        y = 1;
    else if( y == height()-1 )
        y = height()-2;

    GLubyte pixels[36];

    // Read 9 pixels from the framebuffer
    GL->glReadPixels( x-1, y-1, 3, 3, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    // Clear the least significant bytes
    for(int i=0; i<36; ++i)
        if(!((i+1) % 4))
            pixels[i] = 0;

    // Use an uint pointer to access different color ids
    quint32* ptrPixel = (quint32*)pixels;

    QMap<quint32, int> counts;

    // Add-up occourences
    for(int i=0; i<9; ++i)
        counts[ptrPixel[i]]++;

    quint32 pickedColor = 0;
    int maxCount = 0;

    QMap<quint32, int>::iterator iter = counts.begin();

    while(iter != counts.end())
    {
        if(iter.key() > 0)
        {
            int val = iter.value();
            if(val > maxCount)
            {
                maxCount = val;
                pickedColor = iter.key();
            }
        }
        ++iter;
    }

    const QMap<uint, NESpatial*>& spatialsMap = NESCENE.spatials();
    const QMap<uint, NESpatial*>::const_iterator spatialIter = spatialsMap.find(pickedColor);



    if(bForSelect){
        if(spatialIter != spatialsMap.end())
        {
            if(multi)
            {

                if(bDeselectedWhenSelected){

                    QList<NEObject*> selObjs = NESCENE.selectedObjects()->values();
                    if(selObjs.contains(*spatialIter)){
                        selObjs.removeAll(*spatialIter);
                        QList<NENode*> selnodes;
                        foreach (NEObject* obj, selObjs) {
                            selnodes.push_back(qobject_cast<NENode*>( obj));
                        }
                        emit selectObject( 0 );
                        emit selectObjects( selnodes );
                    }
                    else{
                        QList<NENode*> nlist;
                        foreach (NEObject *obj, selObjs) {
                            nlist.push_back(qobject_cast<NENode *>(obj));
                        }
                        nlist.push_back(*spatialIter);
                        emit selectObject( 0 );
                        emit selectObjects( nlist );
                    }

                }
                else{
                    QList<NEObject*> selObjs = NESCENE.selectedObjects()->values();
                    QList<NENode*> nlist;

                    foreach (NEObject *obj, selObjs) {
                        nlist.push_back(qobject_cast<NENode *>(obj));
                    }

                    if(!selObjs.contains(*spatialIter)){
                        nlist.push_back(*spatialIter);
                    }
                    emit selectObject( 0 );
                    emit selectObjects( nlist );
                }
            }
            else
            {
                emit selectObject( *spatialIter );
            }
        }
        else
        {
            emit selectObject(0);
        }
        return NULL;
    }
    else{
        if(spatialIter != spatialsMap.end())
        {
            return *spatialIter;
        }
        else
        {
            return NULL;
        }
    }
}

void NEGLWidget::processDragPickEvent(int pixelRatio)
{
    // Read a pixel from the framebuffer

    int x1 = MAX( MIN(m_SelectionFirst.x(), m_SelectionSecond.x()), 0 ) * pixelRatio;
    int x2 = MIN( MAX(m_SelectionFirst.x(), m_SelectionSecond.x()), width()-1 ) * pixelRatio;
    int dx = x2-x1;

    int y1 = MAX( MIN(m_SelectionFirst.y(), m_SelectionSecond.y()), 0 ) * pixelRatio;
    int y2 = MIN( MAX(m_SelectionFirst.y(), m_SelectionSecond.y()), height()-1 ) * pixelRatio;

    int dy = y2-y1;

    if(dx <= 0 || dy <= 0)
        return;

    GLubyte* pixel = new GLubyte[4*dx*dy];
    quint32* ptrPixel = (quint32*)pixel;

    GL->glReadPixels(x1, y1, dx, dy, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

    //printf("(%d %d), (%d %d)\n", x1, y1, x2, y2);

    QMap<quint32, quint32> pickedColors;

    for(int i=0; i<dx*dy; i++)
    {
        ptrPixel = (quint32*)&(pixel[i*4]);
        pixel[i*4+3] = 0;
        if(*ptrPixel < 1024)
        {
            continue;
        }

        pickedColors[*ptrPixel] = *ptrPixel;
    }

    if(pickedColors.size() == 0)
    {
        emit selectObject(0);
    }

    const QMap<uint, NESpatial*>& spatialsMap = NESCENE.spatials();

    QList<NENode*> nlist;

    foreach(quint32 color, pickedColors)
    {
        const QMap<uint, NESpatial*>::const_iterator spatialIter = spatialsMap.find(color);

        if(spatialIter != spatialsMap.end())
        {
            nlist.push_back(*spatialIter);
        }
    }

    if(nlist.size() > 0)
    {
        if(nlist.size() > 1)
            emit selectObjects( nlist );
        else
            emit selectObject(nlist.first());
    }
    else
        emit selectObject(0);

    delete [] pixel;
}


//!This function initializes neccessary steps for rendering motion vectors in the frame buffer object
void NEGLWidget::initFramebufferForMotionVector()
{
    //GLenum error;
    GLint prevFrameBuffer;
    GL->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFrameBuffer);

    //create framebuffer object
    fbo = 0;
    newFramebuffer(fbo);
    GL->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // you can attach texture or renderbuffer to those framebuffers
    //renderbuffer supports offscreen rendering

    //create renderbuffer object
    depthbuffer = 0;
    newRenderbuffer(depthbuffer);
    GL->glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    GL->glRenderbufferStorage(GL_RENDERBUFFER,  GL_DEPTH_COMPONENT, width(), height()); //create storage for it
    // 2nd parameter can be GL_DEPTH_COMPONENT, GL_RGBA, GL_RGB, GL_STENCIL_INDEX
    GL->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer); // attach renderbuffer to framebuffer

    //create texture object
    img = 0;
    newTexture(img);
    GL->glBindTexture(GL_TEXTURE_2D, img);
    //8 bit per channel
    //	glTexImage2D(GL_TEXTURE_2D, 0 /*level of detail number*/,GL_RGBA8,  width(), height(), 0/*width of the border*/, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //32 bit per channel, but converted to 16 while saving
    GL->glTexImage2D(GL_TEXTURE_2D, 0 /*level of detail number*/,GL_RGBA16F, width(), height(), 0/*width of the border*/, GL_RGBA, GL_FLOAT, NULL);

    //attach texture to framebuffer
    GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0/*fragment shader output value*/, GL_TEXTURE_2D, img, 0);

    //error = GL->glGetError();
    //check if its complete
    /*GLenum status = */GL->glCheckFramebufferStatus(GL_FRAMEBUFFER);
    //	bool check;

    //	if(status == GL_FRAMEBUFFER_COMPLETE) //if true ready to render
    //		check = true;

    GL->glBindFramebuffer(GL_FRAMEBUFFER,prevFrameBuffer);
    GL->glBindTexture(GL_TEXTURE_2D, 0);
    GL->glBindRenderbuffer(GL_RENDERBUFFER, 0);

}

//! This function captures and saves motionvector images from framebuffer object
void NEGLWidget::captureMotionVector()
{
    //    QDir directory(NEBASE.CacheDir());
    //    if(!directory.exists("GLMotionData"))
    //        directory.mkdir("GLMotionData");

    //    char vPath[100];
    //    sprintf(vPath, "/%06d", NESCENE.CurrentFrame()+1);
    //    QString framePath = NEBASE.CacheDir() + "/" + "GLMotionData" + vPath + ".psd";

    //    //16 bit version
    //    GLfloat *data=NULL;
    //    data = (GLfloat *)malloc(width() * height() * /*4*/4 * sizeof(GLfloat));

    //    //8 bit version
    //    //GLubyte *data=NULL;
    //    //data = (GLubyte *)malloc(width() * height() * /*4*/4);

    //    //read from frame buffer
    //    //glReadPixels(0,0,width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, data);

    //    //read from texture
    //    //8 bit version
    //    //glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //    //ilTexImage(width(), height(), 1, (byte)/*4*/4, IL_RGBA, IL_BYTE, data);

    //    //16 bit version
    //    GL->glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data);
    //    ilTexImage(width(), height(), 1, (uchar)4, IL_RGBA, IL_FLOAT, data);

    //    ilSave(IL_PSD, (const char *)framePath.toUtf8());
    //    ilDeleteImages(1, &ImgId);
    //    free(data);
}

//! saveMotionMap is activated from Scene properties-> NESceneManager->GL_Related->saveMotionMap,
//!it saves images to GLMotionData folder in playback mode
void NEGLWidget::renderMotionVectors()
{
    float pixelRatio = devicePixelRatio();
    m_glWidth = width() * pixelRatio;
    m_glHeight = height() * pixelRatio;

    // Disable some opengl states to render each object with single solid color
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    //glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
    GL->glDisable(GL_MULTISAMPLE);
    GL->glDisable(GL_POINT_SMOOTH);
    GL->glLineWidth(1.0f);
    GL->glPointSize(1.0f);

    GLint prevFrameBuffer;
    GL->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFrameBuffer);

    GL->glBindFramebuffer(GL_FRAMEBUFFER,  fbo);
    GL->glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    GL->glBindTexture(GL_TEXTURE_2D, img);
    GL->glDrawBuffer(GL_COLOR_ATTACHMENT0);
    // Clear color & depth buffers
    GL->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    GL->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // If there is an active camera, load its view matrix into the view matrix stack
    if(camera())
    {
        viewStack().loadMatrix(camera()->viewMatrix());
        // Supply the topmost projection matrix in the stack to OpenGL
        projectionStack().pushMatrix(camera()->projectionMatrix());
        setProjectionMatrix();
        projectionStack().popMatrix();
    }

    NERENDERMAN.GLManager()->setFillMode(NEFillMode::kSolidFrontAndBack);
    // Enable depth test to correctly draw the overlapping  primitives
    NERENDERMAN.GLManager()->setDepthTest(kLessEqualDepthTest);
    // Render all spatials in the scene
    foreach(NESpatial* object, NESCENE.spatials())
    {	// Load the object's transformation into the world matrix stack if its geometry requires OpenGL transformation
        if(object->needsGLTransform())
            worldStack().loadMatrix(object->transform());
        // Do not apply any world transformation if the geometry is bound to the simulation data (does not require OpenGL transformation)
        else
            worldStack().loadMatrix(matrix44f());
        // Set the model-view matrix in OpenGL side
        setModelViewMatrix();
        // Render the object using its predefined picking color
        quint32 col32 = object->colorID();
        float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        color[0] = (col32 % 256) % 255; col32 /= 256;
        color[1] = (col32 % 256) % 255; col32 /= 256;
        color[2] = (col32 % 256) % 255; col32 /= 256;
        GL->glUniform4fv( GL->glGetUniformLocation( shaderProgram()->programId(), "meshColor" ), 1, color );
        // Bind the color shader
        NERENDERMAN.GLManager()->bindMotionShader(this);

        GLuint unifLoc = GL->glGetUniformLocation(shaderProgram()->programId(), "viewMatrix");
        GL->glUniformMatrix4fv(unifLoc, 1, GL_FALSE, NERENDERMAN.GLManager()->viewMatrix().data());

        //		object->renderInMotion();
        //printf("\n %s %d", object->objectName().toUtf8(), object->colorID());
    }

    GL->glBindFramebuffer(GL_FRAMEBUFFER,  fbo);
    GL->glBindTexture(GL_TEXTURE_2D, img);
    captureMotionVector();
    GL->glBindTexture(GL_TEXTURE_2D, 0);
    GL->glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer);
    GL->glBindRenderbuffer(GL_RENDERBUFFER, 0);
    setShaderProgram(0);
    // Restore the opengl states back
    GL->glPopAttrib();
}

QOpenGLTexture* loadCubeMap(std::vector<QString>& faces)
{
    QOpenGLTexture *skyboxTexture = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
    skyboxTexture->create();

    QOpenGLTexture::CubeMapFace cubeMapFaceList[6] =
    {
        QOpenGLTexture::CubeMapPositiveX,
        QOpenGLTexture::CubeMapNegativeX,
        QOpenGLTexture::CubeMapPositiveY,
        QOpenGLTexture::CubeMapNegativeY,
        QOpenGLTexture::CubeMapNegativeZ,
        QOpenGLTexture::CubeMapPositiveZ,
    };

    bool isNew = true;
    for( GLuint i = 0; i < faces.size(); i++)
    {
        const QImage image = QImage(faces[i]).convertToFormat(QImage::Format_RGB888);
        if (image.width() != 0)
        {
            if (isNew)
            {
                isNew = false;
                skyboxTexture->setSize(image.width(), image.height());
                skyboxTexture->setFormat(QOpenGLTexture::RGB8_UNorm);
                skyboxTexture->allocateStorage();
            }
            skyboxTexture->bind();
            skyboxTexture->setData(0, 0, cubeMapFaceList[i], QOpenGLTexture::RGB, QOpenGLTexture::UInt8,
                                   (const void*) image.constBits());
        }

    }
    skyboxTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
    skyboxTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

    if (isNew)
    {
        skyboxTexture->destroy();
        delete skyboxTexture;
        skyboxTexture = NULL;
    }

    return skyboxTexture;
}


// Function load an hdr environment map, turn it into a texture, and return the texture ID as a GLuint for use
QOpenGLTexture* loadEnvironmentImage(const char* theFileName)
{
    QOpenGLTexture::TextureFormat texFormat = QOpenGLTexture::RGB32F;
    QOpenGLTexture::PixelType pixelType = QOpenGLTexture::Float32;
    QOpenGLTexture::PixelFormat r = QOpenGLTexture::RGB;
    if (QString(theFileName).right(3) != "hdr")
    {
        texFormat = QOpenGLTexture::RGB32F;
        pixelType = QOpenGLTexture::Float32;
    }
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(theFileName, &width, &height, &nrComponents, 0);
    if (data)
    {
        QOpenGLTexture* hdrTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        hdrTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
        hdrTexture->setMinificationFilter(QOpenGLTexture::Linear);
        hdrTexture->setMagnificationFilter(QOpenGLTexture::Linear);
        hdrTexture->setFormat(texFormat);
        hdrTexture->setSize(width, height);

        hdrTexture->allocateStorage(QOpenGLTexture::PixelFormat::RGB, pixelType );
        hdrTexture->setData(r, pixelType, (const void *)data); // note how we specify the texture's data value to be float

        stbi_image_free(data);

        return hdrTexture;
    }
    else
    {
        //std::cout << "Failed to load HDR image." << std::endl;
        return NULL;
    }
}




bool NEGLWidget::loadCubeMapTexture( std::vector<QString>& faces )
{
    makeCurrent();
    m_SkyBoxTexture = loadCubeMap( faces );

    reloadBuffer();

    doneCurrent();
    return true;
}

bool NEGLWidget::loadEnvironmentMap( QString path )
{
    makeCurrent();
    freeTexture (m_EnvironmentMap);

    // Load equirectangular environment map
    m_EnvironmentMap = loadEnvironmentImage( path.toUtf8() );

    reloadBuffer();
    doneCurrent();
    return true;
}

void NEGLWidget::drawOSDMessage(float width, float height)
{
    float devPixelRatio = devicePixelRatio();
    GL->glActiveTexture( GL_TEXTURE0 );
    GL->glColor4f( 1, 1,1,1);
    if( camera() )
    {
        emit addOSD( 2, NEOsdMsg("", camera()->objectName(), NEObject::TopRight, Color32(255,255,0,255)));
        if(showFPS())
        {
            if(m_frameTime <= 1.0)
                emit addOSD( 1,
                             NEOsdMsg("Display Frequency: ", QString("%1(fps), < 1(ms)").arg((int)(1000/m_frameTime)),
                                      NEObject::TopRight, Color32(255,255,0,255) ) );
            else
                emit addOSD( 1,
                         NEOsdMsg("Display Frequency: ", QString("%1(fps), %2(ms)").arg((int)(1000/m_frameTime)).arg((int)m_frameTime),
                                  NEObject::TopRight, Color32(255,255,0,255) ) );
        }
        else
            emit addOSD( 1, NEOsdMsg("", QString(""), NEObject::TopRight, Color32(255,255,0,255)));
    }

    if(showOsd())
    {
        emit drawOSD(this, width/devPixelRatio, height/devPixelRatio );
    }
}


void NEGLWidget::renderSceneContentsRecursive(NESpatial* spatial)
{
    // Render all spatials in the scene like a scene graph
    foreach(QObject* obj, spatial->children())
    {
        if(NESpatial* object = qobject_cast<NESpatial*>(obj))
        {
            NERENDERMAN.GLManager()->setFillMode(NEFillMode::kSolidFrontAndBack);
            // Load the object's transformation into the world matrix stack if its geometry requires OpenGL transformation
            if(object->needsGLTransform())
            {
                worldStack().multMatrix(object->transform());
                NERENDERMAN.GLManager()->normalMatrix( worldStack().top().normalMatrix() );
            }
            // Do not apply any world transformation if the geometry is bound to the simulation data (does not require OpenGL transformation)
            else
                worldStack().loadMatrix(matrix44f());
            // Update the world matrix in GL manager
            NERENDERMAN.GLManager()->worldMatrix( worldStack().top() );
            // Set the model-view matrix in OpenGL side
            setModelViewMatrix();
            NERENDERMAN.GLManager()->modelViewMatrix( m_MVMatrix );
            // The color of the object is supplied in case per vertex coloring or texture is not specified
            GL->glColor4f(object->Color().redF(), object->Color().greenF(), object->Color().blueF(), object->Color().alphaF());
            // Render the object
            NERENDERMAN.GLManager()->renderSpatial(object, this);

            if(object->children().size())
                renderSceneContentsRecursive(object);
        }
    }
}

void NEGLWidget::render2DWidgets()
{
    // Render 2D widgets over the widget (if there are any) and finish drawing
    GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
    GL->glBindBuffer( GL_ARRAY_BUFFER, 0 );
    GL->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    //GL->glUseProgram( 0 );
    GL->glDisableClientState( GL_COLOR_ARRAY );
    GL->glDisableClientState( GL_NORMAL_ARRAY );
    GL->glDisableClientState( GL_VERTEX_ARRAY );


    //QPainter painter(this);

    /// painter.setRenderHint(QPainter::Antialiasing);
    //painter.drawLine(QPoint(0,0), QPoint(500, 500));
    /// draw2DWidgets(&painter);
    //painter.drawLine(QPoint(0,0), QPoint(500, 500));
    /// painter.end();

    GL->glPopAttrib();
}

void NEGLWidget::draw2DWidgets( QPainter* painter )
{

    //QRect rect(m_SelectionFirst, m_SelectionSecond);
    QPen pen(QColor(255, 255, 255));
    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->setBrush(QColor(0, 0, 255, 31));
    painter->drawRect(m_SelectionArea);

    emit popupDescriptionAndName(m_PopupPos, this, m_PopupSpatial, painter);

    //QString text = tr("OSD Text");
    //QFontMetrics metrics = QFontMetrics(this->font());
    //int border = qMax(4, metrics.leading());

    //QRect rect = metrics.boundingRect(0, 0, width() - 2*border, int(height()*0.125),
    //	Qt::AlignCenter | Qt::TextWordWrap, text);
    //painter->setRenderHint(QPainter::TextAntialiasing);
    //painter->fillRect(QRect(0, 0, width(), rect.height() + 2*border),
    //	QColor(159, 0, 0, 127));
    //painter->setPen(Qt::white);
    //painter->fillRect(QRect(0, 0, width(), rect.height() + 2*border),
    //	QColor(159, 0, 0, 127));
    //painter->drawText((width() - rect.width())/2, border,
    //	rect.width(), rect.height(),
    //	Qt::AlignCenter | Qt::TextWordWrap, text);
}

void NEGLWidget::keyPressEvent( QKeyEvent* event )
{
    if(event->isAutoRepeat())
        return;

    // Pass the event to the parent widget
    event->ignore();
}


void NEGLWidget::keyReleaseEvent( QKeyEvent* event )
{
    switch(event->key())
    {
    case Qt::Key_Alt:
        // Re-enable context menus if alt modifier key is released
        emit setMenuContextMenuPolicySignal(Qt::DefaultContextMenu);
        setContextMenuPolicy(Qt::ActionsContextMenu);
        break;
    }
}

void NEGLWidget::resizeEvent( QResizeEvent *event )
{
    if(!NESCENE.initialized())
    {
        resizeFinished = true;
        QOpenGLWidget::resizeEvent(event);

    }
    else
    {
        resizeFinished = false;
        resizeTimer->stop();
        lastResizeEvent = event;
        resizeTimer->start(50);
    }

    const QSize size = event->size();
}

void NEGLWidget::resizeTimeout()
{
    //qDebug() << Q_FUNC_INFO;
    resizeTimer->stop();
    resizeFinished = true;
    QOpenGLWidget::resizeEvent(new QResizeEvent(size(), lastResizeEvent->oldSize()));
    update();

    // Below is for the worlds most hacky OS to correctly redraw the widget
#ifdef NE_FOR_LINUX
    QTimer::singleShot(0, this, SLOT(repaint()));
    QTimer::singleShot(0, this, SLOT(repaint()));
    QTimer::singleShot(0, this, SLOT(repaint()));
    QTimer::singleShot(0, this, SLOT(repaint()));
#endif
}

bool NEGLWidget::manipSelected(int x, int y)
{
    bool retval = false;
    NEObject* object = NESCENE.selectedObject();
    if( !(object && object->inherits("NESpatial")))
        return retval;
    NESpatial* spatial = (NESpatial*)object;

    if( this->m_ActiveManipulator == kManipTranslate && spatial->isManipulatable(kManipTranslate) )
    {
        CGizmoTransformMove* movegizmo = dynamic_cast< CGizmoTransformMove *>( m_moveGizmo );
        if( movegizmo )
        {
            CGizmoTransformMove::MOVETYPE mt ;
            movegizmo->GetOpType( mt, x, y );

            retval = ( mt != CGizmoTransformMove::MOVE_NONE );
        }
    }
    else if( this->m_ActiveManipulator == kManipRotate && spatial->isManipulatable(kManipRotate) )
    {
        CGizmoTransformRotate* rotategizmo = dynamic_cast< CGizmoTransformRotate *>( m_rotateGizmo );
        if( rotategizmo )
        {
            CGizmoTransformRotate::ROTATETYPE rt;
            rotategizmo->GetOpType( rt, x, y );

            retval = ( rt != CGizmoTransformRotate::ROTATE_NONE );
        }
    }
    else if( this->m_ActiveManipulator == kManipScale && spatial->isManipulatable(kManipScale) )
    {
        CGizmoTransformScale* scalegizmo = dynamic_cast< CGizmoTransformScale *>( m_scaleGizmo );
        if( scalegizmo )
        {
            CGizmoTransformScale::SCALETYPE st;
            scalegizmo->GetOpType( st, x, y );

            retval = ( st != CGizmoTransformScale::SCALE_NONE );
        }
    }
    else if( this->m_ActiveManipulator == kSize && spatial->isManipulatable(kManipScale) )
    {
        CGizmoTransformScale* sizeGizmo = dynamic_cast< CGizmoTransformScale *>( m_sizeGizmo );
        if( sizeGizmo )
        {
            CGizmoTransformScale::SCALETYPE st;
            sizeGizmo->GetOpType( st, x, y );

            retval = ( st != CGizmoTransformScale::SCALE_NONE );
        }
    }
    else if( this->m_ActiveManipulator == kLocation && spatial->isManipulatable(kManipTranslate) )
    {
        CGizmoTransformMove* locationGizmo = dynamic_cast< CGizmoTransformMove *>( m_locationGizmo );
        if( locationGizmo )
        {
            CGizmoTransformMove::MOVETYPE mt ;
            locationGizmo->GetOpType( mt, x, y );

            retval = ( mt != CGizmoTransformMove::MOVE_NONE );
        }
    }
    return retval;
}

AABB getWorldAABB(NESpatial *spatial)
{
    vec3f vertices[8];
    vec3f minP = spatial->aabb().minPos();
    vec3f maxP = spatial->aabb().maxPos();
    vertices[0] = minP;
    vertices[6] = maxP;
    vertices[1] = vec3f(minP.x(),maxP.y(),minP.z());
    vertices[2] = vec3f(maxP.x(),maxP.y(),minP.z());
    vertices[3] = vec3f(maxP.x(),minP.y(),minP.z());
    vertices[4] = vec3f(minP.x(),minP.y(),maxP.z());
    vertices[5] = vec3f(minP.x(),maxP.y(),maxP.z());
    vertices[7] = vec3f(maxP.x(),minP.y(),maxP.z());

    AABB worldAABB;
    for (int i = 0; i < 8; i++)
        worldAABB.insertPoint(spatial->transform() * vertices[i]);

    return worldAABB;
}

bool pointInPlane(const vec3f& pt, const vec3f& ptA, const vec3f& ptB, const vec3f& ptC, double eps)
{
    if (std::abs(vec3f::normal(ptA, ptB, ptC)*(pt-ptA)) <= eps)
        return true;
    else
        return false;
}

bool pointInTriangle(const vec3f& pt, const vec3f& ptA, const vec3f& ptB, const vec3f& ptC, double eps)
{
    double areaABC = 0.5*vec3f::crossProduct(ptB-ptA, ptC-ptA).length();
    double areaBCP = 0.5*vec3f::crossProduct(ptB-pt, ptC-pt).length();
    double areaACP = 0.5*vec3f::crossProduct(ptA-pt, ptC-pt).length();
    double areaABP = 0.5*vec3f::crossProduct(ptA-pt, ptB-pt).length();

    double coeffA = areaBCP/areaABC;
    double coeffB = areaACP/areaABC;
    double coeffC = areaABP/areaABC;

    if (coeffA >= -eps && coeffA <= 1+eps && coeffB >= -eps && coeffB <= 1+eps && coeffC >= -eps && coeffC <= 1+eps)
        return true;
    else
        return false;
}

bool pointInRectangle(const vec3f& pt, const vec3f& ptA, const vec3f& ptB, const vec3f& ptC, const vec3f& ptD, double eps)
{
    return pointInTriangle(pt, ptA, ptB, ptC, eps) || pointInTriangle(pt, ptD, ptC, ptB, eps);
}

bool pointStrictlyInRectangle(const vec3f& pt, const vec3f& ptA, const vec3f& ptB, const vec3f& ptC, const vec3f& ptD, double eps)
{
    if (pointInRectangle(pt, ptA, ptB, ptC, ptD, eps))
    {
        vec3f ptDpY = pt; ptDpY.y() += 5.0*eps;
        vec3f ptDmY = pt; ptDmY.y() -= 5.0*eps;
        vec3f ptDpZ = pt; ptDpZ.z() += 5.0*eps;
        vec3f ptDmZ = pt; ptDmZ.z() -= 5.0*eps;

        if (pointInRectangle(ptDpY, ptA, ptB, ptC, ptD, eps)
         && pointInRectangle(ptDmY, ptA, ptB, ptC, ptD, eps)
         && pointInRectangle(ptDpZ, ptA, ptB, ptC, ptD, eps)
         && pointInRectangle(ptDmZ, ptA, ptB, ptC, ptD, eps))
            return true;
        else
            return false;
    }
    else
        return false;
}
SnapedFace snapedFace(NESpatial *sel, NESpatialMesh *other)
{
    AABB selAABB = getWorldAABB(sel);
    AABB otherAABB = getWorldAABB(other);

    vec3f pt, ptA, ptB, ptC, ptD;

    pt = 0.5*(selAABB.minPos()+vec3f(selAABB.minPos().x(), selAABB.maxPos().y(), selAABB.maxPos().z()));
    ptA = otherAABB.minPos();
    ptB = ptA; ptB.y() = otherAABB.maxPos().y();
    ptC = ptA; ptC.z() = otherAABB.maxPos().z();
    ptD = ptA; ptD.y() = otherAABB.maxPos().y(); ptD.z() = otherAABB.maxPos().z();
    if (pointInPlane(pt, ptA, ptB, ptC, FEps))
        if (pointStrictlyInRectangle(pt, ptA, ptB, ptC, ptD, FEps))
            return NEGATIVE_X;
    ptA = otherAABB.maxPos();
    ptB = ptA; ptB.y() = otherAABB.minPos().y();
    ptC = ptA; ptC.z() = otherAABB.minPos().z();
    ptD = ptA; ptD.y() = otherAABB.minPos().y(); ptD.z() = otherAABB.minPos().z();
    if (pointInPlane(pt, ptA, ptB, ptC, FEps))
        if (pointStrictlyInRectangle(pt, ptA, ptB, ptC, ptD, FEps))
            return NEGATIVE_X;

    pt = 0.5*(selAABB.maxPos()+vec3f(selAABB.maxPos().x(), selAABB.minPos().y(), selAABB.minPos().z()));
    ptA = otherAABB.minPos();
    ptB = ptA; ptB.y() = otherAABB.maxPos().y();
    ptC = ptA; ptC.z() = otherAABB.maxPos().z();
    ptD = ptA; ptD.y() = otherAABB.maxPos().y(); ptD.z() = otherAABB.maxPos().z();
    if (pointInPlane(pt, ptA, ptB, ptC, FEps))
        if (pointStrictlyInRectangle(pt, ptA, ptB, ptC, ptD, FEps))
            return POSITIVE_X;
    ptA = otherAABB.maxPos();
    ptB = ptA; ptB.y() = otherAABB.minPos().y();
    ptC = ptA; ptC.z() = otherAABB.minPos().z();
    ptD = ptA; ptD.y() = otherAABB.minPos().y(); ptD.z() = otherAABB.minPos().z();
    if (pointInPlane(pt, ptA, ptB, ptC, FEps))
        if (pointStrictlyInRectangle(pt, ptA, ptB, ptC, ptD, FEps))
            return POSITIVE_X;

    pt = 0.5*(selAABB.minPos()+vec3f(selAABB.maxPos().x(), selAABB.minPos().y(), selAABB.maxPos().z()));
    ptA = otherAABB.minPos();
    ptB = ptA; ptB.x() = otherAABB.maxPos().x();
    ptC = ptA; ptC.z() = otherAABB.maxPos().z();
    ptD = ptA; ptD.x() = otherAABB.maxPos().x(); ptD.z() = otherAABB.maxPos().z();
    if (pointInPlane(pt, ptA, ptB, ptC, FEps))
        if (pointStrictlyInRectangle(pt, ptA, ptB, ptC, ptD, FEps))
            return NEGATIVE_Y;
    ptA = otherAABB.maxPos();
    ptB = ptA; ptB.x() = otherAABB.minPos().x();
    ptC = ptA; ptC.z() = otherAABB.minPos().z();
    ptD = ptA; ptD.x() = otherAABB.minPos().x(); ptD.z() = otherAABB.minPos().z();
    if (pointInPlane(pt, ptA, ptB, ptC, FEps))
        if (pointStrictlyInRectangle(pt, ptA, ptB, ptC, ptD, FEps))
            return NEGATIVE_Y;

    pt = 0.5*(selAABB.maxPos()+vec3f(selAABB.minPos().x(), selAABB.maxPos().y(), selAABB.minPos().z()));
    ptA = otherAABB.minPos();
    ptB = ptA; ptB.x() = otherAABB.maxPos().x();
    ptC = ptA; ptC.z() = otherAABB.maxPos().z();
    ptD = ptA; ptD.x() = otherAABB.maxPos().x(); ptD.z() = otherAABB.maxPos().z();
    if (pointInPlane(pt, ptA, ptB, ptC, FEps))
        if (pointStrictlyInRectangle(pt, ptA, ptB, ptC, ptD, FEps))
            return POSITIVE_Y;
    ptA = otherAABB.maxPos();
    ptB = ptA; ptB.x() = otherAABB.minPos().x();
    ptC = ptA; ptC.z() = otherAABB.minPos().z();
    ptD = ptA; ptD.x() = otherAABB.minPos().x(); ptD.z() = otherAABB.minPos().z();
    if (pointInPlane(pt, ptA, ptB, ptC, FEps))
        if (pointStrictlyInRectangle(pt, ptA, ptB, ptC, ptD, FEps))
            return POSITIVE_Y;

    pt = 0.5*(selAABB.minPos()+vec3f(selAABB.maxPos().x(), selAABB.maxPos().y(), selAABB.minPos().z()));
    ptA = otherAABB.minPos();
    ptB = ptA; ptB.x() = otherAABB.maxPos().x();
    ptC = ptA; ptC.y() = otherAABB.maxPos().y();
    ptD = ptA; ptD.x() = otherAABB.maxPos().x(); ptD.y() = otherAABB.maxPos().y();
    if (pointInPlane(pt, ptA, ptB, ptC, FEps))
        if (pointStrictlyInRectangle(pt, ptA, ptB, ptC, ptD, FEps))
            return NEGATIVE_Z;
    ptA = otherAABB.maxPos();
    ptB = ptA; ptB.x() = otherAABB.minPos().x();
    ptC = ptA; ptC.y() = otherAABB.minPos().y();
    ptD = ptA; ptD.x() = otherAABB.minPos().x(); ptD.y() = otherAABB.minPos().y();
    if (pointInPlane(pt, ptA, ptB, ptC, FEps))
        if (pointStrictlyInRectangle(pt, ptA, ptB, ptC, ptD, FEps))
            return NEGATIVE_Z;

    pt = 0.5*(selAABB.maxPos()+vec3f(selAABB.minPos().x(), selAABB.minPos().y(), selAABB.maxPos().z()));
    ptA = otherAABB.minPos();
    ptB = ptA; ptB.x() = otherAABB.maxPos().x();
    ptC = ptA; ptC.y() = otherAABB.maxPos().y();
    ptD = ptA; ptD.x() = otherAABB.maxPos().x(); ptD.y() = otherAABB.maxPos().y();
    if (pointInPlane(pt, ptA, ptB, ptC, FEps))
        if (pointStrictlyInRectangle(pt, ptA, ptB, ptC, ptD, FEps))
            return POSITIVE_Z;
    ptA = otherAABB.maxPos();
    ptB = ptA; ptB.x() = otherAABB.minPos().x();
    ptC = ptA; ptC.y() = otherAABB.minPos().y();
    ptD = ptA; ptD.x() = otherAABB.minPos().x(); ptD.y() = otherAABB.minPos().y();
    if (pointInPlane(pt, ptA, ptB, ptC, FEps))
        if (pointStrictlyInRectangle(pt, ptA, ptB, ptC, ptD, FEps))
            return POSITIVE_Z;

    return NONE;
}

void NEGLWidget::mousePressEvent(QMouseEvent* event)
{
    //alt is pressed
    if(event->modifiers() == Qt::AltModifier)
    {
        // Disable context menus if alt modifier key is pressed with the button
        emit setMenuContextMenuPolicySignal(Qt::NoContextMenu);
        setContextMenuPolicy(Qt::NoContextMenu);
        switch(event->button())
        {
        case Qt::LeftButton:
            m_Tracking = kTrackRotate;
            mouseOldX = event->pos().x();
            mouseOldY = event->pos().y();
            clickLocalUp = m_pCamera->up();
            break;
        case Qt::RightButton:
            m_Tracking = kTrackZoom;
            break;
        case Qt::MiddleButton:
            m_Tracking = kTrackTranslate;
            break;
        default:
            break;
        }
    }
    else if(event->modifiers() == (Qt::AltModifier|Qt::ShiftModifier))
    {
        // Disable context menus if alt modifier key is pressed with the button
        emit setMenuContextMenuPolicySignal(Qt::NoContextMenu);
        setContextMenuPolicy(Qt::NoContextMenu);
        switch(event->button())
        {
        case Qt::LeftButton:
            m_Tracking = kTrackSmallRotate;
            mouseOldX = event->pos().x();
            mouseOldY = event->pos().y();
            clickLocalUp = m_pCamera->up();
            break;
        case Qt::RightButton:
            m_Tracking = kTrackSmallZoom;
            break;
        case Qt::MiddleButton:
            m_Tracking = kTrackSmallTranslate;
            break;
        default:
            break;
        }
    }
    else if(event->modifiers() == (Qt::AltModifier|Qt::ControlModifier))
    {
        // Disable context menus if alt modifier key is pressed with the button
        emit setMenuContextMenuPolicySignal(Qt::NoContextMenu);
        setContextMenuPolicy(Qt::NoContextMenu);
        switch(event->button())
        {
        case Qt::LeftButton:
            m_Tracking = kTrackZoom;
            break;
        default:
            break;
        }
    }
    else if(event->modifiers() == (Qt::AltModifier|Qt::ControlModifier|Qt::ShiftModifier))
    {
        // Disable context menus if alt modifier key is pressed with the button
        emit setMenuContextMenuPolicySignal(Qt::NoContextMenu);
        setContextMenuPolicy(Qt::NoContextMenu);
        switch(event->button())
        {
        case Qt::LeftButton:
            m_Tracking = kTrackTranslate;
            break;
        default:
            break;
        }
    }
    else if(event->modifiers() == Qt::ControlModifier || event->modifiers() == Qt::ShiftModifier)
    {
        switch(event->button())
        {
        case Qt::LeftButton:
            setFocus(Qt::OtherFocusReason);
            // Uncomment			NEGUI.setActiveGLWidget(this);
            if( manipSelected( event->pos().x(), event->pos().y() ) == false )
                pick(event->pos().x(), height()-event->pos().y(), devicePixelRatio(), true, true, event->modifiers() == Qt::ControlModifier);
            break;
        default:
            break;
        }
    }
    else
    {
        if(m_ActiveManipulator != kManipNone)
        {

            switch(event->button())
            {
            case Qt::LeftButton:
                setFocus(Qt::OtherFocusReason);
                // Uncomment		NEGUI.setActiveGLWidget(this);
                if( manipSelected( event->pos().x(), event->pos().y() ) == false )
                    pick(event->pos().x(), height()-event->pos().y(), devicePixelRatio());

                if(!NESCENE.selectedObject())
                    goto START_SEL;
                break;

            default:
                break;
            }

            NEObject* object = NESCENE.selectedObject();
            if( !(object && object->inherits("NESpatial")))
                goto START_SEL;
            NESpatial* spatial = (NESpatial*)object;
            if( this->m_ActiveManipulator == kManipTranslate && spatial->isManipulatable(kManipTranslate) )
            {                
                m_moveGizmo->OnMouseDown( event->pos().x(), event->pos().y() );
                m_SelectedPos = spatial->Position();
                m_SelectedAABB = getWorldAABB(spatial);
            }
            else if( this->m_ActiveManipulator == kManipRotate && spatial->isManipulatable(kManipRotate) )
                m_rotateGizmo->OnMouseDown( event->pos().x(), event->pos().y() );
            else if( this->m_ActiveManipulator == kManipScale && spatial->isManipulatable(kManipScale) )
            {
                m_scaleGizmo->OnMouseDown( event->pos().x(), event->pos().y() );
                m_SelectedPos = spatial->Position();
                m_SelectedAABB = getWorldAABB(spatial);
                if( NEGLWidget::m_SnapToBoundary || NEGLWidget::m_SnapToObj )
                {
                    if( NESpatialMesh *selSpatialMesh = qobject_cast<NESpatialMesh *>(spatial))
                    {
                        m_facedOtherSpatial = nullptr;
                        m_facedDirection = NONE;
                        m_snapedFace = NONE;

                        NEZone *zone = nullptr;

                        QObject *parent = selSpatialMesh->parent();
                        while( parent )
                        {
                            if( NEZone *z = qobject_cast<NEZone *>(parent) )
                            {
                                zone = z;
                                break;
                            }
                            parent = parent->parent();
                        }

                        QList<NEZoneCompartment *> compartments = zone->findChildren<NEZoneCompartment *>();
                        if (NEZoneCompartment* selCompart = qobject_cast<NEZoneCompartment*>(selSpatialMesh))
                            compartments.removeOne(selCompart);

                        QList<NEZoneItem*> items = zone->findChildren<NEZoneItem*>();
                        if (NEZoneItem* selItem = qobject_cast<NEZoneItem*>(selSpatialMesh))
                            items.removeOne(selItem);
                        for (NEZoneCompartment* compartment : compartments)
                            items.removeOne(compartment);

                        QList<NESpatialMesh*> spatialMeshes;
                        if (NEGLWidget::m_SnapToBoundary)
                        {
                            for (NEZoneCompartment* compartment : compartments)
                                spatialMeshes.push_back(qobject_cast<NEZoneCompartment*>(compartment));
                        }
                        if (NEGLWidget::m_SnapToObj)
                        {
                            for (NEZoneItem* item : items)
                                spatialMeshes.push_back(qobject_cast<NEZoneItem*>(item));
                        }

                        Q_FOREACH(NESpatialMesh* spatialMesh, spatialMeshes)
                        {
                            SnapedFace sf = snapedFace(selSpatialMesh, spatialMesh);
                            if(sf != NONE)
                            {
                                m_snapedFace = sf;
                                break;
                            }
                        }
                    }
                }
            }
            else if( this->m_ActiveManipulator == kSize && spatial->isManipulatable(kManipScale) )
                m_sizeGizmo->OnMouseDown( event->pos().x(), event->pos().y() );
            else if( this->m_ActiveManipulator == kLocation && spatial->isManipulatable(kManipTranslate) )
                m_locationGizmo->OnMouseDown( event->pos().x(), event->pos().y() );
        }
        else/* if(!NESCENE.selectedObject())*/
        {
START_SEL:
            m_bSelectionStarted = true;
            m_SelectionFirst.setX(event->pos().x());
            m_SelectionFirst.setY(height()-event->pos().y());
        }
    }
    if(m_SnapModeChanged)
    {
        m_measureStep = IGLSurface::MeasureStep::StandBy;
        m_SnapModeChanged = false;
        update();
    }
//    if(!NEGLWidget::m_Measure)
//    {
//        m_measureStep = IGLSurface::MeasureStep::StandBy;
//    }
    mousePosOld = mousePressPos = event->pos();

}

#define adjustOrthogonalPositive( _x_ ) \
    if (std::abs(selMax.##_x_()-m_SelectedAABB.maxPos().##_x_()) > 0) \
    { \
        if( selMax.##_x_() > otherMin.##_x_() && m_SelectedPos.##_x_() < otherMin.##_x_() ) \
        { \
            float diff = selMax.##_x_() - otherMin.##_x_(); \
            selSpatialMesh->SetScale##_x_(selSpatialMesh->Scale().##_x_() - diff); \
            selSpatialMesh->SetPosition##_x_(otherMin.##_x_() - 0.5*selSpatialMesh->Scale##_x_()); \
        } \
        if( selMax.##_x_() > otherMax.##_x_() && m_SelectedPos.##_x_() < otherMax.##_x_() && m_SelectedPos.##_x_() > otherMin.##_x_() ) \
        { \
            float diff = selMax.##_x_() - otherMax.##_x_(); \
            selSpatialMesh->SetScale##_x_(selSpatialMesh->Scale().##_x_() - diff); \
            selSpatialMesh->SetPosition##_x_(otherMax.##_x_() - 0.5*selSpatialMesh->Scale##_x_()); \
        } \
    } \

#define adjustOrthogonalNegative( _x_ ) \
    if (std::abs(selMin.##_x_()-m_SelectedAABB.minPos().##_x_()) > 0) \
    { \
        if( selMin.##_x_() < otherMax.##_x_() && m_SelectedPos.##_x_() > otherMax.##_x_() ) \
        { \
            float diff = otherMax.##_x_() - selMin.##_x_(); \
            selSpatialMesh->SetScale##_x_(selSpatialMesh->Scale().##_x_() - diff); \
            selSpatialMesh->SetPosition##_x_(otherMax.##_x_() + 0.5*selSpatialMesh->Scale##_x_()); \
        } \
        if( selMin.##_x_() < otherMin.##_x_() && m_SelectedPos.##_x_() > otherMin.##_x_() && m_SelectedPos.##_x_() < otherMax.##_x_() ) \
        { \
            float diff = otherMin.##_x_() - selMin.##_x_(); \
            selSpatialMesh->SetScale##_x_(selSpatialMesh->Scale().##_x_() - diff); \
            selSpatialMesh->SetPosition##_x_(otherMin.##_x_() + 0.5*selSpatialMesh->Scale##_x_()); \
        } \
    } \

#define adjustTangential( _x_ ) \
    if (std::abs(selMax.##_x_()-m_SelectedAABB.maxPos().##_x_()) > 0 || std::abs(selMin.##_x_()-m_SelectedAABB.minPos().##_x_()) > 0) \
    { \
        if (selMax.##_x_() > otherMax.##_x_()) \
        { \
            float diff = selMax.##_x_() - otherMax.##_x_(); \
            selSpatialMesh->SetScale##_x_(selSpatialMesh->Scale().##_x_() - diff); \
            selSpatialMesh->SetPosition##_x_(otherMax.##_x_() - 0.5*selSpatialMesh->Scale##_x_()); \
        } \
        if (selMin.##_x_() < otherMin.##_x_()) \
        { \
            float diff = otherMin.##_x_() - selMin.##_x_(); \
            selSpatialMesh->SetScale##_x_(selSpatialMesh->Scale().##_x_() - diff); \
            selSpatialMesh->SetPosition##_x_(otherMin.##_x_() + 0.5*selSpatialMesh->Scale##_x_()); \
        } \
    } \

void NEGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->modifiers() == Qt::AltModifier)
    {
        //m_Tracking = kTrackNone;
        //switch(event->button())
        //{
        //case Qt::LeftButton:
        //	m_Tracking &= ~kTrackRotate;
        //	break;
        //case Qt::RightButton:
        //	m_Tracking &= ~kTrackZoom;
        //	break;
        //case Qt::MiddleButton:
        //	m_Tracking &= ~kTrackTranslate;
        //	break;
        //}
    }
    else
    {
        if(m_bSelectionStarted /*&& m_ActiveManipulator != kManipNone*/)
        {
            switch(event->button())
            {
            case Qt::LeftButton:
                setFocus(Qt::OtherFocusReason);
                // Uncomment				NEGUI.setActiveGLWidget(this);
                if( manipSelected( event->pos().x(), event->pos().y() ) == false )
                    pick(event->pos().x(), height()-event->pos().y(), devicePixelRatio());
                break;
            case Qt::MidButton:
                m_PopupPos = event->pos();
                m_PopupSpatial = pick(event->pos().x(), height()-event->pos().y(), devicePixelRatio(), false, false);
                break;
            default:
                break;
            }
            m_bSelectionStarted = false;
            m_SelectionArea.setCoords(0,0,-1,-1);
        }

        if( (NEGLWidget::m_SnapToBoundary || NEGLWidget::m_SnapToObj) && m_ActiveManipOperation )
        {
            if( this->m_ActiveManipulator == kManipTranslate )
            {
                bool bSnapVent = false;

                if( NESpatialMesh *selSpatialMesh = qobject_cast<NESpatialMesh *>(NESCENE.selectedObject()))
                {
                    if( NEZoneItem *selItem = qobject_cast<NEZoneItem *>(selSpatialMesh))
                        if(selItem->categoryType() == "Simulation Items/Vents")
                            bSnapVent = true;

                    vec3f r1 = m_SelectedPos;
                    vec3f r2 = selSpatialMesh->Position();

                    NEZone *zone = nullptr;

                    QObject *parent = selSpatialMesh->parent();
                    while( parent )
                    {
                        if( NEZone *z = qobject_cast<NEZone *>(parent) )
                        {
                            zone = z;
                            break;
                        }
                        parent = parent->parent();
                    }

                    QList<NEZoneCompartment *> compartments = zone->findChildren<NEZoneCompartment *>();
                    if (NEZoneCompartment* selCompart = qobject_cast<NEZoneCompartment*>(selSpatialMesh))
                        compartments.removeOne(selCompart);

                    QList<NEZoneItem*> items = zone->findChildren<NEZoneItem*>();
                    if (NEZoneItem* selItem = qobject_cast<NEZoneItem*>(selSpatialMesh))
                        items.removeOne(selItem);
                    for (NEZoneCompartment* compartment : compartments)
                        items.removeOne(compartment);

                    QList<NESpatialMesh*> spatialMeshes;
                    if (NEGLWidget::m_SnapToBoundary)
                    {
                        for (NEZoneCompartment* compartment : compartments)
                            spatialMeshes.push_back(qobject_cast<NEZoneCompartment*>(compartment));
                    }
                    if (NEGLWidget::m_SnapToObj)
                    {
                        for (NEZoneItem* item : items)
                            spatialMeshes.push_back(qobject_cast<NEZoneItem*>(item));
                    }

                    QPair<NESpatialMesh *, int> selPair(nullptr, -1);
                    QPair<NESpatialMesh *, int> otherPair(nullptr, -1);
                    vec3f otherNormal;
                    vec3f interPoint;

                    if (bSnapVent)
                        interPoint = selSpatialMesh->findClosetFaceAndIntersectionPointAndRotate(spatialMeshes, r1, r2, selPair, otherPair, otherNormal);
                    else
                        interPoint = selSpatialMesh->findClosetFaceAndIntersectionPoint(spatialMeshes, r1, r2, selPair, otherPair);

                    if( selPair.first != nullptr && otherPair.first != nullptr )
                    {
                        double* faceSize = new double[6];
                        NEIndexedTriMesh &selTriMesh = selPair.first->triangleMesh();
                        NEPrimitivePolygon selFace = selPair.first->quadFaces()[selPair.second];
                        NESpatialMesh * otherSpatialMesh = otherPair.first;
                        NEIndexedTriMesh &otherTriMesh = otherSpatialMesh->triangleMesh();
                        faceSize[0] = INFINITY;
                        faceSize[1] = -INFINITY;
                        faceSize[2] = INFINITY;
                        faceSize[3] = -INFINITY;
                        faceSize[4] = INFINITY;
                        faceSize[5] = -INFINITY;

                        vec3f selInterPoint;
                        vec3f p1, p2, p3;
                        if (bSnapVent)
                        {
                            p1 = selSpatialMesh->transform() * selTriMesh.vertices()[selPair.first->triangleMesh().triangles().at(selPair.second).p1].position;
                            p2 = selSpatialMesh->transform() * selTriMesh.vertices()[selPair.first->triangleMesh().triangles().at(selPair.second).p2].position;
                            p3 = selSpatialMesh->transform() * selTriMesh.vertices()[selPair.first->triangleMesh().triangles().at(selPair.second).p3].position;
                        }
                        else
                        {
                            p1 = selSpatialMesh->transform() * selTriMesh.vertices()[selFace.triangles[0].p1].position;
                            p2 = selSpatialMesh->transform() * selTriMesh.vertices()[selFace.triangles[0].p2].position;
                            p3 = selSpatialMesh->transform() * selTriMesh.vertices()[selFace.triangles[0].p3].position;
                        }

                        vec3f selNormal = vec3f::normal(p1, p2, p3);
                        vec3f dirLine = (r2 - r1).normalized();

                        if (bSnapVent)
                            selInterPoint = linePlaneInterPoint(r1, dirLine, p1, otherNormal);
                        else
                            selInterPoint = linePlaneInterPoint(r1, dirLine, p1, selNormal);

                        if (bSnapVent)
                        {
                            for (size_t i = 0; i < otherTriMesh.vertices().size(); i++)
                            {
                                if ((otherSpatialMesh->transform() * otherTriMesh.vertices()[i].position - (selInterPoint - selPair.first->Position())).x() < faceSize[0])
                                    faceSize[0] = (otherSpatialMesh->transform() * otherTriMesh.vertices()[i].position - (selInterPoint - selPair.first->Position())).x();
                                if ((otherSpatialMesh->transform() * otherTriMesh.vertices()[i].position).x() > faceSize[1])
                                    faceSize[1] = (otherSpatialMesh->transform() * otherTriMesh.vertices()[i].position - (selInterPoint - selPair.first->Position())).x();
                                if ((otherSpatialMesh->transform() * otherTriMesh.vertices()[i].position).y() < faceSize[2])
                                    faceSize[2] = (otherSpatialMesh->transform() * otherTriMesh.vertices()[i].position - (selInterPoint - selPair.first->Position())).y();
                                if ((otherSpatialMesh->transform() * otherTriMesh.vertices()[i].position).y() > faceSize[3])
                                    faceSize[3] = (otherSpatialMesh->transform() * otherTriMesh.vertices()[i].position - (selInterPoint - selPair.first->Position())).y();
                                if ((otherSpatialMesh->transform() * otherTriMesh.vertices()[i].position).z() < faceSize[4])
                                    faceSize[4] = (otherSpatialMesh->transform() * otherTriMesh.vertices()[i].position - (selInterPoint - selPair.first->Position())).z();
                                if ((otherSpatialMesh->transform() * otherTriMesh.vertices()[i].position).z() > faceSize[5])
                                    faceSize[5] = (otherSpatialMesh->transform() * otherTriMesh.vertices()[i].position - (selInterPoint - selPair.first->Position())).z();
                            }

                            if (!std::isnan(selInterPoint.x()) && !std::isnan(selInterPoint.y()) && !std::isnan(selInterPoint.z()))
                            {
                                if( NEZoneItem *selItem = qobject_cast<NEZoneItem *>(selSpatialMesh))
                                {
                                    selItem->setSnapped(bSnapVent);
                                    selItem->setSnappedFaceSize(faceSize);
                                    vec3f offsetVector = selInterPoint-selPair.first->Position();
                                    float sign = (offsetVector.normalized()*(selPair.first->Position()-interPoint).normalized());
                                    selItem->SetPosition(interPoint+sign*offsetVector);
                                    selItem->setSnappedPosition(interPoint+sign*offsetVector);
                                }
                            }
                        }
                        else
                        {
                            if (!std::isnan(selInterPoint.x()) && !std::isnan(selInterPoint.y()) && !std::isnan(selInterPoint.z()))
                            {
                                vec3f offsetVector = selInterPoint-selPair.first->Position();
                                float sign = (offsetVector.normalized()*(selPair.first->Position()-interPoint).normalized());
                                selSpatialMesh->SetPosition(interPoint+sign*offsetVector);
                            }
                        }
                    }
                }
            }
            else if( this->m_ActiveManipulator == kManipScale && m_facedOtherSpatial)
            {
                 if( NESpatialMesh* selSpatialMesh = qobject_cast<NESpatialMesh *>(NESCENE.selectedObject()))
                 {
                    CGizmoTransformScale* scaleGimo = dynamic_cast< CGizmoTransformScale *>( m_scaleGizmo );

                    const AABB otherAabb = getWorldAABB(m_facedOtherSpatial);
                    const vec3f otherMin = otherAabb.minPos();
                    const vec3f otherMax = otherAabb.maxPos();

                    const AABB selAabb = getWorldAABB(selSpatialMesh);
                    const vec3f selMin = selAabb.minPos();
                    const vec3f selMax = selAabb.maxPos();

                    switch (m_snapedFace)
                    {
                    case POSITIVE_X:
//                        qDebug() << "+X";
                        adjustOrthogonalPositive(x);
                        adjustTangential(y);
                        adjustTangential(z);
                        break;
                    case NEGATIVE_X:
//                        qDebug() << "-X";
                        adjustOrthogonalNegative(x);
                        adjustTangential(y);
                        adjustTangential(z);
                        break;
                    case POSITIVE_Y:
//                        qDebug() << "+Y";
                        adjustOrthogonalPositive(y);
                        adjustTangential(x);
                        adjustTangential(z);
                        break;
                    case NEGATIVE_Y:
//                        qDebug() << "-Y";
                        adjustOrthogonalNegative(y);
                        adjustTangential(x);
                        adjustTangential(z);
                        break;
                    case POSITIVE_Z:
//                        qDebug() << "+Z";
                        adjustOrthogonalPositive(z);
                        adjustTangential(x);
                        adjustTangential(y);
                        break;
                    case NEGATIVE_Z:
//                        qDebug() << "-Z";
                        adjustOrthogonalNegative(z);
                        adjustTangential(x);
                        adjustTangential(y);
                        break;
                    case NONE:
//                        qDebug() << "NONE";
                        if( scaleGimo->getScaleType() == CGizmoTransformScale::SCALE_X || m_facedDirection == POSITIVE_X || m_facedDirection == NEGATIVE_X)
                        {
//                            qDebug() << "X";
                            adjustOrthogonalPositive(x);
                            adjustOrthogonalNegative(x);
                            adjustTangential(y);
                            adjustTangential(z);
                        }
                        else if( scaleGimo->getScaleType() == CGizmoTransformScale::SCALE_Y || m_facedDirection == POSITIVE_Y || m_facedDirection == NEGATIVE_Y)
                        {
//                            qDebug() << "Y";
                            adjustOrthogonalPositive(y);
                            adjustOrthogonalNegative(y);
                            adjustTangential(x);
                            adjustTangential(z);
                        }
                        else if( scaleGimo->getScaleType() == CGizmoTransformScale::SCALE_Z || m_facedDirection == POSITIVE_Z || m_facedDirection == NEGATIVE_Z)
                        {
//                            qDebug() << "Z";
                            adjustOrthogonalPositive(z);
                            adjustOrthogonalNegative(z);
                            adjustTangential(x);
                            adjustTangential(y);
                        }

                        break;
                    }

                    m_facedOtherSpatial = nullptr;
                    m_facedDirection = NONE;
                    m_facedDirection = NONE;
                }
            }
        }
        else if (!NEGLWidget::m_SnapToBoundary && !NEGLWidget::m_SnapToObj)
        {
            if( NEZoneItem *selItem = qobject_cast<NEZoneItem *>(NESCENE.selectedObject()))
                if(selItem->categoryType() == "Simulation Items/Vents")
                    selItem->setSnapped(false);
        }

        if (NEGLWidget::m_Measure)
        {
            if(m_measureStep == IGLSurface::MeasureStep::StandBy && m_bSnap)
            {
                m_measureFirstPt.first = m_snapObject;
                m_measureFirstPt.second = m_snapPtId;
                m_measureStep = IGLSurface::MeasureStep::FirstPt;
                m_snapMode = 0;
            }

            else if(m_measureStep == IGLSurface::MeasureStep::FirstPt && m_bSnap)
            {
                m_measureSecondPt.first = m_snapObject;
                m_measureSecondPt.second = m_snapPtId;
                m_measureStep = IGLSurface::MeasureStep::SecondPt;
            }

            else if(m_measureStep == IGLSurface::MeasureStep::SecondPt)
            {
                m_measureStep = IGLSurface::MeasureStep::DrawMeasurement;
            }
            else
            {
                m_measureStep = IGLSurface::MeasureStep::StandBy;
            }
        }

        if (NEGLWidget::m_MeasureEdge)
        {
            if(m_measureStep == IGLSurface::MeasureStep::StandBy && m_bSnapEdge)
            {
                m_measureFirstEdge.first = m_snapObject;
                m_measureFirstEdge.second.first = m_snapEdgeId;
                m_measureFirstEdge.second.second = m_snapObject->transform().inverted() * m_snapPt;
                m_measureStep = IGLSurface::MeasureStep::FirstPt;
                m_snapMode = 1;
            }
            else if(m_measureStep == IGLSurface::MeasureStep::FirstPt && m_bSnapEdge)
            {
                m_measureSecondEdge.first = m_snapObject;
                m_measureSecondEdge.second.first = m_snapEdgeId;
                m_measureSecondEdge.second.second = m_snapObject->transform().inverted() * m_snapPt;
                m_measureStep = IGLSurface::MeasureStep::SecondPt;
            }
            else if(m_measureStep == IGLSurface::MeasureStep::SecondPt)
            {
                m_measureStep = IGLSurface::MeasureStep::DrawMeasurement;
            }
            else
            {
                m_measureStep = IGLSurface::MeasureStep::StandBy;
            }
        }

        if( this->m_ActiveManipulator == kManipTranslate )
            m_moveGizmo->OnMouseUp( event->pos().x(), event->pos().y() );
        else if( this->m_ActiveManipulator == kManipRotate )
            m_rotateGizmo->OnMouseUp( event->pos().x(), event->pos().y() );
        else if( this->m_ActiveManipulator == kManipScale )
            m_scaleGizmo->OnMouseUp( event->pos().x(), event->pos().y() );
        else if( this->m_ActiveManipulator == kSize )
            m_sizeGizmo->OnMouseUp( event->pos().x(), event->pos().y() );
        else if( this->m_ActiveManipulator == kLocation )
            m_locationGizmo->OnMouseUp( event->pos().x(), event->pos().y() );

        if(event->button() == Qt::RightButton)
        {
            if(NESCENE.selectedNode())
                emit showPropertyShortCut(event->pos(), this, NESCENE.selectedNode());
        }

        m_ActiveManipOperation = false;
    }

    m_Tracking = kTrackNone;

    emit updatePropertyEditorUI();
    emit updateGLWindows();
}

void toEuler( double x,double y,double z,double angle, double& heading, double&attitude, double&bank )
{
    double s= sin(angle);
    double c= cos(angle);
    double t=1-c;
    //  if axis is not already normalised then uncomment this
    // double magnitude = Math.sqrt(x*x + y*y + z*z);
    // if (magnitude==0) throw error;
    // x /= magnitude;
    // y /= magnitude;
    // z /= magnitude;
    if ((x*y*t + z*s) > 0.998) { // north pole singularity detected
        heading = 2*atan2(x*sin(angle/2),cos(angle/2)) * 180/M_PI;
        attitude = M_PI/2 * 180/M_PI;
        bank = 0;
        return;
    }
    if ((x*y*t + z*s) < -0.998) { // south pole singularity detected
        heading = -2*atan2(x*sin(angle/2),cos(angle/2)) * 180/M_PI;
        attitude = -M_PI/2 * 180/M_PI;
        bank = 0;
        return;
    }
    heading  = atan2(y * s- x * z * t , 1 - (y*y+ z*z ) * t) * 180/M_PI;
    attitude = asin(x * y * t + z * s) * 180/M_PI ;
    bank     = atan2(x * s - y * z * t , 1 - (x*x + z*z) * t) * 180/M_PI;
}

void NEGLWidget::mouseMoveEvent( QMouseEvent* event )
{
    // Get the rotation from the camera
    if (m_pCamera == NULL)
        return;

    matrix44f rot( m_pCamera->pureRotation() );
    const QPoint delta = event->pos() - mousePosOld;    

    int h = height();
    int w = width();
    QPoint mousePos = event->pos();    
    std::vector<QPair<QPair<NESpatialMesh*, int>, QPoint>> ptList;
    QPair<QPair<NESpatialMesh*, int>, QPoint> ptPair;
    QPair<QPair<NESpatialMesh*, QPair<int, int>>, QPair<QPoint, vec3f>> edgePair;
    std::vector<QPair<QPair<NESpatialMesh*, QPair<int, int>>, QPair<QPoint, vec3f>>> edgeList;
    float minDist = 8000;
    int index;
    m_bSnap = false;
    m_bSnapEdge = false;

    if( this->m_ActiveManipulator != kManipNone )
    {
        NEObject* object = NESCENE.selectedObject();
        if( object && object->inherits("NESpatial"))
        {
            NESpatial* spatial = (NESpatial*)object;

            if( spatial )
            {
                if( this->m_ActiveManipulator == kManipTranslate && spatial->isManipulatable(kManipTranslate) )
                {
                    m_moveGizmo->OnMouseMove( event->pos().x(), event->pos().y() );
                    CGizmoTransformMove* movegizmo = dynamic_cast< CGizmoTransformMove *>( m_moveGizmo);
                    if( movegizmo )
                    {
                        if( movegizmo->getMoveType() != CGizmoTransformMove::MOVE_NONE )
                            this->m_ActiveManipOperation = true;
                        else
                            this->m_ActiveManipOperation = false;
                    }
                }
                else if( this->m_ActiveManipulator == kManipRotate && spatial->isManipulatable(kManipRotate) )
                {
                    m_rotateGizmo->OnMouseMove( event->pos().x(), event->pos().y() );
                    CGizmoTransformRotate* rotategizmo = dynamic_cast< CGizmoTransformRotate *>( m_rotateGizmo );
                    if( rotategizmo )
                    {
                        if( rotategizmo->getRotateType() != CGizmoTransformRotate::ROTATE_NONE )
                            this->m_ActiveManipOperation = true;
                        else
                            this->m_ActiveManipOperation = false;
                    }
                }
                else if( this->m_ActiveManipulator == kManipScale && spatial->isManipulatable(kManipScale) )
                {
                    m_scaleGizmo->OnMouseMove( event->pos().x(), event->pos().y() );
                    CGizmoTransformScale* scalegizmo = dynamic_cast< CGizmoTransformScale *>( m_scaleGizmo );
                    if( scalegizmo )
                    {
                        if( scalegizmo->getScaleType() != CGizmoTransformScale::SCALE_NONE )
                            this->m_ActiveManipOperation = true;
                        else
                            this->m_ActiveManipOperation = false;
                    }
                }
                else if( this->m_ActiveManipulator == kSize && spatial->isManipulatable(kManipScale) )
                {
                    m_sizeGizmo->OnMouseMove( event->pos().x(), event->pos().y() );
                    CGizmoTransformScale* sizegizmo = dynamic_cast< CGizmoTransformScale *>( m_sizeGizmo );
                    if( sizegizmo )
                    {
                        if( sizegizmo->getScaleType() != CGizmoTransformScale::SCALE_NONE )
                            this->m_ActiveManipOperation = true;
                        else
                            this->m_ActiveManipOperation = false;
                    }
                }
                else if( this->m_ActiveManipulator == kLocation && spatial->isManipulatable(kManipTranslate) )
                {
                    m_locationGizmo->OnMouseMove( event->pos().x(), event->pos().y() );
                    CGizmoTransformMove* locationgizmo = dynamic_cast< CGizmoTransformMove *>( m_locationGizmo );
                    if( locationgizmo )
                    {
                        if( locationgizmo->getMoveType() != CGizmoTransformMove::MOVE_NONE )
                            this->m_ActiveManipOperation = true;
                        else
                            this->m_ActiveManipOperation = false;
                    }
                }
                update();
            }
        }
    }
    if( m_bSelectionStarted )
    {
        m_SelectionSecond.setX(event->pos().x());
        m_SelectionSecond.setY(height()-event->pos().y());

        if(m_SelectionFirst != m_SelectionSecond)
        {
            m_SelectionArea.setLeft(MIN(m_SelectionFirst.x(), m_SelectionSecond.x()));
            m_SelectionArea.setRight(MAX(m_SelectionFirst.x(), m_SelectionSecond.x()));

            m_SelectionArea.setBottom(MAX(height()-m_SelectionFirst.y(), height()-m_SelectionSecond.y()));
            m_SelectionArea.setTop(MIN(height()-m_SelectionFirst.y(), height()-m_SelectionSecond.y()));

            update();
        }
    }
    else if(m_Tracking > 0)
    {
        //printf("I am tracking!\n");
        if(m_Tracking == kTrackTranslate || m_Tracking == kTrackSmallTranslate)
        {
            // Build a std::vector for the position difference
            vec3f trans((float)-delta.x(), (float)delta.y(), 0.0);
            if(m_Tracking == kTrackSmallTranslate)
                trans = trans * m_SmallIncrementalAdjustment;

            if(m_pCamera->fixedOrthographic())
            {

                trans *= 2.0 * m_pCamera->OrthoWidth() / (float)this->width();
                // Project the translation to the camera space
                trans = rot*trans;
                // Translate the position and target of camera
                m_pCamera->Translate(trans);
                //	Log()<<"delta: "<<trans; EndLog(NE::kLogInfo);
                //m_pCamera->computeProjection(width(), height());
            }
            else
            {
                // Measure the distance from the camera to the origin
                float dist = fabs(vec3f::dotProduct(-m_pCamera->Position(), m_pCamera->look()));
                // Scale the translation depending on the camera parameters
                trans *= 2.0 * tan((m_pCamera->Fov()*FPiOver180)*0.5)* (dist) / (float)this->width();
                // Project the translation to the camera space
                trans = rot*trans;
                m_pCamera->Translate(trans);
            }

        }
        if((m_Tracking == kTrackRotate || m_Tracking == kTrackSmallRotate) && !m_pCamera->fixedOrthographic() )
        {
            float oldx_scaled = -(2.0f*(float)mousePosOld.x() - (float)this->width()) /  (float)this->width();
            float oldy_scaled = (2.0f*(float)mousePosOld.y() - (float)this->height()) /  (float)this->height();
            float x_scaled =    -(2.0f*(float)event->x() - (float)this->width()) / (float)this->width();
            float y_scaled =    (2.0f*(float)event->y() - (float)this->height()) / (float)this->height();

            matrix33f rot33 = m_pCamera->pureRotation();

            // Get the side std::vector in camera space and up std::vector in world space
            vec3f xTrans = rot33*vec3f( 1, 0, 0 );
            vec3f yTrans;

            // Choose the up std::vector based on the local up std::vector
            if(clickLocalUp*vec3f( 0, 1, 0 ) > 0)
                yTrans = vec3f( 0, 1, 0 );
            else
                yTrans = vec3f( 0, -1, 0 );

            // Construct up and side std::vector rotation quternions based on the delta of the mouse positions (x and y)
            quatf yQuat = quatf::fromAxisAndAngle( yTrans,( x_scaled-oldx_scaled )*100.0f );
            quatf xQuat = quatf::fromAxisAndAngle( xTrans,( oldy_scaled-y_scaled )*100.0f );

            if(m_Tracking == kTrackSmallRotate)
            {
                yQuat = quatf::fromAxisAndAngle( yTrans,( x_scaled-oldx_scaled )*100.0f * m_SmallIncrementalAdjustment);
                xQuat = quatf::fromAxisAndAngle( xTrans,( oldy_scaled-y_scaled )*100.0f * m_SmallIncrementalAdjustment);
            }

            xQuat.normalize();
            yQuat.normalize();

            // Rotate the cam look at around local x
            vec3f rotVec = xQuat.rotatedVector( m_pCamera->Position()-m_pCamera->LookAt() );
            m_pCamera->SetPosition(rotVec+m_pCamera->LookAt());

            // Rotate the cam look at around world y
            rotVec = yQuat.rotatedVector( m_pCamera->Position()-m_pCamera->LookAt() );
            m_pCamera->SetPosition(rotVec+m_pCamera->LookAt());
        }
        if(m_Tracking == kTrackZoom || m_Tracking == kTrackSmallZoom)
        {
            if(m_pCamera->fixedOrthographic())
            {
                float factor = (event->y() - mousePosOld.y()) / (float)this->height();
                if(m_Tracking == kTrackSmallZoom)
                    factor = factor * m_SmallIncrementalAdjustment;
                float OrthoWidth = m_pCamera->OrthoWidth() - factor*m_pCamera->OrthoWidth();
                m_pCamera->SetOrthoWidth(OrthoWidth);
                resizeGL(width(), height());
            }
            else if(m_pCamera->customOrthographic())
            {
                float factor = (event->y() - mousePosOld.y()) / (float)this->height();
                if(m_Tracking == kTrackSmallZoom)
                    factor = factor * m_SmallIncrementalAdjustment;
                float OrthoWidth = m_pCamera->OrthoWidth() - factor*m_pCamera->OrthoWidth();
                m_pCamera->SetOrthoWidth(OrthoWidth);
                resizeGL(width(), height());

                float dist = fabs(vec3f::dotProduct(-m_pCamera->Position(), m_pCamera->look()));
                if(m_Tracking == kTrackSmallZoom)
                    dist = dist * m_SmallIncrementalAdjustment;
                vec3f trans(0, 0, -dist * (event->y() - mousePosOld.y()) / this->height());
                trans = rot * trans;
                m_pCamera->SetPositionZoom(m_pCamera->Position() + trans);
            }
            else
            {
                float dist = fabs(vec3f::dotProduct(-m_pCamera->Position(), m_pCamera->look()));
                if(m_Tracking == kTrackSmallZoom)
                    dist = dist * m_SmallIncrementalAdjustment;
                vec3f trans(0, 0, -dist * (event->y() - mousePosOld.y()) / this->height());
                trans = rot * trans;
                m_pCamera->SetPositionZoom(m_pCamera->Position() + trans);
            }

        }

        update();
    }

    else if(m_ActiveManipOperation)
    {
        NESpatial* object = (NESpatial*)NESCENE.selectedObject();
        QList<NESpatial*> objects;
        foreach (NEObject *obj, NESCENE.selectedObjects()->values()) {
            if(NESpatial *sp = qobject_cast<NESpatial*>(obj)){
                objects.push_back(sp);
            }
        }

        if( this->m_ActiveManipulator == kManipTranslate )
        {
            CGizmoTransformMove* movegizmo = dynamic_cast< CGizmoTransformMove *>( m_moveGizmo);

            if( movegizmo )
            {
                tvector3 orgPos = *movegizmo->editPos() - tvector3(object->Positionx(), object->Positiony(), object->Positionz());
                const tvector3* dpos = &orgPos;
                foreach (NESpatial *sp, objects)
                {
                    if(event->modifiers() == (Qt::ShiftModifier))
                    {
                        if (sp->spatialType() != NESpatial::kOSD)
                            sp->SetSmallAdjustmentLocalPositionFromWorld( sp->Position() + vec3f( dpos->x, dpos->y, dpos->z ) );
                        else
                            sp->SetSmallAdjustmentLocalPositionFromWorld( sp->Position() + vec3f( 1.0f * dpos->x / width(), 1.0f * dpos->y / height(), dpos->z ) );
                    }
                    else
                    {
                        if (sp->spatialType() != NESpatial::kOSD)
                        {
                            if(m_bSnapUnit)
                            {
                                int tmp = 0;
                                vec3f pos = sp->Position() + vec3f( dpos->x, dpos->y, dpos->z );
                                if(abs(dpos->x) > 1e-3 && m_SnapUnit.x() != 0)
                                {
                                    tmp = (int)(pos.x() / m_SnapUnit.x());
                                    pos.setX(tmp * m_SnapUnit.x());
                                }
                                if(abs(dpos->y) > 1e-3 && m_SnapUnit.y() != 0)
                                {
                                    tmp = (int)(pos.y() / m_SnapUnit.y());
                                    pos.setY(tmp * m_SnapUnit.y());
                                }
                                if(abs(dpos->z) > 1e-3 && m_SnapUnit.z() != 0)
                                {
                                    tmp = (int)(pos.z() / m_SnapUnit.z());
                                    pos.setZ(tmp * m_SnapUnit.z());
                                }
                                sp->SetLocalPositionFromWorld(pos);
                            }
                            else
                                sp->SetLocalPositionFromWorld( sp->Position() + vec3f( dpos->x, dpos->y, dpos->z ) );
                        }
                        else
                        {
                            if(m_bSnapUnit)
                            {
                                int tmp = 0;
                                vec3f pos = sp->Position() +  vec3f( 1.0f * dpos->x / width(), 1.0f * dpos->y / height(), dpos->z );
                                sp->SetLocalPositionFromWorld(pos);
                            }
                            else{
                                orgPos = *movegizmo->editPos() - tvector3(object->Positionx()*width(), object->Positiony()*height(), object->Positionz());
                                vec3f pos = sp->Position() +  vec3f( 1.0f * dpos->x / width(), 1.0f * dpos->y / height(), dpos->z );
                                sp->SetLocalPositionFromWorld( pos );
                            }
                        }
                    }
                }
            }
        }
        else if( this->m_ActiveManipulator == kManipRotate )
        {
            CGizmoTransformRotate* rotategizmo = dynamic_cast< CGizmoTransformRotate *>( m_rotateGizmo );
            if( rotategizmo )
            {
                float rotMatValues[9];
                rotategizmo->getRotation(rotMatValues);
                matrix33f rotMat(rotMatValues);
                quatf drotQuat = quatf::fromRotMatrix(rotMat) - object->orientationQuat();
                foreach (NESpatial *sp, objects) {
                    if(event->modifiers() == (Qt::ShiftModifier))
                        sp->SetSmallAdjustmentLocalOrientationFromWorld( sp->orientationQuat() + drotQuat );
                    else
                        sp->SetLocalOrientationFromWorld( sp->orientationQuat() + drotQuat );
                }

            }
        }
        else if( this->m_ActiveManipulator == kManipScale )
        {
            CGizmoTransformScale* scalegizmo = dynamic_cast< CGizmoTransformScale *>( m_scaleGizmo );
            if( scalegizmo )
            {
                const tvector3 orgst = *scalegizmo->editScale() / tvector3(object->Scalex(), object->Scaley(), object->Scalez());
                const tvector3* st = &orgst;
                foreach (NESpatial *sp, objects)
                {
                    if(event->modifiers() == (Qt::ShiftModifier))
                        sp->SetSmallAdjustmentLocalScaleFromWorld( vec3f( sp->Scalex()*st->x, sp->Scaley()*st->y, sp->Scalez()*st->z ) );
                    else
                    {
                        if(m_bSnapUnit)
                        {
                            int tmp = 0;
                            vec3f scale = vec3f( sp->Scalex()*st->x, sp->Scaley()*st->y, sp->Scalez()*st->z );
                            if(abs(sp->Scalex()*st->x) > 1e-3 && m_SnapUnit.x() != 0)
                            {
                                tmp = (int)(scale.x() / m_SnapUnit.x());
                                scale.setX(tmp * m_SnapUnit.x());
                            }
                            if(abs(sp->Scalex()*st->y) > 1e-3 && m_SnapUnit.y() != 0)
                            {
                                tmp = (int)(scale.y() / m_SnapUnit.y());
                                scale.setY(tmp * m_SnapUnit.y());
                            }
                            if(abs(sp->Scalex()*st->z) > 1e-3 && m_SnapUnit.z() != 0)
                            {
                                tmp = (int)(scale.z() / m_SnapUnit.z());
                                scale.setZ(tmp * m_SnapUnit.z());
                            }
                            sp->SetLocalScaleFromWorld(scale);
                        }
                        else
                            sp->SetLocalScaleFromWorld( vec3f( sp->Scalex()*st->x, sp->Scaley()*st->y, sp->Scalez()*st->z ) );
                    }
                }

                if( NEGLWidget::m_SnapToBoundary || NEGLWidget::m_SnapToObj )
                {
                    if( NESpatialMesh *selSpatialMesh = qobject_cast<NESpatialMesh *>(NESCENE.selectedObject()))
                    {
                        if( m_facedOtherSpatial == nullptr)
                        {
                            NEZone *zone = nullptr;

                            QObject *parent = selSpatialMesh->parent();
                            while( parent )
                            {
                                if( NEZone *z = qobject_cast<NEZone *>(parent) )
                                {
                                    zone = z;
                                    break;
                                }
                                parent = parent->parent();
                            }

                            QList<NEZoneCompartment *> compartments = zone->findChildren<NEZoneCompartment *>();
                            if (NEZoneCompartment* selCompart = qobject_cast<NEZoneCompartment*>(selSpatialMesh))
                                compartments.removeOne(selCompart);

                            QList<NEZoneItem*> items = zone->findChildren<NEZoneItem*>();
                            if (NEZoneItem* selItem = qobject_cast<NEZoneItem*>(selSpatialMesh))
                                items.removeOne(selItem);
                            for (NEZoneCompartment* compartment : compartments)
                                items.removeOne(compartment);

                            QList<NESpatialMesh*> spatialMeshes;
                            if (NEGLWidget::m_SnapToBoundary)
                            {
                                for (NEZoneCompartment* compartment : compartments)
                                    spatialMeshes.push_back(qobject_cast<NEZoneCompartment*>(compartment));
                            }
                            if (NEGLWidget::m_SnapToObj)
                            {
                                for (NEZoneItem* item : items)
                                    spatialMeshes.push_back(qobject_cast<NEZoneItem*>(item));
                            }

                            AABB selAABB = getWorldAABB(selSpatialMesh);

                            Q_FOREACH(NESpatialMesh* otherSpatialMesh, spatialMeshes)
                            {
                                AABB otherAABB = getWorldAABB(otherSpatialMesh);
                                if( selAABB.intersects(otherAABB))
                                {
                                    m_facedOtherSpatial = otherSpatialMesh;

                                    const vec3f selMin = selAABB.minPos();
                                    const vec3f selMax = selAABB.maxPos();
                                    const vec3f otherMin = otherAABB.minPos();
                                    const vec3f otherMax = otherAABB.maxPos();

                                    float dist[3];
                                    dist[0] = FLT_MAX;
                                    dist[1] = FLT_MAX;
                                    dist[2] = FLT_MAX;

                                    if( selMin.x() < otherMin.x() && selMax.x() > otherMin.x())
                                        dist[0] = selMax.x() - otherMin.x();
                                    else if (otherMin.x() < selMin.x() && otherMax.x() > selMin.x())
                                        dist[0] = selMin.x() - otherMax.x();

                                    if( selMin.y() < otherMin.y() && selMax.y() > otherMin.y())
                                        dist[1] = selMax.y() - otherMin.y();
                                    else if (otherMin.y() < selMin.y() && otherMax.y() > selMin.y())
                                        dist[1] = selMin.y() - otherMax.y();

                                    if( selMin.z() < otherMin.z() && selMax.z() > otherMin.z())
                                        dist[2] = selMax.z() - otherMin.z();
                                    else if (otherMin.z() < selMin.z() && otherMax.z() > selMin.z())
                                        dist[2] = selMin.z() - otherMax.z();

                                    float minDist = fmin( fmin(abs(dist[0]), abs(dist[1])), abs(dist[2]));

                                    if( minDist == dist[0])
                                        m_facedDirection = POSITIVE_X;
                                    else if( minDist == -dist[0])
                                        m_facedDirection = NEGATIVE_X;
                                    else if( minDist == dist[1])
                                        m_facedDirection = POSITIVE_Y;
                                    else if(minDist == -dist[1])
                                        m_facedDirection = NEGATIVE_Y;
                                    else if( minDist == dist[2])
                                        m_facedDirection = POSITIVE_Z;
                                    else if(minDist == -dist[2])
                                        m_facedDirection = NEGATIVE_Z;

                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
//        else if( this->m_ActiveManipulator == kSize )
//        {
//            CGizmoTransformScale* sizegizmo = dynamic_cast< CGizmoTransformScale *>( m_sizeGizmo );
//            if( sizegizmo )
//            {
//                const tvector3* st = sizegizmo->editScale();
//            }
//        }
//        else if( this->m_ActiveManipulator == kLocation )
//        {
//            CGizmoTransformMove* locationgizmo = dynamic_cast< CGizmoTransformMove *>( m_locationGizmo );
//            if( locationgizmo )
//            {
//                const tvector3* pos = locationgizmo->editPos();
//            }
//        }

        emit updateGLWindows();
    }
/*Snap Point*/
    else if(m_Measure)
    {
        if(NESCENE.selectedObjects()->size())
        {
            foreach(NEObject* object, *(NESCENE.selectedObjects()))
            {
                if(NESpatialMesh* selObject = qobject_cast<NESpatialMesh*>(object))
                {
                    int length = selObject->triangleMesh().vertices().size();
                    for (int i = 0; i < length ; i++)
                    {
                        vec3f pos = selObject->transform() * selObject->triangleMesh().vertices()[i].position;
                        vec3f projectionPos = projectionMatrix() * m_pCamera->viewMatrix() * pos;
                        float screenX = w * (0.5 + projectionPos.x() / 2);
                        float screenY = h - h * (0.5 + projectionPos.y() / 2);
                        ptPair.first.first = selObject;
                        ptPair.first.second = i;
                        ptPair.second = QPoint(screenX, screenY);
                        ptList.push_back(ptPair);
                    }
                }
            }

        }
        else
        {
            foreach( NESpatial* _object, NESCENE.spatials() )
            {
                if (NESpatialMesh* object = qobject_cast<NESpatialMesh*>(_object))
                {
                    int length = object->triangleMesh().vertices().size();
                    for (int i = 0; i < length ; i++)
                    {
                        vec3f pos = object->transform() * object->triangleMesh().vertices()[i].position;
                        vec3f projectionPos = projectionMatrix() * m_pCamera->viewMatrix() * pos;
                        float screenX = w * (0.5 + projectionPos.x() / 2);
                        float screenY = h - h * (0.5 + projectionPos.y() / 2);
                        ptPair.first.first = object;
                        ptPair.first.second = i;
                        ptPair.second = QPoint(screenX, screenY);
                        ptList.push_back(ptPair);
                    }
                }
            }
        }
        for(int i = 0; i < ptList.size() ; i++)
        {
            if (QPoint::dotProduct(ptList.at(i).second - mousePos, ptList.at(i).second - mousePos) < minDist)
            {
                minDist = QPoint::dotProduct(ptList.at(i).second - mousePos, ptList.at(i).second - mousePos);
                index = i;
                m_bSnap = true;
            }
        }

        if(m_bSnap)
        {
            m_snapPtId = ptList.at(index).first.second;
            m_snapObject = ptList.at(index).first.first;
            m_snapPt = m_snapObject->transform() * m_snapObject->triangleMesh().vertices()[m_snapPtId].position;
        }

        update();
    }

    else if(m_MeasureEdge)
    {
//        if(NESCENE.selectedObjects()->size())
//        {
//            foreach(NEObject* object, *(NESCENE.selectedObjects()))
//            {
//                if(NESpatialMesh* selObject = qobject_cast<NESpatialMesh*>(object))
//                {
//                    //int length = selObject->triangleMesh().vertices().size();
//                    int length = selObject->quadFaces().size();
//                    NEIndexedTriMesh &triMesh = selObject->triangleMesh();
//                    if(length >= 4){
//                        for (int i = 0; i < 4 ; i++)
//                        {

//                            NEPrimitivePolygon face = selObject->quadFaces().at(i);
//                            for (int j = 0; j < 4 ; j++)
//                            {
//                                int vertID1 = face.vertices.at(j%4);
//                                int vertID2 = face.vertices.at((j+1)%4);

//                                if(i < 2){
//                                    if(j == 1)
//                                        vertID2 = face.vertices.at(3);
//                                    if(j == 3)
//                                        vertID1 = face.vertices.at(2);
//                                }

//                                vec3f pt1 = triMesh.vertices()[vertID1].position;
//                                pt1 = selObject->transform() * pt1;
//                                vec3f projectionPt1 = projectionMatrix() * m_pCamera->viewMatrix() * pt1;
//                                vec3f pt2 = triMesh.vertices()[vertID2].position;
//                                pt2 = selObject->transform() * pt2;
//                                vec3f projectionPt2 = projectionMatrix() * m_pCamera->viewMatrix() * pt2;

//                                float screenX1 = w * (0.5 + projectionPt1.x() / 2);
//                                float screenY1 = h - h * (0.5 + projectionPt1.y() / 2);

//                                float screenX2 = w * (0.5 + projectionPt2.x() / 2);
//                                float screenY2 = h - h * (0.5 + projectionPt2.y() / 2);

//                                vec2f lineDir = vec2f(screenX2 - screenX1, screenY2 - screenY1);
//                                float len = lineDir.length();
//                                lineDir.normalize();
//                                float dotProduct = vec2f::dotProduct(lineDir, vec2f(mousePos.x() - screenX1, mousePos.y() - screenY1));
//                                if(dotProduct > len)
//                                    dotProduct = len;
//                                if(dotProduct < 0)
//                                    dotProduct = 0;
//                                vec2f pt2D = vec2f(screenX1, screenY1) + lineDir * dotProduct;

//                                vec3f pt3D = projectionPt1 + (projectionPt2 - projectionPt1) * dotProduct / len;
//                                pt3D = m_pCamera->viewMatrix().inverted() * projectionMatrix().inverted() * pt3D;

//                                edgePair.first.first = selObject;
//                                edgePair.first.second.first = vertID1;
//                                edgePair.first.second.second = vertID2;
//                                edgePair.second.first = QPoint(pt2D.x(), pt2D.y());
//                                edgePair.second.second = pt3D;
//                                edgeList.push_back(edgePair);
//                            }
//                        }


//                    }
//                }
//            }
//        }
//        else
//        {
            foreach( NESpatial* object, NESCENE.spatials() )
            {
                if(NESpatialMesh* selObject = qobject_cast<NESpatialMesh*>(object))
                {
                    //int length = selObject->triangleMesh().vertices().size();
                    int length = selObject->quadFaces().size();
                    NEIndexedTriMesh &triMesh = selObject->triangleMesh();
                    if(length >= 4){
                        for (int i = 0; i < 4 ; i++)
                        {
                            NEPrimitivePolygon face = selObject->quadFaces().at(i);
                            for (int j = 0; j < 4 ; j++)
                            {
                                int vertID1 = face.vertices.at(j%4);
                                int vertID2 = face.vertices.at((j+1)%4);

                                if(i < 2){
                                    if(j == 1)
                                        vertID2 = face.vertices.at(3);
                                    if(j == 3)
                                        vertID1 = face.vertices.at(2);
                                }

                                vec3f pt1 = triMesh.vertices()[vertID1].position;
                                pt1 = selObject->transform() * pt1;
                                vec3f projectionPt1 = projectionMatrix() * m_pCamera->viewMatrix() * pt1;
                                vec3f pt2 = triMesh.vertices()[vertID2].position;
                                pt2 = selObject->transform() * pt2;
                                vec3f projectionPt2 = projectionMatrix() * m_pCamera->viewMatrix() * pt2;

                                float screenX1 = w * (0.5 + projectionPt1.x() / 2);
                                float screenY1 = h - h * (0.5 + projectionPt1.y() / 2);

                                float screenX2 = w * (0.5 + projectionPt2.x() / 2);
                                float screenY2 = h - h * (0.5 + projectionPt2.y() / 2);

                                vec2f lineDir = vec2f(screenX2 - screenX1, screenY2 - screenY1);
                                float len = lineDir.length();
                                lineDir.normalize();
                                float dotProduct = vec2f::dotProduct(lineDir, vec2f(mousePos.x() - screenX1, mousePos.y() - screenY1));
                                if(dotProduct > len)
                                    dotProduct = len;
                                if(dotProduct < 0)
                                    dotProduct = 0;
                                vec2f pt2D = vec2f(screenX1, screenY1) + lineDir * dotProduct;

                                vec3f pt3D = projectionPt1 + (projectionPt2 - projectionPt1) * dotProduct / len;
                                pt3D = selObject->transform().inverted() * m_pCamera->viewMatrix().inverted() * projectionMatrix().inverted() * pt3D;

                                edgePair.first.first = selObject;
                                edgePair.first.second.first = vertID1;
                                edgePair.first.second.second = vertID2;
                                edgePair.second.first = QPoint(pt2D.x(), pt2D.y());
                                edgePair.second.second = pt3D;
                                edgeList.push_back(edgePair);
                            }
                        }


                    }
                }
            }
//        }
        for(int i = 0; i < edgeList.size() ; i++)
        {
            if (QPoint::dotProduct(edgeList.at(i).second.first - mousePos, edgeList.at(i).second.first - mousePos) < minDist)
            {
                minDist = QPoint::dotProduct(edgeList.at(i).second.first - mousePos, edgeList.at(i).second.first - mousePos);
                index = i;
                m_bSnapEdge = true;
            }
        }
        if(m_bSnapEdge){
            m_snapObject = edgeList.at(index).first.first;
            m_snapPt = m_snapObject->transform() * edgeList.at(index).second.second;
            m_snapEdgeId.first = edgeList.at(index).first.second.first;
            m_snapEdgeId.second = edgeList.at(index).first.second.second;
        }
        update();
    }

    if(m_SnapModeChanged)
    {
        m_measureStep = IGLSurface::MeasureStep::StandBy;
        m_SnapModeChanged = false;
        update();
    }
    m_mousePos = event->pos();
    mousePosOld = event->pos();
}

void NEGLWidget::resetCamera(){
    camera()->resetCamera();

    NEBASE.updatePropertyEditor();
    NEBASE.updateGLWindows();
}

void NEGLWidget::splitHorizontally()
{
    emit splitGLDockHorizontally(this);
}

void NEGLWidget::splitVertically()
{
    emit splitGLDockVertically(this);
}

void NEGLWidget::splitToQuad()
{
    emit splitGLDockToQuad(this);
}

void NEGLWidget::close()
{
    emit closeGLWidget( this );
}

void NEGLWidget::hide()
{
    emit hideGLWidget( this );
}

void NEGLWidget::drawCameraLookAt()
{
    // Draw the look-at position of the camera
    if(camera())
    {
        worldStack().pushIdentity();
        setModelViewMatrix();
        GL->glColor3f(1,1,1);
        GL->glPointSize(8);
        GL->glBegin(GL_POINTS);
        GL->glVertex3fv((GLfloat*)&camera()->LookAt());
        GL->glEnd();

        GL->glColor3f(1,0,0);
        GL->glPointSize(4);
        GL->glBegin(GL_POINTS);
        GL->glVertex3fv((GLfloat*)&camera()->LookAt());
        GL->glEnd();

        worldStack().popMatrix();
    }
    GL->glPointSize(1);
}

void NEGLWidget::drawManipulator( NESpatial* object, bool pick/*=false*/ )
{
    GL->glPushAttrib( GL_ALL_ATTRIB_BITS );

    if (object->spatialType() != NESpatial::kOSD)
    {
        float viewMat[16];
        float projMat[16];

        GL->glGetFloatv ( GL_MODELVIEW_MATRIX, viewMat );
        GL->glGetFloatv ( GL_PROJECTION_MATRIX, projMat );

        int gizmoOrthoType = -1;
        switch (m_pCamera->OrthoType())
        {
        case NECamera::OrthographicType::Top:
            gizmoOrthoType = 0;
            break;
        case NECamera::OrthographicType::Left:
            gizmoOrthoType = 1;
            break;
        case NECamera::OrthographicType::Front:
            gizmoOrthoType = 2;
            break;
        }

        m_moveGizmo->SetCameraMatrix( viewMat, projMat, m_pCamera->fixedOrthographic() || m_pCamera->customOrthographic(), gizmoOrthoType  );
        m_rotateGizmo->SetCameraMatrix( viewMat, projMat, m_pCamera->fixedOrthographic() || m_pCamera->customOrthographic(), gizmoOrthoType );
        m_scaleGizmo->SetCameraMatrix( viewMat, projMat, m_pCamera->fixedOrthographic() || m_pCamera->customOrthographic(), gizmoOrthoType );

        m_sizeGizmo->SetCameraMatrix( viewMat, projMat, m_pCamera->fixedOrthographic() || m_pCamera->customOrthographic(), gizmoOrthoType );
        m_locationGizmo->SetCameraMatrix( viewMat, projMat, m_pCamera->fixedOrthographic() || m_pCamera->customOrthographic(), gizmoOrthoType );

        IGizmo::LOCATION manipSpace = IGizmo::LOCATE_LOCAL;
        switch (P()->manipSpace())
        {
        case NEGLWidgetProperties::ManipSpace::Local:
            manipSpace = IGizmo::LOCATE_LOCAL;
            break;
        case NEGLWidgetProperties::ManipSpace::World:
            manipSpace = IGizmo::LOCATE_WORLD;
            break;
        case NEGLWidgetProperties::ManipSpace::View:
            manipSpace = IGizmo::LOCATE_VIEW;
            break;
        }
        m_moveGizmo->SetLocation(manipSpace);
        m_rotateGizmo->SetLocation(manipSpace);
        m_scaleGizmo->SetLocation(IGizmo::LOCATE_LOCAL);

        m_sizeGizmo->SetLocation(IGizmo::LOCATE_LOCAL);
        m_locationGizmo->SetLocation(manipSpace);

        switch(m_ActiveManipulator)
        {
        case NEManipulator::kManipTranslate:
            if (object->isManipulatable(kManipTranslate))
                drawTranslationManipulator(object, pick);
            break;
        case NEManipulator::kManipRotate:
            if (object->isManipulatable(kManipRotate))
                drawRotationManipulator(object, pick);
            break;
        case NEManipulator::kManipScale:
            if (object->isManipulatable(kManipScale))
                drawScaleManipulator(object, pick);
            break;
        case NEGLWidget::kSize:
            if (object->isManipulatable(kManipScale))
                drawSizeManipulator(object, pick);
            break;
        case NEGLWidget::kLocation:
            if (object->isManipulatable(kManipTranslate))
                drawLocationManipulator(object, pick);
            break;
        }
    }
    else
    {
        viewStack().pushIdentity();
        worldStack().pushIdentity();
        projectionStack().pushMatrix( osdOrthoMatrix );

        setModelViewMatrix();
        setProjectionMatrix();

        float viewMat_[16];
        float projMat_[16];

        GL->glGetFloatv ( GL_MODELVIEW_MATRIX, viewMat_ );
        GL->glGetFloatv ( GL_PROJECTION_MATRIX, projMat_ );

        m_moveGizmo->SetCameraMatrix( viewMat_, projMat_, true, 2 );
        m_scaleGizmo->SetCameraMatrix( viewMat_, projMat_, true, 2 );

        m_sizeGizmo->SetCameraMatrix( viewMat_, projMat_, true, 2 );
        m_locationGizmo->SetCameraMatrix( viewMat_, projMat_, true, 2 );

        IGizmo::LOCATION manipSpace = IGizmo::LOCATE_2D;
        m_moveGizmo->SetLocation(manipSpace);
        m_rotateGizmo->SetLocation(manipSpace);
        m_scaleGizmo->SetLocation(manipSpace);

        m_sizeGizmo->SetLocation(manipSpace);
        m_locationGizmo->SetLocation(manipSpace);
        if (m_ActiveManipulator == NEManipulator::kManipTranslate && object->isManipulatable(kManipTranslate))
            drawTranslationManipulator(object, pick);
        else if (m_ActiveManipulator == NEManipulator::kManipScale && object->isManipulatable(kManipScale))
            drawScaleManipulator(object, pick);

        viewStack().popMatrix();
        worldStack().popMatrix();
        projectionStack().popMatrix();

        setModelViewMatrix();
        setProjectionMatrix();
    }

    GL->glPopAttrib();
}

void NEGLWidget::drawTranslationManipulator( NESpatial* object, bool pick )
{
    m_gizmoTransform = object->transform();
    if (object->spatialType() == NESpatial::kOSD)
    {
        // Scale normalized OSD position to the screen space
        m_gizmoTransform.data()[12] *= width();
        m_gizmoTransform.data()[13] *= height();
    }
    float *pMatrix = const_cast<float *>( m_gizmoTransform.data() );
    float pivotPos[3] = { 0.0, 0.0, 0.0 };
    m_moveGizmo->SetEditMatrix( pMatrix, pivotPos );
    m_moveGizmo->Draw(shaderProgram()->programId());
}

void NEGLWidget::drawRotationManipulator(NESpatial* object, bool pick)
{
    //float *pMatrix = const_cast< float *>( object->transform().data() );
    m_gizmoTransform = object->transform();
    float *pMatrix = const_cast<float *>( m_gizmoTransform.data() );
    float pivotPos[3] = { object->Pivotx(), object->Pivoty(), object->Pivotz() };
    m_rotateGizmo->SetEditMatrix( pMatrix, pivotPos );
    m_rotateGizmo->Draw(shaderProgram()->programId());
}

void NEGLWidget::drawScaleManipulator(NESpatial* object, bool pick)
{
    m_gizmoTransform = object->transform();
    if (object->spatialType() == NESpatial::kOSD)
    {
        // Scale normalized OSD position to the screen space
        m_gizmoTransform.data()[12] *= width();
        m_gizmoTransform.data()[13] *= height();
    }
    float *pMatrix = const_cast<float *>( m_gizmoTransform.data() );
    float pivotPos[3] = { object->Pivotx(), object->Pivoty(), object->Pivotz() };
    m_scaleGizmo->SetEditMatrix( pMatrix, pivotPos );
    m_scaleGizmo->Draw(shaderProgram()->programId());
}

void NEGLWidget::drawSizeManipulator(NESpatial* object, bool pick)
{
    m_gizmoTransform = object->transform();
    if (object->spatialType() == NESpatial::kOSD)
    {
        // Scale normalized OSD position to the screen space
        m_gizmoTransform.data()[12] *= width();
        m_gizmoTransform.data()[13] *= height();
    }
    float *pMatrix = const_cast<float *>( m_gizmoTransform.data() );
    float pivotPos[3] = { object->Pivotx(), object->Pivoty(), object->Pivotz() };
    m_sizeGizmo->SetEditMatrix( pMatrix, pivotPos );
    m_sizeGizmo->Draw(shaderProgram()->programId());
}

void NEGLWidget::drawLocationManipulator(NESpatial* object, bool pick)
{
    m_gizmoTransform = object->transform();
    if (object->spatialType() == NESpatial::kOSD)
    {
        // Scale normalized OSD position to the screen space
        m_gizmoTransform.data()[12] *= width();
        m_gizmoTransform.data()[13] *= height();
    }
    float *pMatrix = const_cast<float *>( m_gizmoTransform.data() );
    float pivotPos[3] = { 0.0, 0.0, 0.0 };
    m_locationGizmo->SetEditMatrix( pMatrix, pivotPos );
    m_locationGizmo->Draw(shaderProgram()->programId());
}

void NEGLWidget::cacheUnitCube()
{
    GL->glBegin(GL_QUADS);
    GL->glNormal3f(0.0f, 1.0f, 0.0f);
    GL->glVertex3f( 1.0f, 1.0f,-1.0f);			// Top Right Of The Quad (Top)
    GL->glVertex3f(-1.0f, 1.0f,-1.0f);			// Top Left Of The Quad (Top)
    GL->glVertex3f(-1.0f, 1.0f, 1.0f);			// Bottom Left Of The Quad (Top)
    GL->glVertex3f( 1.0f, 1.0f, 1.0f);			// Bottom Right Of The Quad (Top)

    GL->glNormal3f(0.0f, -1.0, 0.0f);
    GL->glVertex3f( 1.0f,-1.0f, 1.0f);			// Top Right Of The Quad (Bottom)
    GL->glVertex3f(-1.0f,-1.0f, 1.0f);			// Top Left Of The Quad (Bottom)
    GL->glVertex3f(-1.0f,-1.0f,-1.0f);			// Bottom Left Of The Quad (Bottom)
    GL->glVertex3f( 1.0f,-1.0f,-1.0f);			// Bottom Right Of The Quad (Bottom)

    GL->glNormal3f(0.0f, 0.0, 1.0f);
    GL->glVertex3f( 1.0f, 1.0f, 1.0f);			// Top Right Of The Quad (Front)
    GL->glVertex3f(-1.0f, 1.0f, 1.0f);			// Top Left Of The Quad (Front)
    GL->glVertex3f(-1.0f,-1.0f, 1.0f);			// Bottom Left Of The Quad (Front)
    GL->glVertex3f( 1.0f,-1.0f, 1.0f);			// Bottom Right Of The Quad (Front)

    GL->glNormal3f(0.0f, 0.0, -1.0f);
    GL->glVertex3f( 1.0f,-1.0f,-1.0f);			// Bottom Left Of The Quad (Back)
    GL->glVertex3f(-1.0f,-1.0f,-1.0f);			// Bottom Right Of The Quad (Back)
    GL->glVertex3f(-1.0f, 1.0f,-1.0f);			// Top Right Of The Quad (Back)
    GL->glVertex3f( 1.0f, 1.0f,-1.0f);			// Top Left Of The Quad (Back)

    GL->glNormal3f(-1.0f, 0.0, 0.0f);
    GL->glVertex3f(-1.0f, 1.0f, 1.0f);			// Top Right Of The Quad (Left)
    GL->glVertex3f(-1.0f, 1.0f,-1.0f);			// Top Left Of The Quad (Left)
    GL->glVertex3f(-1.0f,-1.0f,-1.0f);			// Bottom Left Of The Quad (Left)
    GL->glVertex3f(-1.0f,-1.0f, 1.0f);			// Bottom Right Of The Quad (Left)

    GL->glNormal3f(1.0f, 0.0, 0.0f);
    GL->glVertex3f( 1.0f, 1.0f,-1.0f);			// Top Right Of The Quad (Right)
    GL->glVertex3f( 1.0f, 1.0f, 1.0f);			// Top Left Of The Quad (Right)
    GL->glVertex3f( 1.0f,-1.0f, 1.0f);			// Bottom Left Of The Quad (Right)
    GL->glVertex3f( 1.0f,-1.0f,-1.0f);			// Bottom Right Of The Quad (Right)
    GL->glEnd();
}

/*!
    Sets the image size of saved gl frames
*/

void NEGLWidget::saveGLFrameData()
{
    NEBASE.printEntryInfo("SceneManager","saveGLFrameData");
    QString glFramesDir = NESCENE.glFramesSaveDirectory();

    QString filename = glFramesDir + "/" + this->camera()->objectName() + "/";
    QDir().mkpath(filename);

    QString frameNr;
    frameNr.sprintf("%06d.png", NESCENE.CurrentFrame());
    filename += frameNr;
    //    NEGUI.saveGLFrame(filename);
    QImage image = grabFramebuffer();
    image.save(filename);
}


#include "Utility/NEAABB.h"
//
// Get the center of selection(s) and AABB of them
// If nothing is selected return the center of scene and AABB of the scene (all objects)
//
void NEGLWidget::getSelectedInfo( vec3f& selObjectPos, AABB& selObjectAABB )
{
    selObjectPos = vec3f( 0.0f, 0.0f, 0.0f );
    selObjectAABB.clear();

    // If nothing is selected, frame everything except lights and cameras
    if( NESCENE.selectedNode() == NULL )
    {
        int spatialsSizeNoLightsOrCam = NESCENE.spatials().size();

        foreach( NESpatial* object, NESCENE.spatials() )
        {
            if(object->factory().nodeType() == NE::kObjectLight ||
                    object->factory().nodeType() == NE::kObjectCamera)
            {
                selObjectAABB.insertPoint(vec3f(1,1,1));
                selObjectAABB.insertPoint(vec3f(-1,-1,-1));
                spatialsSizeNoLightsOrCam--;

                continue;
            }

            AABB aabb = object->transform() * object->aabb();
            selObjectAABB.insert( aabb );
            selObjectPos += (aabb.minPos() + aabb.maxPos()) / 2.0f;
        }
        selObjectPos /= spatialsSizeNoLightsOrCam;
    }
    // Otherwise, frame all selected objects
    else
    {
        NESpatial* selObject = static_cast<NESpatial *>( NESCENE.selectedNode() );
        selObjectAABB = selObject->transform() * selObject->aabb();
        selObjectPos = (selObjectAABB.minPos() + selObjectAABB.maxPos()) / 2.0f;

        // Frame the other selected objects
        if(NESCENE.selectedObjects()->size())
        {
            int selectedSize = 1;

            foreach(NEObject* object, *(NESCENE.selectedObjects()))
            {
                if(NESpatial* s = qobject_cast<NESpatial*>(object))
                {
                    AABB aabb = s->transform() * s->aabb();
                    selObjectAABB.insert( aabb );
                    selObjectPos += (aabb.minPos() + aabb.maxPos()) / 2.0f;
                    selectedSize++;
                }
            }
            selObjectPos /= selectedSize;
        }
    }
}

void NEGLWidget::frameSelectionOrtho()
{
    vec3f selObjectPos;
    AABB selObjectAABB;

    getSelectedInfo( selObjectPos, selObjectAABB );

    m_pCamera->SetPosition( selObjectPos );
    //
    // Make sure we set the new look at point
    //
    m_pCamera->SetLookAt( selObjectPos );

    vec3f scaleVector = selObjectAABB.maxPos() - selObjectAABB.minPos();
    float scaleF = scaleVector.x();
    if (scaleF < scaleVector.y())
        scaleF = scaleVector.y();
    if (scaleF < scaleVector.z())
        scaleF = scaleVector.z();
    float OrthoWidth = scaleF;
    m_pCamera->SetOrthoWidth(OrthoWidth);
    resizeGL(width(), height());
    update();
    //Log()<<"frameSelection" ;EndLog(NE::kLogInfo);
}

void NEGLWidget::frameSelection()
{
    if (m_pCamera->fixedOrthographic())
    {
        frameSelectionOrtho();
        return;
    }
    vec3f selObjectPos;
    AABB selObjectAABB;

    getSelectedInfo( selObjectPos, selObjectAABB );
    vec3f centerSel = selObjectPos;

    //
    // First transform the point in camera space and null out its z so that its in the camera plane
    //
    matrix44f camMat = m_pCamera->viewMatrix();
    centerSel = camMat * centerSel;

    centerSel.z() = 0;

    // Transform that camera plane point back to world;

    centerSel = camMat.inverted() * centerSel;

    //
    // Now check to see how far the camera needs to be moved so that the object(s) bounding box is fully on frame
    //
    float distance = 0;

    {
        //
        // Calculate distance to the selection
        //
        float Fov = m_pCamera->Fov();
        float tangent = tan( Fov/180.0f * M_PI ) * 0.5;
        distance = ((selObjectAABB.maxPos() - selObjectAABB.minPos()).length()/tangent);

        //
        // Move that point by the delta of the current distance and the new distance along LookAt
        //
        vec3f LookAt = selObjectPos - centerSel;
        LookAt.normalize();
        float curDist = ( centerSel - selObjectPos ).length();
        distance = curDist - distance;
        centerSel += LookAt * distance;
    }

    //
    // Move the camera to that position
    //
    m_pCamera->SetPosition(centerSel);

    //
    // Move changes orientation to point to the old LookAt position
    // so readjust the LookAt to point to the current
    //
    m_pCamera->SetLookAt( selObjectPos );

    update();
}
