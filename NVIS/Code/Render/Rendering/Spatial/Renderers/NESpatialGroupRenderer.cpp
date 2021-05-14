#include "NESpatialGroupRenderer.h"

#include "NESpatialGroupRenderer.h"

#include "Base/NESceneManager.h"

#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"


#include "Spatial/NESpatialGroup.h"

//
// Spatial Group
//

void NESpatialGroupRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialGroup* spatialGroup = qobject_cast<NESpatialGroup*>(spatial);

    if(spatialGroup && spatialGroup->RenderMode() != NESpatial::NERenderMode::Invisible &&
            (spatialGroup->GroupBox() || (NESCENE.selectedNode() == spatialGroup)))
    {
        GL->glLineWidth(2.0f);
        vec3f m_MinPos = spatialGroup->aabb().minPos();
        vec3f m_MaxPos = spatialGroup->aabb().maxPos();

        GL->glBegin(GL_LINES);
        //bottom
        GL->glVertex3f(m_MinPos.x(), m_MinPos.y(), m_MinPos.z());
        GL->glVertex3f(m_MaxPos.x(), m_MinPos.y(), m_MinPos.z());

        GL->glVertex3f(m_MaxPos.x(), m_MinPos.y(), m_MinPos.z());
        GL->glVertex3f(m_MaxPos.x(), m_MinPos.y(), m_MaxPos.z());

        GL->glVertex3f(m_MaxPos.x(), m_MinPos.y(), m_MaxPos.z());
        GL->glVertex3f(m_MinPos.x(), m_MinPos.y(), m_MaxPos.z());

        GL->glVertex3f(m_MinPos.x(), m_MinPos.y(), m_MaxPos.z());
        GL->glVertex3f(m_MinPos.x(), m_MinPos.y(), m_MinPos.z());

        //vertical
        GL->glVertex3f(m_MinPos.x(), m_MinPos.y(), m_MinPos.z());
        GL->glVertex3f(m_MinPos.x(), m_MaxPos.y(), m_MinPos.z());

        GL->glVertex3f(m_MaxPos.x(), m_MinPos.y(), m_MinPos.z());
        GL->glVertex3f(m_MaxPos.x(), m_MaxPos.y(), m_MinPos.z());

        GL->glVertex3f(m_MaxPos.x(), m_MinPos.y(), m_MaxPos.z());
        GL->glVertex3f(m_MaxPos.x(), m_MaxPos.y(), m_MaxPos.z());

        GL->glVertex3f(m_MinPos.x(), m_MinPos.y(), m_MaxPos.z());
        GL->glVertex3f(m_MinPos.x(), m_MaxPos.y(), m_MaxPos.z());
        //top
        GL->glVertex3f(m_MinPos.x(), m_MaxPos.y(), m_MinPos.z());
        GL->glVertex3f(m_MaxPos.x(), m_MaxPos.y(), m_MinPos.z());

        GL->glVertex3f(m_MaxPos.x(), m_MaxPos.y(), m_MinPos.z());
        GL->glVertex3f(m_MaxPos.x(), m_MaxPos.y(), m_MaxPos.z());

        GL->glVertex3f(m_MaxPos.x(), m_MaxPos.y(), m_MaxPos.z());
        GL->glVertex3f(m_MinPos.x(), m_MaxPos.y(), m_MaxPos.z());

        GL->glVertex3f(m_MinPos.x(), m_MaxPos.y(), m_MaxPos.z());
        GL->glVertex3f(m_MinPos.x(), m_MaxPos.y(), m_MinPos.z());

        GL->glEnd();

        GL->glLineWidth(1.0f);
    }
}
void NESpatialGroupRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    render(spatial, surface);
}

void NESpatialGroupRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    render(spatial, surface);
}


NESpatialGroupRendererFactory::NESpatialGroupRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NESpatialGroupRendererFactory::nodeName() { return "Group Renderer"; }
NEObject *NESpatialGroupRendererFactory::createInstance() { return new NESpatialGroupRenderer(); }
