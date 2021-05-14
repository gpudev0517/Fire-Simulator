#pragma once

#include "Rendering/Spatial/NEOSDBase.h"
#include "Rendering/Spatial/Renderers/NEOSDBaseRenderer.h"
#include "Rendering/Spatial/Renderers/NESpatialMeshRenderer.h"
#include "Rendering/Spatial/Renderers/NESpatialRenderer.h"
#include "Base/NERendererFactory.h"

class IGLSurface;
class NEVertexBuffer;
class NEParticleFluidSolver;


// Particle Rigid
class NECEXP_NERENDERING NEZoneCompartmentRenderer : public NESpatialMeshRenderer
{
    Q_OBJECT
public:
    NEZoneCompartmentRenderer();
    ~NEZoneCompartmentRenderer();

    virtual void connectToSpatial();

    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderOnlyDepth(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
    virtual void renderInMotion(NESpatial* spatial, IGLSurface* surface);

public slots:
    void initObject();
    virtual void updateDrawingBuffers();

protected:


};

class NEZoneCompartmentRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEZoneCompartmentRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};
