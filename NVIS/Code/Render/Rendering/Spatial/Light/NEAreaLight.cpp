#include "Rendering/Spatial/Light/NEAreaLight.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Base/NEBase.h"

class NEAreaLightPrivate
{
public:
    NEAreaLightPrivate(NEAreaLight* s) : S(s)
    {

    }
    NEIndexedTriMesh* m_Mesh;

    // General
    double m_Width;
    double m_Height;
    int m_Samples1;
    int m_Samples2;

    // Povray related
    int m_PovrayAdaptive;
    bool m_PovrayJitter;
    bool m_PovrayCircular;

    vec3f m_Direction;
    vec3f m_NormalizedDirection;

private:
    NEAreaLight *S;
};

NE_PROP_DEF(double, NEAreaLight, Width, 4)
NE_PROP_DEF(double, NEAreaLight, Height, 4)

NE_PROP_DEF(int, NEAreaLight, PovrayAdaptive, 1)
NE_PROP_DEF(bool, NEAreaLight, PovrayJitter, true)
NE_PROP_DEF(bool, NEAreaLight, PovrayCircular, true)
NE_PROP_DEF(int, NEAreaLight, Samples1, 2)
NE_PROP_DEF(int, NEAreaLight, Samples2, 2)

static void initProperties()
{
    NE_INIT_PROP(NEAreaLight, Width);
    NE_INIT_PROP(NEAreaLight, Height);

    NE_INIT_PROP(NEAreaLight, PovrayAdaptive);
    NE_INIT_PROP(NEAreaLight, PovrayJitter);
    NE_INIT_PROP(NEAreaLight, PovrayCircular);

    NE_INIT_PROP(NEAreaLight, Samples1);
    NE_INIT_PROP(NEAreaLight, Samples2);

}

NEAreaLightFactory::NEAreaLightFactory(NEManager* m) : NENodeFactory(m)
{
    if (NEBASE.guiMode())
        m_pMesh = new NEIndexedTriMesh();
    //m_pMesh->loadFromFile(":/Neutrino/Resources/meshes/spotLight.obj");
    //m_pMesh->createDrawingBuffers(kStatic);

    NEIcon icon;
    icon.iconPath = QString(":/Neutrino/Resources/16/areaLight16.png");
    icon.iconSize = QSize(16, 16);
    nodeIcon()->push_back(icon);

    icon.iconPath = QString("::/Neutrino/Resources/22/areaLight22.png");
    icon.iconSize = QSize(22, 22);
    nodeIcon()->push_back(icon);

    icon.iconPath = QString("::/Neutrino/Resources/32/areaLight32.png");
    icon.iconSize = QSize(32, 32);
    nodeIcon()->push_back(icon);

//    nodeIcon()->addFile(":/Neutrino/Resources/16/areaLight16.png", QSize(16, 16));
//    nodeIcon()->addFile(":/Neutrino/Resources/22/areaLight22.png", QSize(22, 22));
//    nodeIcon()->addFile(":/Neutrino/Resources/32/areaLight32.png", QSize(32, 32));
}

NEAreaLightFactory::~NEAreaLightFactory()
{
    if (NEBASE.guiMode())
    {
        m_pMesh->clearDrawingBuffers();
        delete m_pMesh;
    }
}

QString NEAreaLightFactory::nodeName()
{
    return "Area Light";
}

uint NEAreaLightFactory::nodeType()
{
    return NE::kObjectLight;
}

QString NEAreaLightFactory::nodeInherits()
{
    return "Lights";
}

QString NEAreaLightFactory::objectName()
{
    return "AreaLight";
}

QString NEAreaLightFactory::nodeVersion()
{
    return "0.1";
}

QString NEAreaLightFactory::nodeVendor()
{
    return "Neutrino Dynamics";
}

QString NEAreaLightFactory::nodeHelp()
{
    return "Area light";
}

NEObject* NEAreaLightFactory::createInstance()
{
    return new NEAreaLight(this);
}

