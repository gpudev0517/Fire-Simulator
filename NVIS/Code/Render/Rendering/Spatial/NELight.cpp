#include "Rendering/Spatial/NELight.h"

#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"

#include "Base/NEBase.h"

struct ShadowMap
{
    GLuint texture;
    GLuint framebuffer;
};

class NELightPrivate
{
public:
    NELightPrivate(NELight* s) : S(s)
    {

    }

    // Pov-Ray related
    vec3f m_PovrayColor;

    double m_LightIntensity;
    // Shadow related
    QColor m_ShadowColor;
    double m_LightSize;
    NELight::NEShadowMapSize m_ShadowMapSize;
    NELight::NEShadowMode m_ShadowMode;
    ShadowMap m_shadowMap;
    matrix44f m_depthBiasMVP;
    matrix44f m_viewMatrix;
    float m_near;
    float m_far;
    vec2f m_lightRadiusUV;

private:
    NELight* S;
};


NE_PROP_DEF_VEC3(vec3f, float, NELight, PovrayColor, vec3f(1000.0f, 1000.0f, 1000.0f), P)
NE_PROP_DEF(double, NELight, LightIntensity, 1.0)
NE_PROP_DEF(QColor, NELight, ShadowColor, QColor(0,0,0))
NE_PROP_DEF(NELight::NEShadowMode, NELight, ShadowMode, (NELight::NoShadow))
NE_PROP_DEF(double, NELight, LightSize, 0.2)
NE_PROP_DEF(NELight::NEShadowMapSize, NELight, ShadowMapSize, NELight::NEShadowMapSize::T2048)

static void initProperties()
{
    NE_INIT_PROP(NELight, PovrayColor);
    NE_INIT_PROP(NELight, LightIntensity);
    NE_INIT_PROP(NELight, ShadowColor);
    NE_INIT_PROP(NELight, ShadowMode);
    NE_INIT_PROP(NELight, LightSize);
    NE_INIT_PROP(NELight, ShadowMapSize);
}

NELight::NELight(NENodeFactory* factory) : NESpatial(factory)
{
    addNodeTypeToNodeGraphSceneType("Lights", NENode::SceneType::Visualization );

    P = new NELightPrivate(this);
    CALL_ONCE(initProperties);
    SetColor(QColor(255, 255, 255, 255));
    SetOccludeShadows(false);
    SetReceiveShadows(false);


    P->m_PovrayColor = NE_DEFAULT(PovrayColor);
    P->m_LightIntensity = NE_DEFAULT(LightIntensity);
    P->m_ShadowColor = NE_DEFAULT(ShadowColor);
    P->m_ShadowMode = NE_DEFAULT(ShadowMode);
    P->m_LightSize = NE_DEFAULT(LightSize);
    P->m_ShadowMapSize = NE_DEFAULT(ShadowMapSize);
    SetExternalRender(true);
    SetPovrayColor(vec3f(1, 1, 1));

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    if (NEBASE.guiMode())
        ShadowCreate();

    m_bUpdate = false;

}

NELight::~NELight()
{
    if (NEBASE.guiMode())
        ShadowDestroy();
    delete P;
    //setBehavior(inactive);
}

void NELight::ShadowCreate()
{
    GLint prevFrameBuffer;
    GL->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFrameBuffer);

    GL->glGenFramebuffers(1, &P->m_shadowMap.framebuffer);

    GL->glGenTextures(1, &P->m_shadowMap.texture);
    GL->glBindTexture(GL_TEXTURE_2D, P->m_shadowMap.texture);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    /*// This is to allow usage of shadow2DProj function in the shader
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);*/

    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, getShadowMapSize(), getShadowMapSize());
    else
        GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, getShadowMapSize(), getShadowMapSize(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    GL->glBindTexture(GL_TEXTURE_2D, 0);

    GL->glBindFramebuffer(GL_FRAMEBUFFER, P->m_shadowMap.framebuffer);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, P->m_shadowMap.texture, 0);
    GL->glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer);
}

void NELight::ShadowDestroy()
{
    GL->glDeleteTextures(1, &P->m_shadowMap.texture);
    GL->glDeleteFramebuffers(1, &P->m_shadowMap.framebuffer);

    P->m_shadowMap.texture = 0;
    P->m_shadowMap.framebuffer = 0;

}

bool NELight::isManipulatable() const
{
    return true;
}

void NELight::updateUniforms(QOpenGLShaderProgram* program, int lightID)
{

}

uint NELight::SetBehavior(const NENodeState& val)
{
    uint retVal = NENode::SetBehavior(val);
    if(retVal != NE::kReturnSuccess)
        return retVal;

    NERENDERMAN.GLManager()->updateAllShaders();

    return NE::kReturnSuccess;
}

void NELight::bindShadowTexture()
{
    GL->glBindTexture(GL_TEXTURE_2D, P->m_shadowMap.texture);
}

