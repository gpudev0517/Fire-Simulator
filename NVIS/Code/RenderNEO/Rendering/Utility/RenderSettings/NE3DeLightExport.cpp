#include "NE3DeLightExport.h"
#include "Rendering/Spatial/NECamera.h"
#include "Rendering/Spatial/NELight.h"
#include "Core/Physics/Spatial/Physical/Rigid/ParticleRigid/NESphRigid.h"
#if NE_DEFORMABLE
#include "Core/Physics/Spatial/Physical/Deformable/ParticleDeformable/NESphDeformable.h"
#endif
#include "Rendering/MeshConstructor/NEMarchingCubes.h"
#include "Core/Physics/Spatial/Physical/Fluid/NEParticleFluid.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"
#if NE_POST
#include "Rendering/PostProcessor/NEMeshPostProcessor.h"
#endif
#if NE_SPRAY
#include "Core/Physics/Spatial/Physical/Fluid/ParticleFluid/NEFoamSolver.h"
#include "Core/Physics/Spatial/Physical/Fluid/ParticleFluid/Foam/NESphFoam.h"
#endif

#include "Core/Base/NESceneManager.h"
#include "Rendering/Resource/NEMaterial.h"
#include "Core/Resource/Mesh/NEIndexedTriMesh.h"
#include "Core/Physics/Spatial/Physical/Fluid/ParticleFluid/NEFluidParticle.h"
#include "Core/Physics/Spatial/Physical/Fluid/ParticleFluid/NESphLiquidSolver.h"
#include "Rendering/Spatial/Light/NEDirectionalLight.h"
#include "Rendering/Spatial/Light/NESpotLight.h"
#include "Rendering/Spatial/Light/NEAreaLight.h"
#include "Rendering/Spatial/Light/NEPointLight.h"
#include "Rendering/Spatial/Light/NEImageBasedLight.h"

#include "Rendering/Resource/Material/NEClearWaterMaterial.h"
//#include "Resource/Material/NEGlassMaterial.h"
#include "Rendering/Resource/Material/NEOceanMaterial.h"
#include "Rendering/Resource/Material/NEPBRMaterial.h"
#include "Rendering/Resource/Material/NEPhongMaterial.h"
#include "Rendering/Resource/Material/NEStructureMaterial.h"
#include "Rendering/Resource/Material/NEVolumeMaterial.h"

#include "Rendering/Manager/NERenderManager.h"

