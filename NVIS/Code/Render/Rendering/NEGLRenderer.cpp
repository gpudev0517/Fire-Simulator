#include "Base/NEBase.h"
#include "Utility/geometry.h"
#include "Rendering/NEGLRenderer.h"
#include "Resource/Mesh/NETriMesh.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Resource/NEVertexBuffer.h"
#include "Resource/NEIndexBuffer.h"
#include "Resource/NEMaterial.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Manager/NERenderManager.h"
#include "Spatial/NESpatial.h"
#include "Rendering/Spatial/Light/NEDirectionalLight.h"
#include "Rendering/Spatial/Light/NEPointLight.h"
#include "Rendering/Spatial/Light/NESpotLight.h"
#include "Rendering/Spatial/NECamera.h"

#if NE_VOLUME
#include "Volume/Core/NEVolume.h"
#endif

#include "Rendering/Spatial/NEClipPlane.h"
#include "Rendering/Spatial/NEOSDBase.h"

#include "Rendering/Spatial/Renderers/NESpatialRenderer.h"

#if USE_EXT_GFSDK_SSAO
// HBAO+
#include "GFSDK_SSAO_GL/GFSDK_SSAO.h"
#endif

#include "NEGLRenderer_p.h"


void glAssert(const char* msg, long line, const char* file)
{
    struct glError
    {
        GLenum code;
        const char* name;
    };

    static const glError errors[] = {	{GL_NO_ERROR, "No Error"},
                                        {GL_INVALID_ENUM, "Invalid Enum"},
                                        {GL_INVALID_VALUE, "Invalid Value"},
                                        {GL_INVALID_OPERATION, "Invalid Operation"}
                                    #if OGL1
                                        ,{GL_STACK_OVERFLOW, "Stack Overflow"},
                                        {GL_STACK_UNDERFLOW, "Stack Underflow"},
                                        {GL_OUT_OF_MEMORY, "Out Of Memory"}
                                    #endif
                                    };

    GLenum e = GL->glGetError();

    if (e == GL_NO_ERROR)
    {
        return;
    }
    else
    {
        const char* errorName = "Unknown error";

        // find error message
        for (uint32_t i=0; i < sizeof(errors)/sizeof(glError); i++)
        {
            if (errors[i].code == e)
            {
                errorName = errors[i].name;
            }
        }

        printf("OpenGL: %s - error %s in %s at line %d\n", msg, errorName, file, int(line));
        assert(0);
    }
}

#define glVerify(x) x



bool NEGLRenderer::init()
{



    return true;
}

void NEGLRenderer::deInit()
{

}


NEGLRenderer::NEGLRenderer()
{
    // Initialize the private members
    P = new NEGLRendererPrivate(this);

}

NEGLRenderer::~NEGLRenderer()
{

    delete P;
}



void NEGLRenderer::drawRaw(const AABB& aabb)
{
    const vec3f& p1 = aabb.minPos();
    const vec3f& p2 = aabb.maxPos();

    GL->glBegin(GL_LINES);
    //bottom
    GL->glVertex3f(p1.x(), p1.y(), p1.z());
    GL->glVertex3f(p2.x(), p1.y(), p1.z());

    GL->glVertex3f(p2.x(), p1.y(), p1.z());
    GL->glVertex3f(p2.x(), p1.y(), p2.z());

    GL->glVertex3f(p2.x(), p1.y(), p2.z());
    GL->glVertex3f(p1.x(), p1.y(), p2.z());

    GL->glVertex3f(p1.x(), p1.y(), p2.z());
    GL->glVertex3f(p1.x(), p1.y(), p1.z());

    //vertical
    GL->glVertex3f(p1.x(), p1.y(), p1.z());
    GL->glVertex3f(p1.x(), p2.y(), p1.z());

    GL->glVertex3f(p2.x(), p1.y(), p1.z());
    GL->glVertex3f(p2.x(), p2.y(), p1.z());

    GL->glVertex3f(p2.x(), p1.y(), p2.z());
    GL->glVertex3f(p2.x(), p2.y(), p2.z());

    GL->glVertex3f(p1.x(), p1.y(), p2.z());
    GL->glVertex3f(p1.x(), p2.y(), p2.z());
    //top
    GL->glVertex3f(p1.x(), p2.y(), p1.z());
    GL->glVertex3f(p2.x(), p2.y(), p1.z());

    GL->glVertex3f(p2.x(), p2.y(), p1.z());
    GL->glVertex3f(p2.x(), p2.y(), p2.z());

    GL->glVertex3f(p2.x(), p2.y(), p2.z());
    GL->glVertex3f(p1.x(), p2.y(), p2.z());

    GL->glVertex3f(p1.x(), p2.y(), p2.z());
    GL->glVertex3f(p1.x(), p2.y(), p1.z());

    GL->glEnd();
}

void NEGLRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialRenderer* renderer = NERENDERMAN.getRenderer(spatial);
    if (renderer != NULL)
    {
        renderer->renderPicked(spatial, surface);
        return;
    }

    NESpatialRenderer::renderPickedStatic(spatial, surface);
}

void NEGLRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialRenderer* renderer = NERENDERMAN.getRenderer(spatial);
    if (renderer != NULL)
    {
        renderer->renderPickable(spatial, surface);
        return;
    }

    NESpatialRenderer::renderPickableStatic(spatial, surface);
}

void NEGLRenderer::renderShadow(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialRenderer* renderer = NERENDERMAN.getRenderer(spatial);
    if (renderer != NULL)
    {
        renderer->renderShadow(spatial, surface);
        return;
    }
}
