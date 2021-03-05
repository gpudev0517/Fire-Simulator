#include "FVentRenderer.h"
#include "Core/Base/NESceneManager.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Mesh/NESpatialMesh.h"
#include "FVent.h"
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLFunctions_4_1_Compatibility>
#include <QOpenGLFunctions_3_3_Compatibility>
#include "Rendering/Resource/NEVertexBuffer.h"

//
// Zone Item Renderer
//

void FVentRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    FVent *vitem = qobject_cast<FVent *>(spatial);
//    if(FVent *vitem = qobject_cast<FVent *>(spatial))
//    {
////        if( zitem->Failed() && zitem->FailedTime() < NESCENE.ElapsedTime() )
////            GL->glColor4f(zitem->FailedColor().redF(), zitem->FailedColor().greenF(), zitem->FailedColor().blueF(), 1.0f);

//    }
    //gizmoPath = QDir::currentPath() + "//..//Code//FDYNUI//Resources//svg//inspector_naturalvent_01.svg";
    gizmoPath = ":/Resources/svg/inspector_naturalvent_01.svg";
    NESpatialMeshRenderer::render( spatial, surface );
    if(vitem->isSnapped())
    {
        NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(spatial);
        matrix44f modelMat;
        modelMat.setToIdentity();
        GL->glPushAttrib(GL_POLYGON_BIT);
        GL->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (rigid->SpatialMeshType() == NESpatialMesh::Cylinder)
        {
            GL->glColor3f(1, 0, 0);
            for(unsigned i = 0; i < rigid->triangleMesh().numTriangles(); i++ )
            {
                NEVertexBuffer* lineVerts;
                lineVerts = 0;
                lineVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 2, 0, GL_STATIC_DRAW);
                NEPVertex* vertexPtr = (NEPVertex*) lineVerts->beginLoadData();

                vec3f p1 = rigid->triangleMesh().vertices()[rigid->triangleMesh().triangles().at(i).p1].position;
                vec3f p2 = rigid->triangleMesh().vertices()[rigid->triangleMesh().triangles().at(i).p2].position;
                vec3f p3 = rigid->triangleMesh().vertices()[rigid->triangleMesh().triangles().at(i).p3].position;
                if (abs(p1.y() - p2.y()) < 1e-3 && abs(p1.y() - p3.y()) < 1e-3){}
                else
                {
                    if (abs(p1.y() - p2.y()) < 1e-3)
                    {
                        vertexPtr[0].position = p1;
                        vertexPtr[1].position = p2;
                    }
                    else if (abs(p2.y() - p3.y()) < 1e-3)
                    {
                        vertexPtr[0].position = p2;
                        vertexPtr[1].position = p3;
                    }
                    else if (abs(p3.y() - p1.y()) < 1e-3)
                    {
                        vertexPtr[0].position = p3;
                        vertexPtr[1].position = p1;
                    }
                    lineVerts->endLoadData();
                    NERENDERMAN.GLManager()->draw(kLineListPrim, lineVerts, 0, 2);
                }
            }
        }
        else
            NERENDERMAN.GLManager()->drawQuadWireframe(&rigid->triangleMesh(), surface, QColor(Qt::red), modelMat, rigid->DisplayMode() == NESpatial::BoundingBox, false);
        GL->glPopAttrib();
    }
}

void FVentRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPickable(spatial, surface);
}

void FVentRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPicked(spatial, surface);
}

void FVentRenderer::updateDrawingBuffers()
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(m_spatial);
    if (rigid == NULL) return;

    if(rigid->triangleMesh().numTriangles() > 0)
        rigid->triangleMesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}

FVentRendererFactory::FVentRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString FVentRendererFactory::nodeName() { return "Fire Vent Renderer"; }

NEObject *FVentRendererFactory::createInstance() { return new FVentRenderer(); }
