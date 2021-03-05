#pragma once


#include "Rendering/Spatial/Renderers/NESpatialGroupRenderer.h"

#include "Base/NERendererFactory.h"



// Spatial Group
class FDYNCEXP_PLUGINS FEngineRenderer : public NESpatialGroupRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};

class FEngineRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    FEngineRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

