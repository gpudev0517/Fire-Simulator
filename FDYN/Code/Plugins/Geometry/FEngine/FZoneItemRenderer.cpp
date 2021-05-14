#include "FZoneItemRenderer.h"
#include "Core/Base/NESceneManager.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Resource/NEMaterial.h"
#include "Mesh/NESpatialMesh.h"
#include "FZoneItem.h"
#include "Rendering/Manager/NEGLManager.h"

#include "Utility/NEMatrixStack4x4.h"


//
// Zone Item Renderer
//

void FZoneItemRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    if(FZoneItem *zitem = qobject_cast<FZoneItem *>(spatial))
    {
        if(zitem->ShowGhostPosition()){
            renderGhost(spatial, surface);
        }

        if( zitem->Failed() && zitem->FailedTime() < NESCENE.ElapsedTime() )
            GL->glColor4f(zitem->FailedColor().redF(), zitem->FailedColor().greenF(), zitem->FailedColor().blueF(), 1.0f);

        if( zitem->needsGLTransform() )
        {
            surface->worldStack().loadMatrix(zitem->transform());
            NERENDERMAN.GLManager()->normalMatrix( surface->worldStack().top().normalMatrix() );
        }
        // Do not apply any world transformation if the geometry is bound to the simulation data (does not require OpenGL transformation)
        else
            surface->worldStack().loadMatrix(matrix44f());

        // Update the world matrix in GL manager
        NERENDERMAN.GLManager()->worldMatrix( surface->worldStack().top() );

        // Set the model-view matrix in OpenGL side
        surface->setModelViewMatrix();
        NERENDERMAN.GLManager()->modelViewMatrix( surface->modelViewMatrix() );

        NESpatialMeshRenderer::render( spatial, surface );


    }
}

void FZoneItemRenderer::renderGhost(NESpatial* spatial, IGLSurface* surface)
{
    FZoneItem* zitem = qobject_cast<FZoneItem*>(spatial);
    if (zitem == NULL) return;


    if( zitem->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;


    if(NERENDERMAN.GLManager()->isBuffered( &zitem->triangleMesh() ) )
    {
        GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
        if( zitem->needsGLTransform() )
        {
            surface->worldStack().loadMatrix(zitem->ghostTransform());
            NERENDERMAN.GLManager()->normalMatrix( surface->worldStack().top().normalMatrix() );
        }
        // Do not apply any world transformation if the geometry is bound to the simulation data (does not require OpenGL transformation)
        else
            surface->worldStack().loadMatrix(matrix44f());

        // Update the world matrix in GL manager
        NERENDERMAN.GLManager()->worldMatrix( surface->worldStack().top() );

        // Set the model-view matrix in OpenGL side
        surface->setModelViewMatrix();
        NERENDERMAN.GLManager()->modelViewMatrix( surface->modelViewMatrix() );

        GL->glEnable(GL_LIGHTING);
        GL->glEnable(GL_DEPTH_TEST);
        GL->glDepthFunc(GL_LEQUAL);
        GL->glEnable(GL_BLEND);

        matrix44f modelMat = zitem->transform();
        NERENDERMAN.GLManager()->drawSolidWireframe(&zitem->triangleMesh(), surface, zitem->SolidWireframeColor(), zitem->SolidWireframeThickness(), zitem->ContourThreshold(), modelMat, true);
        GL->glDisable(GL_BLEND);
        GL->glPopAttrib();


//        if(zitem->RenderMode() == NESpatial::NERenderMode::SmoothShaded || zitem->RenderMode() == NESpatial::NERenderMode::QuadWireframeOnShaded)
//        {
//            NEMaterial*  material = NERENDERMAN.GLManager()->material( const_cast< NEIndexedTriMesh *>( &zitem->triangleMesh() ));
//            if(material)
//            {
//                {
//                    surface->setShaderProgram(material->shaderProgram());
//                    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "particleTextured" ), false );
//                }
//            }


//            GL->glEnable(GL_BLEND);
//            NERENDERMAN.GLManager()->draw(&zitem->triangleMesh(), surface, false, false, zitem->TwoSidedLighting(),zitem->ReceiveShadows(),zitem->ghostTransform());
//            GL->glPopAttrib();

//        }
    }

}

void FZoneItemRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPickable(spatial, surface);
}

void FZoneItemRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPicked(spatial, surface);
}

void FZoneItemRenderer::updateDrawingBuffers()
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(m_spatial);
    if (rigid == NULL) return;

    if(rigid->triangleMesh().numTriangles() > 0)
        rigid->triangleMesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}

FZoneItemRendererFactory::FZoneItemRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString FZoneItemRendererFactory::nodeName() { return "Fire Plant Item Renderer"; }

NEObject *FZoneItemRendererFactory::createInstance() { return new FZoneItemRenderer(); }
