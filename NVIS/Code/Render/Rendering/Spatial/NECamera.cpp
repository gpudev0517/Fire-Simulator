#include "Rendering/Spatial/NECamera.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"

#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"

#include "Base/NEBase.h"
#include "Base/NEGUICallInterface.h"
#include "NECamera_p.h"



NECameraFactory::NECameraFactory(NEManager* m) : NENodeFactory(m)
{
    if( NEBASE.guiMode() == false )
        return;

    m_pMesh = new NEIndexedTriMesh();
    //m_pMesh->loadFromFile(":Resources/meshes/camera.obj");
    m_pMesh->loadFromFile("System/Meshes/camera.obj");
    //m_pMesh->createDrawingBuffers(GL_DYNAMIC_DRAW);
    NERENDERMAN.GLManager()->createDrawingBuffers(m_pMesh, GL_DYNAMIC_DRAW);

    NEIcon icon;
    icon.iconPath = QString(":/Neutrino/Resources/16/cam16.png");
    icon.iconSize = QSize(16, 16);
    nodeIcon()->push_back(icon);

    icon.iconPath = QString("::/Neutrino/Resources/22/cam22.png");
    icon.iconSize = QSize(22, 22);
    nodeIcon()->push_back(icon);

    icon.iconPath = QString("::/Neutrino/Resources/32/cam32.png");
    icon.iconSize = QSize(32, 32);
    nodeIcon()->push_back(icon);

//    nodeIcon()->addFile(":/Neutrino/Resources/16/cam16.png", QSize(16, 16));
//    nodeIcon()->addFile(":/Neutrino/Resources/22/cam22.png", QSize(22, 22));
//    nodeIcon()->addFile(":/Neutrino/Resources/32/cam32.png", QSize(32, 32));
}

NE_PROP_DEF(double, NECamera, Fov, 45.0)
NE_PROP_DEF(double, NECamera, NearClip, 0.1)
NE_PROP_DEF(double, NECamera, FarClip, 5000.0)
NE_PROP_DEF_VEC3(vec3f, float, NECamera, LookAt, vec3f(0, 0, 0), P)
NE_PROP_DEF(double, NECamera, OrthoWidth, 5.0)
NE_PROP_DEF(bool, NECamera, FlipSide, false)
NE_PROP_DEF(bool, NECamera, FlipUp, false)
NE_PROP_DEF(bool, NECamera, FlipLook, false)
NE_PROP_DEF(bool, NECamera, Ortho, false)
NE_PROP_DEF(NECamera::OrthographicType, NECamera, OrthoType, NECamera::OrthographicType::Custom)

static void initProperties()
{
    NE_INIT_PROP(NECamera, Fov);
    NE_INIT_PROP(NECamera, NearClip);
    NE_INIT_PROP(NECamera, FarClip);
    NE_INIT_PROP(NECamera, LookAt);
    NE_INIT_PROP(NECamera, OrthoWidth);
    NE_INIT_PROP(NECamera, FlipSide);
    NE_INIT_PROP(NECamera, FlipUp);
    NE_INIT_PROP(NECamera, FlipLook);
    NE_INIT_PROP(NECamera, Ortho);
    NE_INIT_PROP(NECamera, OrthoType);
}

NECameraFactory::~NECameraFactory()
{
    if( NEBASE.guiMode() == 0 )
        return;

    m_pMesh->clearDrawingBuffers();
    delete m_pMesh;
}

QString NECameraFactory::nodeName() { return "Camera"; }

uint NECameraFactory::nodeType(){ return NE::kObjectCamera;}

QString NECameraFactory::nodeInherits() { return "Cameras"; }

QString NECameraFactory::objectName() { return "Camera"; }

QString NECameraFactory::nodeVersion() { return "0.1"; }

QString NECameraFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NECameraFactory::nodeHelp() { return "Basic camera"; }

NEObject*NECameraFactory::createInstance() {return new NECamera(this); }

const NEIndexedTriMesh*NECameraFactory::mesh() { return m_pMesh; }

