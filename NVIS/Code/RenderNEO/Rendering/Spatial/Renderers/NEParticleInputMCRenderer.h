#pragma once


#include "Rendering/Spatial/Renderers/NESpatialRenderer.h"
#include "Base/NERendererFactory.h"
#include "NEMCRenderer.h"


class NECEXP_NERENDERNEO NEParticleInputMCRenderer : public NEMarchingCubesRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
};


