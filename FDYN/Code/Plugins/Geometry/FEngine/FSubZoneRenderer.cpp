#include <QOpenGLFunctions_3_3_Compatibility>

#include "FSubZone.h"
#include "Mesh/NESpatialMesh.h"
#include "Rendering/Manager/NERenderManager.h"

#include "FSubZoneRenderer.h"
//
// Spatial Group
//

void FSubZoneRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    FSubZone* zone = qobject_cast<FSubZone*>(spatial);

    FZoneCompartmentRenderer::render( spatial, surface );
}

void FSubZoneRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    FZoneCompartmentRenderer::renderPickable( spatial, surface );
}

void FSubZoneRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    FZoneCompartmentRenderer::renderPicked( spatial, surface );
}

void FSubZoneRenderer::updateDrawingBuffers()
{
    FZoneCompartmentRenderer::updateDrawingBuffers();
}

FSubZoneRendererFactory::FSubZoneRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString FSubZoneRendererFactory::nodeName() { return "Fire Sub Zone Renderer"; }
NEObject *FSubZoneRendererFactory::createInstance() { return new FSubZoneRenderer(); }
