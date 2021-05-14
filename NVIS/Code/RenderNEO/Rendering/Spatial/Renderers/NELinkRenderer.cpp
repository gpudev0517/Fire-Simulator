#include "NELinkRenderer.h"

#include "Physics/Spatial/Physical/NELink.h"
#include "Rendering/Manager/NERenderManager.h"


//
// Links
//



// Link Gizmo
void NELinkRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NELink* gizmo = qobject_cast<NELink*>(spatial);

    if(!gizmo || gizmo->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    GL->glDisable(GL_DEPTH_TEST);
    GL->glLineWidth( 1.0f );

    GL->glBegin( GL_LINES );
    GL->glVertex3f( -1, 0, 0 );
    GL->glVertex3f( 1, 0, 0 );

    GL->glVertex3f( 0, -1, 0 );
    GL->glVertex3f( 0, 1, 0 );

    GL->glVertex3f( 0, 0, -1 );
    GL->glVertex3f( 0, 0, 1 );
    GL->glEnd();

    GL->glPopAttrib();
}

void NELinkRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    render(spatial, surface);
}

void NELinkRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    GL->glEnable(GL_LINE_STIPPLE);
    GL->glLineStipple(4, 0xaaaa);

    render(spatial, surface);

    GL->glPopAttrib();
}

NELinkRendererFactory::NELinkRendererFactory(NEManager* m) : NERendererFactory( m )
{
}

QString NELinkRendererFactory::nodeName() { return "Link Renderer"; }

NEObject *NELinkRendererFactory::createInstance() { return new NELinkRenderer(); }

