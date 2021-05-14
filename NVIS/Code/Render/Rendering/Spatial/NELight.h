#pragma once

#include "Spatial/NESpatial.h"

NE_FWD_DECL_2(class, NERenderSetting, NE_PRIVATE(NELight))

#define SHADOWMAP_SIZE 2048

class NECEXP_NERENDERING NELight : public NESpatial
{
    Q_OBJECT
    NE_CLASS("General Light Settings")

public:

    //! Possible node states.
    enum NEShadowMode { NoShadow = 0, HardShadow = 1, SoftShadow = 2 };
    enum NEShadowMapSize { T1024 = 1, T2048, T4096, T8192, T16384};

    NE_PROP_GROUP_START(ExternalRendering, "External Rendering", "External renderer specific settings", true)

    NE_PROP_GROUP_START(Povray, "Povray", "Povray specific settings", true)
    NE_PROP_3D_RW(vec3f, float, PovrayColor, "Color", "Defines the color of the light", true,)
    NE_PROP_GROUP_END(Povray)
    NE_PROP_GROUP_END(ExternalRendering)
    NE_PROP_RW(double, LightIntensity, "Light Intensity", "Intensity multiplier for Lights - default 1.0" , true,)
    NE_PROP_GROUP_START(ShadowSetting, "Shadow Setting", "Setting Shadow mode and Shadow Color", true)
    NE_PROP_RW(NEShadowMapSize, ShadowMapSize, "Shadow Map Size", "Sets the size of shadow map size(for example : if 1, real size is 1024.)" , true,)
    NE_PROP_RW(NEShadowMode, ShadowMode, "Shadow Mode", "Sets the Shadow mod", true,)
    NE_PROP_RW(QColor, ShadowColor, "Shadow Color", "Sets the Shadow Color", true,)
    NE_PROP_RW(double, LightSize, "Light Size", "Sets the size of light for SoftShadow" , true,)
    NE_PROP_GROUP_END(ShadowSetting)

    Q_ENUMS(NEShadowMode)
    Q_ENUMS(NEShadowMapSize)

public:
    NELight(NENodeFactory* factory);
    virtual ~NELight();

    bool isManipulatable() const;

    virtual void resetProperties();
    virtual void updateUniforms(QOpenGLShaderProgram* program, int lightID);
    virtual void propertyTweak(QtPropertyBase* p);


    uint SetBehavior(const NENodeState& val);

    void bindShadowTexture();
    void bindShadowFramebuffer();
    void setDepthBiasMVP(matrix44f depthBiasMVP);
    matrix44f getDepthBiasMVP();
    void setViewMatrix(matrix44f viewMatrix);
    matrix44f getViewMatrix();
    void setNear(float fNear);
    float getNear();
    void setFar(float fFar);
    float getFar();
    void setLightRadiusUV(vec2f uv);
    vec2f getLightRadiusUV();

    uint m_tmpColorBuffer;

    const QColor LightColor(); //!< Get light color with intensity multiplied.
    int getShadowMapSize();

    void updateShadowTexture();

    void setNeedShadowUpdate(bool bUpdate){
        m_bUpdate = bUpdate;
    }

    bool needShadowUpdate(){
        return m_bUpdate;
    }

private:
    NE_DECL_PRIVATE(NELight)

    void ShadowCreate();
    void ShadowDestroy();

    bool m_bUpdate;
};
