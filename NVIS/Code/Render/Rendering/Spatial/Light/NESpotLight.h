#pragma once

#include "Rendering/Spatial/NELight.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NESpotLight))

class IGLSurface;
class NECEXP_NERENDERING NESpotLight : public NELight
{
    Q_OBJECT
    NE_CLASS("Spot Light")

    NE_PROP_3D_RW(vec3f, float, LookAt, "Aim Position", "The position where the spotlight aimed at", true,)
    NE_PROP_RW(double, Cutoff, "Cutoff Angle", "The half-angle of the cone of the spotlight", true,)
    NE_PROP_RW(double, Exponent, "Exponent", "Controls the transition of umbra and penumbra of the light", true,)
    NE_PROP_RW(double, LinearAttenuation, "LinearAttenuation", "Linear Attenuation of the light", true,)
    NE_PROP_RW(double, QuadraticAttenuation, "Quadratic Attenuation", "Quadratic attenuation of the light source", true,)
    NE_PROP_GROUP_START(ExternalRendering, "External Rendering", "External renderer specific settings", true)

    NE_PROP_GROUP_START(Povray, "Povray", "Povray Specific Settings", true)
	NE_PROP_RW(double, PovrayRadius, "Povray Radius", "Radius of the spotlight cone", true,)
	NE_PROP_RW(int, PovrayAdaptive, "Povray Adaptive", "Controls how much adaptive sampling is used", true,)
	NE_PROP_RW(bool, PovrayJitter, "Povray Jitter", "This command causes the positions of the point lights in the array to be randomly jittered to eliminate any shadow banding that may occur.", true,)
	NE_PROP_GROUP_END(Povray)
    NE_PROP_GROUP_START(PBRT, "PBRT", "PBRT Specific Settings", true)
        NE_PROP_3D_RW(vec3f, float, PbrtColorI, "Color of the light", "Light Color", true,)
    NE_PROP_GROUP_END(PBRT)
    NE_PROP_RW(double, SoftAngle, "Soft Edge Angle", "Soft edge angle of the spot light cone", true,)
    NE_PROP_RW(double, BeamDistribution, "Beam Distribution", "Beam distribution, controling the umbra-penumbra transition", true,)
    NE_PROP_GROUP_END(ExternalRendering)

public:
    NESpotLight(NENodeFactory* factory);
    virtual ~NESpotLight();
    uint init(uint initMode = NE::kInitOnCreate);
    virtual void updateUniforms(IGLSurface* surface, QOpenGLShaderProgram* program, int lightID, bool isActive);
    virtual void propertyTweak(QtPropertyBase* p);


    NEIndexedTriMesh* mesh();


protected:
	NE_DERIVED(NESpatial, Protected)

public:

	void setDirection(vec3f val);
	vec3f direction() const;

public slots:
	uint SetOrientation(const eulerf& val);
	uint SetPosition(const vec3f& val);

private:

	NE_DECL_PRIVATE(NESpotLight)
};


#include "Base/NENodeFactory.h"

class NESpotLightFactory : public NENodeFactory
{
public:
	NESpotLightFactory(NEManager* m);
	virtual ~NESpotLightFactory();
	NE_DERIVED(NENodeFactory, Public)

	const NEIndexedTriMesh* mesh();

private:
	NEIndexedTriMesh* m_pMesh;
};
