#pragma once

#include "Rendering/Spatial/NELight.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NEPointLight))

class NECEXP_NERENDERING NEPointLight : public NELight
{
    Q_OBJECT
    NE_CLASS("Point Light")
    NE_PROP_RW(double, LinearAttenuation, "Linear Attenuation", "Linear attenuationof the light source", true,)
    NE_PROP_RW(double, QuadraticAttenuation, "Quadratic Attenuation", "Quadratic attenuation of the light source", true,)
    NE_PROP_GROUP_START(ExternalRendering, "External Rendering", "External renderer specific settings", true)
    NE_PROP_GROUP_END(ExternalRendering)

public:
    NEPointLight(NENodeFactory* factory);
    virtual ~NEPointLight();
    uint init(uint initMode = NE::kInitOnCreate);

    void render();
    void renderPicked();
    void renderPickable();

    const NEIndexedTriMesh* mesh() const;

    virtual void updateUniforms(QOpenGLShaderProgram* program, int lightID, bool isActive);
    virtual void propertyTweak(QtPropertyBase* p);


public slots:
    uint SetPosition(const vec3f& val);

private:
    NE_DECL_PRIVATE(NEPointLight)
};


#include "Base/NENodeFactory.h"

class NEPointLightFactory : public NENodeFactory
{
public:
    NEPointLightFactory(NEManager* m);
    ~NEPointLightFactory();
    NE_DERIVED(NENodeFactory, Public)

    const NEIndexedTriMesh* mesh();

private:
    NEIndexedTriMesh* m_pMesh;
};
