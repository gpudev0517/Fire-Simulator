#include "FFireDetectRenderer.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Mesh/NESpatialMesh.h"
#include "FZoneItem.h"
#include "FFire.h"


//
// Zone Item Renderer
//

void FFireDetectRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    //gizmoPath = QDir::currentPath() + "//..//Code//FDYNUI//Resources//svg//inspector_scenario_01.svg";
    gizmoPath = ":/Resources/svg/inspector_scenario_01.svg";
    NESpatialMeshRenderer::render( spatial, surface );

}

void FFireDetectRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPickable(spatial, surface);
}

void FFireDetectRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPicked(spatial, surface);
}


void FFireDetectRenderer::updateDrawingBuffers()
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(m_spatial);
    if (rigid == NULL) return;

    if(rigid->triangleMesh().numTriangles() > 0)
        rigid->triangleMesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}

FFireDetectRendererFactory::FFireDetectRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}


QString FFireDetectRendererFactory::nodeName() { return "Fire Detection Renderer"; }

NEObject *FFireDetectRendererFactory::createInstance() { return new FFireDetectRenderer(); }
