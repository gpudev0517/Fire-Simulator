#include "Rendering/Spatial/Light/NEImageBasedLight.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Base/NEBase.h"

#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"


class NEImageBasedLightPrivate
{
public:

    NEImageBasedLightPrivate(NEImageBasedLight* s) : S(s)
    {

    }

    FilePath m_ImgName;
    double m_Rotate;
    NEImageBasedLight::RenderPreset m_Preset;

    //mentalray related
    double m_Ambiance;

    //povray related
    int m_MapType;
    int m_Interpolate;
    int m_SphSize;
    double m_Emission;
    double m_Diffuse;
    vec3f m_RotDirection;

private:
    NEImageBasedLight* S;
};

NE_PROP_DEF(FilePath, NEImageBasedLight, ImgName, FilePath("", ""))
NE_PROP_DEF(double, NEImageBasedLight, Rotate, 0.0)
NE_PROP_DEF(NEImageBasedLight::RenderPreset, NEImageBasedLight, Preset, NEImageBasedLight::defaultPreset)
NE_PROP_DEF(double, NEImageBasedLight, Ambiance, 1.0)
NE_PROP_DEF(int, NEImageBasedLight, MapType, 1)
NE_PROP_DEF(int, NEImageBasedLight, Interpolate, 0.0)
NE_PROP_DEF(int, NEImageBasedLight, SphSize, 20)
NE_PROP_DEF(double, NEImageBasedLight, Emission, 2.2)
NE_PROP_DEF(double, NEImageBasedLight, Diffuse, 0.0)
NE_PROP_DEF_VEC3(vec3f, float, NEImageBasedLight, RotDirection, vec3f(0,1,0), P)

static void initProperties()
{

    NE_INIT_PROP(NEImageBasedLight, ImgName);
    NE_INIT_PROP(NEImageBasedLight, Rotate);
    NE_INIT_PROP(NEImageBasedLight, Preset);
    NE_INIT_PROP(NEImageBasedLight, Ambiance);
    NE_INIT_PROP(NEImageBasedLight, MapType);
    NE_INIT_PROP(NEImageBasedLight, Interpolate);
    NE_INIT_PROP(NEImageBasedLight, SphSize);
    NE_INIT_PROP(NEImageBasedLight, Emission);
    NE_INIT_PROP(NEImageBasedLight, Diffuse);
    NE_INIT_PROP(NEImageBasedLight, RotDirection);
}

NEImageBasedLightFactory::NEImageBasedLightFactory(NEManager* m) : NENodeFactory(m)
{
    if( NEBASE.guiMode() == false )
        return;

    m_pMesh = new NEIndexedTriMesh();
    m_pMesh->loadFromFile("System/Meshes/directionalLight.obj");
    //m_pMesh->createDrawingBuffers(GL_STATIC_DRAW);
    NERENDERMAN.GLManager()->createDrawingBuffers(m_pMesh, GL_STATIC_DRAW);

    NEIcon icon;
    icon.iconPath = QString(":/Neutrino/Resources/16/directionalLight16.png");
    icon.iconSize = QSize(16, 16);
    nodeIcon()->push_back(icon);

    icon.iconPath = QString("::/Neutrino/Resources/22/directionalLight22.png");
    icon.iconSize = QSize(22, 22);
    nodeIcon()->push_back(icon);

    icon.iconPath = QString("::/Neutrino/Resources/32/directionalLight32.png");
    icon.iconSize = QSize(32, 32);
    nodeIcon()->push_back(icon);

//    nodeIcon()->addFile(":/Neutrino/Resources/16/directionalLight16.png", QSize(16, 16));
//    nodeIcon()->addFile(":/Neutrino/Resources/22/directionalLight22.png", QSize(22, 22));
//    nodeIcon()->addFile(":/Neutrino/Resources/32/directionalLight32.png", QSize(32, 32));
}

NEImageBasedLightFactory::~NEImageBasedLightFactory()
{
    if( NEBASE.guiMode() == false )
        return;

    m_pMesh->clearDrawingBuffers();
    delete m_pMesh;
}

QString NEImageBasedLightFactory::nodeName()
{
    return "Image Based Light";
}

uint NEImageBasedLightFactory::nodeType()
{
    return NE::kObjectLight;
}

QString NEImageBasedLightFactory::nodeInherits()
{
    return "Lights";
}

QString NEImageBasedLightFactory::objectName()
{
    return "IBLight";
}

QString NEImageBasedLightFactory::nodeVersion()
{
    return "0.1";
}

QString NEImageBasedLightFactory::nodeVendor()
{
    return "Neutrino Dynamics";
}

QString NEImageBasedLightFactory::nodeHelp()
{
    return "Image Based light";
}

NEObject* NEImageBasedLightFactory::createInstance()
{
    return new NEImageBasedLight(this);
}

const NEIndexedTriMesh* NEImageBasedLightFactory::mesh()
{
    return m_pMesh;
}

