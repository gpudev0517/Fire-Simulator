#pragma once


#include "Rendering/Spatial/Renderers/NESpatialMeshRenderer.h"
#include "Base/NERendererFactory.h"
#include "Rendering/Spatial/Renderers/NESpatialGroupRenderer.h"



// Spatial Group
class FDYNCEXP_PLUGINS FFireRenderer : public NESpatialMeshRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);

public slots:
    void updateDrawingBuffers() override;
};

class FFireRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    FFireRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

