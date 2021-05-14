#pragma once

#include "Base/NENodeFactory.h"
#include "Rendering/Resource/Material/NETransparentMaterial.h"

class NECEXP_NERENDERNEO NEParticleFluidMaterial : public NETransparentMaterial
{
    Q_OBJECT

    NE_PROP_GROUP_START(FluidSurface, "Fluid Surface Settings", "Fluid Surface Settings", true)
    NE_PROP_RW(double, SmoothingRadius, "Smoothing Radius", "Smoothing Radius", true,)
    NE_PROP_RW(double, MaxRadialSamples, "Max Radial Samples", "Maximum samples for surface", true,)
    NE_PROP_RW(double, DepthEdgeFalloff, "Depth Edge Falloff", "Falloff for Blur at the edges", true,)
    NE_PROP_RW(double, DepthBlurFalloff, "Depth Blur Falloff", "Falloff for blur for depth", true,)
    NE_PROP_RW(double, ThicknessParticleScale, "Thickness Scale", "Scale for thickness", true,)
    NE_PROP_RW(double, DepthParticleScale, "Depth Scale", "Depth Scale", true,)
    NE_PROP_GROUP_END(FluidSurface)

    NE_PROP_GROUP_START(FluidRendering, "Fluid Render Settings", "Fluid Render Settings", true)
    NE_PROP_RW(double, FresnelStrength, "Fresnel Strength", "Intensity scale for fresnel effect", true,)
    NE_PROP_RW(int, RenderOptLevel, "Renderer Optimization Level", "Allows rendering a specified portion of the particles for preview purposes", true,)
    NE_PROP_RW(bool, WaterRendering, "Surface Rendering", "Screen Space Fluid Rendering", true,)
    NE_PROP_GROUP_END(FluidRendering)

    NE_PROP_RW(bool, FlatShading, "Flat Shading", "Flat Shading of Material", true,)


public:

    NEParticleFluidMaterial(NENodeFactory* factory);
    virtual ~NEParticleFluidMaterial();

    virtual void updateUniforms();
    virtual MaterialType materialType();


protected:
    virtual bool addSource(NENode* node, NEConn& conn);
    virtual void removeSource(NENode* node, uint type, uint sIndex, uint tIndex);

private:
    /// Radius for smoothing the surface. Higher values result in a smoother surface.
    double m_smoothingRadius;
    /// This parameter controls how many neighboring pixels in each direction are taken into account for the screen space smoothing operation.
    /// It limits how large the smoothing filter defined through the
    /// Smoothing Radiu can become in screen space.
    /// Large values can have an impact on performance.
    double m_maxRadialSamples;
    /// The depth edge falloff controls smoothing across inner silhouettes.
    /// If the depth separation between particles exceeds the size of the particles
    /// smoothing should be turned off to avoid blending between foreground and background.
    /// The depth edge falloff controls how much smoothing is tuned down based on the depth separation. A value of 0.0 disables all smoothing.
    double m_depthEdgeFalloff;
    double m_depthBlurFalloff;
    /// Translucent surfaces look more convincing if the background becomes less visible where the liquid is thicker.
    /// The screen space surface technique provides a thickness value to the material used to render the surface to achieve such an effect.
    /// The thickness value is computed using an additive blend between the particles rendered as spheres.
    /// The size of the spheres is controlled by this parameter.
    double m_thicknessParticleScale;
    /// The size of the emitted particles influences the distance of the rendered surface to the particles.
    /// Larger particle sizes result in a surface that makes the liquid appear to have more volume.
    /// The Depth Particle Scale controls the particle sizes influencing the shape of the surface.
    double m_depthParticleScale;

    /// The fresnel brightness strength (0~1)
    double m_fresnelStrength;
    int m_RenderOptLevel;
    bool m_WaterRendering;

    bool m_flatShading;
};


class NEParticleFluidMaterialFactory : public NENodeFactory
{
	Q_INTERFACES(NENodeFactory)

public:
    NEParticleFluidMaterialFactory(NEManager* m);
    ~NEParticleFluidMaterialFactory();

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
