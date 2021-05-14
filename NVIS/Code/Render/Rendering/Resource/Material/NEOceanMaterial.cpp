#include "Rendering/Resource/Material/NEOceanMaterial.h"


NEOceanMaterial::NEOceanMaterial(NENodeFactory* factory) : NETransparentMaterial(factory)
{
    setOceanType(NEOceanPreset::Ocean);
}

NEOceanMaterial::NEOceanPreset NEOceanMaterial::oceanType() const {return m_Preset;}

void NEOceanMaterial::setOceanType(NEOceanMaterial::NEOceanPreset preset) {m_Preset = preset; setPresetValues();}

float NEOceanMaterial::reflectivity() const { return m_Reflectivity; }

void NEOceanMaterial::setReflectivity(float val) { m_Reflectivity = val; }

QColor NEOceanMaterial::reflectivityColor() const { return m_ReflectivityColor; }

void NEOceanMaterial::setReflectivityColor(QColor val) { m_ReflectivityColor = val; }

float NEOceanMaterial::reflectivityCutoff() const { return m_ReflectivityCutoff; }

void NEOceanMaterial::setReflectivityCutoff(float val) { m_ReflectivityCutoff = val; }

bool NEOceanMaterial::reflectionFalloff() const { return m_ReflectionFalloff; }

void NEOceanMaterial::setReflectionFalloff(bool val) { m_ReflectionFalloff = val; }

float NEOceanMaterial::reflFalloffDist() const { return m_ReflFalloffDist; }

void NEOceanMaterial::setReflFalloffDist(float val) { m_ReflFalloffDist = val; }

QColor NEOceanMaterial::reflectionFalloffColor() const { return m_ReflectionFalloffColor; }

void NEOceanMaterial::setReflectionFalloffColor(QColor val) { m_ReflectionFalloffColor = val; }

bool NEOceanMaterial::useReflectionFalloffColor() const { return m_UseReflectionFalloffColor; }

void NEOceanMaterial::setUseReflectionFalloffColor(bool val) { m_UseReflectionFalloffColor = val; }

float NEOceanMaterial::refractivityCutoff() const { return m_RefractivityCutoff; }

void NEOceanMaterial::setRefractivityCutoff(float val) { m_RefractivityCutoff = val; }

float NEOceanMaterial::transparency() const { return m_Transparency; }

void NEOceanMaterial::setTransparency(float val) { m_Transparency = val; }

QColor NEOceanMaterial::refractionColor() const { return m_RefractionColor; }

void NEOceanMaterial::setRefractionColor(QColor val) { m_RefractionColor = val; }

bool NEOceanMaterial::refractionFalloff() const { return m_RefractionFalloff; }

void NEOceanMaterial::setRefractionFalloff(bool val) { m_RefractionFalloff = val; }

float NEOceanMaterial::refrFalloffDist() const { return m_RefrFalloffDist; }

void NEOceanMaterial::setRefrFalloffDist(float val) { m_RefrFalloffDist = val; }

QColor NEOceanMaterial::refractionFalloffColor() const { return m_RefractionFalloffColor; }

void NEOceanMaterial::setRefractionFalloffColor(QColor val) { m_RefractionFalloffColor = val; }

bool NEOceanMaterial::useRefractionFalloffColor() const { return m_UseRefractionFalloffColor; }

void NEOceanMaterial::setUseRefractionFalloffColor(bool val) { m_UseRefractionFalloffColor = val; }

bool NEOceanMaterial::fresnel() const { return m_Fresnel; }

void NEOceanMaterial::setFresnel(bool val) { m_Fresnel = val; }

bool NEOceanMaterial::thinWalled() const { return m_ThinWalled; }

void NEOceanMaterial::setThinWalled(bool val) { m_ThinWalled = val; }

bool NEOceanMaterial::refractiveCaustics() const { return m_RefractiveCaustics; }

void NEOceanMaterial::setRefractiveCaustics(bool val) { m_RefractiveCaustics = val; }

QColor NEOceanMaterial::diffuse() const { return m_Diffuse; }

