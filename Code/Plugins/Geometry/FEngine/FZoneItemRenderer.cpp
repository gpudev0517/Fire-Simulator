#include "FZoneItemRenderer.h"
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

void FZoneItemRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    if(FZoneItem *zitem = qobject_cast<FZoneItem *>(spatial))
    {
        if( zitem->Failed() && zitem->FailedTime() < NESCENE.ElapsedTime() )
            GL->glColor4f(zitem->FailedColor().redF(), zitem->FailedColor().greenF(), zitem->FailedColor().blueF(), 1.0f);
    }
    //gizmoPath = QDir::currentPath() + "//..//Code//FDYNUI//Resources//svg//inspector_cable_runway_01.svg";
    gizmoPath = ":/Resources/svg/inspector_cable_runway_01.svg";
    NESpatialMeshRenderer::render( spatial, surface );
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
