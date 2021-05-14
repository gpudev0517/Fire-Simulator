#pragma once

#include "Rendering/Spatial/NELight.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NEAreaLight))

class NECEXP_NERENDERING NEAreaLight : public NELight
{
private:
    Q_OBJECT
    NE_CLASS("Area Light")

public:


    NE_PROP_RW(double, Width, "Width", "Sets the width of the rectangular area light", true,)
    NE_PROP_RW(double, Height, "Height", "Sets the height of the rectangular area light", true,)
    NE_PROP_RW(int, Samples1, "Samples W", "The number of samples sent along the width", true,)
    NE_PROP_RW(int, Samples2, "Samples H", "The number of samples sent along the height", true,)
    NE_PROP_GROUP_START(ExternalRendering, "External Rendering", "Renderer specific settings", true)

    NE_PROP_GROUP_START(Povray, "Povray", "Povray specific settings", true)
    NE_PROP_RW(int, PovrayAdaptive, "Povray Adaptive", "Controls how much adaptive sampling is i=used", true,)
    NE_PROP_RW(bool, PovrayJitter, "Povray Jitter", "This command causes the positions of the point lights in the array to be randomly jittered to eliminate any shadow banding that may occur.", true,)
    NE_PROP_RW(bool, PovrayCircular , "Povray Circular", "Is used to create better circular soft shadows", true,)
    NE_PROP_GROUP_END(Povray)
    NE_PROP_GROUP_END(ExternalRendering)


public:
    NEAreaLight(NENodeFactory* factory);
    virtual ~NEAreaLight();
    uint init(uint initMode = NE::kInitOnCreate);

    virtual void updateUniforms(QOpenGLShaderProgram* program, int lightID);
protected:
    NE_DERIVED(NESpatial, Protected)

public slots:
    uint SetPosition(const vec3f& Position);
    const vec3f& direction() const;
    uint SetOrientation(const eulerf& rotation);


private:
    NE_DECL_PRIVATE(NEAreaLight)
};


#include "Base/NENodeFactory.h"

class NEAreaLightFactory : public NENodeFactory
{
public:
    NEAreaLightFactory(NEManager* m);
    ~NEAreaLightFactory();
    NE_DERIVED(NENodeFactory, Public)

    const NEIndexedTriMesh* mesh();

private:
    NEIndexedTriMesh* m_pMesh;
};