NECameraPrivate::NECameraPrivate(NECamera* s) : S(s),
    m_OrthoWidth(NE_DEFAULT(OrthoWidth)),
    m_Fov(NE_DEFAULT(Fov)),
    m_NearClip(NE_DEFAULT(NearClip)),
    m_FarClip(NE_DEFAULT(FarClip)),
    m_LookAt(NE_DEFAULT(LookAt)),
    m_IsViewer(false),
    m_FlipSide(NE_DEFAULT(FlipSide)),
    m_FlipUp(NE_DEFAULT(FlipUp)),
    m_FlipLook(NE_DEFAULT(FlipLook)),
    m_Ortho(NE_DEFAULT(Ortho)),
    m_OrthoType(NE_DEFAULT(OrthoType))
{
    m_isFlipped = false;
}

NECamera::NECamera(NENodeFactory* factory) : NESpatial(factory)
{
    addNodeTypeToNodeGraphSceneType("Cameras", NENode::SceneType::Visualization );

    P = new NECameraPrivate(this);
    CALL_ONCE(initProperties);
    SetPivot(vec3f(0.0f, 0.0f, 0.0f));
    SetScale(vec3f(1.5f, 1.5f, 1.5f));
    //SetPosition(vec3f(10.0f, 10.0f, 10.0f));

    SetLookAt(P->m_LookAt);
    SetFov(P->m_Fov);
    SetExternalRender(true);

    setHidden(true);
}

uint NECamera::init(uint initMode)
{
    uint ret = NESpatial::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    P->m_Mesh = const_cast<NEIndexedTriMesh*>(((NECameraFactory&)factory()).mesh());

    P->m_Look = (P->m_LookAt-Position()).normalized();

    if (P->m_FlipSide)
        P->m_Side = -P->m_Side;
    if (P->m_FlipUp)
        P->m_Up = -P->m_Up;
    if (P->m_FlipLook)
        P->m_Look = -P->m_Look;

    if( NEBASE.guiMode() )
        NEBASE.getGUICallInterface()->resizeGLWindows();

    float viewport[4];
    GL->glGetFloatv(GL_VIEWPORT, viewport);
    P->computeProjection(viewport[2], viewport[3]);
    P->computeViewer();
    P->updateViewer();

    hideProperty("Fov", fixedOrthographic());
    hideProperty("LookAt", !perspective());
    hideProperty("Up", !perspective());
    hideProperty("Side", !perspective());
    hideProperty("OrthoWidth", !fixedOrthographic());
    hideProperty("FlipSide", !fixedOrthographic());
    hideProperty("FlipUp", !fixedOrthographic());
    hideProperty("FlipLook", !fixedOrthographic());

    //initializeGUIActions();
    return NE::kReturnSuccess;
}

void NECamera::resetCamera()
{
    if (customOrthographic())
        resetCustomOrtho();
    else if (fixedOrthographic())
        resetFixedOrtho();
    else
        resetPersp();
}

NECamera::~NECamera()
{
    delete P;
}

void NECamera::resetPersp()
{
    SetPosition(vec3f(10, 10, 10));
    vec3f lookAt = NE_DEFAULT(LookAt);
    double fov = NE_DEFAULT(Fov);
    double nearClip = NE_DEFAULT(NearClip);
    double farClip = NE_DEFAULT(FarClip);

    SetLookAt(lookAt);
    SetFov(fov);
    SetNearClip(nearClip);
    SetFarClip(farClip);

    if( NEBASE.guiMode() )
        NEBASE.getGUICallInterface()->updateGLWindowsFromBase();


}

void NECamera::resetCustomOrtho()
{
    SetPosition(vec3f(10, 10, 10));
    P->m_LookAt = NE_DEFAULT(LookAt);
    P->m_Fov = NE_DEFAULT(Fov);
    P->m_NearClip = NE_DEFAULT(NearClip);
    P->m_FarClip = NE_DEFAULT(FarClip);

    SetLookAt(P->m_LookAt);
    SetFov(P->m_Fov);

    if( NEBASE.guiMode() )
        NEBASE.getGUICallInterface()->updateGLWindowsFromBase();
}

