#include "NEOSDBaseRenderer.h"
#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Manager/NERenderManager.h"



//
// OSDBase
//
void NEOSDBaseRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NEOSDBase* osdBase = qobject_cast<NEOSDBase*>(spatial);
    if (osdBase == NULL) return;
    if(osdBase->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadIdentity();

    int xRes = surface->getWidth() * osdBase->Positionx();
    int yRes = surface->getHeight() * osdBase->Positiony();

    NERENDERMAN.GLManager()->bindPositionShader(surface);
    NERENDERMAN.GLManager()->updateCPUniforms(surface);

    int left = xRes - osdBase->leftMargin() - osdBase->osdSize().x() / 2;
    int right = xRes + osdBase->osdSize().x() / 2;
    int top = yRes - osdBase->topMargin() - osdBase->osdSize().y() / 2;
    int bottom = yRes + osdBase->osdSize().y() / 2;

    drawOSDRect(left, top, right, bottom, surface);

    GL->glPopAttrib();
}

void NEOSDBaseRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NEOSDBase* osdBase = qobject_cast<NEOSDBase*>(spatial);
    if (osdBase == NULL) return;
    if(osdBase->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadIdentity();

    int xRes = surface->getWidth() * osdBase->Positionx();
    int yRes = surface->getHeight() * osdBase->Positiony();

    NERENDERMAN.GLManager()->bindPositionShader(surface);
    NERENDERMAN.GLManager()->updateCPUniforms(surface);

    static Color32 OsdLine(0, 255, 0, 255);

    NERENDERMAN.GLManager()->setFillMode(kWireframeFrontAndBack);
    GL->glColor4ubv((GLubyte*)&OsdLine);

    int left = xRes - osdBase->leftMargin() - osdBase->osdSize().x() / 2;
    int right = xRes + osdBase->osdSize().x() / 2;
    int top = yRes - osdBase->topMargin() - osdBase->osdSize().y() / 2;
    int bottom = yRes + osdBase->osdSize().y() / 2;

    drawOSDRect(left, top, right, bottom, surface);

    GL->glPopAttrib();
}

void NEOSDBaseRenderer::drawOSDRect(int x1, int y1, int x2, int y2, IGLSurface *surface)
{
    float currentColor[4];
    GL->glGetFloatv( GL_CURRENT_COLOR, currentColor );
    GL->glUniform4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "meshColor" ), 1, currentColor );

    GLshort vtx1[] = { (GLshort)x1, (GLshort)y1,    (GLshort)x1, (GLshort)y2, (GLshort)x2, (GLshort)y2, (GLshort)x2, (GLshort)y1};
    GL->glEnableClientState(GL_VERTEX_ARRAY);
    GL->glVertexPointer(2, GL_SHORT, 0, vtx1);
    GL->glDrawArrays(GL_QUADS,0,4);
    GL->glDisableClientState(GL_VERTEX_ARRAY);
}

void NEOSDBaseRenderer::glPrint(QString name, int x, int y, QColor colorval, FTTextureFont* textureFont, QColor outlineColor, FTOutlineFont* textureOutlineFont)
{
    Color32 color(colorval.red(), colorval.green(), colorval.blue(), 255);
    GL->glColor3ubv((GLubyte*)&color);
    textureFont->Render(name.toUtf8(), -1, FTPoint(x, y));

    if(textureOutlineFont != NULL){
        Color32 outline(outlineColor.red(), outlineColor.green(), outlineColor.blue(), 255);
        GL->glColor3ubv((GLubyte*)&outline);
        textureOutlineFont->Render(name.toUtf8(), -1, FTPoint(x, y));
    }
}