void NE3DeLightExport::exportRib(const QString path,
                                 const NE3DeLightSettings* settings,
                                 QMap<QString,NECamera*> camerasMap,
                                 QMap<QString,NELight*> lightsMap,
                                 QMap<uint,NESpatial*> spatialsMap)
{
    bool motionBlur = settings->MotionBlur();
    std::vector<RtObjectHandle> areaLightPatchHandles;
    std::vector<QString> areaLightPatchNames;
    std::vector<RtObjectHandle> rigidHandles;
    std::vector<QString> rigidNames;
    QString pathWithRibExt = path+".rib";
    RiBegin(pathWithRibExt.toStdString().c_str());
    // Export area light objects
    foreach(NELight* light,lightsMap)
        if (NEAreaLight* areaLight = qobject_cast<NEAreaLight*>(light))
        {
            RtObjectHandle areaLightPatchHandle;
            areaLightPatchHandle = exportAreaLightPatchObject(areaLight);
            areaLightPatchHandles.push_back(areaLightPatchHandle);
            areaLightPatchNames.push_back(areaLight->objectName());
        }
    // Export rigid objects
    foreach(NESpatial* spatial,spatialsMap)
        if (NERigid* rigid = qobject_cast<NERigid*>(spatial))
        {
            RtObjectHandle rigidHandle;
            rigidHandle = exportRigidObject(rigid,motionBlur);
            rigidHandles.push_back(rigidHandle);
            rigidNames.push_back(rigid->objectName());
        }
    // Export display, camera and other options
    foreach(NECamera* camera,camerasMap)
        if (camera->objectName() == NERENDERSET.Camera())
        {
            exportOptions(path,camera,motionBlur);
            break;
        }
    RiWorldBegin();
    // Export lights
    foreach(NELight* light,lightsMap)
        if (light->ExternalRender())
        {
            if (NEAreaLight* areaLight = qobject_cast<NEAreaLight*>(light))
            {
                RiAttributeBegin();
                for (size_t i = 0; i != areaLightPatchNames.size(); i++)
                    if (areaLightPatchNames[i] == areaLight->objectName())
                    {
                        exportAreaLightPatchInstance(areaLight,areaLightPatchHandles[i]);
                        break;
                    }
                RtLightHandle areaLightHandle = exportAreaLight(areaLight);
                RiAttributeEnd();
                RiIlluminate(areaLightHandle,RI_TRUE);
            }
            else if (NEDirectionalLight* directionalLight = qobject_cast<NEDirectionalLight*>(light))
            {
                RiAttributeBegin();
                RtLightHandle directionalLightHandle = exportDirectionalLight(directionalLight);
                RiAttributeEnd();
                RiIlluminate(directionalLightHandle,RI_TRUE);
            }
            else if (NEPointLight* pointLight = qobject_cast<NEPointLight*>(light))
            {
                RiAttributeBegin();
                RtLightHandle pointLightHandle = exportPointLight(pointLight);
                RiAttributeEnd();
                RiIlluminate(pointLightHandle,RI_TRUE);
            }
            else if (NESpotLight* spotLight = qobject_cast<NESpotLight*>(light))
            {
                RiAttributeBegin();
                RtLightHandle spotLightHandle = exportSpotLight(spotLight);
                RiAttributeEnd();
                RiIlluminate(spotLightHandle,RI_TRUE);
            }
        }
    // Export meshes and fluid particles
    foreach(NESpatial* spatial,spatialsMap)
        if (spatial->ExternalRender())
        {
            if (NERigid* rigid = qobject_cast<NERigid*>(spatial))
            {
                RiAttributeBegin();
                for (size_t i = 0; i != rigidNames.size(); i++)
                    if (rigidNames[i] == rigid->objectName())
                    {
                        exportRigidInstance(rigid,rigidHandles[i],motionBlur);
                        break;
                    }
                RiAttributeEnd();
            }
            else if (NEDeformable* deformable = qobject_cast<NEDeformable*>(spatial))
            {
                RiAttributeBegin();
                exportDeformable(deformable,motionBlur);
                RiAttributeEnd();
            }
            else if (NEMeshConstructor* meshConstructor = qobject_cast<NEMeshConstructor*>(spatial))
            {
                RiAttributeBegin();
                exportFluidSurface(meshConstructor,motionBlur);
                RiAttributeEnd();
            }
            else if (NEMeshPostProcessor* meshPostProcessor = qobject_cast<NEMeshPostProcessor*>(spatial))
            {
                RiAttributeBegin();
                exportPostProcessedMesh(meshPostProcessor,motionBlur);
                RiAttributeEnd();
            }
            else if (NESphLiquidSolver* fluidSolver = qobject_cast<NESphLiquidSolver*>(spatial))
            {
                RiAttributeBegin();
                exportFluidParticles(fluidSolver,motionBlur);
                RiAttributeEnd();
            }
            else if (NESphFoam* foamSolver = qobject_cast<NESphFoam*>(spatial))
            {
                RiAttributeBegin();
                exportFoamParticles(foamSolver,motionBlur);
                RiAttributeEnd();
            }
    }
    RiWorldEnd();
    RiEnd();
    areaLightPatchHandles.clear();
    areaLightPatchNames.clear();
    rigidHandles.clear();
    rigidNames.clear();
}