void NECamera::resetFixedOrtho()
{
    SetFarClip(5000);
    SetNearClip(-5000);
    P->m_OrthoWidth = 5.0f;

    if(OrthoType() == NECamera::OrthographicType::Top)
    {
        P->m_Up = vec3f(0, 0, -1);
        P->m_Side = vec3f(1, 0, 0);
        P->m_Look = vec3f(0, -1, 0);
    }
    else if(OrthoType() == NECamera::OrthographicType::Bottom)
    {
        P->m_Up = vec3f(0, 0, -1);
        P->m_Side = vec3f(1, 0, 0);
        P->m_Look = vec3f(0, -1, 0);
        SetFlipLook( true );
    }
    else if(OrthoType() == NECamera::OrthographicType::Left)
    {
        P->m_Up = vec3f(0, 1, 0);
        P->m_Side = vec3f(0, 0, 1);
        P->m_Look = vec3f(1, 0, 0);
    }
    else if(OrthoType() == NECamera::OrthographicType::Right)
    {
        P->m_Up = vec3f(0, 1, 0);
        P->m_Side = vec3f(0, 0, 1);
        P->m_Look = vec3f(1, 0, 0);
        SetFlipLook( true );
    }
    else if(OrthoType() == NECamera::OrthographicType::Front)
    {
        P->m_Up = vec3f(0, 1, 0);
        P->m_Side = vec3f(1, 0, 0);
        P->m_Look = vec3f(0, 0, -1);
    }
    else if(OrthoType() == NECamera::OrthographicType::Back)
    {
        P->m_Up = vec3f(0, 1, 0);
        P->m_Side = vec3f(1, 0, 0);
        P->m_Look = vec3f(0, 0, -1);
        SetFlipLook( true );
    }

    if (OrthoType() != NECamera::OrthographicType::Custom)
    {
        P->m_LookAt = vec3f(0, 0, 0);
        NESpatial::SetPosition(P->m_Look * -10);
    }

    P->updateViewer();

    if( NEBASE.guiMode() )
        NEBASE.getGUICallInterface()->updateGLWindowsFromBase();
}

const double& NECamera::Fov() const
{
    return P->m_Fov;
}

uint NECamera::SetFov(const double& fov)
{
    NE_PROP_SET_EVAL(Fov, P, fov);
    //P->computeProjection(NERENDERMAN.GLManager()->glWidth(), NERENDERMAN.GLManager()->glHeight());
    //gustaw temp
    float viewport[4];
    GL->glGetFloatv(GL_VIEWPORT, viewport);
    P->computeProjection(viewport[2], viewport[3]);

    return NE::kReturnSuccess;
}

const double& NECamera::NearClip() const
{
    return P->m_NearClip;
}

uint NECamera::SetNearClip(const double& nearClip)
{
    NE_PROP_SET_EVAL(NearClip, P, nearClip);
    //P->computeProjection(NERENDERMAN.GLManager()->glWidth(), NERENDERMAN.GLManager()->glHeight());
    //gustaw temp
    float viewport[4];
    GL->glGetFloatv(GL_VIEWPORT, viewport);
    P->computeProjection(viewport[2], viewport[3]);

    return NE::kReturnSuccess;
}

const double& NECamera::FarClip() const
{
    return P->m_FarClip;
}

uint NECamera::SetFarClip(const double& farClip)
{
    NE_PROP_SET_EVAL(FarClip, P, farClip);
    //P->computeProjection(NERENDERMAN.GLManager()->glWidth(), NERENDERMAN.GLManager()->glHeight());
    //gustaw temp
    float viewport[4];
    GL->glGetFloatv(GL_VIEWPORT, viewport);
    P->computeProjection(viewport[2], viewport[3]);

    return NE::kReturnSuccess;
}

void NECamera::render()
{

}

void NECamera::renderPickable()
{

}

void NECamera::renderPicked()
{

}

const NEIndexedTriMesh* NECamera::mesh() const
{
    return P->m_Mesh;
}

void NECamera::SetMesh(NEIndexedTriMesh* mesh)
{
    P->m_Mesh = mesh;
}

void NECamera::computeProjection(int width, int height)
{
    P->computeProjection( width, height );
}

const vec3f& NECamera::LookAt() const
{
    return P->m_LookAt;
}

