#pragma once

#include "NESpatialRenderer.h"
#include "Base/NERendererFactory.h"



//
// Lights
//
class NEDirectionalLightRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};

class NEDirectionalLightRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEDirectionalLightRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

class NEPointLightRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};

class NEPointLightRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEPointLightRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

class NEAreaLightRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};

class NEAreaLightRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEAreaLightRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};


class NESpotLightRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};

class NESpotLightRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NESpotLightRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};


