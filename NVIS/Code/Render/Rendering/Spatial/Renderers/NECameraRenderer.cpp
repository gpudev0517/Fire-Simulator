#include "NECameraRenderer.h"

#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"

#include "Rendering/Spatial/NECamera.h"

//
// Camera
//
void NECameraRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NECamera* camera = qobject_cast<NECamera*>(spatial);
    if (camera == NULL) return;

    if(!camera->mesh())
        camera->SetMesh(const_cast<NEIndexedTriMesh*>(((NECameraFactory&)camera->factory()).mesh()));

    if(camera->isViewer() == false && camera->RenderMode() != NESpatial::NERenderMode::Invisible && !camera->fixedOrthographic())
    {
        NERENDERMAN.GLManager()->draw(camera->mesh(), surface, camera->DisplayMode() == NESpatial::BoundingBox, false, true, false, matrix44f());
        NERENDERMAN.GLManager()->setFillMode(NEFillMode::kWireframeFrontAndBack);
        GL->glColor4f(0.0, 0.0, 0.0, 1.0);
        GL->glEnable(GL_POLYGON_OFFSET_LINE);
        GL->glPolygonOffset(-0.01f, -0.01f);
        renderPicked(camera, surface);
        GL->glDisable(GL_POLYGON_OFFSET_LINE);
    }
}

void NECameraRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NECamera* camera = qobject_cast<NECamera*>(spatial);
    if (camera == NULL) return;
    if(camera->isViewer() == false && camera->RenderMode() != NESpatial::NERenderMode::Invisible && !camera->fixedOrthographic())
        NERENDERMAN.GLManager()->draw(camera->mesh(), surface, camera->DisplayMode() == NESpatial::BoundingBox, true, true, false, matrix44f());
}

void NECameraRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    renderPickable(spatial, surface);
}

NECameraRendererFactory::NECameraRendererFactory(NEManager* m) : NERendererFactory( m )
{

}

QString NECameraRendererFactory::nodeName() { return "Camera Renderer"; }

NEObject *NECameraRendererFactory::createInstance()
{
    return new NECameraRenderer();
}

