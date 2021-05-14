#include "Rendering/Spatial/Light/NESpotLight.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Base/NEBase.h"

#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEIGLSurface.h"


#include "Rendering/Spatial/NECamera.h"

class NESpotLightPrivate
{
public:
    NESpotLightPrivate(NESpotLight* s) : S(S)
    {

    }

    vec3f m_LookAt;
    // vec3f m_NormalizedLookAt;
    vec3f m_Direction;
    vec3f m_NormalizedDirection;
    double m_Cutoff;
    double m_Exponent;
    double m_LinearAttenuation;
    double m_QuadraticAttenuation;

    // Povray related
    double m_PovrayRadius;
    int m_PovrayAdaptive;
    bool m_PovrayJitter;

    // PBRT related
    vec3f m_PbrtColorI;

    double m_SoftAngle;
    double m_BeamDistribution;

    NEIndexedTriMesh* m_Mesh;
private:
    NESpotLight* S;

};

NE_PROP_DEF_VEC3(vec3f, float, NESpotLight, LookAt, vec3f(0, 0, 0), P)
NE_PROP_DEF(double, NESpotLight, Cutoff, 30.0f)
NE_PROP_DEF(double, NESpotLight, Exponent, 0.0f)
NE_PROP_DEF(double, NESpotLight, LinearAttenuation, 0.0f)
NE_PROP_DEF(double, NESpotLight, QuadraticAttenuation, 0.0f)
NE_PROP_DEF(int, NESpotLight, PovrayRadius, 15.0f)
NE_PROP_DEF(int, NESpotLight, PovrayAdaptive, 2)
NE_PROP_DEF(bool, NESpotLight, PovrayJitter, true)

NE_PROP_DEF_VEC3(vec3f, float, NESpotLight, PbrtColorI, vec3f(1000, 1000, 1000), P)
NE_PROP_DEF(double, NESpotLight, SoftAngle, 5.0)
NE_PROP_DEF(double, NESpotLight, BeamDistribution, 2.0)


static void initProperties()
{
    NE_INIT_PROP(NESpotLight, LookAt);
    NE_INIT_PROP(NESpotLight, Cutoff);
    NE_INIT_PROP(NESpotLight, Exponent);
    NE_INIT_PROP(NESpotLight, LinearAttenuation);
    NE_INIT_PROP(NESpotLight, QuadraticAttenuation);
    NE_INIT_PROP(NESpotLight, PovrayRadius);
    NE_INIT_PROP(NESpotLight, PovrayAdaptive);
    NE_INIT_PROP(NESpotLight, PovrayJitter);

    NE_INIT_PROP(NESpotLight, PbrtColorI);
    NE_INIT_PROP(NESpotLight, SoftAngle);
    NE_INIT_PROP(NESpotLight, BeamDistribution);
}


NESpotLightFactory::NESpotLightFactory(NEManager* m) : NENodeFactory(m)
{
    if( NEBASE.guiMode() == false )
        return;

    m_pMesh = new NEIndexedTriMesh();
    m_pMesh->loadFromFile("System/Meshes/spotLight.obj");
    //m_pMesh->createDrawingBuffers(GL_STATIC_DRAW);
    NERENDERMAN.GLManager()->createDrawingBuffers(m_pMesh, GL_STATIC_DRAW);

    NEIcon icon;
    icon.iconPath = QString(":/Neutrino/Resources/16/spotLight16.png");
    icon.iconSize = QSize(16, 16);
    nodeIcon()->push_back(icon);

    icon.iconPath = QString("::/Neutrino/Resources/22/spotLight22.png");
    icon.iconSize = QSize(22, 22);
    nodeIcon()->push_back(icon);

    icon.iconPath = QString("::/Neutrino/Resources/32/spotLight32.png");
    icon.iconSize = QSize(32, 32);
    nodeIcon()->push_back(icon);

//    nodeIcon()->addFile(":/Neutrino/Resources/16/spotLight16.png", QSize(16, 16));
//    nodeIcon()->addFile(":/Neutrino/Resources/22/spotLight22.png", QSize(22, 22));
//    nodeIcon()->addFile(":/Neutrino/Resources/32/spotLight32.png", QSize(32, 32));
}

NESpotLightFactory::~NESpotLightFactory()
{
    if( NEBASE.guiMode() == false )
        return;

    m_pMesh->clearDrawingBuffers();
    delete m_pMesh;
}

QString NESpotLightFactory::nodeName()
{
    return "Spot Light";
}

uint NESpotLightFactory::nodeType()
{
    return NE::kObjectLight;
}

QString NESpotLightFactory::nodeInherits()
{
    return "Lights";
}

QString NESpotLightFactory::objectName()
{
    return "SpotLight";
}

QString NESpotLightFactory::nodeVersion()
{
    return "0.1";
}

