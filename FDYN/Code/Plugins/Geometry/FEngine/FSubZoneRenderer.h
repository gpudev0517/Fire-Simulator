#pragma once


#include "Rendering/Spatial/Renderers/NESpatialMeshRenderer.h"
#include "Base/NERendererFactory.h"

#include "FZoneCompartmentRenderer.h"



// Spatial Group
class FDYNCEXP_PLUGINS FSubZoneRenderer : public FZoneCompartmentRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);

public slots:
    virtual void updateDrawingBuffers();
};

class FSubZoneRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    FSubZoneRendererFactory(NEManager* m = 0);

    QString nodeName();

private:
    NEObject* createInstance();
};