void NELight::bindShadowFramebuffer()
{
    GL->glBindFramebuffer(GL_FRAMEBUFFER, P->m_shadowMap.framebuffer);
}

void NELight::setDepthBiasMVP(matrix44f depthBiasMVP)
{
    P->m_depthBiasMVP = depthBiasMVP;
}

matrix44f NELight::getDepthBiasMVP()
{
    return P->m_depthBiasMVP;
}

void NELight::setViewMatrix(matrix44f viewMatrix)
{
    P->m_viewMatrix = viewMatrix;
}

matrix44f NELight::getViewMatrix()
{
    return P->m_viewMatrix;
}

void NELight::setNear(float fNear)
{
    P->m_near = fNear;
}

float NELight::getNear()
{
    return P->m_near;
}

void NELight::setFar(float fFar)
{
    P->m_far = fFar;
}

float NELight::getFar()
{
    return P->m_far;
}

const QColor NELight::LightColor()
{
    int red = Color().red() ;
    int green = Color().green() ;
    int blue = Color().blue() ;
    if (red > 255) red = 255;
    if (green > 255) green = 255;
    if (blue > 255) blue = 255;
    return QColor(red, green, blue);
}

void NELight::setLightRadiusUV(vec2f uv)
{
    P->m_lightRadiusUV = uv;
}

vec2f NELight::getLightRadiusUV()
{
    return P->m_lightRadiusUV;
}

int NELight::getShadowMapSize()
{
    int size = ShadowMapSize();
    size = std::pow(2,9+size);
    return size;
}

void NELight::updateShadowTexture()
{
    if (!NEBASE.guiMode())
        return;
    ShadowDestroy();

    GLuint currFBO;
    // Enum has MANY names based on extension/version
    // but they all map to 0x8CA6
    GL->glGetIntegerv(0x8CA6, (GLint*)&currFBO);
    ShadowCreate();
    GL->glBindFramebuffer(GL_FRAMEBUFFER, currFBO);
}

const vec3f& NELight::PovrayColor() const
{
    return P->m_PovrayColor;
}

uint NELight::SetPovrayColor(const vec3f& val)
{
    NE_PROP_SET_AND_EVAL(PovrayColor, P, val);
    return NE::kReturnSuccess;
}


void NELight::propertyTweak(QtPropertyBase* p)
{
    hideProperty("OccludeShadows", true);
    hideProperty("ReceiveShadows", true);

    NESpatial::propertyTweak(p);
}

const double& NELight::LightIntensity() const
{
    return P->m_LightIntensity;
}

uint NELight::SetLightIntensity(const double& val)
{
    if (val < 0.0)
        return NE::kReturnNotChanged;
    NE_PROP_SET_AND_EVAL(LightIntensity, P, val);
    return NE::kReturnSuccess;
}

const NELight::NEShadowMode& NELight::ShadowMode() const
{
    return P->m_ShadowMode;
}

uint NELight::SetShadowMode(const NEShadowMode& val)
{
    NE_PROP_SET_AND_EVAL(ShadowMode, P, val);
    setNeedShadowUpdate(true);
    return NE::kReturnSuccess;
}

const QColor& NELight::ShadowColor() const
{
    return P->m_ShadowColor;
}

uint NELight::SetShadowColor(const QColor& val)
{
    NE_PROP_SET_AND_EVAL(ShadowColor, P, val);
    return NE::kReturnSuccess;
}

const double& NELight::LightSize() const
{
    return P->m_LightSize;
}

uint NELight::SetLightSize(const double& val)
{
    NE_PROP_SET_AND_EVAL(LightSize, P, val);
    return NE::kReturnSuccess;
}

const NELight::NEShadowMapSize& NELight::ShadowMapSize() const
{
    return P->m_ShadowMapSize;
}

uint NELight::SetShadowMapSize(const NELight::NEShadowMapSize& val)
{
    if (!NEBASE.guiMode())
    {
        return NE::kReturnSuccess;
    }
    GLint maxSize;
    GL->glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
    NELight::NEShadowMapSize maxShadowMapSize = (NELight::NEShadowMapSize)(int)(std::log2(maxSize)-9);
    if(val > maxShadowMapSize)
    {
        NE_PROP_SET_AND_EVAL(ShadowMapSize, P, maxShadowMapSize);
    }
    else if (val < 0 )
    {
        NE_PROP_SET_AND_EVAL(ShadowMapSize, P, NELight::NEShadowMapSize::T1024);
    }
    else
    {
        NE_PROP_SET_AND_EVAL(ShadowMapSize, P, val);
    }

    //updateShadowTexture();
    setNeedShadowUpdate(true);
    return NE::kReturnSuccess;
}

void NELight::resetProperties()
{
    NESpatial::resetProperties();

    ResetPovrayColor();
    ResetLightIntensity();
    ResetShadowColor();
    ResetShadowMode();
    ResetLightSize();
}