QString NESpotLightFactory::nodeVendor()
{
    return "Neutrino Dynamics";
}

QString NESpotLightFactory::nodeHelp()
{
    return "Spot light";
}

NEObject* NESpotLightFactory::createInstance()
{
    return new NESpotLight(this);
}

const NEIndexedTriMesh* NESpotLightFactory::mesh()
{
    return m_pMesh;
}

NESpotLight::NESpotLight(NENodeFactory* factory) : NELight(factory)
{
    P = new NESpotLightPrivate(this);
    CALL_ONCE(initProperties);
    SetPosition(vec3f(-3, 3, 3));
    updateWorldTransform();

    SetLookAt(NE_DEFAULT(LookAt));
    P->m_Cutoff = NE_DEFAULT(Cutoff);
    P->m_Exponent = NE_DEFAULT(Exponent);

    P->m_LinearAttenuation = NE_DEFAULT(LinearAttenuation);
    P->m_QuadraticAttenuation = NE_DEFAULT(QuadraticAttenuation);

    P->m_PovrayRadius = NE_DEFAULT(PovrayRadius);
    P->m_PovrayAdaptive = NE_DEFAULT(PovrayAdaptive);
    P->m_PovrayJitter = NE_DEFAULT(PovrayJitter);

    P->m_PbrtColorI = NE_DEFAULT(PbrtColorI);

    P->m_SoftAngle = NE_DEFAULT(SoftAngle);
    P->m_BeamDistribution = NE_DEFAULT(BeamDistribution);
}

NESpotLight::~NESpotLight()
{
    delete P;
}

uint NESpotLight::init(uint initMode)
{
    NESpatial::init(initMode);
    P->m_Mesh = const_cast<NEIndexedTriMesh*>(((NESpotLightFactory&)factory()).mesh());
    return NE::kReturnSuccess;
}

void NESpotLight::render()
{
}

void NESpotLight::renderPicked()
{
}

void NESpotLight::renderPickable()
{
}

NEIndexedTriMesh* NESpotLight::mesh()
{
    return P->m_Mesh;
}

const vec3f& NESpotLight::LookAt() const
{
    return P->m_LookAt;
}


void NESpotLight::updateUniforms(IGLSurface* surface, QOpenGLShaderProgram* program, int lightID, bool isActive)
{
    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].isactive").toUtf8().constData(), isActive);
    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].color").toUtf8().constData(), LightColor());
    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].intensity").toUtf8().constData(), (float) LightIntensity());

    // Map light position from world space to eye space before passing to the shader
    vec3f posMapped = NERENDERMAN.GLManager()->viewMatrix() * Position();
    QVector3D* posQVec = reinterpret_cast<QVector3D*>(&posMapped);

    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].pos").toUtf8().constData(), *posQVec);
    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].attLin").toUtf8().constData(),
                             (GLfloat)P->m_LinearAttenuation);
    program->setUniformValue((const char*)(QString("SpotLights[" + QString::number(lightID) + "].attQuad").toUtf8()), (GLfloat)P->m_QuadraticAttenuation);

    // Map light direction from world space to eye space before passing to the shader
    matrix44f cm = surface->camera()->viewMatrix();
    vec3f dirMapped = cm.mapVector(P->m_Direction);
    dirMapped.normalize();

    QVector3D* dirQVec = reinterpret_cast<QVector3D*>(&dirMapped);

    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].dir").toUtf8().constData(),
                             *dirQVec);
    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].cosCutoff").toUtf8().constData(),
                             cosf((float)DEGREE_TO_RADIAN(P->m_Cutoff / 2)));
    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].exponent").toUtf8().constData(),
                             (GLfloat)P->m_Exponent);
    // Texture 0 and 1 for standard material
    int totalId = MAX_MATERIAL_TEXTURES + NERENDERMAN.GLManager()->getDirectionLights().size()+lightID;
    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].shadowMap").toUtf8().constData(), totalId);
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( program->programId(), QString("SpotLights[" + QString::number(lightID) + "].DepthBiasMVP").toUtf8().constData()),
                        1, GL_FALSE, getDepthBiasMVP().data() );
    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].castingShadow").toUtf8().constData(), (int)ShadowMode());
    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].shadowColor").toUtf8().constData(), this->ShadowColor() );

    GL->glUniformMatrix4fv( GL->glGetUniformLocation( program->programId(), QString("SpotLights[" + QString::number(lightID) + "].lightView").toUtf8().constData()),
                        1, GL_FALSE, getViewMatrix().data() );
    GL->glUniform2f(GL->glGetUniformLocation( program->programId(), QString("SpotLights[" + QString::number(lightID) + "].lightRadiusUV").toUtf8().constData()),getLightRadiusUV().x(),getLightRadiusUV().y());
    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].lightZNear").toUtf8().constData(), getNear());
    program->setUniformValue(QString("SpotLights[" + QString::number(lightID) + "].lightZFar").toUtf8().constData(), getFar());

    GL->glActiveTexture(GL_TEXTURE0 + totalId);
    bindShadowTexture();
}

