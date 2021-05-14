#pragma once

#include "Rendering/Spatial/NEOSDBase.h"
#include "Rendering/Spatial/Renderers/NEOSDBaseRenderer.h"
#include "Rendering/Spatial/Renderers/NESpatialRenderer.h"
#include "Base/NERendererFactory.h"


// MeshPostProcessor
class NECEXP_NERENDERNEO NEMeshPostProcessorRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};

class  NEMeshPostProcessorRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEMeshPostProcessorRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};


