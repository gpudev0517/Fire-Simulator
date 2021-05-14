#pragma once

#include "Rendering/Resource/Material/NETransparentMaterial.h"

class NEFEXP_NERENDERING NEOceanMaterial : public NETransparentMaterial
{
    Q_OBJECT

    Q_PROPERTY(NEOceanPreset preset READ oceanType WRITE setOceanType)

    Q_PROPERTY(PropertyGroupStart MRReflectivity READ groupStart)
    Q_PROPERTY(double reflectivity READ reflectivity WRITE setReflectivity)
    Q_PROPERTY(QColor refl_color READ reflectivityColor WRITE setReflectivityColor)
    Q_PROPERTY(double refl_cutoff READ reflectivityCutoff WRITE setReflectivityCutoff)
    Q_PROPERTY(bool refl_falloff READ reflectionFalloff WRITE setReflectionFalloff)
    Q_PROPERTY(double refl_falloff_dist READ reflFalloffDist WRITE setReflFalloffDist)
    Q_PROPERTY(bool use_refl_falloff_color READ useReflectionFalloffColor WRITE setUseReflectionFalloffColor)
    Q_PROPERTY(QColor refl_falloff_color READ reflectionFalloffColor WRITE setReflectionFalloffColor)
    Q_PROPERTY(PropertyGroupEnd MRReflectivity_End READ groupEnd)

    Q_PROPERTY(PropertyGroupStart MRRefractivity READ groupStart)
    Q_PROPERTY(QColor refr_color READ refractionColor WRITE setRefractionColor)
    Q_PROPERTY(bool refr_falloff READ refractionFalloff WRITE setRefractionFalloff)
    Q_PROPERTY(double refr_falloff_dist READ refrFalloffDist WRITE setRefrFalloffDist)
    Q_PROPERTY(bool use_refr_falloff_color READ useRefractionFalloffColor WRITE setUseRefractionFalloffColor)
    Q_PROPERTY(QColor refr_falloff_color READ refractionFalloffColor WRITE setRefractionFalloffColor)
    Q_PROPERTY(bool refractiveCaustics READ refractiveCaustics WRITE setRefractiveCaustics)
    Q_PROPERTY(PropertyGroupEnd MRRefractivity_End READ groupEnd)

    Q_PROPERTY(PropertyGroupStart MRDiffuse READ groupStart)
    Q_PROPERTY(QColor diffuse READ diffuse WRITE setDiffuse)
    Q_PROPERTY(double diffWeight READ diffWeight WRITE setDiffWeight)
    Q_PROPERTY(bool noDiffBump READ noDiffBump WRITE setNoDiffBump)
    Q_PROPERTY(PropertyGroupEnd MRDiffuse_End READ groupEnd)

    Q_PROPERTY(PropertyGroupStart MRPhysicalSky READ groupStart)
    Q_PROPERTY(double haze READ physicalSkyHaze WRITE setPhysicalSkyHaze)
    Q_PROPERTY(double redblueshift READ physicalSkyRedBlueShift WRITE setPhysicalSkyRedBlueShift)
    Q_PROPERTY(PropertyGroupEnd MRPhysicalSky_End READ groupEnd)

    Q_PROPERTY(double MRtransparency READ transparency WRITE setTransparency)
    Q_PROPERTY(bool MRfresnel READ fresnel WRITE setFresnel)
    Q_PROPERTY(bool MRthinWalled READ thinWalled WRITE setThinWalled)

    Q_CLASSINFO("help-preset", "Presets.")


    Q_ENUMS(NEOceanPreset)

public:

    enum NEOceanPreset
    {
        Ocean,
        TropicalOcean
    };

     NEOceanMaterial(NENodeFactory* factory);

public slots:

    NEOceanPreset oceanType() const;
    void setOceanType(NEOceanPreset preset);

    float reflectivity() const;
    void setReflectivity(float val);

    QColor reflectivityColor() const;
    void setReflectivityColor(QColor val);

    float reflectivityCutoff() const;
    void setReflectivityCutoff(float val);

    bool reflectionFalloff() const;
    void setReflectionFalloff(bool val);

    float reflFalloffDist() const;
    void setReflFalloffDist(float val);

    QColor reflectionFalloffColor() const;
    void setReflectionFalloffColor(QColor val);

    bool useReflectionFalloffColor() const;
    void setUseReflectionFalloffColor(bool val);

    float refractivityCutoff() const;
    void setRefractivityCutoff(float val);

    float transparency() const;
    void setTransparency(float val);

    QColor refractionColor() const;
    void setRefractionColor(QColor val);

    bool refractionFalloff() const;
    void setRefractionFalloff(bool val);

    float refrFalloffDist() const;
    void setRefrFalloffDist(float val);

    QColor refractionFalloffColor() const;
    void setRefractionFalloffColor(QColor val);

    bool useRefractionFalloffColor() const;
    void setUseRefractionFalloffColor(bool val);

    bool fresnel() const;
    void setFresnel(bool val);

    bool thinWalled() const;
    void setThinWalled(bool val);

    bool refractiveCaustics() const;
    void setRefractiveCaustics(bool val);

    QColor diffuse() const;
    void setDiffuse(QColor val);

    float diffWeight() const;
    void setDiffWeight(float val);

    bool noDiffBump() const;
    void setNoDiffBump(bool val);

    float physicalSkyHaze() const;
    void setPhysicalSkyHaze(float val);

    float physicalSkyRedBlueShift() const;
    void setPhysicalSkyRedBlueShift(float val);

protected:
    //!
    void setPresetValues();
    //!
    void setOceanPreset();
    //!
    void setTropicalOceanPreset();


private:

    ///Mental Ray Related
    //! Different presets can be set for the transparent material
    NEOceanPreset m_Preset;
    //! Overall reflectivity level. Multiplied by the brdf xx degree refl parameters.
    float m_Reflectivity;
    //! Overall reflectivity color. Default white.
    QColor m_ReflectivityColor;
    //! The importance cutoff for reflections. Rays that contribute less than this threshold are ignored.
    float m_ReflectivityCutoff;
    //!
    bool m_ReflectionFalloff;
    //!
    bool m_UseReflectionFalloffColor;
    //!  The distance at ..
    float m_ReflFalloffDist;
    //!
    QColor m_ReflectionFalloffColor;

    //!
    float m_RefractivityCutoff;
    //! The overall transparency level.
    float m_Transparency;
    //! The transparency (refraction) color.
    QColor m_RefractionColor;
    //! Enable distance falloff for refractions (transparency).
    bool m_RefractionFalloff;
    //!
    bool m_UseRefractionFalloffColor;
    //!
    QColor m_RefractionFalloffColor;
    //!  The distance at where no transparency is seen.
    float m_RefrFalloffDist;
    //! When on, uses the Fresnel equation (based on IOR) for the reflectivity curve.
    bool m_Fresnel;
    //!  Treat surfaces as thin wafers of material, rather than the boundaries of solids.
    bool m_ThinWalled;
    //!
    bool m_RefractiveCaustics;
    //!
    QColor m_Diffuse;
    //!
    float m_DiffWeight;
    //
    bool m_NoDiffBump;

    float m_PhysicalSkyHaze;
    float m_PhysicalSkyRedBlueShift;

};


#include "Base/NENodeFactory.h"

class NEOceanMaterialFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEOceanMaterialFactory(NEManager* m);
    ~NEOceanMaterialFactory();

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