void NE3DeLightExport::exportOptions(const QString path,
                                     const NECamera* camera,
                                     bool motionBlur)
{
    // Export display parameters
    QString pathWithFormatExt = path+".#r";
    RiDisplay(pathWithFormatExt.toStdString().c_str(),
              NERENDERSET.outputFormatToString(NERENDERSET.OutFileType()).toStdString().c_str(),
              "rgba",
              RI_NULL);
    RiFormat(NERENDERSET.Resolution().width(),
             NERENDERSET.Resolution().height(),
             1.0);
    // Export camera and viewing parameters
    RiClipping(camera->NearClip(),
               camera->FarClip());
    if (motionBlur)
    {
        float frameDuration = 1.0/NESCENE.RecordFrameRate();
        RiShutter(NESCENE.CurrentFrame(),NESCENE.CurrentFrame()+frameDuration);
        RtFloat offset = NESCENE.CurrentFrame();
        RiOption("shutter",
                 "offset",&offset,
                 RI_NULL);
    }
    RtFloat fov = camera->Fov();
    RiProjection(RI_PERSPECTIVE,
                 RI_FOV,&fov,
                 RI_NULL);
    RtMatrix viewTransform;
    viewTransform[0][0] = camera->viewMatrix().row(0).x();
    viewTransform[1][0] = camera->viewMatrix().row(0).y();
    viewTransform[2][0] = camera->viewMatrix().row(0).z();
    viewTransform[3][0] = camera->viewMatrix().row(0).w();
    viewTransform[0][1] = camera->viewMatrix().row(1).x();
    viewTransform[1][1] = camera->viewMatrix().row(1).y();
    viewTransform[2][1] = camera->viewMatrix().row(1).z();
    viewTransform[3][1] = camera->viewMatrix().row(1).w();
    viewTransform[0][2] = -camera->viewMatrix().row(2).x();
    viewTransform[1][2] = -camera->viewMatrix().row(2).y();
    viewTransform[2][2] = -camera->viewMatrix().row(2).z();
    viewTransform[3][2] = -camera->viewMatrix().row(2).w();
    viewTransform[0][3] = camera->viewMatrix().row(3).x();
    viewTransform[1][3] = camera->viewMatrix().row(3).y();
    viewTransform[2][3] = camera->viewMatrix().row(3).z();
    viewTransform[3][3] = camera->viewMatrix().row(3).w();
    RiTransform(viewTransform);
}

RtObjectHandle NE3DeLightExport::exportRigidObject(const NERigid *rigid,
                                                   bool motionBlur)
{
    std::vector<vec3f> velocities;
    if (motionBlur)
    {
        // Compute velocity of each rigid vertex
        std::vector<NECTNPVertex> vertices = rigid->triangleMesh().vertices();
        for (size_t i = 0; i != rigid->triangleMesh().numVertices(); i++)
        {
            vec3f position = vertices[i].position;
            vec3f velocity = rigid->LinearVelocity()+vec3f::crossProduct(rigid->AngularVelocity(),rigid->orientationQuat().rotatedVector(position));
            velocities.push_back(velocity);
        }
    }
    // Export rigid mesh object
    RtObjectHandle rigidHandle;
    rigidHandle = RiObjectBegin();
    if (rigid->ThreeDelightRenderingType() == NERigid::Polygons)
        exportMesh(&rigid->triangleMesh(),Polygons,rigid->Color(),rigid->ThreeDelightOpacity(),motionBlur,velocities);
    else if (rigid->ThreeDelightRenderingType() == NERigid::SubdivisionSurface)
        exportMesh(&rigid->triangleMesh(),SubdivisionSurface,rigid->Color(),rigid->ThreeDelightOpacity(),motionBlur,velocities);
    RiObjectEnd();
    return rigidHandle;
}

void NE3DeLightExport::exportRigidInstance(const NERigid* rigid,
                                           RtObjectHandle rigidHandle,
                                           bool motionBlur)
{
    // Export transform
    exportTransform(rigid,motionBlur);
    // Export material
    if (NEStructureMaterial* structureMaterial = qobject_cast<NEStructureMaterial*>(rigid->triangleMesh().material()))
        exportStructureMaterial(structureMaterial);
    // Export rigid mesh using object handle
    RiObjectInstance(rigidHandle);
}

void NE3DeLightExport::exportDeformable(const NEDeformable* deformable,
                                        bool motionBlur)
{
    // Export transform
    exportTransform(deformable,motionBlur);
    // Export material
    if (NEStructureMaterial* structureMaterial = qobject_cast<NEStructureMaterial*>(deformable->triangleMesh()->material()))
        exportStructureMaterial(structureMaterial);
    // Export deformable mesh
    RiSolidBegin(RI_PRIMITIVE);
    if (deformable->ThreeDelightRenderingType() == NEDeformable::Polygons)
        exportMesh(deformable->triangleMesh(),Polygons,deformable->Color(),deformable->ThreeDelightOpacity(),motionBlur,deformable->triangleMesh()->velocities());
    else if (deformable->ThreeDelightRenderingType() == NEDeformable::SubdivisionSurface)
        exportMesh(deformable->triangleMesh(),SubdivisionSurface,deformable->Color(),deformable->ThreeDelightOpacity(),motionBlur,deformable->triangleMesh()->velocities());
    RiSolidEnd();
}

