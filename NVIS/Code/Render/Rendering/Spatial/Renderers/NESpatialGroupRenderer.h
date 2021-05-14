#pragma once


#include "NESpatialRenderer.h"
#include "Base/NERendererFactory.h"



// Spatial Group
class NECEXP_NERENDERING NESpatialGroupRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};

class NESpatialGroupRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NESpatialGroupRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

