#pragma once

#include "Utility/FilePath.h"
#include "Rendering/Spatial/NELight.h"

NE_FWD_DECL_2(class, FilePath, NE_PRIVATE(NEImageBasedLight))

class NECEXP_NERENDERING NEImageBasedLight : public NELight
{
    Q_OBJECT
    NE_CLASS("Image Based Light Settings")

public:
    enum RenderPreset
    {
        defaultPreset = 1,
        PovrayPreset = 2,
    };

private:
    NE_PROP_RW(RenderPreset, Preset, "Preset", "Render preset", true,)
    NE_PROP_RW(FilePath, ImgName, "File Path", "Chose the psth of the file to be loaded", true,)
    NE_PROP_RW(double, Rotate, "Rotation", "Sets the rotation amount for the spherically mapped image", true,)
    NE_PROP_GROUP_START(ExternalRendering, "External Rendering", "External renderer specific settings", true)
    NE_PROP_GROUP_START(Mentalray, "Mentalray", "Mentalray specific settings", true)
    NE_PROP_RW(double, Ambiance, "Ambiance", "Sets the ambient amount", true,)
    NE_PROP_RW(double, Emission, "'Emission", "Sets the emission amount", true,)
    NE_PROP_GROUP_END(Mentalray)
    NE_PROP_GROUP_START(Povray, "Povray", "Povray specific settings", true)
    NE_PROP_RW(int, SphSize, "Sph Size Path", "Sets the Sph size", true,)
    NE_PROP_RW(int, MapType, "Map Type", "Sets the type of mapping", true,)
    NE_PROP_RW(int, Interpolate, "Interpolate", "Sets the interpolation type", true,)
    NE_PROP_RW(double, Diffuse, "Diffuse", "Set the diffuse amount of the light", true,)
    NE_PROP_3D_RW(vec3f, float, RotDirection, "Rotation Direction", "Sets the rotation direction", true,)
    NE_PROP_GROUP_END(Povray)
    NE_PROP_GROUP_END(ExternalRendering)

    Q_ENUMS(RenderPreset)

public:
    NEImageBasedLight(NENodeFactory* factory);
    virtual ~NEImageBasedLight();
    uint init(uint initMode = NE::kInitOnCreate);
    virtual void updateUniforms(QOpenGLShaderProgram* program, int lightID);
    virtual void propertyTweak(QtPropertyBase* p);

    uint SetPosition(const vec3f& val);

protected:
    void callPreset();
    void callDefaultPreset();
    void callMentalrayPreset();
    void callPovrayPreset();

    NE_DERIVED(NESpatial, Protected)

private:
    NE_DECL_PRIVATE(NEImageBasedLight)

};


#include "Base/NENodeFactory.h"

class NEImageBasedLightFactory : public NENodeFactory
{
public:
    NEImageBasedLightFactory(NEManager* m);
    ~NEImageBasedLightFactory();
    NE_DERIVED(NENodeFactory, Public)

    const NEIndexedTriMesh* mesh();

private:
    NEIndexedTriMesh* m_pMesh;
};