NEImageBasedLight::NEImageBasedLight(NENodeFactory* factory) : NELight(factory)
{
    P = new NEImageBasedLightPrivate(this);
    CALL_ONCE(initProperties);
    P->m_ImgName.setFilter("All files (*.*)");
    P->m_ImgName.setPath("");
    P->m_ImgName.setDialogType(FilePath::kOpenFileType);
    SetPreset(defaultPreset);

    callDefaultPreset();
}

NEImageBasedLight::~NEImageBasedLight()
{
    delete P;
}




uint NEImageBasedLight::init(uint initMode)
{
    NESpatial::init(initMode);
    return NE::kReturnSuccess;
}

void NEImageBasedLight::render()
{

}

void NEImageBasedLight::renderPickable()
{

}

void NEImageBasedLight::renderPicked()
{

}


void NEImageBasedLight::updateUniforms(QOpenGLShaderProgram* program, int lightID)
{

}

uint NEImageBasedLight::SetPosition(const vec3f& val)
{
    if(NESpatial::SetPosition(val) != NE::kReturnSuccess)
        return NE::kReturnFail;

    return NE::kReturnSuccess;
}

void NEImageBasedLight::propertyTweak(QtPropertyBase* p)
{
    hideProperty("Pivot", true);
    hideProperty("Orientation", true);
    NELight::propertyTweak(p);
}

const FilePath& NEImageBasedLight::ImgName() const
{
    return P->m_ImgName;
}

uint NEImageBasedLight::SetImgName(const FilePath& val)
{
    P->m_ImgName.setPath(val.path());
    return NE::kReturnSuccess;
}

const double& NEImageBasedLight::Rotate() const
{
    return P->m_Rotate;
}

uint NEImageBasedLight::SetRotate(const double& val)
{
    NE_PROP_SET_AND_EVAL(Rotate, P, val);
    return NE::kReturnSuccess;
}

const double& NEImageBasedLight::Ambiance() const
{
    return P->m_Ambiance;
}

uint NEImageBasedLight::SetAmbiance(const double& val)
{
    NE_PROP_SET_AND_EVAL(Ambiance, P, val);
    return NE::kReturnSuccess;
}

const int& NEImageBasedLight::MapType() const
{
    return P->m_MapType;
}

uint NEImageBasedLight::SetMapType(const int& val)
{
    NE_PROP_SET_AND_EVAL(MapType, P, val);
    return NE::kReturnSuccess;
}

const int & NEImageBasedLight::Interpolate() const
{
    return P->m_Interpolate;
}

uint NEImageBasedLight::SetInterpolate(const int& val)
{
    NE_PROP_SET_AND_EVAL(Interpolate, P, val);
    return NE::kReturnSuccess;
}

const int& NEImageBasedLight::SphSize() const
{
    return P->m_SphSize;
}

uint NEImageBasedLight::SetSphSize(const int& val)
{
    NE_PROP_SET_AND_EVAL(SphSize, P, val);
    return NE::kReturnSuccess;
}

const double& NEImageBasedLight::Emission() const
{
    return P->m_Emission;
}

uint NEImageBasedLight::SetEmission(const double& val)
{
    NE_PROP_SET_AND_EVAL(Emission, P, val);
    return NE::kReturnSuccess;
}

const double& NEImageBasedLight::Diffuse() const
{
    return P->m_Diffuse;
}

uint NEImageBasedLight::SetDiffuse(const double& val)
{
    NE_PROP_SET_AND_EVAL(Diffuse, P, val);
    return NE::kReturnSuccess;
}

const vec3f& NEImageBasedLight::RotDirection() const
{
    return P->m_RotDirection;
}

uint NEImageBasedLight::SetRotDirection(const vec3f& val)
{
    NE_PROP_SET_AND_EVAL(RotDirection, P, val);
    return NE::kReturnSuccess;
}

const NEImageBasedLight::RenderPreset& NEImageBasedLight::Preset() const
{
    return P->m_Preset;
}

uint NEImageBasedLight::SetPreset(const NEImageBasedLight::RenderPreset& val)
{
    P->m_Preset = val;
    callPreset();
    return NE::kReturnSuccess;
}

void NEImageBasedLight::callPreset()
{
    if(RenderPreset() == defaultPreset)
        callDefaultPreset();

    else if(RenderPreset() == PovrayPreset)
        callPovrayPreset();
}

void NEImageBasedLight::callDefaultPreset()
{
    SetRotate(0.0f);

    SetAmbiance(1.0);

    SetSphSize(10);
    SetMapType(1);
    SetInterpolate(2);
    SetEmission(1.0);
    SetDiffuse(0.0);
    SetRotDirection(vec3f(0, 1, 0));
}

void NEImageBasedLight::callMentalrayPreset()
{
    SetRotate(0.0f);
    SetAmbiance(1.0);
}

void NEImageBasedLight::callPovrayPreset()
{
    SetImgName(FilePath("24.hdr"));
    SetRotate(150.0f);
    SetSphSize(20);
    SetMapType(1);
    SetInterpolate(2);
    SetEmission(2.2);
    SetDiffuse(0.0);
    SetRotDirection(vec3f(0, 1, 0));
    SetPosition(vec3f(0, 0, 0));
}



