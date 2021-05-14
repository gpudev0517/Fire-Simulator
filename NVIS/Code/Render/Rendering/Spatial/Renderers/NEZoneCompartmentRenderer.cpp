#include "NEZoneCompartmentRenderer.h"

#include "Base/NEBase.h"
#include "Mesh/NEZoneCompartment.h"

#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"

#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Rendering/Resource/NEMaterial.h"
#include "Rendering/Resource/NEVertexBuffer.h"

//
// Zone Container Mesh Rendering
//

NEZoneCompartmentRenderer::NEZoneCompartmentRenderer()
{
}

NEZoneCompartmentRenderer::~NEZoneCompartmentRenderer()
{
}

void NEZoneCompartmentRenderer::connectToSpatial()
{
    NEZoneCompartment* rigid = qobject_cast<NEZoneCompartment*>(m_spatial);
    if (rigid == NULL) return;

    QObject::connect( rigid, SIGNAL(initObject()), this, SLOT(initObject()), Qt::ConnectionType::DirectConnection );
    QObject::connect( rigid, SIGNAL(updateDrawingBuffers()), this, SLOT(updateDrawingBuffers()), Qt::ConnectionType::DirectConnection);
}

void NEZoneCompartmentRenderer::updateDrawingBuffers()
{
    NEZoneCompartment* rigid = qobject_cast<NEZoneCompartment*>(m_spatial);
    if (rigid == NULL) return;

    if(rigid->triangleMesh().numTriangles() > 0)
        rigid->triangleMesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}

void NEZoneCompartmentRenderer::initObject()
{
    NEZoneCompartment* rigid = qobject_cast<NEZoneCompartment*>(m_spatial);
    if (rigid == NULL) return;

    if( NERENDERMAN.GLManager()->material( &rigid->triangleMesh() ) == 0)
    {
        NEMaterial* defaultMaterial = NERENDERMAN.materials()["DefaultMaterial"];
        rigid->connectTo(reinterpret_cast<NENode*>(defaultMaterial), NE::kIOTriangleMesh, 0, 0);
    }
}



void NEZoneCompartmentRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEZoneCompartment* rigid = qobject_cast<NEZoneCompartment*>(spatial);

    if (rigid == NULL) return;

    if( rigid->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;

    if(NERENDERMAN.GLManager()->isBuffered( &rigid->triangleMesh() ) && rigid->Behavior() == NENode::Inactive && rigid->RenderMode() != NESpatial::Wireframe){

        GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
        //GL->glEnable(GL_BLEND);
        GL->glEnable(GL_DEPTH_TEST);
        GL->glEnable(GL_STENCIL_TEST);
        GL->glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        GL->glStencilFunc(GL_ALWAYS, 1, 0xFF);
        GL->glStencilMask(0xFF);
        NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, false, false , false,rigid->transform(), 1);
        GL->glPopAttrib();

        GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
        GL->glEnable(GL_STENCIL_TEST);
        GL->glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        GL->glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        GL->glStencilMask(0x00);
        //GL->glDisable(GL_DEPTH_TEST);
        NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, false, true,false ,rigid->transform(), 2);
        GL->glPopAttrib();
        return;
    }

    if(NERENDERMAN.GLManager()->isBuffered( &rigid->triangleMesh() ) )
    {
        if(rigid->RenderMode() == NESpatial::NERenderMode::Wireframe)
        {
            GL->glPushAttrib(GL_POLYGON_BIT);
            GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            NERENDERMAN.GLManager()->drawRaw(&rigid->triangleMesh(), surface, false, rigid->DisplayMode() == NESpatial::BoundingBox);
            GL->glPopAttrib();
        }
        else if(rigid->RenderMode() == NESpatial::NERenderMode::FlatShaded)
        {
            NERENDERMAN.GLManager()->drawFlat(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox);
        }
        else if (rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframe || rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframeOccluded || rigid->RenderMode() == NESpatial::NERenderMode::Occluded)
        {
            if(rigid->RenderMode() != NESpatial::Occluded)
            {
                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

                matrix44f modelMat = rigid->transform();
                NERENDERMAN.GLManager()->drawQuadWireframe(&rigid->triangleMesh(), surface, rigid->Color(), modelMat, rigid->DisplayMode() == NESpatial::BoundingBox, false);
                GL->glPopAttrib();

            }
        }
        else if (rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframeHidden || rigid->RenderMode() == NESpatial::NERenderMode::Hidden)
        {
            if(rigid->RenderMode() != NESpatial::Hidden)
            {
                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
                matrix44f modelMat = rigid->transform();
                NERENDERMAN.GLManager()->drawQuadWireframe(&rigid->triangleMesh(), surface, rigid->Color(), modelMat, rigid->DisplayMode() == NESpatial::BoundingBox, true);
                GL->glPopAttrib();
            }
        }
        else if(rigid->RenderMode() == NESpatial::NERenderMode::SmoothShaded || rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframeOnShaded)
        {
            NEMaterial*  material = NERENDERMAN.GLManager()->material( const_cast< NEIndexedTriMesh *>( &rigid->triangleMesh() ));
            if(material)
            {
                {
                    surface->setShaderProgram(material->shaderProgram());
                    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "particleTextured" ), false );
                }
            }

            GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
            GL->glEnable(GL_BLEND);
            NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, false, true ,false ,rigid->transform());
            GL->glPopAttrib();

            if(rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframeOnShaded)
            {
                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
                matrix44f modelMat = rigid->transform();
                NERENDERMAN.GLManager()->drawQuadWireframe(&rigid->triangleMesh(), surface, rigid->SolidWireframeColor(), modelMat, rigid->DisplayMode() == NESpatial::BoundingBox, false);
                GL->glPopAttrib();
            }
        }
        //rigid->SetScale(oldScale);
    }

    if(rigid->DrawSolidWireframe() && NERENDERMAN.GLManager()->isBuffered( &rigid->triangleMesh() ))
    {
        GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
        GL->glEnable(GL_LIGHTING);
        GL->glEnable(GL_DEPTH_TEST);
        GL->glDepthFunc(GL_LEQUAL);
        GL->glEnable(GL_BLEND);

        matrix44f modelMat = rigid->transform();
        NERENDERMAN.GLManager()->drawSolidWireframe(&rigid->triangleMesh(), surface, rigid->SolidWireframeColor(), rigid->SolidWireframeThickness(), rigid->ContourThreshold(), modelMat);
        GL->glDisable(GL_BLEND);
        GL->glPopAttrib();
    }

}


