#include "NEMCRenderer.h"

#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NERenderNEOManager.h"
#include "Rendering/NEGLRenderer.h"

#include "PostProcessor/NEMarchingCubes.h"


//
// MeshConstructor
//

void NEMarchingCubesRenderer::initObject()
{
    NEMeshConstructor* rigid = qobject_cast<NEMeshConstructor*>(m_spatial);
    if (rigid == NULL) return;

    if( NERENDERMAN.GLManager()->material( const_cast< NEIndexedTriMesh *>( &( rigid->triangleMesh() ) ) ) == 0 )
    {
        NEMaterial* defaultMaterial = NERENDERMAN.materials()["DefaultMaterial"];
        rigid->connectTo( reinterpret_cast<NENode*>(defaultMaterial), NE::kIOTriangleMesh, 0, 0 );
    }
}


void NEMarchingCubesRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEMarchingCubes* marchingCubes = qobject_cast<NEMarchingCubes*>(spatial);

    if(marchingCubes && marchingCubes->RenderMode() != NESpatial::NERenderMode::Invisible &&
            NERENDERMAN.GLManager()->isBuffered( &marchingCubes->mesh() ) )
    {
        if(marchingCubes->RenderMode() == NESpatial::NERenderMode::Wireframe)
        {
            GL->glPushAttrib(GL_POLYGON_BIT);
            GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            NERENDERMAN.GLManager()->drawRaw(&marchingCubes->mesh(), surface, false, marchingCubes->DisplayMode() == NESpatial::BoundingBox);
            GL->glPopAttrib();
        }
    }
}

void NEMarchingCubesRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NEMarchingCubes* marchingCubes = qobject_cast<NEMarchingCubes*>(spatial);

    if(marchingCubes && marchingCubes->RenderMode() != NESpatial::NERenderMode::Invisible &&
            NERENDERMAN.GLManager()->isBuffered( &marchingCubes->mesh() ) )
    {
        if(marchingCubes->RenderMode() == NESpatial::NERenderMode::Wireframe)
        {
            GL->glPushAttrib(GL_POLYGON_BIT);
            GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            NERENDERMAN.GLManager()->drawRaw(&marchingCubes->mesh(), surface, false, marchingCubes->DisplayMode() == NESpatial::BoundingBox);
            GL->glPopAttrib();
        }
        else
            NERENDERMAN.GLManager()->draw(&marchingCubes->mesh(), surface, marchingCubes->DisplayMode() == NESpatial::BoundingBox, true, true, false, matrix44f());
    }
}

void NEMarchingCubesRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    renderPickable(spatial, surface);
}

void NEMarchingCubesRenderer::renderWater(NESpatial* spatial, IGLSurface* surface)
{
    NEMarchingCubes* marchingCubes = qobject_cast<NEMarchingCubes*>(spatial);

    if(marchingCubes && marchingCubes->RenderMode() != NESpatial::NERenderMode::Invisible &&
            NERENDERMAN.GLManager()->isBuffered( &marchingCubes->mesh() ) )
    {
        if(marchingCubes->RenderMode() != NESpatial::NERenderMode::Wireframe)
        {
            NERENDERMAN.GLManager()->drawWaterMesh(surface, &marchingCubes->mesh(), marchingCubes->transform());
        }
    }
}

void NEMarchingCubesRenderer::connectToSpatial()
{
    NEMarchingCubes* marchingCubes = qobject_cast<NEMarchingCubes*>(m_spatial);
    if (marchingCubes == NULL) return;

//    QObject::connect( marchingCubes, SIGNAL(initObject()), this, SLOT(initObject()), Qt::ConnectionType::DirectConnection );
    QObject::connect( marchingCubes, SIGNAL(updateDrawingBuffers()), this, SLOT(updateDrawingBuffers()), Qt::ConnectionType::DirectConnection);
}

void NEMarchingCubesRenderer::updateDrawingBuffers()
{
    NEMarchingCubes* marchingCubes = qobject_cast<NEMarchingCubes*>(m_spatial);
    if (marchingCubes == NULL) return;

    if(marchingCubes->mesh().numTriangles() > 0)
        marchingCubes->mesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}
