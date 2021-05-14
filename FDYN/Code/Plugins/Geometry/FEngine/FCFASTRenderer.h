#pragma once


#include "Rendering/Spatial/Renderers/NESpatialMeshRenderer.h"
#include "Base/NERendererFactory.h"
#include "Rendering/Spatial/Renderers/NESpatialGroupRenderer.h"

#include "Rendering/Spatial/Renderers/NEOSDBaseRenderer.h"


// Spatial Group
class FDYNCEXP_PLUGINS FCFASTRenderer : public NESpatialMeshRenderer
{
    Q_OBJECT
public:
    void render(NESpatial* spatial, IGLSurface* surface) override;
    void renderPickable(NESpatial* spatial, IGLSurface* surface) override;
    void renderPicked(NESpatial* spatial, IGLSurface* surface) override;

    void renderColorRamp(NESpatial* spatial, IGLSurface* surface);

public slots:
    void updateDrawingBuffers() override;
};

class FCFASTRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    FCFASTRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};


class FDYNCEXP_PLUGINS FCFASTPlaneRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    void render(NESpatial* spatial, IGLSurface* surface) override;
    void renderPickable(NESpatial* spatial, IGLSurface* surface) override;
    void renderPicked(NESpatial* spatial, IGLSurface* surface) override;

};


class FCFASTPlaneRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    FCFASTPlaneRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};





class FOSDCFASTRenderer : public NEOSDBaseRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};




class FOSDCFASTRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    FOSDCFASTRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

