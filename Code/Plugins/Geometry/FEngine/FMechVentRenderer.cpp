#include "FMechVentRenderer.h"
#include "Core/Base/NESceneManager.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Mesh/NESpatialMesh.h"
#include "FVent.h"
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLFunctions_4_1_Compatibility>
#include <QOpenGLFunctions_3_3_Compatibility>

//
// Zone Item Renderer
//

void FMechVentRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    if(FVent *vitem = qobject_cast<FVent *>(spatial))
    {
//        if( zitem->Failed() && zitem->FailedTime() < NESCENE.ElapsedTime() )
//            GL->glColor4f(zitem->FailedColor().redF(), zitem->FailedColor().greenF(), zitem->FailedColor().blueF(), 1.0f);
    }

    //spatial->SetColor( QColor( 170, 85, 0 ) );
    //gizmoPath = QDir::currentPath() + "//..//Code//FDYNUI//Resources//svg//inspector_mechvent_01.svg";
    gizmoPath = ":/Resources/svg/inspector_mechvent_01.svg";
    NESpatialMeshRenderer::render( spatial, surface );
}

void FMechVentRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPickable(spatial, surface);
}

void FMechVentRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialMeshRenderer::renderPicked(spatial, surface);
}

void FMechVentRenderer::updateDrawingBuffers()
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(m_spatial);
    if (rigid == NULL) return;

    if(rigid->triangleMesh().numTriangles() > 0)
        rigid->triangleMesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}

FMechVentRendererFactory::FMechVentRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString FMechVentRendererFactory::nodeName() { return "Fire Mech Vent Renderer"; }

NEObject *FMechVentRendererFactory::createInstance() { return new FMechVentRenderer(); }