void NE3DeLightExport::exportFluidSurface(const NEMeshConstructor* meshConstructor,
                                          bool motionBlur)
{
    // Export transform
    exportTransform(meshConstructor,motionBlur);
    // Export material
    if (NEStructureMaterial* structureMaterial = qobject_cast<NEStructureMaterial*>(meshConstructor->triangleMesh().material()))
        exportStructureMaterial(structureMaterial);
    // Export fluid surface mesh
    RiSolidBegin(RI_PRIMITIVE);
    if (meshConstructor->ThreeDelightRenderingType() == NEMeshConstructor::Polygons)
        exportMesh(&meshConstructor->triangleMesh(),Polygons,meshConstructor->Color(),meshConstructor->ThreeDelightOpacity(),motionBlur,meshConstructor->triangleMesh().velocities());
    else if (meshConstructor->ThreeDelightRenderingType() == NEMeshConstructor::SubdivisionSurface)
        exportMesh(&meshConstructor->triangleMesh(),SubdivisionSurface,meshConstructor->Color(),meshConstructor->ThreeDelightOpacity(),motionBlur,meshConstructor->triangleMesh().velocities());
    RiSolidEnd();
}

void NE3DeLightExport::exportPostProcessedMesh(const NEMeshPostProcessor* meshPostProcessor,
                                               bool motionBlur)
{
    // Export transform
    exportTransform(meshPostProcessor,motionBlur);
    // Export material
    if (NEStructureMaterial* structureMaterial = qobject_cast<NEStructureMaterial*>(meshPostProcessor->outputMesh()->material()))
        exportStructureMaterial(structureMaterial);
    // Export post-processed mesh
    RiSolidBegin(RI_PRIMITIVE);
    if (meshPostProcessor->ThreeDelightRenderingType() == NEMeshPostProcessor::Polygons)
        exportMesh(meshPostProcessor->outputMesh(),Polygons,meshPostProcessor->Color(),meshPostProcessor->ThreeDelightOpacity(),motionBlur,meshPostProcessor->outputMesh()->velocities());
    else if (meshPostProcessor->ThreeDelightRenderingType() == NEMeshPostProcessor::SubdivisionSurface)
        exportMesh(meshPostProcessor->outputMesh(),SubdivisionSurface,meshPostProcessor->Color(),meshPostProcessor->ThreeDelightOpacity(),motionBlur,meshPostProcessor->outputMesh()->velocities());
    RiSolidEnd();
}

