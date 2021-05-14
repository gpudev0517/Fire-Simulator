#include "FEngineRenderer.h"

#include "Rendering/Spatial/Renderers/NESpatialGroupRenderer.h"
#include "Rendering/Manager/NERenderManager.h"



//
// Spatial Group
//

void FEngineRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialGroupRenderer::render( spatial, surface );
}

void FEngineRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialGroupRenderer::renderPickable( spatial, surface );
    //render(spatial, surface);
}

void FEngineRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialGroupRenderer::renderPicked( spatial, surface );
    //render(spatial, surface);
}


FEngineRendererFactory::FEngineRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}
QString FEngineRendererFactory::nodeName() { return "Fire Engine Renderer"; }
NEObject *FEngineRendererFactory::createInstance() { return new FEngineRenderer(); }
