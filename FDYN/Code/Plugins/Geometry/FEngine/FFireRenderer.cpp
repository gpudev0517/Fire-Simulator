#include "FFireRenderer.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Mesh/NESpatialMesh.h"
#include "FZoneItem.h"
#include "FFire.h"


//
// Zone Item Renderer
//

void FFireRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    //gizmoPath = QDir::currentPath() + "//..//Code//FDYNUI//Resources//svg//inspector_scenario_01.svg";
    gizmoPath = ":/Resources/svg/inspector_scenario_01.svg";
    NESpatialMeshRenderer::render( spatial, surface );

}

void FFireRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPickable(spatial, surface);
}

void FFireRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPicked(spatial, surface);
}


void FFireRenderer::updateDrawingBuffers()
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(m_spatial);
    if (rigid == NULL) return;

    if(rigid->triangleMesh().numTriangles() > 0)
        rigid->triangleMesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}

FFireRendererFactory::FFireRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}


QString FFireRendererFactory::nodeName() { return "Fire Definition Renderer"; }

NEObject *FFireRendererFactory::createInstance() { return new FFireRenderer(); }