uint NECamera::SetLookAt(const vec3f& val)
{
    // Prevent too strong zooms-in, which might lead
    // to a null look vector for the view transform
    vec3f look = val-Position();
//    if (look.length() < 0.001)
//        return NE::kReturnNotChanged;

    // Prevent rotations that might lead to a look vector
    // too parallel to the y-axis, which would lead
    // to a null side vector for the view transform
    look = look.normalized();
    vec3f up = vec3f(0.0f, 1.0f, 0.0f);
    if (vec3f::areEqual(look, up, 0.1f) || vec3f::areEqual(look, -up, 0.1f))
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL_VEC3(LookAt, P, val);
    P->m_Look = (P->m_LookAt-Position()).normalized();
    P->computeViewer();

    return NE::kReturnSuccess;
}

const matrix44f& NECamera::projectionMatrix() const
{
    return P->m_Projection;
}

const vec3f& NECamera::Side() const
{
    return P->m_Side;
}

const bool& NECamera::FlipSide() const
{
    return P->m_FlipSide;
}

const bool& NECamera::FlipUp() const
{
    return P->m_FlipUp;
}

const bool& NECamera::FlipLook() const
{
    return P->m_FlipLook;
}

bool NECamera::fixedOrthographic() const
{
    return P->m_Ortho && P->m_OrthoType != NECamera::OrthographicType::Custom;
}

bool NECamera::customOrthographic() const
{
    return P->m_Ortho && P->m_OrthoType == NECamera::OrthographicType::Custom;
}

bool NECamera::perspective() const
{
    return !fixedOrthographic() && !customOrthographic();
}

const matrix44f& NECamera::viewMatrix() const
{
    return P->m_ViewTransform;
}

void NECameraPrivate::computeProjection(int width/*=0*/, int height/*=0*/)
{
    m_Projection.setToIdentity();

    float aspect;

    if(width == 0 || height == 0)
        aspect = 1.0;
    else
        aspect = (float)width / (float)height;

    if(S->fixedOrthographic())
    {
        m_Projection.ortho(-m_OrthoWidth, m_OrthoWidth, -m_OrthoWidth / aspect, m_OrthoWidth / aspect, m_NearClip, m_FarClip);
    }
    else if(S->customOrthographic())
    {
        double top = tan(DEGREE_TO_RADIAN(m_Fov) * 0.5) * m_OrthoWidth * 20.0;
        double bottom = -top;
        double left = (1.0 / aspect) * bottom;
        double right = (1.0 / aspect) * top;
        m_Projection.ortho(bottom, top, left, right, S->NearClip(), S->FarClip());
        //m_Projection.perspective(m_Fov, aspect, m_NearClip, m_FarClip);
    }
    else
    {
        double top = tan(DEGREE_TO_RADIAN(m_Fov) * 0.5) * S->NearClip();
        double bottom = -top;
        double left = (1.0 / aspect) * bottom;
        double right = (1.0 / aspect) * top;
        m_Projection.frustum(bottom, top, left, right, S->NearClip(), S->FarClip());
    }
}

uint NECamera::SetPosition(const vec3f& position)
{
    uint ret = NESpatial::SetPosition(position);
    if (ret != NE::kReturnSuccess)
        return ret;

    P->m_Look = (P->m_LookAt-Position()).normalized();
    P->computeViewer();

    return NE::kReturnSuccess;
}

uint NECamera::SetPositionZoom(const vec3f& val)
{
    vec3f unitLook = (P->m_LookAt-Position()).normalized();

    uint ret = NESpatial::SetPosition(val);
    if (ret != NE::kReturnSuccess)
        return ret;

    // Recompute a look-at vector so as to prevent a look
    // vector pointing in the opposite direction when
    // zooming in too strongly
    P->m_LookAt = Position()+unitLook;
    P->m_Look = (P->m_LookAt-Position()).normalized();
    P->computeViewer();

    return NE::kReturnSuccess;
}

uint NECamera::Translate(const vec3f& val)
{
    uint ret = NESpatial::SetPosition(Position() + val);
    if (ret != NE::kReturnSuccess)
        return ret;

    NE_PROP_SET_AND_EVAL_VEC3(LookAt, P, LookAt() + val);
    P->computeViewer();

    return NE::kReturnSuccess;
}

void NECameraPrivate::computeViewer()
{
    m_ViewTransform.setToIdentity();
    computeLookAtMatrix();
    S->NESpatial::SetOrientation((eulerAnglesFromDirection((m_LookAt - S->Position()).normalized())));
    S->SetOrientation(eulerAnglesFromMatrix(m_ViewRotation));
}

