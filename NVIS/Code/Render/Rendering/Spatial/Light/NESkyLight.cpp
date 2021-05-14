#include "Rendering/Spatial/Light/NESkyLight.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/NEGLRenderer.h"

class NESkyLightPrivate
{
public:
    NESkyLightPrivate(NESkyLight* s) : S(s)
    {
    }
    /* Properties */
    double m_Turbidity;
    QColor m_GroundColor;
    double m_Elevation;
    double m_Azimuth;
    bool m_DrawSunDisk;
    bool m_DrawGround;
private:
    NESkyLight* S;
};

NESkyLightFactory::NESkyLightFactory(NEManager* m) : NENodeFactory(m)
{
}

NESkyLightFactory::~NESkyLightFactory()
{
}

QString NESkyLightFactory::nodeName()
{
    return "Sky Light";
}

uint NESkyLightFactory::nodeType()
{
    return NE::kObjectLight;
}

QString NESkyLightFactory::nodeInherits()
{
    return "Lights";
}

QString NESkyLightFactory::objectName()
{
    return "SkyLight";
}

QString NESkyLightFactory::nodeVersion()
{
    return "0.1";
}

QString NESkyLightFactory::nodeVendor()
{
    return "Neutrino Dynamics";
}

QString NESkyLightFactory::nodeHelp()
{
    return "Sky light";
}

NEObject* NESkyLightFactory::createInstance()
{
    return new NESkyLight(this);
}

NE_PROP_DEF(double,NESkyLight,Turbidity,3.0)
NE_PROP_DEF(QColor,NESkyLight,GroundColor,QColor(255,255,255))
NE_PROP_DEF(double,NESkyLight,Elevation,45.0)
NE_PROP_DEF(double,NESkyLight,Azimuth,90.0)
NE_PROP_DEF(bool,NESkyLight,DrawSunDisk,true)
NE_PROP_DEF(bool,NESkyLight,DrawGround,true)

static void initProperties()
{
    NE_INIT_PROP(NESkyLight,Turbidity);
    NE_INIT_PROP(NESkyLight,GroundColor);
    NE_INIT_PROP(NESkyLight,Elevation);
    NE_INIT_PROP(NESkyLight,Azimuth);
    NE_INIT_PROP(NESkyLight,DrawSunDisk);
    NE_INIT_PROP(NESkyLight,DrawGround);
}

NESkyLight::NESkyLight(NENodeFactory* factory) : NELight(factory)
{
    P = new NESkyLightPrivate(this);
    CALL_ONCE(initProperties);
    P->m_Turbidity = NE_DEFAULT(Turbidity);
    P->m_GroundColor = NE_DEFAULT(GroundColor);
    P->m_Elevation = NE_DEFAULT(Elevation);
    P->m_Azimuth = NE_DEFAULT(Azimuth);
    P->m_DrawSunDisk = NE_DEFAULT(DrawSunDisk);
    P->m_DrawGround = NE_DEFAULT(DrawGround);
}

NESkyLight::~NESkyLight()
{
    delete P;
}

const double& NESkyLight::Turbidity() const
{
    return P->m_Turbidity;
}

const QColor& NESkyLight::GroundColor() const
{
    return P->m_GroundColor;
}

const double& NESkyLight::Elevation() const
{
    return P->m_Elevation;
}

const double& NESkyLight::Azimuth() const
{
    return P->m_Azimuth;
}

const bool& NESkyLight::DrawSunDisk() const
{
    return P->m_DrawSunDisk;
}

const bool& NESkyLight::DrawGround() const
{
    return P->m_DrawGround;
}

uint NESkyLight::SetTurbidity(const double &val)
{
    if (val < 1.0 ||
        val > 10.0)
        return NE::kReturnNotChanged;
    NE_PROP_SET_AND_EVAL(Turbidity,P,val);
    return NE::kReturnSuccess;
}

uint NESkyLight::SetGroundColor(const QColor &val)
{
    NE_PROP_SET_AND_EVAL(GroundColor,P,val);
    return NE::kReturnSuccess;
}

uint NESkyLight::SetElevation(const double &val)
{
    if (val < 0.0 ||
        val > 90.0)
        return NE::kReturnNotChanged;
    NE_PROP_SET_AND_EVAL(Elevation,P,val);
    return NE::kReturnSuccess;
}

uint NESkyLight::SetAzimuth(const double &val)
{
    if (val < 0.0 ||
        val > 360.0)
        return NE::kReturnNotChanged;
    NE_PROP_SET_AND_EVAL(Elevation,P,val);
    return NE::kReturnSuccess;
}

uint NESkyLight::SetDrawSunDisk(const bool& val)
{
    NE_PROP_SET_AND_EVAL(DrawSunDisk,P,val);
    return NE::kReturnSuccess;
}

uint NESkyLight::SetDrawGround(const bool& val)
{
    NE_PROP_SET_AND_EVAL(DrawGround,P,val);
    return NE::kReturnSuccess;
}

uint NESkyLight::SetPosition(const vec3f& val)
{
    if (NELight::SetPosition(val) == NE::kReturnNotChanged)
        return NE::kReturnNotChanged;
    return NE::kReturnSuccess;
}

uint NESkyLight::SetOrientation(const eulerf& val)
{
    if (NELight::SetOrientation(val) == NE::kReturnNotChanged)
        return NE::kReturnNotChanged;
    return NE::kReturnSuccess;
}

uint NESkyLight::SetScale(const vec3f& val)
{
    if (NELight::SetScale(val) == NE::kReturnNotChanged)
        return NE::kReturnNotChanged;
    return NE::kReturnSuccess;
}

uint NESkyLight::init(uint initMode)
{
    NESpatial::init(initMode);
    return NE::kReturnSuccess;
}

void NESkyLight::resetProperties()
{
    NELight::resetProperties();
    ResetTurbidity();
    ResetGroundColor();
    ResetElevation();
    ResetAzimuth();
    ResetDrawSunDisk();
    ResetDrawGround();
}

void NESkyLight::propertyTweak(QtPropertyBase* p)
{
    NELight::propertyTweak(p);
    hideProperty("Position",true);
    hideProperty("Orientation",true);
    hideProperty("Scale",true);
    hideProperty("Pivot",true);
    hideProperty("RenderMode",true);
    hideProperty("Color",true);
}

void NESkyLight::render()
{
}

void NESkyLight::renderPicked()
{
}

void NESkyLight::renderPickable()
{
}
