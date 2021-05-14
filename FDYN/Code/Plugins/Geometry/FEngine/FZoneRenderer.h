#pragma once


#include "Rendering/Spatial/Renderers/NESpatialMeshRenderer.h"
#include "Base/NERendererFactory.h"

#include "Rendering/Spatial/Renderers/NEZoneRenderer.h"



// Spatial Group
class FDYNCEXP_PLUGINS FZoneRenderer : public NEZoneRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);

private:
    NEIndexedTriMesh createMesh(AABB outerBB, AABB innerBB);
};

class FZoneRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    FZoneRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