void NECameraPrivate::updateViewer()
{
    m_ViewTransform.setToIdentity();
    updateLookAtMatrix();
    S->NESpatial::SetOrientation((eulerAnglesFromDirection((m_LookAt - S->Position()).normalized())));
    S->SetOrientation(eulerAnglesFromMatrix(m_ViewRotation));
}

void NECamera::setViewer(bool flag)
{
    P->m_IsViewer = flag;
}

const matrix33f& NECamera::pureRotation() const
{
    return P->m_ViewRotation;
}

const vec3f& NECamera::up() const
{
    return P->m_Up;
}

const vec3f& NECamera::side() const
{
    return P->m_Side;
}

const vec3f& NECamera::look() const
{
    return P->m_Look;
}

bool NECamera::isViewer() const
{
    return P->m_IsViewer;
}

const vec3f& NECamera::Up() const
{
    return P->m_Up;
}

void NECameraPrivate::computeLookAtMatrix()
{
    m_Look = (m_LookAt-S->Position()).normalized();

    if (S->fixedOrthographic())
    {
        updateLookAtMatrix();
        return;
    }

    float pi = 3.1415927;

    // horizontal angle
    float theta = atan2(m_Look.z(), m_Look.x());
    // vertical angle
    float phi = asin(m_Look.y());

    // for singular
    if (cos(phi) < 0.2)
    {
        if (abs(abs(theta - m_lastTheta) - pi) < 0.1)
        {
            m_isFlipped = !m_isFlipped;
        }
    }
    m_lastTheta = theta;

    // get up vector based on theta and phi
    float phi1 = phi + pi / 2;
    if (m_isFlipped)
    {
        phi1 = -phi1;
        theta = theta + pi;
    }

    m_Up = vec3f(cos(phi1)*cos(theta),sin(phi1),cos(phi1)*sin(theta));
    m_Side = vec3f::crossProduct(m_Look,m_Up).normalized();

    updateLookAtMatrix();
}


void NECameraPrivate::updateLookAtMatrix()
{
    matrix33f& m3x3 = m_ViewRotation;

    m3x3(0, 0) = m_Side.x();
    m3x3(1, 0) = m_Side.y();
    m3x3(2, 0) = m_Side.z();

    m3x3(0, 1) = m_Up.x();
    m3x3(1, 1) = m_Up.y();
    m3x3(2, 1) = m_Up.z();

    m3x3(0, 2) = -m_Look.x();
    m3x3(1, 2) = -m_Look.y();
    m3x3(2, 2) = -m_Look.z();

    matrix44f& m = m_ViewTransform;

    m(0, 0) = m3x3(0, 0);
    m(1, 0) = m3x3(0, 1);
    m(2, 0) = m3x3(0, 2);
    m(0, 1) = m3x3(1, 0);
    m(1, 1) = m3x3(1, 1);
    m(2, 1) = m3x3(1, 2);
    m(0, 2) = m3x3(2, 0);
    m(1, 2) = m3x3(2, 1);
    m(2, 2) = m3x3(2, 2);


    m(3, 0) = 0.0f;
    m(3, 1) = 0.0f;
    m(3, 3) = 1.0f;
    m(0, 3) = 0.0f;
    m(1, 3) = 0.0f;
    m(2, 3) = 0.0f;
    m(3, 2) = 0.0f;


    m_ViewTransform.translate(-S->Position());

}

const vec3f& NECamera::lookNormalized() const
{
    return P->m_Look;
}

void NECamera::addKeyframe(uint currentFrame, const QStringList& propertyNames, NE::NECurveType type)
{
    QStringList modList = propertyNames;
    if(propertyNames.indexOf("Position") > -1)
        modList += "SideVector";


    NENode::addKeyframe(currentFrame, modList, type);

}

const double& NECamera::OrthoWidth() const
{
    return P->m_OrthoWidth;
}

const vec3f& NECamera::Scale() const
{
    return NESpatial::Scale();
}

const vec3f& NECamera::Pivot() const
{
    return NESpatial::Pivot();
}

uint NECamera::SetOrthoWidth(const double &val)
{
    NE_PROP_SET_EVAL(OrthoWidth, P, val);

    return NE::kReturnSuccess;
}