void NE3DeLightExport::exportMesh(const NEIndexedTriMesh* mesh,
                                  NEMeshRenderingType renderingType,
                                  const QColor& color,
                                  const vec3f& opacity,
                                  bool motionBlur,
                                  const std::vector<vec3f>& velocities)
{
    float frameDuration = 1.0/NESCENE.RecordFrameRate();
    if (motionBlur)
        RiMotionBegin(2,0.0,frameDuration);
    // Convert the vertice id array to a zero-based vertice
    // id array, and get other necessary mesh data
    RtInt* nbVertices = new RtInt[mesh->numTriangles()];
    RtInt* verticesZeroBased = new RtInt[mesh->numTriangles()*3];
    std::vector<RtInt> verticeIds;
    for (size_t i = 0; i != mesh->numTriangles(); i++)
    {
        NETriangleIndexed triangle = mesh->triangles()[i];
        verticeIds.push_back(triangle.p1);
        verticeIds.push_back(triangle.p2);
        verticeIds.push_back(triangle.p3);
    }
    std::sort(verticeIds.begin(),verticeIds.end());
    std::vector<RtInt>::iterator it;
    it = std::unique(verticeIds.begin(),verticeIds.end());
    verticeIds.resize(std::distance(verticeIds.begin(),it));
    for (size_t i = 0; i != mesh->numTriangles(); i++)
    {
        size_t j = i*3;
        nbVertices[i] = 3;
        NETriangleIndexed triangle = mesh->triangles()[i];
        std::vector<RtInt>::iterator it;
        it = find(verticeIds.begin(),verticeIds.end(),triangle.p1);
        verticesZeroBased[j] = std::distance(verticeIds.begin(),it);
        it = find(verticeIds.begin(),verticeIds.end(),triangle.p2);
        verticesZeroBased[j+1] = std::distance(verticeIds.begin(),it);
        it = find(verticeIds.begin(),verticeIds.end(),triangle.p3);
        verticesZeroBased[j+2] = std::distance(verticeIds.begin(),it);
    }
    RtPoint* positions = new RtPoint[verticeIds.size()];
    RtNormal* normals = new RtNormal[verticeIds.size()];
    RtColor meshColor = {color.red()/255.0f,color.green()/255.0f,color.blue()/255.0f};
    RtColor meshOpacity = {opacity.x(),opacity.y(),opacity.z()};
    for (size_t i = 0; i != verticeIds.size(); i++)
    {
        NECTNPVertex vertex = mesh->vertices()[verticeIds[i]];
        positions[i][0] = vertex.position.x();
        positions[i][1] = vertex.position.y();
        positions[i][2] = vertex.position.z();
        normals[i][0] = vertex.normal.x();
        normals[i][1] = vertex.normal.y();
        normals[i][2] = vertex.normal.z();
    }
    // Export mesh as polygons
    if (renderingType == Polygons)
    {
        RiPointsPolygons(mesh->numTriangles(),
                         nbVertices,
                         verticesZeroBased,
                         RI_P,positions,
                         RI_N,normals,
                         "constant color Cs",&meshColor,
                         "constant color Os",&meshOpacity,
                         RI_NULL);
        if (motionBlur)
        {
            for (size_t i = 0; i != verticeIds.size(); i++)
            {
                vec3f velocity = velocities[verticeIds[i]];
                positions[i][0] += frameDuration*velocity.x();
                positions[i][1] += frameDuration*velocity.y();
                positions[i][2] += frameDuration*velocity.z();
            }
            RiPointsPolygons(mesh->numTriangles(),
                             nbVertices,
                             verticesZeroBased,
                             RI_P,positions,
                             RI_N,normals,
                             "constant color Cs",&meshColor,
                             "constant color Os",&meshOpacity,
                             RI_NULL);
        }
    }
    // Export mesh as subdivision surface
    else if (renderingType == SubdivisionSurface)
    {
        RtInt nTags = 1;
        RtToken interpolateBoundary[1] = {"interpolateboundary"};
        RtInt nArgs[2] = {1,0};
        RtInt intArgs[1] = {1};
        RtFloat floatArgs[1] = {};
        RiSubdivisionMesh("catmull-clark",
                          mesh->numTriangles(),
                          nbVertices,
                          verticesZeroBased,
                          nTags, interpolateBoundary,
                          nArgs, intArgs, floatArgs,
                          RI_P,positions,
                          RI_N,normals,
                          "constant color Cs",&meshColor,
                          "constant color Os",&meshOpacity,
                          RI_NULL);
        if (motionBlur)
        {
            for (size_t i = 0; i != verticeIds.size(); i++)
            {
                vec3f velocity = velocities[verticeIds[i]];
                positions[i][0] += frameDuration*velocity.x();
                positions[i][1] += frameDuration*velocity.y();
                positions[i][2] += frameDuration*velocity.z();
            }
            RiSubdivisionMesh("catmull-clark",
                              mesh->numTriangles(),
                              nbVertices,
                              verticesZeroBased,
                              nTags, interpolateBoundary,
                              nArgs, intArgs, floatArgs,
                              RI_P,positions,
                              RI_N,normals,
                              "constant color Cs",&meshColor,
                              "constant color Os",&meshOpacity,
                              RI_NULL);
        }
    }
    delete[] nbVertices;
    delete[] verticesZeroBased;
    delete[] positions;
    delete[] normals;
    if (motionBlur)
        RiMotionEnd();
}

void NE3DeLightExport::exportTransform(const NESpatial* spatial,
                                       bool motionBlur)
{
    float frameDuration = 1.0/NESCENE.RecordFrameRate();
    // Export translation
    if (motionBlur)
        RiMotionBegin(2,0.0,frameDuration);
    RiTranslate(spatial->Positionx(),
                spatial->Positiony(),
                spatial->Positionz());
    if (motionBlur)
    {
     RiTranslate(spatial->Positionx(),
                 spatial->Positiony(),
                 spatial->Positionz());
     RiMotionEnd();
    }
    // Export rotations
    if (motionBlur)
        RiMotionBegin(2,0.0,frameDuration);
    RiRotate(spatial->Orientation().psi(),
             0.0,0.0,1.0);
    if (motionBlur)
    {
        RiRotate(spatial->Orientation().psi(),
                 0.0,0.0,1.0);
        RiMotionEnd();
    }
    if (motionBlur)
        RiMotionBegin(2,0.0,frameDuration);
    RiRotate(spatial->Orientation().theta(),
             0.0,1.0,0.0);
    if (motionBlur)
    {
        RiRotate(spatial->Orientation().theta(),
                 0.0,1.0,0.0);
        RiMotionEnd();
    }
    if (motionBlur)
        RiMotionBegin(2,0.0,frameDuration);
    RiRotate(spatial->Orientation().phi(),
             1.0,0.0,0.0);
    if (motionBlur)
    {
        RiRotate(spatial->Orientation().phi(),
                 1.0,0.0,0.0);
        RiMotionEnd();
    }
    // Export scaling
    if (motionBlur)
        RiMotionBegin(2,0.0,frameDuration);
    RiScale(spatial->Scalex(),
            spatial->Scaley(),
            spatial->Scalez());
    if (motionBlur)
    {
        RiScale(spatial->Scalex(),
                spatial->Scaley(),
                spatial->Scalez());
        RiMotionEnd();
    }
}

