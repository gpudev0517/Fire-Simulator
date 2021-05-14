#include "Rendering/Spatial/Light/NEPointLight.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Base/NEBase.h"

#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"

class NEPointLightPrivate
{
public:
    NEPointLightPrivate(NEPointLight* s) : S(s)
    {

    }

    double m_LinearAttenuation;
    double m_QuadraticAttenuation;
    NEIndexedTriMesh* m_Mesh;
    bool m_MentalShadow;
    double m_MentalFactor;
private:
    NEPointLight* S;
};

NE_PROP_DEF(double, NEPointLight, LinearAttenuation, 0.0)
NE_PROP_DEF(double, NEPointLight, QuadraticAttenuation, 0.0)


static void initProperties()
{

}

NEPointLightFactory::NEPointLightFactory(NEManager* m) : NENodeFactory(m)
{
    if( NEBASE.guiMode() == false )
        return;

    m_pMesh = new NEIndexedTriMesh();
    m_pMesh->loadFromFile("System/Meshes/pointLight.obj");
    //m_pMesh->createDrawingBuffers(GL_STATIC_DRAW);
    NERENDERMAN.GLManager()->createDrawingBuffers(m_pMesh, GL_STATIC_DRAW);

    NEIcon icon;
    icon.iconPath = QString(":/Neutrino/Resources/16/pointLight16.png");
    icon.iconSize = QSize(16, 16);
    nodeIcon()->push_back(icon);

    icon.iconPath = QString("::/Neutrino/Resources/22/pointLight22.png");
    icon.iconSize = QSize(22, 22);
    nodeIcon()->push_back(icon);

    icon.iconPath = QString("::/Neutrino/Resources/32/pointLight32.png");
    icon.iconSize = QSize(32, 32);
    nodeIcon()->push_back(icon);

//    nodeIcon()->addFile(":/Neutrino/Resources/16/pointLight16.png", QSize(16, 16));
//    nodeIcon()->addFile(":/Neutrino/Resources/22/pointLight22.png", QSize(22, 22));
//    nodeIcon()->addFile(":/Neutrino/Resources/32/pointLight32.png", QSize(32, 32));
}

NEPointLightFactory::~NEPointLightFactory()
{
    if( NEBASE.guiMode() == false )
        return;

    m_pMesh->clearDrawingBuffers();
    delete m_pMesh;
}

QString NEPointLightFactory::nodeName()
{
    return "Point Light";
}

uint NEPointLightFactory::nodeType()
{
    return NE::kObjectLight;
}

QString NEPointLightFactory::nodeInherits()
{
    return "Lights";
}

QString NEPointLightFactory::objectName()
{
    return "PointLight";
}

QString NEPointLightFactory::nodeVersion()
{
    return "0.1";
}

QString NEPointLightFactory::nodeVendor()
{
    return "Neutrino Dynamics";
}

QString NEPointLightFactory::nodeHelp()
{
    return "Point light";
}

NEObject* NEPointLightFactory::createInstance()
{
    return new NEPointLight(this);
}

const NEIndexedTriMesh* NEPointLightFactory::mesh()
{
    return m_pMesh;
}

NEPointLight::NEPointLight(NENodeFactory* factory) : NELight(factory)
{
    P = new NEPointLightPrivate(this);
    CALL_ONCE(initProperties);
    SetScale(vec3f(0.8f, 0.8f, 0.8f));
    SetPosition(vec3f(-5, 5, -5));
    P->m_LinearAttenuation = NE_DEFAULT(LinearAttenuation);
    P->m_QuadraticAttenuation = NE_DEFAULT(QuadraticAttenuation);


}

NEPointLight::~NEPointLight()
{

}

uint NEPointLight::init(uint initMode)
{
    NESpatial::init(initMode);
    P->m_Mesh = const_cast<NEIndexedTriMesh*>(((NEPointLightFactory&)factory()).mesh());
    return NE::kReturnSuccess;
}

void NEPointLight::render()
{
}

void NEPointLight::renderPicked()
{
}

void NEPointLight::renderPickable()
{
}

const NEIndexedTriMesh* NEPointLight::mesh() const
{
    return P->m_Mesh;
}

void NEPointLight::updateUniforms(QOpenGLShaderProgram* program, int lightID, bool isActive)
{
    program->setUniformValue(QString("PointLights[" + QString::number(lightID) + "].isactive").toUtf8().constData(), isActive);
    program->setUniformValue(QString("PointLights[" + QString::number(lightID) + "].color").toUtf8().constData(), LightColor());
    program->setUniformValue(QString("PointLights[" + QString::number(lightID) + "].color").toUtf8().constData(), (float) LightIntensity());

    // Map light position from world space to eye space before passing to the shader
    vec3f posMapped = NERENDERMAN.GLManager()->viewMatrix() * Position();
    QVector3D* posQVec = reinterpret_cast<QVector3D*>(&posMapped);
    program->setUniformValue(QString("PointLights[" + QString::number(lightID) + "].pos").toUtf8().constData(), *posQVec);
    program->setUniformValue(QString("PointLights[" + QString::number(lightID) + "].attLin").toUtf8().constData(),
                             (float)P->m_LinearAttenuation);
    program->setUniformValue(QString("PointLights[" + QString::number(lightID) + "].attQuad").toUtf8().constData(),
                             (float) P->m_QuadraticAttenuation);
}

uint NEPointLight::SetPosition(const vec3f& val)
{
    if(NESpatial::SetPosition(val) != NE::kReturnSuccess)
        return NE::kReturnFail;

    return NE::kReturnSuccess;
}

void NEPointLight::propertyTweak(QtPropertyBase* p)
{
    hideProperty("Pivot", true);
    hideProperty("Orientation", true);
    NELight::propertyTweak(p);
}

const double& NEPointLight::LinearAttenuation() const
{
    return P->m_LinearAttenuation;
}

uint NEPointLight::SetLinearAttenuation(const double& val)
{
    NE_PROP_SET_AND_EVAL(LinearAttenuation , P, val);
    return NE::kReturnSuccess;
}

const double& NEPointLight::QuadraticAttenuation() const
{
    return P->m_QuadraticAttenuation;
}

uint NEPointLight::SetQuadraticAttenuation(const double& val)
{
    NE_PROP_SET_AND_EVAL(QuadraticAttenuation, P, val);
    return NE::kReturnSuccess;
}