uint NESpotLight::SetLookAt(const vec3f& val)
{
    P->m_LookAt = val;
    P->m_Direction = P->m_LookAt - Position();
    P->m_NormalizedDirection = P->m_Direction.normalized();
    NESpatial::SetOrientation(eulerAnglesFromDirection(-P->m_NormalizedDirection));
    return NE::kReturnSuccess;
}


uint NESpotLight::SetOrientation(const eulerf& rotation)
{
    NESpatial::SetOrientation(rotation);
    P->m_NormalizedDirection = -directionFromEulerAngles(Orientation());
    P->m_Direction = P->m_Direction.length()*P->m_NormalizedDirection;
    P->m_LookAt = Position() + P->m_Direction;
    return NE::kReturnSuccess;
}

uint NESpotLight::SetPosition(const vec3f& val)
{
    vec3f offset = val - Position();
    if(NESpatial::SetPosition(val) != NE::kReturnSuccess)
        return NE::kReturnFail;

    SetLookAt(P->m_LookAt + offset);

    return NE::kReturnSuccess;
}

const double& NESpotLight::Cutoff() const
{
    return P->m_Cutoff;
}

uint NESpotLight::SetCutoff(const double& val)
{
    NE_PROP_SET_AND_EVAL(Cutoff, P, val);
    if (P->m_SoftAngle >= P->m_Cutoff)
        P->m_SoftAngle = P->m_Cutoff-FEps;
    return NE::kReturnSuccess;
}

const double& NESpotLight::Exponent() const
{
    return P->m_Exponent;
}

uint NESpotLight::SetExponent(const double& val)
{
    NE_PROP_SET_AND_EVAL(Exponent, P, val);
    return NE::kReturnSuccess;
}

const double& NESpotLight::LinearAttenuation() const
{
    return P->m_LinearAttenuation;
}

uint NESpotLight::SetLinearAttenuation(const double& val)
{
    NE_PROP_SET_AND_EVAL(LinearAttenuation, P, val);
    return NE::kReturnSuccess;
}

const double& NESpotLight::QuadraticAttenuation() const
{
    return P->m_QuadraticAttenuation;
}

uint NESpotLight::SetQuadraticAttenuation(const double& val)
{
    NE_PROP_SET_AND_EVAL(QuadraticAttenuation, P, val);
    return NE::kReturnSuccess;
}

const double& NESpotLight::PovrayRadius() const
{
    return P->m_PovrayRadius;
}

uint NESpotLight::SetPovrayRadius(const double& val)
{
    NE_PROP_SET_AND_EVAL(PovrayRadius, P, val);
    return NE::kReturnSuccess;
}

const int& NESpotLight::PovrayAdaptive() const
{
    return P->m_PovrayAdaptive;
}

uint NESpotLight::SetPovrayAdaptive(const int& val)
{
    NE_PROP_SET_AND_EVAL(PovrayAdaptive, P, val);
    return NE::kReturnSuccess;
}

const bool& NESpotLight::PovrayJitter() const
{
    return P->m_PovrayJitter;
}

uint NESpotLight::SetPovrayJitter(const bool& val)
{
    NE_PROP_SET_AND_EVAL(PovrayJitter, P, val);
    return NE::kReturnSuccess;
}


const vec3f& NESpotLight::PbrtColorI() const
{
    return P->m_PbrtColorI;
}

uint NESpotLight::SetPbrtColorI(const vec3f& color)
{
    NE_PROP_SET_AND_EVAL(PbrtColorI, P, color);
    return NE::kReturnSuccess;
}

const double& NESpotLight::SoftAngle() const
{
    return P->m_SoftAngle;
}

uint NESpotLight::SetSoftAngle(const double& val)
{
    if (val <= 0 || val >= P->m_Cutoff)
        return NE::kReturnNotChanged;
    NE_PROP_SET_AND_EVAL(SoftAngle, P, val);
    return NE::kReturnSuccess;
}

const double& NESpotLight::BeamDistribution() const
{
    return P->m_BeamDistribution;
}

uint NESpotLight::SetBeamDistribution(const double& val)
{
    if (val <= 0)
        return NE::kReturnNotChanged;
    NE_PROP_SET_AND_EVAL(BeamDistribution, P, val);
    return NE::kReturnSuccess;
}

void NESpotLight::propertyTweak(QtPropertyBase* p)
{
    hideProperty("Pivot", true);
    hideProperty("Orientation", true);
    NELight::propertyTweak(p);
}

vec3f NESpotLight::direction() const
{
    return P->m_Direction;
}

void NESpotLight::setDirection(vec3f val)
{
    P->m_Direction = val;
}

