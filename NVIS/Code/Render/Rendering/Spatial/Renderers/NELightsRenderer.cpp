#include "NELightsRenderer.h"

#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"


#include "Rendering/Spatial/Light/NEDirectionalLight.h"
#include "Rendering/Spatial/Light/NEPointLight.h"
#include "Rendering/Spatial/Light/NEAreaLight.h"
#include "Rendering/Spatial/Light/NESpotLight.h"



//
// Light
//
void NEDirectionalLightRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEDirectionalLight* directionalLight = qobject_cast<NEDirectionalLight*>(spatial);
    if (directionalLight == NULL) return;
    if(directionalLight->RenderMode() != NESpatial::NERenderMode::Invisible)
    {
        NERENDERMAN.GLManager()->draw(directionalLight->mesh(), surface, directionalLight->DisplayMode() == NESpatial::BoundingBox, false, true, false, matrix44f());
        NERENDERMAN.GLManager()->setFillMode(NEFillMode::kWireframeFrontAndBack);
        GL->glColor4f(0.0, 0.0, 0.0, 1.0);
        GL->glEnable(GL_POLYGON_OFFSET_LINE);
        GL->glPolygonOffset(-0.01f, -0.01f);
        renderPicked(directionalLight, surface);
        GL->glDisable(GL_POLYGON_OFFSET_LINE);
    }
}

void NEDirectionalLightRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NEDirectionalLight* directionalLight = qobject_cast<NEDirectionalLight*>(spatial);
    if (directionalLight == NULL) return;
    NERENDERMAN.GLManager()->draw(directionalLight->mesh(), surface, directionalLight->DisplayMode() == NESpatial::BoundingBox, true, true, false, matrix44f());
}

void NEDirectionalLightRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NEDirectionalLight* directionalLight = qobject_cast<NEDirectionalLight*>(spatial);
    if (directionalLight == NULL) return;
    if(directionalLight->RenderMode() == NESpatial::NERenderMode::Invisible) return;
    NERENDERMAN.GLManager()->draw(directionalLight->mesh(), surface, directionalLight->DisplayMode() == NESpatial::BoundingBox, true, true, false, matrix44f());
}



NEDirectionalLightRendererFactory::NEDirectionalLightRendererFactory(NEManager* m) : NERendererFactory( m )
{

}

QString NEDirectionalLightRendererFactory::nodeName() { return "Directional Light Renderer"; }

NEObject *NEDirectionalLightRendererFactory::createInstance()
{
    return new NEDirectionalLightRenderer();
}



void NEPointLightRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEPointLight* pointLight = qobject_cast<NEPointLight*>(spatial);
    if (pointLight == NULL) return;

    NERENDERMAN.GLManager()->draw(pointLight->mesh(), surface, pointLight->DisplayMode() == NESpatial::BoundingBox, false, true, false, matrix44f());
    NERENDERMAN.GLManager()->setFillMode(NEFillMode::kWireframeFrontAndBack);
    float currentColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    if (surface->shaderProgram()) GL->glUniform4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "meshColor" ), 1, currentColor );
    GL->glEnable(GL_POLYGON_OFFSET_LINE);
    GL->glPolygonOffset(-0.01f, -0.01f);
    renderPicked(pointLight, surface);
    GL->glDisable(GL_POLYGON_OFFSET_LINE);
}

void NEPointLightRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NEPointLight* pointLight = qobject_cast<NEPointLight*>(spatial);
    if (pointLight == NULL) return;
    if(pointLight->RenderMode() == NESpatial::NERenderMode::Invisible) return;
    NERENDERMAN.GLManager()->draw(pointLight->mesh(), surface, pointLight->DisplayMode() == NESpatial::BoundingBox, true, true, false, matrix44f());
}

void NEPointLightRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    renderPickable(spatial, surface);
}

NEPointLightRendererFactory::NEPointLightRendererFactory(NEManager* m) : NERendererFactory( m )
{

}

QString NEPointLightRendererFactory::nodeName() { return "Point Light Renderer"; }

NEObject *NEPointLightRendererFactory::createInstance()
{
    return new NEPointLightRenderer();
}




void NEAreaLightRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEAreaLight* areaLight = qobject_cast<NEAreaLight*>(spatial);
    if (areaLight == NULL) return;

    if(areaLight->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    float halfW = areaLight->Width() * 0.5;
    float halfH = areaLight->Height() * 0.5;

    GL->glBegin(GL_QUADS);
    GL->glVertex3f(0.0f - halfW * 0.75f, 0.2f, 0.0f - halfH * 0.75f);
    GL->glVertex3f(0.0f + halfW * 0.75f, 0.2f, 0.0f - halfH * 0.75f);
    GL->glVertex3f(0.0f + halfW * 0.75f, 0.2f, 0.0f + halfH * 0.75f);
    GL->glVertex3f(0.0f - halfW * 0.75f, 0.2f, 0.0f + halfH * 0.75f);
    GL->glEnd();

    GL->glBegin(GL_QUADS);
    GL->glVertex3f(0.0f - halfW, 0.0f, 0.0f - halfH);
    GL->glVertex3f(0.0f + halfW, 0.0f, 0.0f - halfH);
    GL->glVertex3f(0.0f + halfW, 0.0f, 0.0f + halfH);
    GL->glVertex3f(0.0f - halfW, 0.0f, 0.0f + halfH);
    GL->glEnd();

    GL->glBegin(GL_QUADS);
    GL->glVertex3f(0.0f - halfW, 0.0f, 0.0f - halfH);
    GL->glVertex3f(0.0f - halfW * 0.75, 0.2f, 0.0f - halfH * 0.75f);
    GL->glVertex3f(0.0f + halfW * 0.75, 0.2f, 0.0f - halfH * 0.75f);
    GL->glVertex3f(0.0f + halfW, 0.0f, 0.0f - halfH);
    GL->glEnd();

    GL->glBegin(GL_QUADS);
    GL->glVertex3f(0.0f + halfW, 0.0f, 0.0f - halfH);
    GL->glVertex3f(0.0f + halfW * 0.75f, 0.2f, 0.0f - halfH * 0.75f);
    GL->glVertex3f(0.0f + halfW * 0.75f, 0.2f, 0.0f + halfH * 0.75f);
    GL->glVertex3f(0.0f + halfW, 0.0f, 0.0f + halfH);
    GL->glEnd();

    GL->glBegin(GL_QUADS);
    GL->glVertex3f(0.0f + halfW, 0.0f, 0.0f + halfH);
    GL->glVertex3f(0.0f + halfW * 0.75f, 0.2f, 0.0f + halfH * 0.75f);
    GL->glVertex3f(0.0f - halfW * 0.75f, 0.2f, 0.0f + halfH * 0.75f);
    GL->glVertex3f(0.0f - halfW, 0.0f, 0.0f + halfH);
    GL->glEnd();

    GL->glBegin(GL_QUADS);
    GL->glVertex3f(0.0f - halfW, 0.0f, 0.0f + halfH);
    GL->glVertex3f(0.0f - halfW * 0.75f, 0.2f, 0.0f + halfH * 0.75f);
    GL->glVertex3f(0.0f - halfW * 0.75f, 0.2f, 0.0f - halfH * 0.75f);
    GL->glVertex3f(0.0f - halfW, 0.0f, 0.0f - halfH);
    GL->glEnd();
}

void NEAreaLightRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    render(spatial, surface);
}

void NEAreaLightRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    render(spatial, surface);
}

NEAreaLightRendererFactory::NEAreaLightRendererFactory(NEManager* m) : NERendererFactory( m )
{

}

QString NEAreaLightRendererFactory::nodeName() { return "Area Light Renderer"; }

NEObject *NEAreaLightRendererFactory::createInstance()
{
    return new NEAreaLightRenderer();
}




void NESpotLightRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NESpotLight* spotLight = qobject_cast<NESpotLight*>(spatial);
    if (spotLight == NULL) return;

    if(spotLight->RenderMode() == NESpatial::NERenderMode::Invisible) return;

    NERENDERMAN.GLManager()->draw(spotLight->mesh(), surface, spotLight->DisplayMode() == NESpatial::BoundingBox, false, true, false, matrix44f());
    NERENDERMAN.GLManager()->setFillMode(NEFillMode::kWireframeFrontAndBack);
    GL->glColor4f(0.0, 0.0, 0.0, 1.0);
    GL->glEnable(GL_POLYGON_OFFSET_LINE);
    GL->glPolygonOffset(-0.01f, -0.01f);
    renderPicked(spatial, surface);
    GL->glDisable(GL_POLYGON_OFFSET_LINE);
}

void NESpotLightRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    renderPicked(spatial, surface);
}

void NESpotLightRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NESpotLight* spotLight = qobject_cast<NESpotLight*>(spatial);
    if (spotLight == NULL) return;
    if(spotLight->RenderMode() == NESpatial::NERenderMode::Invisible) return;
    NERENDERMAN.GLManager()->draw(spotLight->mesh(), surface, spotLight->DisplayMode() == NESpatial::BoundingBox, true, true, false, matrix44f());
}

NESpotLightRendererFactory::NESpotLightRendererFactory(NEManager* m) : NERendererFactory( m )
{

}

QString NESpotLightRendererFactory::nodeName() { return "Spot Light Renderer"; }

NEObject *NESpotLightRendererFactory::createInstance()
{
    return new NESpotLightRenderer();
}