uint NECamera::SetFlipSide(const bool& arg)
{
    NE_PROP_SET_EVAL(FlipSide, P, arg);
    P->m_Side = -P->m_Side;
    P->updateViewer();

    return NE::kReturnSuccess;
}

uint NECamera::SetFlipUp(const bool& arg)
{
    NE_PROP_SET_EVAL(FlipUp, P, arg);
    P->m_Up = -P->m_Up;
    P->updateViewer();

    return NE::kReturnSuccess;
}

uint NECamera::SetFlipLook(const bool& arg)
{
    NE_PROP_SET_EVAL(FlipLook, P, arg);

    if(OrthoType() == NECamera::OrthographicType::Top || NECamera::OrthographicType::Bottom)
        NESpatial::SetPosition(vec3f(Position().x(), -Position().y(), Position().z()));
    else if(OrthoType() == NECamera::OrthographicType::Left || NECamera::OrthographicType::Right)
        NESpatial::SetPosition(vec3f(-Position().x(), Position().y(), Position().z()));
    else if(OrthoType() == NECamera::OrthographicType::Front || NECamera::OrthographicType::Back)
        NESpatial::SetPosition(vec3f(Position().x(), Position().y(), -Position().z()));

    P->m_Look = -P->m_Look;
    P->updateViewer();

    return NE::kReturnSuccess;
}

const bool& NECamera::Ortho() const
{
    return P->m_Ortho;
}

uint NECamera::SetOrtho(const bool& ortho)
{
    NE_PROP_SET_EVAL(Ortho, P, ortho);

    if (fixedOrthographic())
        setOrthoValues();

    hideProperty("Fov", fixedOrthographic());
    hideProperty("LookAt", !perspective());
    hideProperty("Up", !perspective());
    hideProperty("Side", !perspective());
    hideProperty("OrthoWidth", !fixedOrthographic());
    hideProperty("FlipSide", !fixedOrthographic());
    hideProperty("FlipUp", !fixedOrthographic());
    hideProperty("FlipLook", !fixedOrthographic());

    return NE::kReturnSuccess;
}

const NECamera::OrthographicType& NECamera::OrthoType() const
{
    return P->m_OrthoType;
}

uint NECamera::SetOrthoType(const OrthographicType& type)
{
    NE_PROP_SET_EVAL(OrthoType, P, type);

    if (fixedOrthographic())
        setOrthoValues();

    hideProperty("Fov", fixedOrthographic());
    hideProperty("LookAt", !perspective());
    hideProperty("Up", !perspective());
    hideProperty("Side", !perspective());
    hideProperty("OrthoWidth", !fixedOrthographic());
    hideProperty("FlipSide", !fixedOrthographic());
    hideProperty("FlipUp", !fixedOrthographic());
    hideProperty("FlipLook", !fixedOrthographic());

    return NE::kReturnSuccess;
}

uint NECamera::setOrthoValues()
{
    if (customOrthographic())
    {
        resetCustomOrtho();

        if (NEBASE.guiMode())
            NEBASE.getGUICallInterface()->resizeGLWindows();
    }
    else if (fixedOrthographic())
        resetFixedOrtho();
    else
    {
        resetPersp();

        if (NEBASE.guiMode())
            NEBASE.getGUICallInterface()->resizeGLWindows();
    }

    return NE::kReturnSuccess;
}

void NECamera::propertyTweak(QtPropertyBase* p)
{
    lockProperty("Orientation", true);
    lockProperty("Up", true);
    lockProperty("Side", true);
    NESpatial::propertyTweak(p);
}

/*!
*/
void NECamera::initializeGUIActions()
{
    NESpatial::initializeGUIActions();

    QAction* tmpAction = new QAction("Reset camera", NULL);

    //deInitializeGUIActions();
    if(fixedOrthographic())
    {
        connect(tmpAction, &QAction::triggered, this, &NECamera::resetFixedOrtho);
    }
    else if(customOrthographic())
    {
        connect(tmpAction, &QAction::triggered, this, &NECamera::resetCustomOrtho);
    }
    else
    {
        connect(tmpAction, &QAction::triggered, this, &NECamera::resetPersp);
    }
    addAction(tmpAction);
}

