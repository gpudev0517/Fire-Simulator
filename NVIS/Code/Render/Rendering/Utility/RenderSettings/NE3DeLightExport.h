#include "Core/Utility/Singleton.h"
#include "Rendering/Utility/RenderSettings/NERenderSetting.h"
#include "Core/Spatial/NESpatial.h"
#include "3DeLight/ri.h"
#include "Rendering/Utility/RenderSettings/NE3DeLightSettings.h"

class NE3DeLightSettings;
class NECamera;
class NELight;
class NEAreaLight;
class NEDirectionalLight;
class NEPointLight;
class NESpotLight;
class NEImageBasedLight;
class NERigid;
class NEDeformable;
class NEMeshConstructor;
class NEMeshPostProcessor;
class NEParticleFluid;
class NESphLiquidSolver;
class NESphFoam;
class NEIndexedTriMesh;
class NEMaterial;
class NEStructureMaterial;

class NE3DeLightExport : public Singleton<NE3DeLightExport>
{
public:
    enum NEMeshRenderingType {Polygons = 0, SubdivisionSurface = 1};

    void exportRib(const QString path,
                   const NE3DeLightSettings* settings,
                   QMap<QString,NECamera*> camerasMap,
                   QMap<QString,NELight*> lightsMap,
                   QMap<uint,NESpatial*> spatialsMap);

private:
    void exportOptions(const QString path,
                       const NECamera* camera,
                       bool motionBlur);
    RtObjectHandle exportRigidObject(const NERigid* rigid,
                                     bool motionBlur);
    void exportRigidInstance(const NERigid* rigid,
                             RtObjectHandle rigidHandle,
                             bool motionBlur);
    void exportDeformable(const NEDeformable* deformable,
                          bool motionBlur);
    void exportFluidSurface(const NEMeshConstructor* meshConstructor,
                            bool motionBlur);
    void exportPostProcessedMesh(const NEMeshPostProcessor* meshPostProcessor,
                                 bool motionBlur);
    void exportMesh(const NEIndexedTriMesh* mesh,
                    NEMeshRenderingType renderingType,
                    const QColor& color,
                    const vec3f& opacity,
                    bool motionBlur,
                    const std::vector<vec3f>& velocities);
    void exportTransform(const NESpatial* spatial,
                         bool motionBlur);
    void exportFluidParticles(const NESphLiquidSolver* fluidSolver,
                              bool motionBlur);
    void exportFoamParticles(const NESphFoam* foamSolver,
                             bool motionBlur);
    void exportParticles(const NEParticleFluid* fluid,
                         float particleDiameter,
                         bool motionBlur);
    RtObjectHandle exportAreaLightPatchObject(const NEAreaLight* light);
    void exportAreaLightPatchInstance(const NEAreaLight* light,
                                      RtObjectHandle areaLightPatchHandle);
    RtLightHandle exportAreaLight(const NEAreaLight* light);
    RtLightHandle exportDirectionalLight(const NEDirectionalLight* light);
    RtLightHandle exportPointLight(const NEPointLight* light);
    RtLightHandle exportSpotLight(const NESpotLight* light);
    void exportStructureMaterial(const NEStructureMaterial* structureMaterial);
};

#define NE3DELIGHTEXP NE3DeLightExport::Instance()
