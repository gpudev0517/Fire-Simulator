#pragma once


#include "Rendering/Spatial/Renderers/NESpatialRenderer.h"
#include "Base/NERendererFactory.h"


//
// MeshConstructor
//
class NECEXP_NERENDERNEO NEMarchingCubesRenderer : public NESpatialRenderer
{
    Q_OBJECT

public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
    virtual void renderWater(NESpatial* spatial, IGLSurface* surface);
    virtual void connectToSpatial();

public slots:
    void initObject();
    void updateDrawingBuffers();
};