void NE3DeLightExport::exportFluidParticles(const NESphLiquidSolver* fluidSolver,
                                            bool motionBlur)
{
    exportParticles(fluidSolver,fluidSolver->RestDistance(),motionBlur);
}

void NE3DeLightExport::exportFoamParticles(const NESphFoam* foamSolver,
                                           bool motionBlur)
{
    exportParticles(foamSolver,2*foamSolver->particleSize(),motionBlur);
}

void NE3DeLightExport::exportParticles(const NEParticleFluid* fluid,
                                       float particleDiameter,
                                       bool motionBlur)
{
    float frameDuration = 1.0/NESCENE.RecordFrameRate();
    if (motionBlur)
        RiMotionBegin(2,0.0,frameDuration);
    const NEVectorMetaData& particles = *(fluid->particlesVector());
    if (fluid->ThreeDelightRenderingType() == NEParticleFluid::Points ||
        fluid->ThreeDelightRenderingType() == NEParticleFluid::Disks ||
        fluid->ThreeDelightRenderingType() == NEParticleFluid::Spheres)
    {
        // Get necessary fluid particle data
        RtPoint* positions = new RtPoint[particles.objectCount];
        RtColor* colors = new RtColor[particles.objectCount];
        RtColor* opacities = new RtColor[particles.objectCount];
        size_t i = 0;
        for_all(NEFluidParticle,particle,particles)
        {
            positions[i][0] = particle->position().x();
            positions[i][1] = particle->position().y();
            positions[i][2] = particle->position().z();
            colors[i][0] = particle->color().red/255.0;
            colors[i][1] = particle->color().green/255.0;
            colors[i][2] = particle->color().blue/255.0;
            opacities[i][0] = fluid->ThreeDelightOpacity().x();
            opacities[i][1] = fluid->ThreeDelightOpacity().y();
            opacities[i][2] = fluid->ThreeDelightOpacity().z();
            i++;
        }
        RtFloat width = particleDiameter;
        RtString uniformStringType;
        if (fluid->ThreeDelightRenderingType() == NEParticleFluid::Points)
            uniformStringType = {"particule"};
        else if (fluid->ThreeDelightRenderingType() == NEParticleFluid::Disks)
            uniformStringType = {"disk"};
        else if (fluid->ThreeDelightRenderingType() == NEParticleFluid::Spheres)
            uniformStringType = {"sphere"};
        // Export fluid particles as either points, disks or spheres
        RiPoints(particles.objectCount,
                 RI_P,positions,
                 RI_CS,colors,
                 RI_OS,opacities,
                 RI_CONSTANTWIDTH,&width,
                 "uniform string type", &uniformStringType,
                 RI_NULL);
        if (motionBlur)
        {
            i = 0;
            for_all(NEFluidParticle,particle,particles)
            {
                positions[i][0] += frameDuration*particle->velocity().x();
                positions[i][1] += frameDuration*particle->velocity().y();
                positions[i][2] += frameDuration*particle->velocity().z();
                i++;
            }
            RiPoints(particles.objectCount,
                     RI_P,positions,
                     RI_CS,colors,
                     RI_OS,opacities,
                     RI_CONSTANTWIDTH,&width,
                     "uniform string type", &uniformStringType,
                     RI_NULL);
        }
        delete[] positions;
        delete[] colors;
        delete[] opacities;
    }
    else if (fluid->ThreeDelightRenderingType() == NEParticleFluid::Blobbies)
    {
        // Get necessary fluid particle data
        RtInt* code = new RtInt[particles.objectCount*3+2];
        RtFloat* floats = new RtFloat[particles.objectCount*16];
        RtString strings[1] = {""};
        RtColor* colors = new RtColor[particles.objectCount];
        RtColor* opacities = new RtColor[particles.objectCount];
        RtFloat twiceParticleDiameter = 2*particleDiameter;
        size_t i,j = 0;
        for_all(NEFluidParticle,particle,particles)
        {
            code[2*i] = 1001;
            code[2*i+1] = j;
            floats[j+0] = twiceParticleDiameter;
            floats[j+1] = 0.0;
            floats[j+2] = 0.0;
            floats[j+3] = 0.0;
            floats[j+4] = 0.0;
            floats[j+5] = twiceParticleDiameter;
            floats[j+6] = 0.0;
            floats[j+7] = 0.0;
            floats[j+8] = 0.0;
            floats[j+9] = 0.0;
            floats[j+10] = twiceParticleDiameter;
            floats[j+11] = 0.0;
            floats[j+12] = particle->position().x();
            floats[j+13] = particle->position().y();
            floats[j+14] = particle->position().z();
            floats[j+15] = 1.0;
            colors[i][0] = particle->color().red/255.0;
            colors[i][1] = particle->color().green/255.0;
            colors[i][2] = particle->color().blue/255.0;
            opacities[i][0] = fluid->ThreeDelightOpacity().x();
            opacities[i][1] = fluid->ThreeDelightOpacity().y();
            opacities[i][2] = fluid->ThreeDelightOpacity().z();
            i++;
            j += 16;
        }
        code[particles.objectCount*2] = 0;
        code[particles.objectCount*2+1] = particles.objectCount;
        size_t offset = particles.objectCount*2+2;
        for (size_t k = 0; k != particles.objectCount; k++)
            code[offset+k] = k;
        // Export particles as blobbies
        RiBlobby(particles.objectCount,
                 particles.objectCount*3+2,code,
                 particles.objectCount*16,floats,
                 0,strings,
                 RI_CS,colors,
                 RI_OS,opacities,
                 RI_NULL);
        if (motionBlur)
        {
            j = 0;
            for_all(NEFluidParticle,particle,particles)
            {
                floats[j+12] += frameDuration*particle->velocity().x();
                floats[j+13] += frameDuration*particle->velocity().y();
                floats[j+14] += frameDuration*particle->velocity().z();
                j += 16;
            }
            RiBlobby(particles.objectCount,
                     particles.objectCount*3+2,code,
                     particles.objectCount*16,floats,
                     0,strings,
                     RI_CS,colors,
                     RI_OS,opacities,
                     RI_NULL);
        }
        delete[] code;
        delete[] floats;
        delete[] colors;
        delete[] opacities;
    }
    if (motionBlur)
        RiMotionEnd();
}

