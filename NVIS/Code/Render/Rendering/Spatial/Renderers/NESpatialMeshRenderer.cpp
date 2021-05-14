#include "NESpatialMeshRenderer.h"

#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include "Mesh/NESpatialMesh.h"
#include "Mesh/NEZoneItem.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"

#include "Rendering/Resource/NEMaterial.h"
#include "Rendering/Resource/NEVertexBuffer.h"
#include "Utility/NEMatrixStack4x4.h"
#include "Geometry/Mesh/NEZone.h"

//
// Spatial Mesh
//

NESpatialMeshRenderer::NESpatialMeshRenderer()
{
}

NESpatialMeshRenderer::~NESpatialMeshRenderer()
{
}

void NESpatialMeshRenderer::connectToSpatial()
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(m_spatial);
    if (rigid == NULL) return;

    QObject::connect( rigid, SIGNAL(initObject()), this, SLOT(initObject()), Qt::ConnectionType::DirectConnection );
    QObject::connect( rigid, SIGNAL(updateDrawingBuffers()), this, SLOT(updateDrawingBuffers()), Qt::ConnectionType::DirectConnection);
}

void NESpatialMeshRenderer::updateDrawingBuffers()
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(m_spatial);
    if (rigid == NULL) return;

    if(rigid->triangleMesh().numTriangles() > 0)
        rigid->triangleMesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}

void NESpatialMeshRenderer::initObject()
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(m_spatial);
    if (rigid == NULL) return;

    if( NERENDERMAN.GLManager()->material( &rigid->triangleMesh() ) == 0)
    {
        NEMaterial* defaultMaterial = NERENDERMAN.materials()["DefaultMaterial"];
        rigid->connectTo(reinterpret_cast<NENode*>(defaultMaterial), NE::kIOTriangleMesh, 0, 0);
    }
}



void NESpatialMeshRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(spatial);
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
        NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, false, rigid->TwoSidedLighting(),rigid->ReceiveShadows(),rigid->transform(), 1);
        GL->glPopAttrib();

        GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
        GL->glEnable(GL_STENCIL_TEST);
        GL->glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        GL->glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        GL->glStencilMask(0x00);
        //GL->glDisable(GL_DEPTH_TEST);
        NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, false, rigid->TwoSidedLighting(),rigid->ReceiveShadows(),rigid->transform(), 2);
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
            NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, false, rigid->TwoSidedLighting(),rigid->ReceiveShadows(),rigid->transform());
            GL->glPopAttrib();

            if(rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframeOnShaded)
            {
                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
                matrix44f modelMat = rigid->transform();
                NERENDERMAN.GLManager()->drawQuadWireframe(&rigid->triangleMesh(), surface, rigid->SolidWireframeColor(), modelMat, rigid->DisplayMode() == NESpatial::BoundingBox, false);
                GL->glPopAttrib();
            }
        }
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

    if(surface->showGizmo() && NERENDERMAN.GLManager()->isBuffered( &rigid->triangleMesh())){
        const std::vector<NECTNPVertex>& verts = rigid->triangleMesh().vertices();
//        vec3f centerPt(0,0,0);
//        if(verts.size() > 0){
//            vec3f minPt = verts[0].position;
//            vec3f maxPt = verts[0].position;
//            for(ulong i = 1; i < verts.size(); i++){
//                if(verts[i].position.x() < minPt.x()) minPt.x() = verts[i].position.x();
//                if(verts[i].position.y() < minPt.y()) minPt.y() = verts[i].position.y();
//                if(verts[i].position.z() < minPt.z()) minPt.z() = verts[i].position.z();
//                if(verts[i].position.x() > maxPt.x()) maxPt.x() = verts[i].position.x();
//                if(verts[i].position.y() > maxPt.y()) maxPt.y() = verts[i].position.y();
//                if(verts[i].position.z() > maxPt.z()) maxPt.z() = verts[i].position.z();
//            }
//            centerPt.set((minPt.x()+maxPt.x())/2,(minPt.y()+maxPt.y())/2,(minPt.z()+maxPt.z())/2);
//            centerPt = rigid->Position();
//        }
        matrix44f mat;
        eulerf angle = surface->calculateDrawAngle(spatial);
        quatf quatX, quatY, quatZ, quatXYZ;

//        vec3f pt = rigid->transform() * centerPt;
//        vec3f tempPt = surface->projectionMatrix() * surface->viewMatrix() * pt;
//        tempPt = surface->viewMatrix().inverted() * surface->projectionMatrix().inverted() * tempPt;
        NEZone *zone = qobject_cast<NEZone *>(rigid->parent());
        if(zone)
            mat.translate(zone->Position());
        mat.translate(rigid->Position());
        vec3f xaxis = rigid->getRotateTransform() * vec3f(1,0,0);
        vec3f yaxis = rigid->getRotateTransform() * vec3f(0,1,0);
        vec3f zaxis = rigid->getRotateTransform() * vec3f(0,0,1);

        quatX = quatX.fromAxisAndAngle( xaxis.x(),xaxis.y(),xaxis.z(), angle.phi() );
        quatY = quatY.fromAxisAndAngle( yaxis.x(),yaxis.y(),yaxis.z(), angle.theta() );
        quatZ = quatZ.fromAxisAndAngle( zaxis.x(),zaxis.y(),zaxis.z(), angle.psi() );
        quatXYZ = quatZ * quatY * quatX;
        mat.rotate( quatXYZ );
        QPair<matrix44f, QPair< QString, QColor>> gizmo;
        gizmo.first = mat;
        gizmo.second.first = gizmoPath;
        gizmo.second.second = spatial->Color();
        if (rigid->SpatialMeshType() != NESpatialMesh::None)
            surface->m_gizmoList.push_back(gizmo);
    }

}

void NESpatialMeshRenderer::renderOnlyDepth(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(spatial);
    if (rigid == NULL) return;

    if(rigid->RenderMode() == NESpatial::QuadWireframeOccluded || rigid->RenderMode() == NESpatial::Occluded)
    {
        GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
        GL->glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, false, rigid->TwoSidedLighting(),rigid->ReceiveShadows(),rigid->transform());
        GL->glPopAttrib();
    }
}

void NESpatialMeshRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(spatial);
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

void NESpatialMeshRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(spatial);
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

void NESpatialMeshRenderer::renderShadow(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(spatial);
    if (rigid == NULL) return;

    if( rigid->RenderMode() == NESpatial::NERenderMode::Invisible || rigid->RenderMode() == NESpatial::NERenderMode::Wireframe || rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframe || rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframeHidden  || rigid->RenderMode() == NESpatial::NERenderMode::Hidden  )
        return;
    if(rigid->SpatialMeshType() == NESpatialMesh::None)
        return;
    if (!rigid->OccludeShadows())
        return;
    NERENDERMAN.GLManager()->drawShadowMap(surface, &rigid->triangleMesh());
}

void NESpatialMeshRenderer::renderInMotion(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(spatial);
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

NESpatialMeshRendererFactory::NESpatialMeshRendererFactory(NEManager* m): NERendererFactory( m )
{
    NERenderManager* rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString NESpatialMeshRendererFactory::nodeName() { return "Spatial Mesh Renderer"; }
NEObject *NESpatialMeshRendererFactory::createInstance() { return new NESpatialMeshRenderer(); }
