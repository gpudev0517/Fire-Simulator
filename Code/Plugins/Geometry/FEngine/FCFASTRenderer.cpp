#include "FCFASTRenderer.h"
#include "Core/Base/NESceneManager.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Mesh/NESpatialMesh.h"
#include "FZoneItem.h"
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLFunctions_4_1_Compatibility>
#include <QOpenGLFunctions_3_3_Compatibility>

//
// Zone Item Renderer
//

void FCFASTRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    spatial->SetColor( QColor( 38, 77, 57 ) );

    if(FZoneItem *zitem = qobject_cast<FZoneItem *>(spatial))
    {
        if( zitem->Failed() && zitem->FailedTime() < NESCENE.ElapsedTime() )
            GL->glColor4f(zitem->FailedColor().redF(), zitem->FailedColor().greenF(), zitem->FailedColor().blueF(), 1.0f);
    }
    //gizmoPath = QDir::currentPath() + "//..//Code//FDYNUI//Resources//svg//inspector_cable_runway_01.svg";
    gizmoPath = "";
    NESpatialMeshRenderer::render( spatial, surface );
}

void FCFASTRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPickable(spatial, surface);
}

void FCFASTRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPicked(spatial, surface);
}

void FCFASTRenderer::updateDrawingBuffers()
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(m_spatial);
    if (rigid == NULL) return;

    if(rigid->triangleMesh().numTriangles() > 0)
        rigid->triangleMesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}

FCFASTRendererFactory::FCFASTRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString FCFASTRendererFactory::nodeName() { return "CFAST Renderer"; }

NEObject *FCFASTRendererFactory::createInstance() { return new FCFASTRenderer(); }
