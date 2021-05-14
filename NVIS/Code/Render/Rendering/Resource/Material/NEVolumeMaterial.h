#pragma once

#include "Rendering/Resource/NEMaterial.h"


/*
        The volume material renders a set of particle volumetrically.
        Currently, this is not done in OpenGL Preview but in external renderer only.
*/
class NEVolumeMaterial : public NEMaterial
{
    Q_OBJECT
    Q_PROPERTY(double unit_density READ uintDensity WRITE setUnitDensity)
    Q_PROPERTY(double max_stepsize READ maxStepsize WRITE setMaxStepsize)
    Q_PROPERTY(double min_stepsize READ minStepsize WRITE setMinStepsize)
    Q_PROPERTY(VolumeShaderMethods density_shader READ densityShaderMethod WRITE setDensityShaderMethod)
    //Q_PROPERTY(VolumeRayMarchMethod ray_marcher READ rayMarcher WRITE setRayMarcher)
    Q_PROPERTY(VolumeStepsizing stepsizing READ stepsizing WRITE setStepsizing)
    Q_PROPERTY(VolumeLightEnergyMethod light_model READ lightModel WRITE setLightModel)


    //the sharp shade is one of two input shaders used by the adaptive path tracer
    //currently both shaders are put here
    Q_PROPERTY(PropertyGroupStart Shader READ groupStart)
    Q_PROPERTY(int max_nr_elements READ maxNrElements WRITE setMaxNrElements)
    //Q_PROPERTY(int distanceexponent READ distanceExponent WRITE setDistanceExponent)
    Q_PROPERTY(double spray_radius READ sprayRadius WRITE setSprayRadius)
    Q_PROPERTY(double surface_radius READ surfaceRadius WRITE setSurfaceRadius)
    Q_PROPERTY(double surface_neighbor_weighting READ surfaceNeighborWeighting WRITE setSurfaceNeighborWeighting)
    Q_PROPERTY(double spray_weighting READ sprayWeighting WRITE setSprayWeighting)
    Q_PROPERTY(bool clamp_density READ clampDensity WRITE setClampDensity)
    Q_PROPERTY(bool use_lifetime READ useLifetime WRITE setUseLifetime)
    Q_PROPERTY(VolumeKernel kernel READ kernel WRITE setKernel)
    //Q_PROPERTY(int kernelexponent READ kernelExponent WRITE setKernelExponent)
    //Q_PROPERTY(VolumeKernel valuemethod READ valueMethod WRITE setValueMethod)
    Q_PROPERTY(ShaderMethod shadermethod READ shaderMethod WRITE setShaderMethod)
    Q_PROPERTY(PropertyGroupEnd Shader_End READ groupEnd)

    Q_ENUMS(VolumeShaderMethods)
    Q_ENUMS(VolumeRayMarchMethod)
    Q_ENUMS(VolumeStepsizing)
    Q_ENUMS(VolumeLightEnergyMethod)

    Q_ENUMS(VolumeKernel)
    Q_ENUMS(ShaderMethod)
public:
    //Enumeration definition
    enum VolumeShaderMethods
    {
        Debug = 0,
        Combined_Smooth,
        Smooth =3,
        Sharp = 4,
        Interpolated = 5,
        Weighted_Sum =6
    };

    enum VolumeRayMarchMethod
    {
        Random = 0,
        Deterministic
    };

    enum VolumeStepsizing
    {
        Constant = 0,
        Adaptive
    };

    enum VolumeKernel
    {
        Distance = 1,
        CubicSpline = 2,
        Gaussian = 3,
        ConstantValue = 4,
        NumElements = 5
    };

    enum VolumeLightEnergyMethod
    {
        VLE_Vath = 0,
        VLE_Debug = 1,
        VLE_AlphaCompose = 2,
        VLE_TransferFunction = 3
    };

    //FoamLookupShaderRelated
    enum ShaderMethod
    {
        Ihmsen = 0,
        Vath = 1
    };

