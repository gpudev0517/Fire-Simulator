#include "NEBoundedForceRenderer.h"

#include "Plugins/ForceFields/BoundedForce/NEBoundedForce.h"

//
//  Bounded Force
//
void NEBoundedForceRenderer::render(NESpatial* spatial, IGLWidget* widget)
{
    NEBoundedForce *bforce = qobject_cast< NEBoundedForce *>( spatial );

    if( !bforce )
        return;

    if( bforce->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;


    glLineWidth(2.0f);
    GLRENDERER.drawRaw( bforce->aabb() );
    glLineWidth(1.0f);

}

void NEBoundedForceRenderer::renderPicked(NESpatial* spatial, IGLWidget* widget)
{
    render( spatial, widget );
}

void NEBoundedForceRenderer::renderPickable(NESpatial* spatial, IGLWidget* widget)
{
    render( spatial, widget );
}



NEBoundedForceRendererFactory::NEBoundedForceRendererFactory(NEManager* m) : NERendererFactory( m )
{
}

QString NEBoundedForceRendererFactory::nodeName() { return "Bounded Force Renderer"; }
NESpatialRenderer* NEBoundedForceRendererFactory::createInstance() { return new NEBoundedForceRenderer(); }
