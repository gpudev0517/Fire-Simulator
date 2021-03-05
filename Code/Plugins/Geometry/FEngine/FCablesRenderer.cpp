#include "FCablesRenderer.h"
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

void FCablesRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    spatial->SetColor( QColor( 38, 77, 57 ) );

    if(FZoneItem *zitem = qobject_cast<FZoneItem *>(spatial))
    {
        if( zitem->Failed() && zitem->FailedTime() < NESCENE.ElapsedTime() )
            GL->glColor4f(zitem->FailedColor().redF(), zitem->FailedColor().greenF(), zitem->FailedColor().blueF(), 1.0f);
    }
    //gizmoPath = QDir::currentPath() + "//..//Code//FDYNUI//Resources//svg//inspector_cable_runway_01.svg";
    gizmoPath = ":/Resources/svg/inspector_cable_runway_01.svg";
    NESpatialMeshRenderer::render( spatial, surface );
}

void FCablesRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPickable(spatial, surface);
}

void FCablesRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPicked(spatial, surface);
}

void FCablesRenderer::updateDrawingBuffers()
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(m_spatial);
    if (rigid == NULL) return;

    if(rigid->triangleMesh().numTriangles() > 0)
        rigid->triangleMesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}

FCablesRendererFactory::FCablesRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString FCablesRendererFactory::nodeName() { return "Cables Renderer"; }

NEObject *FCablesRendererFactory::createInstance() { return new FCablesRenderer(); }
