#pragma once


#include "Rendering/Spatial/Renderers/NESpatialMeshRenderer.h"
#include "Base/NERendererFactory.h"

#include "Rendering/Spatial/Renderers/NEZoneCompartmentRenderer.h"



// Spatial Group
class FDYNCEXP_PLUGINS FZoneCompartmentRenderer : public NEZoneCompartmentRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);

public slots:
    virtual void updateDrawingBuffers();
};

class FZoneCompartmentRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    FZoneCompartmentRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

