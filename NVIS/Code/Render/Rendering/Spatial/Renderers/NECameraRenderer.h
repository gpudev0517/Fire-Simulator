#pragma once


#include "NESpatialGroupRenderer.h"

//
// Camera
//
class NECameraRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};

class NECameraRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NECameraRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};


