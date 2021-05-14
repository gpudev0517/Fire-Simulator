#include "NERenderNEOManager.h"


#include "Base/NESceneManager.h"

#include "Physics/Spatial/NEMeasurementField.h"
#if NE_GRIDFLUID
#include "Grid/Physics/Spatial/Physical/Fluid/NEGridFluid.h"
#endif
#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NESphRigid.h"
#include "Spatial/NESpatialGroup.h"
#include "Base/NEBase.h"
#include "Utility/neutrino_config.h"

#include "PostProcessor/NEMeshPostProcessor.h"

#include "Rendering/Spatial/NEOSDMeasurement.h"
#include "Rendering/Spatial/NEOSDParticleFluid.h"
#include "Rendering/Spatial/NEOSDParticleRigid.h"
#include "Rendering/Spatial/NEOSDRigidTopography.h"

#include "Rendering/Manager/NEGLManager.h"

#include "Rendering/Resource/Material/NEParticleFluidMaterial.h"


#include "Rendering/Spatial/Renderers/NEParticleFluidSolverRenderer.h"
#include "Rendering/Spatial/Renderers/NERigidTopographyRenderer.h"
#include "Rendering/Spatial/Renderers/NEMeasurementFieldRenderer.h"
#include "Rendering/Spatial/Renderers/NEMeshPostProcessorRenderer.h"


#if NE_COUPLING
#include "Rendering/Spatial/Renderers/NECouplerRenderer.h"
//#include "Rendering/Spatial/NEHeightFieldSourceRenderer.h"
#include "Rendering/Spatial/Renderers/NELinkRenderer.h"
#endif

#include "Rendering/Spatial/NELight.h"
#include "Rendering/Spatial/NECamera.h"

#include "Rendering/Utility/NEOsd.h"
#include "Rendering/Utility/RenderSettings/NERenderSetting.h"
#include "Rendering/Utility/RenderSettings/NEPovrayExport.h"


using namespace std;

class NERenderNEOManagerPrivate
{
public:
    NERenderNEOManagerPrivate(NERenderNEOManager* s) : S(s)
    {

    }

    void loadStatics();

    NEMaterial* m_pFluidMaterial;       /**< Pointer to the defaut material */

    void initBasicSceneElements();

private:
    NERenderNEOManager* S;
};



NERenderNEOManager::NERenderNEOManager(): NERenderManager()
{
    P = new NERenderNEOManagerPrivate(this);
}

NERenderNEOManager::~NERenderNEOManager()
{
}

uint NERenderNEOManager::init()
{
    NERenderManager::init();

    NEOSD.Instance();

    P->loadStatics();

    return NE::kReturnSuccess;
}

void NERenderNEOManagerPrivate::initBasicSceneElements()
{
    NESCENE.createObject("Fluid Material", "DefaultFluidMaterial");
    m_pFluidMaterial    = S->materials()["DefaultFluidMaterial"];
    m_pFluidMaterial->setStatic(true);
}


void NERenderNEOManagerPrivate::loadStatics()
{
    EXEC_IF(1, S->addStaticToMaps( new NEOSDMeasurementFactory(S)) );
    EXEC_IF(1, S->addStaticToMaps( new NEOSDParticleFluidFactory(S)) );
    EXEC_IF(1, S->addStaticToMaps( new NEOSDParticleRigidFactory(S)) );

    EXEC_IF(1, S->addRendererToMaps( new NERigidTopographyRendererFactory(S)) );
    EXEC_IF(1, S->addStaticToMaps( new NEOSDRigidTopographyFactory(S)) );

    EXEC_IF(1, S->addRendererToMaps( new NEMeasurementFieldRendererFactory(S)) );

    EXEC_IF(1, S->addRendererToMaps( new NEOSDParticleFluidRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NEOSDParticleRigidRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NEOSDMeasurementRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NEOSDRigidTopographyRendererFactory(S)) );

    EXEC_IF(1, S->addStaticToMaps(new NEParticleFluidMaterialFactory(S)));

    EXEC_IF(1, S->addRendererToMaps( new NEMeshPostProcessorRendererFactory(S)) );
#if NE_COUPLING
    EXEC_IF(1, S->addRendererToMaps( new NEInflowCouplerRendererFactory(S)) );
    //EXEC_IF(1, S->addRendererToMaps( new NEHeightFieldSourceRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NELinkRendererFactory(S)) );
#endif
}

void NERenderNEOManager::initBasicSceneElements()
{
    NERenderManager::initBasicSceneElements();
    P->initBasicSceneElements();
}
