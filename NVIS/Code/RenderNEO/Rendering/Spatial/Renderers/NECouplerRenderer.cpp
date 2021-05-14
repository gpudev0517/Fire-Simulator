#include "NECouplerRenderer.h"
#include "Physics/Spatial/Coupling/NECoupler.h"
#include "Physics/Spatial/Coupling/Coupler/NEInflowCoupler.h"
#include "Rendering/Manager/NERenderManager.h"

//
// Inflow Coupler Rendering
//

void NECouplerRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NECoupler *coupler = qobject_cast< NECoupler *>( spatial );

    if( !coupler )
	    return;

    if( coupler->RenderMode() == NESpatial::NERenderMode::Invisible )
	    return;

    GL->glLineWidth(2.0f);
    // For each connexion of the connexion graph, draw a line

    GL->glBegin( GL_LINES );
    for (size_t i = 0; i != coupler->vecListConnectedComponents().size(); i++)
    {
        std::list<size_t>::const_iterator itBegin = (coupler->vecListConnectedComponents())[i].begin();
        std::list<size_t>::const_iterator itEnd = (coupler->vecListConnectedComponents()[i]).end();
        std::list<size_t>::const_iterator itLast = std::prev((coupler->vecListConnectedComponents()[i]).end());
        for (std::list<size_t>::const_iterator it = itBegin; it != itEnd; it++)
        {
            vec2f pos;
            if (*it < coupler->vecConvertedGauges().size())
                pos = coupler->vecConvertedGauges()[*it];
            else
                pos = coupler->vecConvertedExtremPoints()[*it - coupler->vecConvertedGauges().size()];
            GL->glVertex3f( pos.x(),0.0,pos.y() );

            // There is no need in duplicating a vertex at the chain extremity

            if (it != itBegin && it != itLast)
                GL->glVertex3f( pos.x(),0.0,pos.y() );
        }
    }
    GL->glEnd();
    // For each gauge, draw an orange cross
    GL->glColor3f( 1.0,0.5,0.0 );
    GL->glBegin( GL_LINES );
    for (size_t i = 0; i != coupler->vecConvertedGauges().size(); i++)
    {
            vec2f pos = coupler->vecConvertedGauges()[i];
            GL->glVertex3f( pos.x() - coupler->partSizeOverScaleX(), 0.0, pos.y() );
            GL->glVertex3f( pos.x() + coupler->partSizeOverScaleZ(), 0.0, pos.y() );
            GL->glVertex3f( pos.x(), 0.0, pos.y() - coupler->partSizeOverScaleX() );
            GL->glVertex3f( pos.x(), 0.0, pos.y() + coupler->partSizeOverScaleZ() );
    }
    GL->glEnd();
    // For each extremity point, draw an purple cross
    GL->glColor3f(1.0,0.0,0.5);
    GL->glBegin(GL_LINES);
    for (size_t i = 0; i != coupler->vecConvertedExtremPoints().size(); i++)
    {
            vec2f pos = coupler->vecConvertedExtremPoints()[i];
            GL->glVertex3f( pos.x() - coupler->partSizeOverScaleX(), 0.0, pos.y() );
            GL->glVertex3f( pos.x() + coupler->partSizeOverScaleZ(), 0.0, pos.y() );
            GL->glVertex3f( pos.x(), 0.0, pos.y() - coupler->partSizeOverScaleX() );
            GL->glVertex3f( pos.x(), 0.0, pos.y() + coupler->partSizeOverScaleZ() );
    }
    GL->glEnd();
}

void NECouplerRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    render( spatial, surface );
}

void NECouplerRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    render( spatial, surface );
}

void NEInflowCouplerRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEInflowCoupler *coupler = qobject_cast< NEInflowCoupler *>( spatial );

    if( !coupler )
	    return;

    if( coupler->RenderMode() == NESpatial::NERenderMode::Invisible )
	    return;

    NECouplerRenderer::render( spatial, surface );

    // For each source draw a cyan cross
    GL->glLineWidth(2.0f);
    GL->glColor3f(0.0,1.0,1.0);
    GL->glBegin(GL_LINES);
    for (size_t i = 0; i != coupler->vecSourcePositions().size(); i++)
    {
            vec2f pos = vec2f(coupler->vecSourcePositions()[i].x(),coupler->vecSourcePositions()[i].z());
            GL->glVertex3f(pos.x()-coupler->halfPartSizeOverScale(),0.0,pos.y());
            GL->glVertex3f(pos.x()+coupler->halfPartSizeOverScale(),0.0,pos.y());
            GL->glVertex3f(pos.x(),0.0,pos.y()-coupler->halfPartSizeOverScale());
            GL->glVertex3f(pos.x(),0.0,pos.y()+coupler->halfPartSizeOverScale());
    }
    GL->glEnd();
}

void NEInflowCouplerRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    render( spatial, surface );
}

void NEInflowCouplerRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    render( spatial, surface );
}

NEInflowCouplerRendererFactory::NEInflowCouplerRendererFactory(NEManager* m) : NERendererFactory( m )
{
}

QString NEInflowCouplerRendererFactory::nodeName() { return "Inflow Coupler Renderer"; }

NEObject *NEInflowCouplerRendererFactory::createInstance() { return new NEInflowCouplerRenderer(); }