    NEFEXP_NERENDERING NEVolumeMaterial(NENodeFactory* factory);
    virtual NEFEXP_NERENDERING ~NEVolumeMaterial();
    virtual NEFEXP_NERENDERING void updateUniforms();

    //! Property tweak is overwritten from NENode in order to disable some unused properties for volume shader.
    virtual void propertyTweak(QtPropertyBase* p);

public slots:
    float uintDensity() const;
    void setUnitDensity(float val);

    float minStepsize() const;
    void setMinStepsize(float val);

    float maxStepsize() const;
    void setMaxStepsize(float val);

    VolumeShaderMethods densityShaderMethod() const;
    void setDensityShaderMethod(VolumeShaderMethods val);

    //VolumeRayMarchMethod rayMarcher() const { return m_RayMarchMethod; }
    //void setRayMarcher(VolumeRayMarchMethod val) { m_RayMarchMethod = val; }

    VolumeStepsizing stepsizing() const;
    void setStepsizing(VolumeStepsizing val);

    VolumeLightEnergyMethod lightModel() const;
    void setLightModel(VolumeLightEnergyMethod model);

    //Sharp shader specific
    VolumeKernel kernel() const;
    void setKernel(VolumeKernel val);

    VolumeKernel valueMethod() const;
    void setValueMethod(VolumeKernel val);

    ShaderMethod shaderMethod() const;
    void setShaderMethod(ShaderMethod val);

    //float maxRadius() const { return m_MaxRadius; }
    //void setMaxRadius(float val) { m_MaxRadius = val; }
    float surfaceRadius() const;
    void setSurfaceRadius(float val);

    float sprayRadius() const;
    void setSprayRadius(float val);

    float surfaceNeighborWeighting() const;
    void setSurfaceNeighborWeighting(float val);

    float sprayWeighting() const;
    void setSprayWeighting(float val);

    int maxNrElements() const;
    void setMaxNrElements(int val);

    int distanceExponent() const;
    void setDistanceExponent(int val);

    int kernelExponent() const;
    void setKernelExponent(int val);

    bool clampDensity() const;
    void setClampDensity(bool val);

    bool useLifetime() const;
    void setUseLifetime(bool val);

    vec3f volumeCubeMinPos() const;
    void setVolumeCubeMinPos(const vec3f& v);

    vec3f volumeCubeMaxPos() const;
    void setVolumeCubeMaxPos(const vec3f& v);


protected:

private:

    //!
    float m_UnitDensity;
    //! The maximal stepsize used for raymarching.
    float m_MaxStepsize;
    //! The minimal stepsize used for raymarching.
    float m_MinStepsize;
    //!
    VolumeShaderMethods m_DensityShaderMethod;
    //!
    VolumeRayMarchMethod m_RayMarchMethod;
    //!
    VolumeStepsizing m_Stepsizing;
    //!
    VolumeLightEnergyMethod m_LightModel;
    //!
    AABB m_VolumeCube;

    //Sharp shader specific
    //!
    VolumeKernel m_Kernel;
    //!
    VolumeKernel m_ValueMethod;
    //!
    bool m_ClampDensity;
    //!
    bool m_UseLifetime;
    //!
    int m_KernelExponent;
    //!
    int m_MaxNrElements;
    //!
    int m_DistanceExponent;
    //!
    float m_MaxRadius;
    //!
    float m_SprayRadius;
    //!
    float m_SurfaceRadius;
    //!
    float m_SurfaceNeighborWeighting;
    //!
    float m_SprayWeighting;
    //!
    ShaderMethod m_ShaderMethod;
    //! Povray variables
};


#include "Base/NENodeFactory.h"

class NEVolumeMaterialFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEVolumeMaterialFactory(NEManager* m);
    ~NEVolumeMaterialFactory();

    QString nodeName();
    uint nodeType();
    QString nodeInherits();
    QString objectName();
    QString nodeVersion();
    QString nodeVendor();
    QString nodeHelp();

private:
    NEObject* createInstance();
};
