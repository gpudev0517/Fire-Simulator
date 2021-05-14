#pragma once

#include "Base/NENodeFactory.h"
#include "Rendering/Resource/Material/NETransparentMaterial.h"

class NEFEXP_NERENDERING NEClearWaterMaterial : public NETransparentMaterial
{
    Q_OBJECT

    Q_PROPERTY(NEClearWaterPreset preset READ clearWaterType WRITE setClearWaterType)

    Q_PROPERTY(PropertyGroupStart MRMIAGeneral READ groupStart)
    Q_PROPERTY(bool defaultDiffuse READ defaultDiffuse WRITE setDefaultDiffuse)
    Q_PROPERTY(QColor diffuse READ diffuse WRITE setDiffuse)
    Q_PROPERTY(double diffWeight READ diffWeight WRITE setDiffWeight)
    Q_PROPERTY(double reflectivity READ reflectivity WRITE setReflectivity)
    Q_PROPERTY(double MRtransparency READ transparency WRITE setTransparency)
    Q_PROPERTY(bool MRfresnel READ fresnel WRITE setFresnel)
    Q_PROPERTY(bool RefractiveCaustics READ doRefractiveCaustics WRITE setDoRefractiveCaustics)
    Q_PROPERTY(PropertyGroupEnd MRMIAGeneral_End READ groupEnd)

    Q_PROPERTY(PropertyGroupStart MRMIAColoredRefract READ groupStart)
    Q_PROPERTY(bool refr_falloff READ refractionFalloff WRITE setRefractionFalloff)
    Q_PROPERTY(double refr_falloff_dist READ refrFalloffDist WRITE setRefrFalloffDist)
    Q_PROPERTY(bool use_refr_falloff_color READ useRefractionFalloffColor WRITE setUseRefractionFalloffColor)
    Q_PROPERTY(QColor refr_falloff_color READ refractionFalloffColor WRITE setRefractionFalloffColor)
    Q_PROPERTY(bool MRthinWalled READ thinWalled WRITE setThinWalled)
    Q_PROPERTY(PropertyGroupEnd MRMIAColoredRefract_End READ groupEnd)

    Q_PROPERTY(PropertyGroupStart MRDielectric READ groupStart)
    Q_PROPERTY(QColor outColor READ outColor WRITE setOutColor)
    Q_PROPERTY(double outIOR READ outIOR WRITE setOutIOR)
    Q_PROPERTY(double phongcoef READ phongCoef WRITE setPhongCoef)
    Q_PROPERTY(bool ignoreNormals READ ignoreNormals WRITE setIgnoreNormals)
    Q_PROPERTY(PropertyGroupEnd MRDielectric_End READ groupEnd)

    Q_CLASSINFO("help-preset", "Presets for render.")

    Q_ENUMS(NEClearWaterPreset)

public:

    enum NEClearWaterPreset
    {
        PovrayRadiosity,
        Dielectric
    };

    NEClearWaterMaterial(NENodeFactory* factory);
    virtual  ~NEClearWaterMaterial();


public slots:

    virtual  void updateUniforms();
    NEClearWaterPreset clearWaterType() const;
    void setClearWaterType(NEClearWaterPreset preset);

    float reflectivity() const;
    void setReflectivity(float val);

    float transparency() const;
    void setTransparency(float val);

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

    QColor diffuse() const;
    void setDiffuse(QColor val);

    float diffWeight() const;
    void setDiffWeight(float val);

    QColor outColor() const;
    void setOutColor(QColor val);

    float outIOR() const;
    void setOutIOR(float val);

    bool ignoreNormals() const;
    void setIgnoreNormals(bool val);

    float phongCoef() const;
    void setPhongCoef(float val);

    bool defaultDiffuse() const;
    void setDefaultDiffuse(bool val);

    bool doRefractiveCaustics() const;
    void setDoRefractiveCaustics(bool val);
protected:
    //!
    void setPresetValues();
    //!
    void setMiaWaterPreset();
    //!
    void setMiaColoredRefractPreset();
    //!
    void setMiaColoredDiffusePreset();
    //!
    void setDielectricPreset();

private:

    ///Mental Ray Related

    // MIA related
    //! Different presets can be set for the transparent material
    NEClearWaterPreset m_Preset;
    //! Diffuse color
    QColor m_Diffuse;
    //! Weight of the diffuse reflections
    float m_DiffWeight;
    //!
    bool m_DefaultDiffuse;

    //! Overall reflectivity level. Multiplied by the brdf xx degree refl parameters.
    float m_Reflectivity;
    //! The overall transparency level.
    float m_Transparency;
    //! When on, uses the Fresnel equation (based on IOR) for the reflectivity curve.
    bool m_Fresnel;
    //!
    bool m_DoRefractiveCaustics;

    //! Enable distance falloff for refractions (transparency).
    bool m_RefractionFalloff;
    //!
    bool m_UseRefractionFalloffColor;
    //!
    QColor m_RefractionFalloffColor;
    //!  The distance at where no transparency is seen.
    float m_RefrFalloffDist;
    //!  Treat surfaces as thin wafers of material, rather than the boundaries of solids.
    bool m_ThinWalled;


    // Dielectric related
    QColor m_OutColor;
    float m_outIOR;
    bool m_IgnoreNormals;
    float m_PhongCoef;

};


class NEClearWaterMaterialFactory : public NENodeFactory
{
    Q_INTERFACES(NENodeFactory)

public:
    NEClearWaterMaterialFactory(NEManager* m);
    ~NEClearWaterMaterialFactory();

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
