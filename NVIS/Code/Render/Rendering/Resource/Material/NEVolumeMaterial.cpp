#include "Rendering/Resource/Material/NEVolumeMaterial.h"


NEVolumeMaterial::NEVolumeMaterial(NENodeFactory* factory) : NEMaterial(factory)
{
    setShaderProgram(new QOpenGLShaderProgram());
    //volume material is not designed for meshes but for particles only
    setInputs(NE::kIOParticles);

    //set values
    setUnitDensity(1.0f);
    setMinStepsize(0.01f);
    setMaxStepsize(0.02f);
    setDensityShaderMethod(VolumeShaderMethods::Sharp);
    setStepsizing(VolumeStepsizing::Constant);

    //set values of sharp shader
    //setDynamicRadius(false);
    setKernelExponent(1);
    setDistanceExponent(1);
    setMaxNrElements(10);
    setSprayRadius(0.01f);
    setSurfaceRadius(0.02f);
    setSurfaceNeighborWeighting(1.0f);
    setSprayWeighting(1.f);
    setClampDensity(true);
    setUseLifetime(true);
    setValueMethod(VolumeKernel::ConstantValue);
    setKernel(VolumeKernel::Distance);
    setShaderMethod(ShaderMethod::Ihmsen);
    setLightModel(VolumeLightEnergyMethod::VLE_TransferFunction);
}

NEVolumeMaterial::~NEVolumeMaterial()
{
    shaderProgram()->removeAllShaders();
}

void NEVolumeMaterial::updateUniforms()
{
    //NEMaterial::updateUniforms();
}

void NEVolumeMaterial::propertyTweak( QtPropertyBase* p )
{
    hideProperty("Ambient", true);
    hideProperty("Specular", true);
    hideProperty("Ambiance", true);
    hideProperty("KSpecular", true);
    hideProperty("KDiffuse", true);
    hideProperty("KAmbient", true);

    NEMaterial::propertyTweak(p);
}

float NEVolumeMaterial::uintDensity() const { return m_UnitDensity; }

void NEVolumeMaterial::setUnitDensity(float val) { m_UnitDensity = val; }

float NEVolumeMaterial::minStepsize() const { return m_MinStepsize; }

void NEVolumeMaterial::setMinStepsize(float val) { m_MinStepsize = val; }

float NEVolumeMaterial::maxStepsize() const { return m_MaxStepsize; }

void NEVolumeMaterial::setMaxStepsize(float val) { m_MaxStepsize = val; }

NEVolumeMaterial::VolumeShaderMethods NEVolumeMaterial::densityShaderMethod() const { return m_DensityShaderMethod; }

void NEVolumeMaterial::setDensityShaderMethod(NEVolumeMaterial::VolumeShaderMethods val) { m_DensityShaderMethod = val; }

NEVolumeMaterial::VolumeStepsizing NEVolumeMaterial::stepsizing() const { return m_Stepsizing; }

void NEVolumeMaterial::setStepsizing(NEVolumeMaterial::VolumeStepsizing val) { m_Stepsizing = val; }

NEVolumeMaterial::VolumeLightEnergyMethod NEVolumeMaterial::lightModel() const {return m_LightModel;}

void NEVolumeMaterial::setLightModel(NEVolumeMaterial::VolumeLightEnergyMethod model){m_LightModel = model;}

NEVolumeMaterial::VolumeKernel NEVolumeMaterial::kernel() const { return m_Kernel; }

void NEVolumeMaterial::setKernel(NEVolumeMaterial::VolumeKernel val) { m_Kernel = val; }

NEVolumeMaterial::VolumeKernel NEVolumeMaterial::valueMethod() const { return m_ValueMethod; }

void NEVolumeMaterial::setValueMethod(NEVolumeMaterial::VolumeKernel val) { m_ValueMethod = val; }

NEVolumeMaterial::ShaderMethod NEVolumeMaterial::shaderMethod() const { return m_ShaderMethod; }

void NEVolumeMaterial::setShaderMethod(NEVolumeMaterial::ShaderMethod val) { m_ShaderMethod = val; }

float NEVolumeMaterial::surfaceRadius() const { return m_SurfaceRadius; }

void NEVolumeMaterial::setSurfaceRadius(float val) { m_SurfaceRadius = val; }

float NEVolumeMaterial::sprayRadius() const { return m_SprayRadius; }

void NEVolumeMaterial::setSprayRadius(float val) { m_SprayRadius = val; }

float NEVolumeMaterial::surfaceNeighborWeighting() const {return m_SurfaceNeighborWeighting;}

void NEVolumeMaterial::setSurfaceNeighborWeighting(float val){m_SurfaceNeighborWeighting = val;}

float NEVolumeMaterial::sprayWeighting() const {return m_SprayWeighting;}

void NEVolumeMaterial::setSprayWeighting(float val){m_SprayWeighting = val;}

int NEVolumeMaterial::maxNrElements() const { return m_MaxNrElements; }

void NEVolumeMaterial::setMaxNrElements(int val) { m_MaxNrElements = val; }

int NEVolumeMaterial::distanceExponent() const { return m_DistanceExponent; }

void NEVolumeMaterial::setDistanceExponent(int val) { m_DistanceExponent = val; }

int NEVolumeMaterial::kernelExponent() const { return m_KernelExponent; }

void NEVolumeMaterial::setKernelExponent(int val) { m_KernelExponent = val; }

bool NEVolumeMaterial::clampDensity() const { return m_ClampDensity; }

void NEVolumeMaterial::setClampDensity(bool val) { m_ClampDensity = val; }

bool NEVolumeMaterial::useLifetime() const { return m_UseLifetime; }

void NEVolumeMaterial::setUseLifetime(bool val) { m_UseLifetime = val; }

vec3f NEVolumeMaterial::volumeCubeMinPos() const {return m_VolumeCube.minPos();}

void NEVolumeMaterial::setVolumeCubeMinPos(const vec3f& v) {m_VolumeCube.setMin(v);}

vec3f NEVolumeMaterial::volumeCubeMaxPos() const {return m_VolumeCube.maxPos();}

void NEVolumeMaterial::setVolumeCubeMaxPos(const vec3f& v) {m_VolumeCube.setMax(v);}

NEVolumeMaterialFactory::NEVolumeMaterialFactory(NEManager* m) : NENodeFactory(m) {}

NEVolumeMaterialFactory::~NEVolumeMaterialFactory() {}

QString NEVolumeMaterialFactory::nodeName() { return "Volume Material"; }

uint NEVolumeMaterialFactory::nodeType(){ return NE::kObjectMaterial;}

QString NEVolumeMaterialFactory::nodeInherits() { return "Materials";}

QString NEVolumeMaterialFactory::objectName() { return "VolumeMaterial"; }

QString NEVolumeMaterialFactory::nodeVersion() { return "0.1"; }

QString NEVolumeMaterialFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEVolumeMaterialFactory::nodeHelp() { return "Volume material"; }

NEObject*NEVolumeMaterialFactory::createInstance() { return new NEVolumeMaterial(this); }