void NEOceanMaterial::setDiffuse(QColor val) { m_Diffuse = val; }

float NEOceanMaterial::diffWeight() const { return m_DiffWeight; }

void NEOceanMaterial::setDiffWeight(float val) { m_DiffWeight = val; }

bool NEOceanMaterial::noDiffBump() const { return m_NoDiffBump; }

void NEOceanMaterial::setNoDiffBump(bool val) { m_NoDiffBump = val; }

float NEOceanMaterial::physicalSkyHaze() const { return m_PhysicalSkyHaze; }

void NEOceanMaterial::setPhysicalSkyHaze(float val) { m_PhysicalSkyHaze = val; }

float NEOceanMaterial::physicalSkyRedBlueShift() const { return m_PhysicalSkyRedBlueShift; }

void NEOceanMaterial::setPhysicalSkyRedBlueShift(float val) { m_PhysicalSkyRedBlueShift = val; }

/*!
*/
void NEOceanMaterial::setPresetValues()
{
    switch(m_Preset)
    {
    case NEOceanPreset::Ocean:
        setOceanPreset();
        break;
    case NEOceanPreset::TropicalOcean:
        setTropicalOceanPreset();
        break;
    }
}

/*!
*/
void NEOceanMaterial::setOceanPreset()
{
    setReflectivity(5.0f);
    setReflectivityColor(QColor(255,255,255));
    setReflectivityCutoff(0.0f);
    setRefractivityCutoff(0.0f);
    setTransparency(0.0f);
    SetIndexOfRefraction( 1.33 );
    setRefractionColor(QColor(255,255,255));
    setRefractionFalloff(false);
    setReflectionFalloff(true);
    setReflFalloffDist(10);
    setUseReflectionFalloffColor(true);
    setReflectionFalloffColor(QColor::fromRgbF(0.33,0.66,1.0));
    setUseRefractionFalloffColor(false);
    setRefractionFalloffColor(QColor::fromRgbF(0.33,0.66,1.0));
    setRefrFalloffDist(10.f);
    setFresnel(true);
    setThinWalled(true);
    setRefractiveCaustics(false);
    setDiffWeight(0.01f);
    setDiffuse(QColor(255,255,255));
    setNoDiffBump(true);
    setPhysicalSkyHaze(0.3f);
    setPhysicalSkyRedBlueShift(0.3f);
}

/*!
*/
void NEOceanMaterial::setTropicalOceanPreset()
{
    setReflectivity(1.0f);
    setDiffuse(QColor(0,128,0));
    setDiffWeight(0.2f);
    setReflectivityColor(QColor(0,128,0));
    setReflectivityCutoff(0.01f);
    setRefractivityCutoff(0.0f);
    setTransparency(0.0f);
    SetIndexOfRefraction(1.33f);
    setRefractionColor(QColor(255,255,255));
    setRefractionFalloff(true);
    setUseReflectionFalloffColor(false);
    setUseRefractionFalloffColor(true);
    setRefractionFalloffColor(QColor(83,195,204));
    setRefrFalloffDist(70.f);
    setFresnel(true);
    setThinWalled(true);
    setRefractiveCaustics(false);
    setNoDiffBump(true);
    setPhysicalSkyHaze(0.0);
    setPhysicalSkyRedBlueShift(0.0);
}



NEOceanMaterialFactory::NEOceanMaterialFactory(NEManager* m) : NENodeFactory(m) {}

NEOceanMaterialFactory::~NEOceanMaterialFactory() {}

QString NEOceanMaterialFactory::nodeName() { return "Ocean Material"; }

uint NEOceanMaterialFactory::nodeType(){ return NE::kObjectMaterial;}

QString NEOceanMaterialFactory::nodeInherits() { return "NEMaterial";}

QString NEOceanMaterialFactory::objectName() { return "Ocean"; }

QString NEOceanMaterialFactory::nodeVersion() { return "0.1"; }

QString NEOceanMaterialFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEOceanMaterialFactory::nodeHelp() { return "Ocean material"; }

NEObject*NEOceanMaterialFactory::createInstance() { return new NEOceanMaterial(this); }