RtObjectHandle NE3DeLightExport::exportAreaLightPatchObject(const NEAreaLight *light)
{
    RtObjectHandle areaLightPatchHandle;
    areaLightPatchHandle = RiObjectBegin();
    RtPoint positions[4];
    positions[0][0] = -0.5*light->Width();
    positions[0][1] = 0.0;
    positions[0][2] = -0.5*light->Height();
    positions[1][0] = 0.5*light->Width();
    positions[1][1] = 0.0;
    positions[1][2] = -0.5*light->Height();
    positions[2][0] = -0.5*light->Width();
    positions[2][1] = 0.0;
    positions[2][2] = 0.5*light->Height();
    positions[3][0] = 0.5*light->Width();
    positions[3][1] = 0.0;
    positions[3][2] = 0.5*light->Height();
    RiPatch(RI_BILINEAR,
            RI_P,&positions,
            RI_NULL);
    RiObjectEnd();
    return areaLightPatchHandle;
}

void NE3DeLightExport::exportAreaLightPatchInstance(const NEAreaLight *light,
                                                    RtObjectHandle areaLightPatchHandle)
{
    exportTransform(light,false);
    RiObjectInstance(areaLightPatchHandle);
}

RtLightHandle NE3DeLightExport::exportAreaLight(const NEAreaLight* light)
{
    RtFloat intensity = light->ThreeDelightIntensity();
    RtColor color = {light->Color().red()/255.0f,light->Color().green()/255.0f,light->Color().blue()/255.0f};
    return RiAreaLightSource("finite",
                             RI_INTENSITY,&intensity,
                             RI_LIGHTCOLOR,&color,
                             RI_NULL);
}