const NEIndexedTriMesh* NEAreaLightFactory::mesh()
{
    return m_pMesh;
}


NEAreaLight::NEAreaLight(NENodeFactory* factory) : NELight(factory)
{
    P = new NEAreaLightPrivate(this);
    CALL_ONCE(initProperties);

    SetPosition(vec3f(0, 7, 0));

    P->m_Width = NE_DEFAULT(Width);
    P->m_Height = NE_DEFAULT(Height);
    P->m_Samples1 = NE_DEFAULT(Samples1);
    P->m_Samples2 = NE_DEFAULT(Samples2);
    P->m_PovrayAdaptive = NE_DEFAULT(PovrayAdaptive);
    P->m_PovrayJitter = NE_DEFAULT(PovrayJitter);
    P->m_PovrayCircular = NE_DEFAULT(PovrayCircular);

}
void NEAreaLight::updateUniforms(QOpenGLShaderProgram* program, int lightID)
{

}

NEAreaLight::~NEAreaLight()
{
    delete P;
}

uint NEAreaLight::init(uint initMode)
{
    P->m_Mesh = const_cast<NEIndexedTriMesh*>(((NEAreaLightFactory&)factory()).mesh());
    return NE::kReturnSuccess;
}

void NEAreaLight::render()
{
}

void NEAreaLight::renderPickable()
{
}

void NEAreaLight::renderPicked()
{
}

uint NEAreaLight::SetPosition(const vec3f& position)
{
    if(NESpatial::SetPosition(position) != NE::kReturnSuccess)
        return NE::kReturnFail;

    return NE::kReturnSuccess;
}

const vec3f& NEAreaLight::direction() const
{
    return P->m_Direction;
}

uint NEAreaLight::SetOrientation(const eulerf& rotation)
{
    const vec3f v = vec3f(rotation.phi(), rotation.theta(), rotation.psi());
    P->m_Direction = directionFromEulerAngles(rotation);
    P->m_NormalizedDirection = P->m_Direction.normalized();
    NESpatial::SetOrientation(rotation);

    return NE::kReturnSuccess;
}

const double& NEAreaLight::Width() const
{
    return P->m_Width;
}

uint NEAreaLight::SetWidth(const double& val)
{
    NE_PROP_SET_AND_EVAL(Width, P, val);
    return NE::kReturnSuccess;
}

const double& NEAreaLight::Height() const
{
    return P->m_Height;
}

uint NEAreaLight::SetHeight(const double& val)
{
    NE_PROP_SET_AND_EVAL(Height, P, val);
    return NE::kReturnSuccess;
}

const int& NEAreaLight::Samples1() const
{
    return P->m_Samples1;
}

uint NEAreaLight::SetSamples1(const int& val)
{
    NE_PROP_SET_AND_EVAL(Samples1, P, val);
    return NE::kReturnSuccess;
}

const int& NEAreaLight::Samples2() const
{
    return P->m_Samples2;
}

uint NEAreaLight::SetSamples2(const int& val)
{
    NE_PROP_SET_AND_EVAL(Samples2, P, val);
    return NE::kReturnSuccess;
}

const int& NEAreaLight::PovrayAdaptive() const
{
    return P->m_PovrayAdaptive;
}

uint NEAreaLight::SetPovrayAdaptive(const int& val)
{
    NE_PROP_SET_AND_EVAL(PovrayAdaptive, P, val);
    return NE::kReturnSuccess;
}

const bool& NEAreaLight::PovrayJitter() const
{
    return P->m_PovrayJitter;
}

uint NEAreaLight::SetPovrayJitter(const bool& val)
{
    NE_PROP_SET_AND_EVAL(PovrayJitter, P, val);
    return NE::kReturnSuccess;
}

const bool& NEAreaLight::PovrayCircular() const
{
    return P-> m_PovrayCircular;
}

uint NEAreaLight::SetPovrayCircular(const bool& val)
{
    NE_PROP_SET_AND_EVAL(PovrayCircular, P, val);
    return NE::kReturnSuccess;
}

