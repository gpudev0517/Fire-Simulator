#pragma once

#include "Rendering/Spatial/NEOSDBase.h"

#include "NESpatialRenderer.h"
#include "Base/NERendererFactory.h"


// OSD
//
class NECEXP_NERENDERING NEOSDBaseRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);

    static void glPrint(QString name, int x, int y, QColor colorval, FTTextureFont* textureFont, QColor outlineColor, FTOutlineFont* textureOutlineFont);
protected:
    static void drawOSDRect(int x1, int y1, int x2, int y2, IGLSurface* surface);

};