RtLightHandle NE3DeLightExport::exportSpotLight(const NESpotLight* light)
{
    RtFloat intensity = light->ThreeDelightIntensity();
    RtColor color = {light->Color().red()/255.0f,light->Color().green()/255.0f,light->Color().blue()/255.0f};
    RtPoint from = {light->Positionx(),light->Positiony(),light->Positionz()};
    RtPoint to = {light->LookAtx(),light->LookAty(),light->LookAtz()};
    RtFloat coneAngle = light->Cutoff();
    RtFloat coneDeltaAngle = light->ThreeDelightSoftAngle();
    RtFloat beamDistribution = light->ThreeDelightBeamDistribution();
    return RiLightSource(RI_SPOTLIGHT,
                         RI_INTENSITY,&intensity,
                         RI_LIGHTCOLOR,&color,
                         RI_FROM,&from,
                         RI_TO,&to,
                         RI_CONEANGLE,&coneAngle,
                         RI_CONEDELTAANGLE,&coneDeltaAngle,
                         RI_BEAMDISTRIBUTION,&beamDistribution,
                         RI_NULL);
}

RtLightHandle NE3DeLightExport::exportDirectionalLight(const NEDirectionalLight* light)
{
    RtFloat intensity = light->ThreeDelightIntensity();
    RtColor color = {light->Color().red()/255.0f,light->Color().green()/255.0f,light->Color().blue()/255.0f};
    RtPoint from = {light->Positionx(),light->Positiony(),light->Positionz()};
    vec3f vecTo = light->Position()-light->Direction();
    RtPoint to = {vecTo.x(),vecTo.y(),vecTo.z()};
    return RiLightSource(RI_DISTANTLIGHT,
                         RI_INTENSITY,&intensity,
                         RI_LIGHTCOLOR,&color,
                         RI_FROM,&from,
                         RI_TO,&to,
                         RI_NULL);
}

RtLightHandle NE3DeLightExport::exportPointLight(const NEPointLight* light)
{
    RtFloat intensity = light->ThreeDelightIntensity();
    RtColor color = {light->Color().red()/255.0f,light->Color().green()/255.0f,light->Color().blue()/255.0f};
    RtPoint from = {light->Positionx(),light->Positiony(),light->Positionz()};
    return RiLightSource(RI_POINTLIGHT,
                         RI_INTENSITY,&intensity,
                         RI_LIGHTCOLOR,&color,
                         RI_FROM,&from,
                         RI_NULL);
}

void NE3DeLightExport::exportStructureMaterial(const NEStructureMaterial *structureMaterial)
{
    RtToken shaderName;
    RtFloat kA;
    if (structureMaterial->ThreeDelightMaterialType() == NEStructureMaterial::Matte)
       {
        shaderName = RI_MATTE;
        kA = structureMaterial->KAmbient();
        RtFloat kD = structureMaterial->KDiffuse();
        RiSurface(shaderName,
                  RI_KA,&kA,
                  RI_KD,&kD,
                  RI_NULL);
        }
    else if (structureMaterial->ThreeDelightMaterialType() == NEStructureMaterial::Metal)
    {
        shaderName = RI_METAL;
        kA = structureMaterial->KAmbient();
        RtFloat kS = structureMaterial->KSpecular();
        RtFloat roughness = structureMaterial->ThreeDelightRoughness();
        RiSurface(shaderName,
                  RI_KA,&kA,
                  RI_KS,&kS,
                  RI_ROUGHNESS,&roughness,
                  RI_NULL);
    }
    else if (structureMaterial->ThreeDelightMaterialType() == NEStructureMaterial::Plastic)
    {
        shaderName = RI_PLASTIC;
        kA = structureMaterial->KAmbient();
        RtFloat kD = structureMaterial->KDiffuse();
        RtFloat kS = structureMaterial->KSpecular();
        RtFloat roughness = structureMaterial->ThreeDelightRoughness();
        RtColor specularColor = {structureMaterial->ThreeDelightSpecularColor().red()/255.0f,structureMaterial->ThreeDelightSpecularColor().green()/255.0f,structureMaterial->ThreeDelightSpecularColor().blue()/255.0f};
        RiSurface(shaderName,
                  RI_KA,&kA,
                  RI_KD,&kD,
                  RI_KS,&kS,
                  RI_ROUGHNESS,&roughness,
                  RI_SPECULARCOLOR,&specularColor,
                  RI_NULL);
    }
}








