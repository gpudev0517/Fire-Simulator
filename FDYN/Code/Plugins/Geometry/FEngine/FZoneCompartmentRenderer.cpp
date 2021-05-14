#include <QOpenGLFunctions_3_3_Compatibility>

#include "FZoneCompartmentRenderer.h"
#include "FZoneCompartment.h"
#include "FZone.h"
#include "Mesh/NESpatialMesh.h"
#include "Rendering/Manager/NERenderManager.h"


//
// Spatial Group
//

void FZoneCompartmentRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEZoneCompartmentRenderer::render( spatial, surface );
}

void FZoneCompartmentRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NEZoneCompartmentRenderer::renderPickable( spatial, surface );
}

void FZoneCompartmentRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NEZoneCompartmentRenderer::renderPicked( spatial, surface );
}

void FZoneCompartmentRenderer::updateDrawingBuffers()
{
    FZoneCompartment* comp = qobject_cast<FZoneCompartment*>(m_spatial);
    if (comp == NULL) return;

    if(comp->triangleMesh().numTriangles() > 0)
        comp->triangleMesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}

FZoneCompartmentRendererFactory::FZoneCompartmentRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString FZoneCompartmentRendererFactory::nodeName() { return "Fire Zone Boundary Renderer"; }
NEObject *FZoneCompartmentRendererFactory::createInstance() { return new FZoneCompartmentRenderer(); }
