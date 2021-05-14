#include <QOpenGLFunctions_3_3_Compatibility>

#include "FZoneRenderer.h"
#include "FZone.h"
#include "Mesh/NESpatialMesh.h"
#include "Rendering/Manager/NERenderManager.h"

#include <QOpenGLShader>
#include <QImage>
#include <QOpenGLTexture>
#include "NENode.h"
#include <QGLWidget>
#include "Rendering/Manager/NEIGLSurface.h"

#include "Base/NEBase.h"
#include "Mesh/NEZone.h"

#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"

#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Rendering/Resource/NEMaterial.h"
#include "Rendering/Resource/NEVertexBuffer.h"
#include "Rendering/Spatial/NEFloorPlane.h"

//
// Spatial Group
//

void FZoneRenderer::render(NESpatial* spatial, IGLSurface* surface)
{

    //qDebug() << "FZoneRenderer::render is called " << spatial->objectName();
    if( FZone* zone = qobject_cast<FZone*>(spatial) )
    {
//            GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

//            NERENDERMAN.GLManager()->bindFloorDrawingShader(surface);
//            NERENDERMAN.GLManager()->updateFloorDrawingUniforms(surface,
//                                                                zone->XMin() / zone->XSize(), zone->XMax() / zone->XSize(),
//                                                                zone->ZMin() / zone->ZSize(), zone->ZMax() / zone->ZSize());
//            GL->glEnable(GL_TEXTURE_2D);
//            GL->glEnable(GL_BLEND);
//            GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//            QOpenGLTexture* texture;
//            if(zone->Revert())
//                texture = new QOpenGLTexture(zone->textureImage().mirrored());
//            else
//                texture = new QOpenGLTexture(zone->textureImage());
//            texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
//            texture->setMagnificationFilter(QOpenGLTexture::Linear);
//            GLuint texID = texture->textureId();
//            GL->glActiveTexture(GL_TEXTURE0);
//            GL->glBindTexture(GL_TEXTURE_2D, texID);
//            GL->glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//            GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//            GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//            NEVertexBuffer* quadVerts;
//            quadVerts = 0;
//            quadVerts = NERENDERMAN.GLManager()->createVertexBuffer(kTPFormat, 4, 0, GL_STATIC_DRAW);
//            NETPVertex* vertexPtr = (NETPVertex*) quadVerts->beginLoadData();

//            NECTNPVertex pt0 = zone->triangleMesh().vertices().at(0);
//            NECTNPVertex pt1 = zone->triangleMesh().vertices().at(1);
//            NECTNPVertex pt2 = zone->triangleMesh().vertices().at(2);
//            NECTNPVertex pt3 = zone->triangleMesh().vertices().at(3);

//            vertexPtr[0].position = pt0.position;
//            vertexPtr[0].tCoord = pt0.tCoord;
//            vertexPtr[1].position = pt1.position;
//            vertexPtr[1].tCoord = pt1.tCoord;
//            vertexPtr[2].position = pt2.position;
//            vertexPtr[2].tCoord = pt2.tCoord;
//            vertexPtr[3].position = pt3.position;
//            vertexPtr[3].tCoord = pt3.tCoord;

//            quadVerts->endLoadData();
//            NERENDERMAN.GLManager()->draw(kQuadListPrim, quadVerts);

//            delete(texture);
//            GL->glPopAttrib();
//        }


        if(zone->ShowCompBB())
        {
            surface->setShaderProgram(0);

            NEIndexedTriMesh triMesh = createMesh(zone->outerCompAABB(), zone->compAABB());

            float dashSize = 0;
            float gapSize = 0;
            if(zone->LineType() == FZone::NELineType::Dot)
            {
                dashSize = 3;
                gapSize = 3;
            }
            else if (zone->LineType() == FZone::NELineType::Dash)
            {
                dashSize = 10;
                gapSize = 5;
            }

            GL->glColor3f(1, 0, 0);
            if (dashSize > 0) {
                NERENDERMAN.GLManager()->bindLineStippleShader(surface);
                NERENDERMAN.GLManager()->updateLineStippleUniforms(surface, dashSize, gapSize);
            }

            for(int i = 0; i < 6; i++)
            {
                NEVertexBuffer* lineVerts;
                lineVerts = 0;
                lineVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                NEPVertex* vertexPtr = (NEPVertex*) lineVerts->beginLoadData();

                vertexPtr[0].position = zone->transform().inverted() * triMesh.vertices().at(4 * i).position;
                vertexPtr[1].position = zone->transform().inverted() * triMesh.vertices().at(4 * i + 1).position;
                vertexPtr[2].position = zone->transform().inverted() * triMesh.vertices().at(4 * i + 2).position;
                vertexPtr[3].position = zone->transform().inverted() * triMesh.vertices().at(4 * i + 3).position;

                lineVerts->endLoadData();
                NERENDERMAN.GLManager()->draw(kLineLoopPrim, lineVerts);
            }

            GL->glColor3f(0, 0, 1);
            if (dashSize > 0) {
                NERENDERMAN.GLManager()->bindLineStippleShader(surface);
                NERENDERMAN.GLManager()->updateLineStippleUniforms(surface, dashSize, gapSize);
            }

            for(int i = 6; i < 12; i++)
            {
                NEVertexBuffer* lineVerts;
                lineVerts = 0;
                lineVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                NEPVertex* vertexPtr = (NEPVertex*) lineVerts->beginLoadData();

                vertexPtr[0].position = zone->transform().inverted() * triMesh.vertices().at(4 * i).position;
                vertexPtr[1].position = zone->transform().inverted() * triMesh.vertices().at(4 * i + 1).position;
                vertexPtr[2].position = zone->transform().inverted() * triMesh.vertices().at(4 * i + 2).position;
                vertexPtr[3].position = zone->transform().inverted() * triMesh.vertices().at(4 * i + 3).position;

                lineVerts->endLoadData();
                NERENDERMAN.GLManager()->draw(kLineLoopPrim, lineVerts);
            }
        }
        else
        {
            NEZoneRenderer::render( spatial, surface );
        }
    }
}

void FZoneRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NEZoneRenderer::renderPickable( spatial, surface );
}

void FZoneRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NEZoneRenderer::renderPicked( spatial, surface );
}

NEIndexedTriMesh FZoneRenderer::createMesh(AABB outerBB, AABB innerBB)
{

    NEIndexedTriMesh triMesh;
    triMesh.resize();
    NECTNPVertex v;

    v.position = outerBB.minPos();
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.minPos().x(), outerBB.minPos().y(), outerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.minPos().x(), outerBB.maxPos().y(), outerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.minPos().x(), outerBB.maxPos().y(), outerBB.minPos().z());
    triMesh.appendVertex(v);

//    NETriangleIndexed t1(1, 3, 0), t2(1, 2, 3);
//    triMesh.appendTriangle(t1);
//    triMesh.appendTriangle(t2);

    v.position = outerBB.minPos();
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.minPos().x(), outerBB.minPos().y(), outerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.maxPos().x(), outerBB.minPos().y(), outerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.maxPos().x(), outerBB.minPos().y(), outerBB.minPos().z());
    triMesh.appendVertex(v);

//    NETriangleIndexed t3(5, 7, 4), t4(5, 6, 7);
//    triMesh.appendTriangle(t3);
//    triMesh.appendTriangle(t4);

    v.position = outerBB.minPos();
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.minPos().x(), outerBB.maxPos().y(), outerBB.minPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.maxPos().x(), outerBB.maxPos().y(), outerBB.minPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.maxPos().x(), outerBB.minPos().y(), outerBB.minPos().z());
    triMesh.appendVertex(v);

//    NETriangleIndexed t5(9, 11, 8), t6(9, 10, 11);
//    triMesh.appendTriangle(t5);
//    triMesh.appendTriangle(t6);

    v.position = outerBB.maxPos();
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.minPos().x(), outerBB.maxPos().y(), outerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.minPos().x(), outerBB.maxPos().y(), outerBB.minPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.maxPos().x(), outerBB.maxPos().y(), outerBB.minPos().z());
    triMesh.appendVertex(v);

//    NETriangleIndexed t7(13, 15, 12), t8(13, 14, 15);
//    triMesh.appendTriangle(t7);
//    triMesh.appendTriangle(t8);

    v.position = outerBB.maxPos();
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.maxPos().x(), outerBB.minPos().y(), outerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.maxPos().x(), outerBB.minPos().y(), outerBB.minPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.maxPos().x(), outerBB.maxPos().y(), outerBB.minPos().z());
    triMesh.appendVertex(v);

//    NETriangleIndexed t9(17, 19, 16), t10(17, 18, 19);
//    triMesh.appendTriangle(t9);
//    triMesh.appendTriangle(t10);

    v.position = outerBB.maxPos();
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.minPos().x(), outerBB.maxPos().y(), outerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.minPos().x(), outerBB.minPos().y(), outerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(outerBB.maxPos().x(), outerBB.minPos().y(), outerBB.maxPos().z());
    triMesh.appendVertex(v);

//    NETriangleIndexed t11(21, 23, 20), t12(21, 22, 23);
//    triMesh.appendTriangle(t7);
//    triMesh.appendTriangle(t8);

    //innerBB
    v.position = innerBB.minPos();
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.minPos().x(), innerBB.minPos().y(), innerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.minPos().x(), innerBB.maxPos().y(), innerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.minPos().x(), innerBB.maxPos().y(), innerBB.minPos().z());
    triMesh.appendVertex(v);

    v.position = innerBB.minPos();
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.minPos().x(), innerBB.minPos().y(), innerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.maxPos().x(), innerBB.minPos().y(), innerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.maxPos().x(), innerBB.minPos().y(), innerBB.minPos().z());
    triMesh.appendVertex(v);

    v.position = innerBB.minPos();
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.minPos().x(), innerBB.maxPos().y(), innerBB.minPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.maxPos().x(), innerBB.maxPos().y(), innerBB.minPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.maxPos().x(), innerBB.minPos().y(), innerBB.minPos().z());
    triMesh.appendVertex(v);

    v.position = innerBB.maxPos();
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.minPos().x(), innerBB.maxPos().y(), innerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.minPos().x(), innerBB.maxPos().y(), innerBB.minPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.maxPos().x(), innerBB.maxPos().y(), innerBB.minPos().z());
    triMesh.appendVertex(v);

    v.position = innerBB.maxPos();
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.maxPos().x(), innerBB.minPos().y(), innerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.maxPos().x(), innerBB.minPos().y(), innerBB.minPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.maxPos().x(), innerBB.maxPos().y(), innerBB.minPos().z());
    triMesh.appendVertex(v);

    v.position = innerBB.maxPos();
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.minPos().x(), innerBB.maxPos().y(), innerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.minPos().x(), innerBB.minPos().y(), innerBB.maxPos().z());
    triMesh.appendVertex(v);
    v.position = vec3f(innerBB.maxPos().x(), innerBB.minPos().y(), innerBB.maxPos().z());
    triMesh.appendVertex(v);

    triMesh.updateDrawingBuffers(GL_DYNAMIC_DRAW);

    return triMesh;
}

FZoneRendererFactory::FZoneRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString FZoneRendererFactory::nodeName() { return "Fire Zone Renderer"; }
NEObject *FZoneRendererFactory::createInstance() { return new FZoneRenderer(); }