void NEZoneCompartmentRenderer::renderOnlyDepth(NESpatial* spatial, IGLSurface* surface)
{
    NEZoneCompartment* rigid = qobject_cast<NEZoneCompartment*>(spatial);
    if (rigid == NULL) return;

    if(rigid->RenderMode() == NESpatial::QuadWireframeOccluded || rigid->RenderMode() == NESpatial::Occluded)
    {
        GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
        GL->glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, false, true, false,rigid->transform());
        GL->glPopAttrib();
    }
}

void NEZoneCompartmentRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NEZoneCompartment* rigid = qobject_cast<NEZoneCompartment*>(spatial);
    if (rigid == NULL) return;

    if( NEBASE.guiMode() == false )
        return;

    if( rigid->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;

    if( NERENDERMAN.GLManager()->isBuffered( &rigid->triangleMesh() ) )
    {

        if(rigid->RenderMode() == NESpatial::NERenderMode::Wireframe || rigid->RenderMode() == NESpatial::QuadWireframe || rigid->RenderMode() == NESpatial::QuadWireframeOccluded || rigid->RenderMode() == NESpatial::QuadWireframeHidden )
        {
            GL->glPushAttrib(GL_POLYGON_BIT);
            GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            NERENDERMAN.GLManager()->drawRaw(&rigid->triangleMesh(), surface, false, rigid->DisplayMode() == NESpatial::BoundingBox);
            GL->glPopAttrib();

        }

        else
        {
            NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, true, true, false, matrix44f());
        }
    }

}

void NEZoneCompartmentRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NEZoneCompartment* rigid = qobject_cast<NEZoneCompartment*>(spatial);
    if (rigid == NULL) return;

    if( NEBASE.guiMode() == false )
        return;

    if( rigid->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;

    if(  NERENDERMAN.GLManager()->isBuffered( &rigid->triangleMesh() ) )
    {
        if(rigid->RenderMode() == NESpatial::NERenderMode::Wireframe)
        {
            GL->glPushAttrib(GL_POLYGON_BIT);
            GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            NERENDERMAN.GLManager()->drawRaw(&rigid->triangleMesh(), surface, false, rigid->DisplayMode() == NESpatial::BoundingBox);
            GL->glPopAttrib();
        }
        else
        {
            matrix44f modelMat;
            modelMat.setToIdentity();
            GL->glPushAttrib(GL_POLYGON_BIT);
            GL->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            NERENDERMAN.GLManager()->drawQuadWireframe(&rigid->triangleMesh(), surface, QColor(Qt::green), modelMat, rigid->DisplayMode() == NESpatial::BoundingBox, false);
            GL->glPopAttrib();
            //NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, true, true, false, matrix44f());
        }
    }

}


void NEZoneCompartmentRenderer::renderInMotion(NESpatial* spatial, IGLSurface* surface)
{
    NEZoneCompartment* rigid = qobject_cast<NEZoneCompartment*>(spatial);
    if (rigid == NULL) return;

    if(rigid->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    if(NERENDERMAN.GLManager()->isBuffered( &rigid->triangleMesh() ) )
    {
        if(rigid->RenderMode() != NESpatial::NERenderMode::Wireframe)
        {
            NERENDERMAN.GLManager()->drawInMotion(surface, &rigid->triangleMesh());
        }
    }
}


NEZoneCompartmentRendererFactory::NEZoneCompartmentRendererFactory(NEManager* m): NERendererFactory( m )
{
    NERenderManager* rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString NEZoneCompartmentRendererFactory::nodeName() { return "Zone Compartment Renderer"; }
NEObject *NEZoneCompartmentRendererFactory::createInstance() { return new NEZoneCompartmentRenderer(); }
