#include "NEMeshPostProcessorRenderer.h"

#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"


#include "PostProcessor/NEMeshPostProcessor.h"

//
// MeshPostProcessor
//
void NEMeshPostProcessorRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEMeshPostProcessor* meshPostProcessor = qobject_cast<NEMeshPostProcessor*>(spatial);

    if(meshPostProcessor &&
       meshPostProcessor->RenderMode() != NESpatial::NERenderMode::Invisible && NERENDERMAN.GLManager()->isBuffered( meshPostProcessor->outputMesh() ))
    {
        if(meshPostProcessor->RenderMode() == NESpatial::NERenderMode::Wireframe)
        {
            GL->glPushAttrib(GL_POLYGON_BIT);
            GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            NERENDERMAN.GLManager()->drawRaw(meshPostProcessor->outputMesh(), surface, false, meshPostProcessor->DisplayMode() == NESpatial::BoundingBox);
            GL->glPopAttrib();
        }
        else
            NERENDERMAN.GLManager()->draw(meshPostProcessor->outputMesh(), surface, meshPostProcessor->DisplayMode() == NESpatial::BoundingBox, false, true, false, matrix44f());
    }
}
void NEMeshPostProcessorRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NEMeshPostProcessor* meshPostProcessor = qobject_cast<NEMeshPostProcessor*>(spatial);

    if(meshPostProcessor->RenderMode() != NESpatial::NERenderMode::Invisible && NERENDERMAN.GLManager()->isBuffered( meshPostProcessor->outputMesh() ))
    {
        if(meshPostProcessor->RenderMode() == NESpatial::NERenderMode::Wireframe)
        {
            GL->glPushAttrib(GL_POLYGON_BIT);
            GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            NERENDERMAN.GLManager()->drawRaw(meshPostProcessor->outputMesh(), surface, false, meshPostProcessor->DisplayMode() == NESpatial::BoundingBox);
            GL->glPopAttrib();
        }
        else
            NERENDERMAN.GLManager()->draw(meshPostProcessor->outputMesh(), surface, meshPostProcessor->DisplayMode() == NESpatial::BoundingBox, true, true, false, matrix44f());
    }
}

void NEMeshPostProcessorRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    renderPickable(spatial, surface);
}


NEMeshPostProcessorRendererFactory::NEMeshPostProcessorRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEMeshPostProcessorRendererFactory::nodeName() { return "Mesh Decimator Renderer"; }
NEObject *NEMeshPostProcessorRendererFactory::createInstance() { return new NEMeshPostProcessorRenderer(); }

